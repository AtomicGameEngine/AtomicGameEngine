/// <reference path="/Users/josh/Dev/thunderbeast/AtomicGameEngine/Bin/Atomic.d.ts" />
/// <reference path="/Users/josh/Dev/thunderbeast/AtomicGameEngine/Bin/ToolCore.d.ts" />
/// <reference path="/Users/josh/Dev/thunderbeast/AtomicGameEngine/Bin/Editor.d.ts" />

/// <reference path="./AtomicWork.d.ts" />

import ui = require("./ui/ui");

print(ui.ui);

var env = ToolCore.getToolEnvironment();

var system = ToolCore.getToolSystem();

system.loadProject("/Users/josh/Dev/atomic/AtomicExamples/NewSpaceGame");




