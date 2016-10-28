"use strict";

let express = require('express');
let path = require('path');

function initialize(webconfig, webapp) {
    webapp.use(express.static(path.join(__dirname, webconfig.path)));
}

module.exports = {
    initialize: initialize
};