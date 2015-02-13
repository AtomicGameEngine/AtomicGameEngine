
// This script is the main entry point of the game

require("AtomicGame");

Atomic.game.init(start, update);


// called at the start of play
function start() {

	var game = Atomic.game;

	// create a 2D scene
	game.createScene3D();

	var scene = game.scene;

	// zone
	var zoneNode = scene.createChild("Zone")
    var zone = zoneNode.createComponent("Zone");
    zone.boundingBox = [-1000, -1000, -1000, 1000, 1000 , 1000];
    zone.ambientColor = [0.35, 0.35, 0.35];
    zone.fogColor = [0.1, 0.1, 0.1, 1.0];
    zone.fogStart = 10;
    zone.fogEnd = 100;

    game.cameraNode.position = [0, 2.5, -6];
    game.cameraNode.pitch(20);

    var lightNode = scene.createChild("Directional Light");
    lightNode.direction = [0.6, -1.0, 0.8];
    var light = lightNode.createComponent("Light")
    light.lightType = Atomic.LIGHT_DIRECTIONAL;

	// create the game component
	var node = game.scene.createChild("Chest");
	node.createJSComponent("Chest");

}

// called per frame
function update(timeStep) {


}
