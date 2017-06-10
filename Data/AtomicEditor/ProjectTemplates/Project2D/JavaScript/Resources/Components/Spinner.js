"atomic component";
//inspector fields to make speed variable visible in editor
var inspectorFields = {
    speed: 1.0
}

exports.component = function(self) {
    
    //update function is called each frame.
    //timeStep: time since last call to update in seconds
    self.update = function(timeStep) {
        //roll a node
        self.node.roll(self.speed * timeStep * 75);

    }

}