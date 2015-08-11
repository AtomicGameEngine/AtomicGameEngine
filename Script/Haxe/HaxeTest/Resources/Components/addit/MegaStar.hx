package components.addit;

import atomic.Atomic;
import components.SuperStar;
import scripts.utils.Loler;

class MegaStar extends SuperStar {
	
	var pos:Vector2;
	
	function new() {
		super(0);
	}

	override function start() {
		pos = node.position2D;
		var lol:Loler = new Loler("lol");
		var nlol:Loler = new Loler("foo");
		node.scale2D = [4, 4];

		var str:String = "ReplAce me :)";
		str = StringTools.replace(str, "Ace", "LOL");
		trace(str);
		
		var c:MegaColor = MegaColor.Supa;
	}
	
	override function update(time:Float) {
		super.update(time);
		onKeyDown();
	}
	
	function onKeyDown():Void {
		if (Atomic.input.getKeyDown(Atomic.KEY_LEFT)) {
			pos[0] -= 0.5;
		}
		node.position2D = pos;
	}
}

enum MegaColor {
	Supa;
	NotSupa;
}
