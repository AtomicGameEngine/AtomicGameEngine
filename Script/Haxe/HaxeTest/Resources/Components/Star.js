var Stars = require("./Stars");
var __extends = (this && this.__extends) || function (d, b) {for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];function __() { this.constructor = d; };__.prototype = b.prototype;d.prototype = new __();};
var Star = (function(_super) {
__extends(Star, _super);
function Star () {
	Atomic.JSComponent.call(this);
	this.speed = 1;
	var type = Stars.RandomStar;
};
Star.prototype.speed = null;
Star.prototype.update = function(timeStep) {
	this.node.rotate2D(this.speed);
	this.node.rotateAround2D([1,1],timeStep * 50,Atomic.TS_WORLD);
};
;
return Star;
})(Atomic.JSComponent);
module.exports = Star;
