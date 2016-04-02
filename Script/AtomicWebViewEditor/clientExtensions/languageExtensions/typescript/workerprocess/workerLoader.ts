//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
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
