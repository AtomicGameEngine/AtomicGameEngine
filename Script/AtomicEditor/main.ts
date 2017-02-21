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

/// <reference path="../TypeScript/Atomic.d.ts" />
/// <reference path="../TypeScript/ToolCore.d.ts" />
/// <reference path="../TypeScript/Editor.d.ts" />
/// <reference path="../TypeScript/AtomicWork.d.ts" />

// Bring in AtomicEventLoop so the editor has access to the timer apis.  This
// attaches the timer apis to the global object so they are available everywhere from this point.
import "AtomicEventLoop";
import Editor from "editor/Editor";

class Main {

    static Editor:Editor;

    static main() {
        Main.Editor = new Editor();
    }
}

Main.main();
