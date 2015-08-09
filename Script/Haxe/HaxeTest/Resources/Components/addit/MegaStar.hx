package components.addit;

import components.SuperStar;
import scripts.utils.Loler;

class MegaStar extends SuperStar {
	
	override function start() {
		var lol:Loler = new Loler("lol");
		var nlol:Loler = new Loler("foo");
		node.scale2D = [4, 4];
		
		var str:String = "ReplAce me :)";
		str = StringTools.replace(str, "Ace", "LOL");
		trace(str);
	}
	
}