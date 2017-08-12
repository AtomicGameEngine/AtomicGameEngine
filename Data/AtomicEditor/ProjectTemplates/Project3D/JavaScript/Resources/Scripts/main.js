// This script is the main entry point of the game

var scene = Atomic.player.loadScene("Scenes/Scene.scene");

exports.update = function() {

    if (Atomic.input.getKeyDown(Atomic.KEY_ESCAPE)) {
        Atomic.engine.exit();
    }

}
