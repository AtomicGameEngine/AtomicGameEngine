package components;

import components.Star;

//Yeah, we imported Star, but we still need to define it.
//https://github.com/HaxeFoundation/haxe/issues/3560

//examples: 
//@:require("CStar") - will require a component Star(Components/Star)
//@:require("SFoo") - will require a script Foo(Scripts/Foo)
//@:require("MBar") - will require a module Bar(Modules/Bar)
@:require("CStar")
class SuperStar extends Star {
	
	function start():Void {
		node.scale2D = [2, 2];
	}
	
}