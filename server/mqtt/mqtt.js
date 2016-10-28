"use strict";

let mqtt = require('mqtt');
var _ = require('lodash');

let client;

var subscriptions = {};
function connect(config) {
    config.clientId += Math.random();
    client = mqtt.connect(config.host, config);
    client.on('error', err => console.log("MQTT eror: " + err));
    client.on('connect', function () {
        console.log('Connected to CloudMQTT');
        client.publish('ping');
        _.forOwn(subscriptions, (fn, action) => client.subscribe(action + '/#'));
    });

    client.on('message', function (topic, message, packet) {
        let topicParts = topic.split('/');
        let topicAction = topicParts[0];
        let topicDevice = topicParts[1];
        let fn = subscriptions[topicAction];
        fn && fn(topicDevice, message);
    });
}

function on(action, fn) {
    subscriptions[action] = fn;
    client.subscribe(action + '/#');
}

function send(device, action, message) {
    client.publish(device + '/' + action, message);
}

module.exports = {
    connect: connect,
    on: on,
    send: send
};