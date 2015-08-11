"atomic component";
var __extends = (this && this.__extends) || function (d, b) {for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];function __() { this.constructor = d; };__.prototype = b.prototype;d.prototype = new __();};
var Player = (function(_super) {
__extends(Player, _super);
function Player () {
	this.speed = 0.05;
	Atomic.JSComponent.call(this);
};
Player.prototype.pos = null;
Player.prototype.speed = null;
Player.prototype.c = null;
Player.prototype.time = null;
Player.prototype.start = function() {
	this.pos = this.node.position2D;
};
Player.prototype.update = function(time) {
	this.time += time;
	this.onKeyDown();
	console.log(this.c(this.time));
	this.node.position2D = this.pos;
};
Player.prototype.onKeyDown = function() {
	if(Atomic.input.getKeyDown(Atomic.KEY_LEFT)) this.pos[0] -= this.speed;
};
;
return Player;
})(Atomic.JSComponent);
module.exports = Player;
