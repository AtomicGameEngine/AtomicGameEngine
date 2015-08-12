package atomic;

//Atomic Haxe Definitions

extern class AtomicPlayer {


}

//----------------------------------------------------
// MODULE: Player
//----------------------------------------------------


@:native("Atomic.Player")
extern class Player extends AObject {

    var currentScene: Atomic.Scene;

      // Construct.
    function new();

    function loadScene(filename: String, ?camera: Atomic.Camera): Atomic.Scene;
    function getCurrentScene(): Atomic.Scene;

}


