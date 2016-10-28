"use strict";
let WebSocketServer = require("ws").Server;
let server, events = {};

function handleMessage(msg, id) {
    msg = JSON.parse(msg);
    if (events[msg.type])
        events[msg.type](msg.data);
    console.log(msg);
}
function initialize(httpserver) {
    server = new WebSocketServer({ server: httpserver });

    server.on("connection", function (ws) {
        console.log('new websocket connection');
        ws.on("message", handleMessage);
        ws.on("close", () => console.log('disconnected'));
    });
};

function on(event, fn) {
    events[event] = fn;
}

function send(type, data) {
    let message = JSON.stringify({ type: type, data: data });
    server.clients.forEach(function each(client) {
        client.send(message);
    });
}

module.exports = {
    initialize: initialize,
    on: on,
    send: send
};