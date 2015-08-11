package components;

import components.Star;

class SuperStar extends Star {
	
	function new(a:Int) {
		super();
		trace(a);
	}
	
	function start():Void {
		node.scale2D = [2, 2];
	}
	
}