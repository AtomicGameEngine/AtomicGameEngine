
var ui = require("./ui/ui");

var env = Atomic.getToolEnvironment();

print(env.rootSourceDir);

var system = Atomic.getToolSystem();

system.loadProject("/Users/josh/Dev/atomic/AtomicExamples/NewSpaceGame");

new Atomic.PlayCmd().run();

( function() {



}());
