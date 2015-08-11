var Color = require("modules/Color");
"atomic component";
var __extends = (this && this.__extends) || function (d, b) {for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];function __() { this.constructor = d; };__.prototype = b.prototype;d.prototype = new __();};
var Star = (function(_super) {
__extends(Star, _super);
function Star () {
	Atomic.JSComponent.call(this);
	this.speed = 1;
	var color = Star.getColor();
	switch(color[1]) {
	case 0:
		console.log("Color was red");
		break;
	case 1:
		console.log("Color was green");
		break;
	case 2:
		console.log("Color was blue");
		break;
	case 3:
		var b = color[4];
		var g = color[3];
		var r = color[2];
		console.log("r: " + r + ", g: " + g + ", b: " + b);
		break;
	}
};
Star.getColor = function() {
	return Color.Rgb(255,0,255);
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
