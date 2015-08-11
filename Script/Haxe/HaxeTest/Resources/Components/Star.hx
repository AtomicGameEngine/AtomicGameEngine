package components;

import atomic.Atomic;

class Star extends JSComponent {

	var speed:Int;

	function new() {
		super();
		speed = 1;
		var color = getColor();
		switch (color) {
		  case Red: trace("Color was red");
		  case Green: trace("Color was green");
		  case Blue: trace("Color was blue");
		  case Rgb(r, g, b): trace('r: $r, g: $g, b: $b');
		}
	  }

	  static function getColor():Color {
		return Color.Rgb(255, 0, 255);
	  }

	function update(timeStep:Float):Void {
		this.node.rotate2D(speed);
		this.node.rotateAround2D([1, 1], timeStep * 50, TransformSpace.TS_WORLD);
	}
}

//Just a test of enums
enum Color {
  Red;
  Green;
  Blue;
  Rgb(r:Int, g:Int, b:Int);
}