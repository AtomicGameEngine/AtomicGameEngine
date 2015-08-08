var __extends = (this && this.__extends) || function (d, b) {for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];function __() { this.constructor = d; };__.prototype = b.prototype;d.prototype = new __();};
"atomic component";
var Star = (function(_super) {
__extends(Star, _super);
function Star() {
	Atomic.JSComponent.call(this);
};
Star.prototype.update = function(timeStep) {
	this.node.rotate2D(timeStep * 75);
	this.node.rotateAround2D([1,1],timeStep * 50,Atomic.TS_WORLD);
};
;
return Star;
})(Atomic.JSComponent);
module.exports = Star;