
var Module;
/*
window.onerror = function(event) {
  // TODO: do not warn on ok events like simulating an infinite loop or exitStatus
  Module.setStatus('Exception thrown, see JavaScript console');
  Module.setStatus = function(text) {
    if (text) Module.printErr('[post-exception status] ' + text);
  };
};
*/

var AtomicLoader = function(playerCanvas, spinner, width, height) {

  AtomicLoader.instance = this;

  this.playerCanvas = playerCanvas;
  this.spinner = spinner;
  this.width = width;
  this.height = height;

  playerCanvas.width = width;
  playerCanvas.height = height;

  // starts invisible
  playerCanvas.style.display = 'none';

  this.backgroundImageData = null;

  this.progress = 0.0;
  this.progressText = "Downloading"

  this.initModuleObject();

}

AtomicLoader.prototype.constructor = AtomicLoader;

AtomicLoader.prototype.initModuleObject = function() {

  Module = {

    arguments: ["-x", AtomicLoader.instance.width.toString(), "-y", AtomicLoader.instance.height.toString()],

    preRun:[function() {
      var loader = AtomicLoader.instance;
    }],

    postRun: [function() {
      var loader = AtomicLoader.instance;
      loader.playerCanvas.style.display = 'block';
      loader.spinner.style.display = 'none';
    }],

    canvas: (function() {
      var loader = AtomicLoader.instance;
      // As a default initial behavior, pop up an alert when webgl context is lost. To make your
      // application robust, you may want to override this behavior before shipping!
      // See http://www.khronos.org/registry/webgl/specs/latest/1.0/#5.15.2
      loader.playerCanvas.addEventListener("webglcontextlost", function(e) { alert('WebGL context lost. You will need to reload the page.'); e.preventDefault(); }, false);
      return loader.playerCanvas;
    })(),

    setStatus: function(text) {

    },
    totalDependencies: 0,
    monitorRunDependencies: function(left) {
      this.totalDependencies = Math.max(this.totalDependencies, left);
      Module.setStatus(left ? 'Preparing... (' + (this.totalDependencies-left) + '/' + this.totalDependencies + ')' : 'All downloads complete.');
    }
  };
}
