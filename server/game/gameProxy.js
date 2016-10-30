"use strict";

var _ = require('lodash');

let Connect4Game = require('./Connect4Game.js');
const MIN_TIME_BETWEEN_GAMES = 5 * 1000;
let mqtt, expressApp, game, websocketserver, gameConfig;

function buttonPressed(deviceId, message) {
    let player = gameConfig.getPlayer(deviceId);
    if (!player)
        return;

    let otherPlayer = gameConfig.getOtherPlayer(deviceId);
    if (game.finished) {
        if (Date.now() - game.finishedTime < MIN_TIME_BETWEEN_GAMES)
            return;
        game = new Connect4Game();
        sendFullGameStateToClient(deviceId);
        sendFullGameStateToClient(otherPlayer.id);
    }
    let addResult = game.addAtColumn(player.id, Number(message));
    if (!addResult)
        return;
    [player, otherPlayer].forEach(p => {
        mqtt.send(p.id, 'addpiece', [addResult.row, addResult.column, player.hue, player.luminosity, deviceId === p.id ? 0 : 1].join('|'));
    });
    sendGameStateThroughSocket();
    if (addResult.hasWon) {
        let winPieces = _([player.hue, player.luminosity])
            .concat(_.flatten(addResult.hasWon.map(p => [p.row, p.col])).join(''))
            .flatten()
            .join('|');
        mqtt.send(player.id, 'gamefinished', '1|' + winPieces);
        mqtt.send(otherPlayer.id, 'gamefinished', '0|' + winPieces);
    }
    console.log(JSON.stringify(addResult));
}

function deviceConnected(deviceId) {
    console.log("device connected: " + deviceId);
    sendFullGameStateToClient(deviceId);
}

function getWholeBoard(deviceId) {
    let players = gameConfig.getPlayers();
    let playersByIndex = _(players)
        .map((p, i) => [p.id, i + 1])
        .fromPairs()
        .value();
    let msg = players.map((p) => [p.hue, p.luminosity]);
    var serializedboard = game.serializeBoard().map(v => v ? playersByIndex[v] : 0).join('');
    msg.push(serializedboard);
    return _.flatMap(msg).join('|');
}

function resetGame() {
    game = new Connect4Game();
    sendGameStateThroughSocket();
    sendFullGameToBothDevices();
}

function sendFullGameToBothDevices() {
    gameConfig
        .getPlayers()
        .forEach(p => sendFullGameStateToClient(p.id));
}

function sendFullGameStateToClient(playerId) {
    let player = gameConfig.getPlayer(playerId);
    if (player) {
        mqtt.send(playerId, 'volume', player.volume + "");
        mqtt.send(playerId, 'devicecolor', [player.hue, player.luminosity / 2, game.canPlayerPlay(playerId) ? 1 : 0].join('|'));
        mqtt.send(playerId, 'setboard', getWholeBoard());
    }
}

function sendGameStateThroughSocket() {
    var state = {
        board: game.board,
        finished: game.finished,
        players: gameConfig.getPlayers()
    };
    websocketserver.send('gamestate', state);
}

function onConfigUpdate(config) {
    sendGameStateThroughSocket();
    sendFullGameToBothDevices();
}

function initialize(_gameConfig, _mqtt, _websocketserver, _expressApp) {
    mqtt = _mqtt;
    expressApp = _expressApp;
    websocketserver = _websocketserver;
    gameConfig = _gameConfig;
    gameConfig.onConfigUpdate(onConfigUpdate);

    game = new Connect4Game();

    mqtt.on("button", buttonPressed);
    mqtt.on("connected", deviceConnected);
    mqtt.on("pong", deviceConnected);
    websocketserver.on('getstate', sendGameStateThroughSocket);
    websocketserver.on('resetboard', resetGame);
}

module.exports = {
    initialize: initialize
};