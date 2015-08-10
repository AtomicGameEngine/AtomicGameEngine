var __extends = (this && this.__extends) || function (d, b) {for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];function __() { this.constructor = d; };__.prototype = b.prototype;d.prototype = new __();};
var Main = (function(_super) {
__extends(Main, _super);
function Main (){};
Main.main = function() {
	Atomic.player.loadScene("Scenes/Scene.scene");
};
Main.main();
;
return Main;
})(Object);
module.exports = Main;
