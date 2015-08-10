var Star = require("components/Star");
var __extends = (this && this.__extends) || function (d, b) {for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];function __() { this.constructor = d; };__.prototype = b.prototype;d.prototype = new __();};
var SuperStar = (function(_super) {
__extends(SuperStar, _super);
function SuperStar () {
	Star.call(this);
};
SuperStar.prototype.start = function() {
	this.node.scale2D = [2,2];
};
;
return SuperStar;
})(Star);
module.exports = SuperStar;
