"atomic component";

var inspectorFields = {
  speed: 1.0
}

module.exports = function(self) {

  self.update = function(timeStep) {

    self.node.rotate2D(timeStep * 75 * self.speed);

  }

}