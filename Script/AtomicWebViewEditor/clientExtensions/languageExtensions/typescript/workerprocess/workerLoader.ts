//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

/**
 * Typescript Language Worker - Handles bringing down the source, providing completions, and compiling.
 * since this is a shared web worker, each editor tab will be sharing the same data.
 */
"use strict";

// Bring in the system js library so we can import the modules we need.  The path here is based upon the location
// of this SharedWorker in the directory tree
importScripts("../../../../../systemjs/system.js");
System.config({
    defaultJSExtensions: true,
    meta: {
        "../../../../modules/typescript.js": {
            format: "global",
            exports: "ts"
        }
    }
});

// hook into the first time a tab connects...need to do some set up and some async pulling down of required modules
function firstTimeConnect(e: any) {
    // Going low-level to use the systemjs import system.  import will return a promise that will resolve
    // to the module being required in.  Once this is loaded in and instantiated, everything downstream can just
    // use normal typescript style imports to bring in other modules.
    System.import("./TypescriptLanguageServiceWebWorker").then((TypescriptLanguageServiceWebWorker) => {
        const worker = new TypescriptLanguageServiceWebWorker.default();

        // remove the event listener for the first connect
        self.removeEventListener("connect", firstTimeConnect);

        // hook up the permanent event listener
        self.addEventListener("connect", worker.connect.bind(worker));

        // forward the connect call
        worker.connect(e);
    });
}
self.addEventListener("connect", firstTimeConnect);
