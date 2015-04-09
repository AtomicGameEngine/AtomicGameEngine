var AtomicLoader;
if (typeof AtomicLoader === 'undefined') AtomicLoader = eval('(function() { try { return AtomicLoader || {} } catch(e) { return {} } })()');

window.addEventListener("keydown", function(e) {

  // Disable stock behavior, which was causing scroll errors
  // space and arrow keys
  if([32, 37, 38, 39, 40].indexOf(e.keyCode) > -1) {
    e.preventDefault();
  }
}, false);


var atomicCanvas = document.getElementById('canvas');

var _backgroundImageData = null;

// <canvas id="loadcanvas" width="800" height="600" oncontextmenu="event.preventDefault()"></canvas>

function DrawProgress(text, current) {

  var bg = document.getElementById('loadcanvas');
  var ctx = bg.getContext('2d');
  var rect = bg.getBoundingClientRect();

  var cwidth = rect.width;
  var cheight = rect.height;

  // for background image
  if (!_backgroundImageData)
    _backgroundImageData = ctx.getImageData(0, 0, cwidth, cheight);

  ctx.putImageData(_backgroundImageData, 0, 0);

  ctx.font="30px Arial";

  var progressText = (current * 100) + "%";

  var textSize =  ctx.measureText(progressText);

  ctx.fillText(progressText, cwidth/2 - textSize.width/2, cheight/2 + 15);

  ctx.beginPath();
  ctx.strokeStyle = '#99CC33';
  ctx.lineCap = 'square';
  ctx.closePath();
  ctx.fill();
  ctx.lineWidth = 10.0;

  var circ = Math.PI * 2;
  var quart = Math.PI / 2;
  ctx.beginPath();
  ctx.arc(cwidth/2, cheight/2, 70, -(quart), ((circ) * current) - quart, false);
  ctx.stroke();

}

document.getElementById('canvas').style.display = 'none';

DrawProgress("Downloading", 0.1);

var Module = {

  arguments: ["-x", "800", "-y", "600"],

  preRun:[function() {
    DrawProgress("Initializing", 0.75);
  }],

  postRun: [function() {
    document.getElementById('loadcanvas').style.display = 'none';
    document.getElementById('canvas').style.display = 'block';
  }],

  print: (function() {
    var element = document.getElementById('output');
    if (element) element.value = ''; // clear browser cache
    return function(text) {
      if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
      // These replacements are necessary if you render to raw HTML
      //text = text.replace(/&/g, "&amp;");
      //text = text.replace(/</g, "&lt;");
      //text = text.replace(/>/g, "&gt;");
      //text = text.replace('\n', '<br>', 'g');
      console.log(text);
      if (element) {
        element.value += text + "\n";
        element.scrollTop = element.scrollHeight; // focus on bottom
      }
    };
  })(),
  printErr: function(text) {
    if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
    if (0) { // XXX disabled for safety typeof dump == 'function') {
      dump(text + '\n'); // fast, straight to the real console
    } else {
      console.error(text);
    }
  },
  canvas: (function() {
    var canvas = document.getElementById('canvas');

    // As a default initial behavior, pop up an alert when webgl context is lost. To make your
    // application robust, you may want to override this behavior before shipping!
    // See http://www.khronos.org/registry/webgl/specs/latest/1.0/#5.15.2
    canvas.addEventListener("webglcontextlost", function(e) { alert('WebGL context lost. You will need to reload the page.'); e.preventDefault(); }, false);

    return canvas;
  })(),
  setStatus: function(text) {

  },
  totalDependencies: 0,
  monitorRunDependencies: function(left) {
    this.totalDependencies = Math.max(this.totalDependencies, left);
    Module.setStatus(left ? 'Preparing... (' + (this.totalDependencies-left) + '/' + this.totalDependencies + ')' : 'All downloads complete.');
  }
};

Module.setStatus('Downloading...');
window.onerror = function(event) {
  // TODO: do not warn on ok events like simulating an infinite loop or exitStatus
  Module.setStatus('Exception thrown, see JavaScript console');
  Module.setStatus = function(text) {
    if (text) Module.printErr('[post-exception status] ' + text);
  };
};
