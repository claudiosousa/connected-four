angular.module('connect4app', ['ngRoute', 'ngResource']).config(function ($routeProvider) {
    $routeProvider
        .when('/game', {
            templateUrl: 'current-game/game.html',
            controller: 'gameCtrl'
        })
        .when('/config', {
            templateUrl: 'config/config.html',
            controller: 'configCtrl'
        })
        .otherwise({
            redirectTo: '/game'
        });
}).run(function ($rootScope, $location, socket) {
    $rootScope.navTo = function (dest) {
        $location.path(dest);
    }
})
