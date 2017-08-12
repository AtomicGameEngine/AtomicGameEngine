
// This script is the main entry point of the game

exports.update = function() {

    if (Atomic.input.getKeyDown(Atomic.KEY_ESCAPE)) {
        Atomic.engine.exit();
    }

}
