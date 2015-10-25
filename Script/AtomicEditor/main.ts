//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

/// <reference path="../TypeScript/Atomic.d.ts" />
/// <reference path="../TypeScript/AtomicNET.d.ts" />
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
