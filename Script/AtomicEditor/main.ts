/// <reference path="../TypeScript/Atomic.d.ts" />
/// <reference path="../TypeScript/ToolCore.d.ts" />
/// <reference path="../TypeScript/Editor.d.ts" />
/// <reference path="../TypeScript/AtomicWork.d.ts" />

import Editor = require("editor/Editor");

class Main {

    static Editor:Editor;

    static main() {
        Main.Editor = new Editor();
    }
}

Main.main();
