angular.module('connect4app').controller('gameCtrl', function($scope, $timeout, socket) {
    socket.send('getstate');
    socket.on('gamestate', function(gamestate) {
        $timeout(() => {
            $scope.gamestate = gamestate;
            var playerColors = _.fromPairs(gamestate.players.map(p => [p.id, {
                hue: p.hue,
                luminosity: p.luminosity
            }]));
            gamestate.pieces = gamestate.board.map(col => _.reverse(col.map(cell => ({
                val: cell,
                color: playerColors[cell]
            }))));
            $scope.$apply();
        });
    })

    $scope.resetGame = () => {
        socket.send('resetboard');
    }
});