package components;

import atomic.Atomic;
import haxe.ds.ArraySort;

class Star extends JSComponent {
	
	function new() {
		super();
	}
	
	function update(timeStep:Float):Void {
		this.node.rotate2D(timeStep * 75);
		this.node.rotateAround2D([1, 1], timeStep * 50, Atomic.TS_WORLD);
	}
}