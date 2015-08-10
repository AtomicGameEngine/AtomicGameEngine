var __extends = (this && this.__extends) || function (d, b) {for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];function __() { this.constructor = d; };__.prototype = b.prototype;d.prototype = new __();};
var StringTools = (function(_super) {
__extends(StringTools, _super);
function StringTools (){};
StringTools.replace = function(s,sub,by) {
	return s.split(sub).join(by);
};
;
return StringTools;
})(Object);
module.exports = StringTools;
