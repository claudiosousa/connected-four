angular.module('connect4app').service('socket', function ($rootScope, $q) {
    var websocket = new WebSocket('ws' + (location.protocol.indexOf('s') > 0 ? 's' : '') + '://' + location.host);

    var events = {};

    var websocketConnecton = $q.defer();
    websocket.onopen = function () {
        console.log('Websocket connected');
        websocketConnecton.resolve();
    };

    websocket.onmessage = function (evt) {
        console.log("Message is received...\n" + evt.data);
        var msg = JSON.parse(evt.data);
        if (events[msg.type])
            events[msg.type](msg.data);
    };

    websocket.onclose = function () {
        console.log("Connection is closed...");
    };

    function send(type, data) {
        websocketConnecton.promise.then(() => {
            websocket.send(JSON.stringify({ type: type, data: data }));
        });
    }

    function on(type, fn) {
        events[type] = fn;
    }
    function off(type) {
        delete events[type];
    }

    return {
        send: send,
        on: on,
        off: off
    }
})
