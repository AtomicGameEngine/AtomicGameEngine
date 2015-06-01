
var ui = require("./ui/ui");

var env = ToolCore.getToolEnvironment();

print(env.rootSourceDir);

var system = ToolCore.getToolSystem();

system.loadProject("/Users/josh/Dev/atomic/AtomicExamples/NewSpaceGame");

var cool = new Editor.MyJSClass();
print(cool.aha);


( function() {



}());
