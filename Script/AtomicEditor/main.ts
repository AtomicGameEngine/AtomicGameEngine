/// <reference path="../TypeScript/Atomic.d.ts" />
/// <reference path="../TypeScript/ToolCore.d.ts" />
/// <reference path="../TypeScript/Editor.d.ts" />
/// <reference path="../TypeScript/AtomicWork.d.ts" />

import Editor = require("editor/Editor");

//Main is still evaling for now, need to have it done through requiring it
class Main {
    static Editor:Editor;

    static main() {
        Main.Editor = new Editor();
    }
}

Main.main();
