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

import {AbstractTextResourceEditorBuilder} from "./AbstractTextResourceEditorBuilder";
import * as EditorEvents from "../../editor/EditorEvents";

export default class DukDebuggerResourceEditorBuilder extends AbstractTextResourceEditorBuilder {

    private debuggerRunning = false;

    constructor() {
        super();
        this.subscribeToEvent(Atomic.GoJSDebuggerEvent(() => {
            console.log("======== got a debugger event");
            this.debuggerRunning = true;
        }));

        this.subscribeToEvent(Atomic.ScriptEvent(EditorEvents.IPCPlayerExitRequestEventType, () => {
            this.debuggerRunning = false;
        }));
    }

    canHandleResource(resourcePath: string): boolean {
        if (resourcePath == "Duktape Debugger") {
            this.debuggerRunning = true;
            return true;
        }

/*
        if (this.debuggerRunning) {
            const ext = Atomic.getExtension(resourcePath).toLowerCase();
            return ext == ".js" || ext == ".ts";
        }
*/
        return false;
    }

    /**
     * Returns the URL to load into the web view.  Override this to return a custom url
     * @return {string}
     */
    getEditorUrl(): string {
        return `atomic://${ToolCore.toolEnvironment.toolDataDir}CodeEditor/duk_debug.html`;
    }
}
