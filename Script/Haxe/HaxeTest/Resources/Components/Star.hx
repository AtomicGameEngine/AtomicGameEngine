package components;

import atomic.Atomic;

class Star extends JSComponent {

	var speed:Int;

	function new() {
		super();
		speed = 1;
	}
	
	function update(timeStep:Float):Void {
		this.node.rotate2D(speed);
		this.node.rotateAround2D([1, 1], timeStep * 50, Atomic.TS_WORLD);
	}
}

enum Stars {
  SuperStar;
  NotSuperStar;
  RandomStar;
  Star(x:Int, y:Int);
}