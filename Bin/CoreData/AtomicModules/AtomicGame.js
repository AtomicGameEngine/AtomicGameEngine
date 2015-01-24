
Atomic.editor = null;

function Game() {

	this.engine = Atomic.GetEngine();
	this.cache = Atomic.GetResourceCache();	
	this.renderer = Atomic.GetRenderer();
	this.graphics = Atomic.GetGraphics();
	this.input = Atomic.GetInput();

}

Game.prototype.init = function(start, update) {

	this.start = start;
	this.update = update;

	// register globals to get at quickly
	__js_atomicgame_start = start;
	__js_atomicgame_update = update;

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

    var viewport = new Atomic.Viewport(scene, camera);

    if (Atomic.editor) {
        var _viewport = Atomic.editor.viewport;
        viewport.rect = [_viewport.left, _viewport.top, _viewport.right, _viewport.bottom];
    }

    this.renderer.setViewport(0, viewport);

    this.scene = scene;
    this.cameraNode = cameraNode;
    this.camera = camera;
    this.viewport = viewport;    

    return scene;

}

Game.prototype.createScene3D = function() {

    var scene = new Atomic.Scene();
    scene.createComponent("Octree");

    var cameraNode = scene.createChild("Camera");
    cameraNode.position = [0.0, 0.0, -10.0];

    var camera = cameraNode.createComponent("Camera");    

    var viewport = new Atomic.Viewport(scene, camera);

    if (Atomic.editor) {
        var _viewport = Atomic.editor.viewport;
        viewport.rect = [_viewport.left, _viewport.top, _viewport.right - 8, _viewport.bottom];
    }

    this.renderer.setViewport(0, viewport);

    this.scene = scene;
    this.cameraNode = cameraNode;
    this.camera = camera;
    this.viewport = viewport;    

    return scene;

}


Atomic.game = exports.game = new Game();

