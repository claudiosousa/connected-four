angular.module('connect4app')
    .controller('configCtrl', function ($scope, $timeout, socket) {
        socket.send('getconfig');
        socket.on('gameconfig', function (gameconfig) {
            gameconfig.forEach(g => {
                g.hue *= 360;
                g.luminosity *= 100;
                g.volume = Math.log(g.volume) / Math.log(2);
            });
            $timeout(() => $scope.gameconfig = gameconfig);
        });
        function normalizeFloat(nbIn) {
            return (nbIn + "").substr(0, 8);
        }

        var sendNewColor = _.throttle(function () {
            let gameconfig = angular.copy($scope.gameconfig);
            gameconfig.forEach(g => {
                g.hue = normalizeFloat(g.hue / 360);
                g.luminosity = normalizeFloat(g.luminosity / 100)
                g.volume = Math.pow(2, g.volume);
            })
            socket.send('setconfig', gameconfig);
        }, 100);

        $scope.updateConfig = function (gameconf) {
            sendNewColor();
        }
    });