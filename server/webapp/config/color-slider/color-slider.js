(function () {
  "use strict!";

  angular.module('connect4app')
    .directive('colorSlider', function () {
      return {
        templateUrl: "config/color-slider/color-slider.html",
        scope: {
          color: '=ngModel',
          onChange: "&"
        },
        link: function ($scope, $element, $attrs) {
          var hueBackground = Array.apply(null, { length: 37 }).map(function (v, i) {
            return i * 10;
          });

          var luminosityBackground = Array.apply(null, { length: 51 }).map(function (v, i) {
            return i;
          });

          var MAX_LUMINOSITY = 50;
          var MIN_VISIBLE_LUMINOSITY = 18;
          function normalizeLuminosity(luminosity) {
            return MIN_VISIBLE_LUMINOSITY + luminosity * (MAX_LUMINOSITY - MIN_VISIBLE_LUMINOSITY)/MAX_LUMINOSITY;
          }

          $scope.hueBackground = function () {
            return '-webkit-linear-gradient(left, ' +
              hueBackground
                .map(function (val) {
                  return 'hsla(' + val + ', 100%, ' + normalizeLuminosity($scope.color.luminosity) + '%, 1)';
                }).join(',') + ')'
          }


          $scope.luminosityBackground = function () {
            return '-webkit-linear-gradient(left, ' +
              luminosityBackground
                .map(function (val) {
                  return 'hsla(' + $scope.color.hue + ', 100%, ' + normalizeLuminosity(val) + '%, 1)';
                }).join(',') + ')'
          }
        }
      }
    });
})();