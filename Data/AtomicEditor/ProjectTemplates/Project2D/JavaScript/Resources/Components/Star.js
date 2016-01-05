"atomic component";

var inspectorFields = {
  speed: 1.0
}

exports.component = function(self) {

  self.update = function(timeStep) {

    self.node.rotate2D(timeStep * 75 * self.speed);

  }

}
