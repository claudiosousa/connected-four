"use strict";

let fs = require('fs');
var _ = require('lodash');

const CONFIG_FILE = 'gameConfig/gameConfig.json';
let websocketserver, config;

function getPlayer(deviceId) {
    return deviceId && config.devices.find(device => device.id === deviceId);
}

function getOtherPlayer(deviceId) {
    return deviceId && config.devices.find(device => device.id !== deviceId);
}

function sendConfig() {
    websocketserver.send('gameconfig', config.devices);
}

function setConfig(newconfig) {
    config.devices = newconfig;
    saveConfig();
    configUpdateFns.forEach(fn => fn(config));
}

function loadConfig() {
    config = JSON.parse(fs.readFileSync(CONFIG_FILE, 'utf8'));
}

function saveConfig() {
    fs.writeFileSync(CONFIG_FILE, JSON.stringify(config, null, '\t'), 'utf8')
}

function initialize(_websocketserver) {
    websocketserver = _websocketserver;
    loadConfig();

    websocketserver.on('getconfig', sendConfig);
    websocketserver.on('setconfig', setConfig);
}

let configUpdateFns = []

function onConfigUpdate(fn) {
    configUpdateFns.push(fn);
}
module.exports = {
    initialize: initialize,
    getPlayers: () => config.devices,
    getPlayer: getPlayer,
    getOtherPlayer: getOtherPlayer,
    onConfigUpdate: onConfigUpdate
}