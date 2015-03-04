
Atomic.editor = null;

function Game() {

	this.engine = Atomic.getEngine();
	this.cache = Atomic.getResourceCache();	
	this.renderer = Atomic.getRenderer();
	this.graphics = Atomic.getGraphics();
	this.input = Atomic.getInput();
    this.ui = Atomic.getUI();

    this.input.setMouseVisible(true);

    if (Atomic.platform == "Android") {
        this.renderer.reuseShadowMaps = false;
        this.renderer.shadowQuality = Atomic.SHADOWQUALITY_LOW_16BIT;
    }

}

Game.prototype.init = function(start, update) {

	this.update = update;

	// register global to get at quickly
	__js_atomicgame_update = update;

    if (typeof(start) === "function")
        start();

}

Game.prototype.getSpriteSheet2D = function(xmlFile) {

	return this.cache.getResource("SpriteSheet2D", xmlFile);

}

Game.prototype.getSpriteSheet = Game.prototype.getSpriteSheet2D;

Game.prototype.getSound = function(soundFile) {

    return this.cache.getResource("Sound", soundFile);

}

Game.prototype.getSprite2D = function(spriteFile) {

    return this.cache.getResource("Sprite2D", spriteFile);

}


Game.prototype.showDebugHud = function() {

	var uiStyle = this.cache.getResource("XMLFile", "UI/DefaultStyle.xml");
    var debugHud = this.engine.createDebugHud();
    debugHud.defaultStyle = uiStyle;
    debugHud.toggleAll();

}

Game.prototype.createScene2D = function() {

	var scene = new Atomic.Scene();
    scene.createComponent("Octree");

    var cameraNode = scene.createChild("Camera");
    cameraNode.position = [0.0, 0.0, -10.0];

    var camera = cameraNode.createComponent("Camera");    
    camera.orthographic = true;
    camera.orthoSize = this.graphics.height * Atomic.PIXEL_SIZE;

    var viewport = null;
    
    if (Atomic.editor) {
        viewport = Atomic.editor.setView(scene, camera);
    } else {
        viewport = new Atomic.Viewport(scene, camera);
        viewport.rect = [0, 0, this.graphics.width, this.graphics.height];
        this.renderer.setViewport(0, viewport);
    }

    this.scene = scene;
    this.cameraNode = cameraNode;
    this.camera = camera;
    this.viewport = viewport;    

    return scene;

}

Game.prototype.createScene3D = function(filename) {

    var scene = new Atomic.Scene();

    if (typeof(filename) == "string")
        scene.loadXML(filename)
    else
        scene.createComponent("Octree");

    var cameraNode = scene.createChild("Camera");
    cameraNode.position = [0.0, 0.0, -10.0];

    var camera = cameraNode.createComponent("Camera");    

    var viewport = null;
    if (Atomic.editor) {
        viewport = Atomic.editor.setView(scene, camera);
    } else {
        viewport = new Atomic.Viewport(scene, camera);
        this.renderer.setViewport(0, viewport);
    }

    this.scene = scene;
    this.cameraNode = cameraNode;
    this.camera = camera;
    this.viewport = viewport;    

    return scene;

}


Atomic.game = exports.game = new Game();

