"atomic component";
//inspector fields to make speed variable visible in editor
var inspectorFields = {
    speed: 1.0
}

exports.component = function(self) {
    
    //update function calls each frame
    self.update = function(timeStep) {
        //roll a node
        self.node.roll(timeStep * 100);

    }

}