var StringTools = require("Modules/StringTools");
var Loler = require("scripts/utils/Loler");
var SuperStar = require("components/SuperStar");
var __extends = (this && this.__extends) || function (d, b) {for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];function __() { this.constructor = d; };__.prototype = b.prototype;d.prototype = new __();};
var MegaStar = (function(_super) {
__extends(MegaStar, _super);
function MegaStar () {
	SuperStar.call(this);
};
MegaStar.prototype.start = function() {
	var lol = new Loler("lol");
	var nlol = new Loler("foo");
	this.node.scale2D = [4,4];
	var str = "ReplAce me :)";
	str = StringTools.replace(str,"Ace","LOL");
	console.log(str);
};
;
return MegaStar;
})(SuperStar);
module.exports = MegaStar;
