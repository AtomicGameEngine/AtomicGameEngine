
var game = Atomic.game;

function Editor() {

	// default to the entire screen
	this.viewport = {
		left: 0,
		top: 0,
		right: game.graphics.width,
		bottom: game.graphics.height
	}

}

Editor.prototype.setViewport = function(left, top, right, bottom) {
	
	this.viewport.left = left;
	this.viewport.top = top;
	this.viewport.right = right;
	this.viewport.bottom = bottom;

}


var editor = new Editor();

Atomic.editor = exports.editor = editor;