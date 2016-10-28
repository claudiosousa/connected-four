"use strict";

let fs = require('fs');
let expressApp = require('express')();
var http = require("http");
let mqtt = require('./mqtt/mqtt.js');
let webserver = require('./web/webserver.js');
let websocketserver = require('./web/websocketserver.js');
let gameProxy = require('./game/gameProxy.js');
let gameConfig = require('./gameConfig/gameConfig.js');

let config = JSON.parse(fs.readFileSync('config.json', 'utf8'));

var port = process.env.PORT || config.webserver.port;
var httpserver = http.createServer(expressApp);
httpserver.listen(port);
console.log("http server listening on %d", port);


mqtt.connect(config.mqtt, expressApp);
websocketserver.initialize(httpserver);
gameConfig.initialize(websocketserver);
gameProxy.initialize(gameConfig, mqtt, websocketserver, expressApp);
webserver.initialize(config.webserver, expressApp);
