package scripts;

import atomic.Atomic;

import components.Star;
import components.SuperStar;

class Main {
	
	static inline function main() {
		//Just a note:
		//I used untyped here because Atomic.player is not exists in definitions
		//untyped will compile your code as is
		//right now only one way to use player, input, graphics, engine, etc...(need to write definitions for that)
		untyped Atomic.player.loadScene("Scenes/Scene.scene");
	}

}