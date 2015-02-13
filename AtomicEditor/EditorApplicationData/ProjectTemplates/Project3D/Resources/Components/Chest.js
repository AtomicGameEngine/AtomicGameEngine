var game = Atomic.game;
var node = self.node;

function start() {

    var cache = game.cache;

    var model = node.createComponent("StaticModel");
    model.setModel(cache.getResource("Model", "Models/Chest.mdl"));
    model.setMaterial(cache.getResource("Material", "Materials/Chest.xml"));
        
    node.pitch(-90);    
        
}


function update(timeStep) {

   node.roll(timeStep * 75);

}