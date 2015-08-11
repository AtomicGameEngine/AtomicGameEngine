var MegaColor = require("modules/MegaColor");
var StringTools = require("modules/StringTools");
var Loler = require("scripts/utils/Loler");
var SuperStar = require("components/SuperStar");
"atomic component";
var __extends = (this && this.__extends) || function (d, b) {for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];function __() { this.constructor = d; };__.prototype = b.prototype;d.prototype = new __();};
var MegaStar = (function(_super) {
__extends(MegaStar, _super);
function MegaStar () {
	SuperStar.call(this,0);
};
MegaStar.prototype.pos = null;
MegaStar.prototype.start = function() {
	this.pos = this.node.position2D;
	var lol = new Loler("lol");
	var nlol = new Loler("foo");
	this.node.scale2D = [4,4];
	var str = "ReplAce me :)";
	str = StringTools.replace(str,"Ace","LOL");
	console.log(str);
	var c = MegaColor.Supa;
};
MegaStar.prototype.update = function(time) {
	SuperStar.prototype.update.call(this,time);
	this.onKeyDown();
};
MegaStar.prototype.onKeyDown = function() {
	if(Atomic.input.getKeyDown(Atomic.KEY_LEFT)) this.pos[0] -= 0.5;
	this.node.position2D = this.pos;
};
;
return MegaStar;
})(SuperStar);
module.exports = MegaStar;
