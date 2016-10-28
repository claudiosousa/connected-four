"use strict";
var _ = require('lodash');

const COLS = 7;
const ROWS = 6;

const searchDirections = [
    [0, 1], //horizontal
    [1, 0], //vertical
    [1, 1], //South East
    [-1, 1] //South West
];

function checkIfBoardWonAt(board, col, row) {
    let player = board[col][row];

    let foundMatchingPieces;
    function checkBoardInDirection(x, y, direction) {
        x += direction[0];
        y += direction[1];
        //we leave if out of bounds, or similar piece not found
        if (x < 0 || x >= COLS || y < 0 || y >= ROWS || board[x][y] !== player)
            return 0;
        foundMatchingPieces.push({ col: x, row: y });
        return 1 + checkBoardInDirection(x, y, direction);
    }
    let foundWin = searchDirections.some(direction => {
        foundMatchingPieces = [{ col, row }];
        var found = checkBoardInDirection(col, row, direction) + //search in one direction
            checkBoardInDirection(col, row, direction.map(d => -d)) + //opposite direction
            1; //add starting piece
        return found >= 4;
    });

    return foundWin && foundMatchingPieces;
}

class Connect4Game {
    constructor() {
        this.resetBoard();
        this.finished = false;
        this.finishedTime = 0;
    }

    resetBoard() {
        this.board = [];
        for (let i = 0; i < COLS; i++) {
            let arr = [];
            arr.length = ROWS;
            this.board.push(arr);
        }
    }

    canPlayerPlay(player) {
        return this.lastPlayer !== player && !this.finished;
    }

    addAtColumn(player, column) {
        if (!this.canPlayerPlay(player))
            return;
        let row = _.findIndex(this.board[column], v => v == undefined);
        if (row === -1)
            return;
        this.board[column][row] = player;
        this.lastPlayer = player;
        this.finished = this.checkIfWonAt(column, row);
        if (this.finished)
            this.finishedTime = Date.now();
        return {
            row: row,
            column: column,
            hasWon: this.finished
        };
    }

    checkIfWonAt(column, row) {
        return checkIfBoardWonAt(this.board, column, row);
    }

    serializeBoard() {
        let res = [];
        for (let y = 0; y < ROWS; y++)
            for (let x = 0; x < COLS; x++)
                res.push(this.board[x][y]);
        return res;
    }
}

module.exports = Connect4Game;