var game = Atomic.game;
var node = self.node;

function start() {

	var sprite2D = node.createComponent("StaticSprite2D");
	sprite2D.sprite = game.getSprite2D("Sprites/star.png");
	sprite2D.blendMode = Atomic.BLEND_ALPHA;
	
}

function update(timeStep) {	

	node.roll(timeStep * 100);

}