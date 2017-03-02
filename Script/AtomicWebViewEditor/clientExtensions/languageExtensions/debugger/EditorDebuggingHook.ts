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
// Based upon the TypeScript language services example at https://github.com/Microsoft/TypeScript/wiki/Using-the-Compiler-API#incremental-build-support-using-the-language-services

import HostInteropType from "../../../interop";
import ClientExtensionEventNames from "../../ClientExtensionEventNames";
import BreakpointDecoratorManager from "../../../debugger/BreakpointDecoratorManager";
import * as debuggerProxy from "../../../debugger/HostDebuggerExtensionProxy";

/**
 * Resource extension that handles compiling or transpling typescript on file save.
 */
export default class EditorDebuggingHook implements Editor.ClientExtensions.WebViewServiceEventListener {
    name = "EditorDebuggingHook";
    description = "This extension adds debugger functionality to the current editor";

    /**
     * current filename
     * @type {string}
     */
    filename: string;

    /**
     * Is this instance of the extension active?  Only true if the current editor
     * is a typescript file.
     * @type {Boolean}
     */
    private active = false;

    private serviceLocator: Editor.ClientExtensions.ClientServiceLocator;

    private editor: monaco.editor.IStandaloneCodeEditor;

    private breakpointDecorator: BreakpointDecoratorManager;

    /**
    * Inject this language service into the registry
     * @param  {Editor.ClientExtensions.ClientServiceLocator} serviceLocator
     */
    initialize(serviceLocator: Editor.ClientExtensions.ClientServiceLocator) {
        // initialize the language service
        this.serviceLocator = serviceLocator;
        serviceLocator.clientServices.register(this);
    }

    /**
     * Determines if the file name/path provided is something we care about
     * @param  {string} path
     * @return {boolean}
     */
    private isValidFiletype(path: string): boolean {

        // For now, only allow JS files
        let ext = path.split(".").pop();
        return ext == "js";
    }

    /**
     * Grab the list of breakpoints from the host
     */
    async retrieveBreakpoints() {
        let s = await debuggerProxy.getBreakpoints();

        // If the filename starts with "Resources" then trim it off since the module
        // name won't have that, but the editor uses it
        s.forEach(b => {
            if (b.fileName == this.filename) {
                this.breakpointDecorator.addBreakpointDecoration(b.fileName, b.lineNumber);
            }
        });
    }

    registerDebuggerFunctions() {
        // Register the callback functions
        const interop = HostInteropType.getInstance();
        interop.addCustomHostRoutine(
            debuggerProxy.debuggerHostKeys.toggleBreakpoint,
            (filename, linenumber) => {
                if (filename == this.filename || "Resources/" + filename == this.filename) {
                    this.breakpointDecorator.toggleBreakpoint(this.filename, linenumber);
                }
            });

        interop.addCustomHostRoutine(
            debuggerProxy.debuggerHostKeys.addBreakpoint,
            (filename, linenumber) => {
                if (filename == this.filename || "Resources/" + filename == this.filename) {
                    this.breakpointDecorator.addBreakpointDecoration(this.filename, linenumber);
                }
            });

        interop.addCustomHostRoutine(
            debuggerProxy.debuggerHostKeys.removeBreakpoint,
            (filename, linenumber) => {
                if (filename == this.filename || "Resources/" + filename == this.filename) {
                    this.breakpointDecorator.removeBreakpointDecoration(this.filename, linenumber);
                }
            });

        debuggerProxy.registerDebuggerListener(this.filename);
    }

    /**
     * Called when the editor needs to be configured for a particular file
     * @param  {Editor.ClientExtensions.EditorFileEvent} ev
     */
    configureEditor(ev: Editor.ClientExtensions.EditorFileEvent) {
        if (this.isValidFiletype(ev.filename)) {
            let editor = ev.editor as monaco.editor.IStandaloneCodeEditor;
            this.editor = editor; // cache this so that we can reference it later
            this.active = true;
            this.filename = ev.filename;


            // for now, we just want to set breakpoints in JS files
            editor.updateOptions({
                glyphMargin: true
            });

            this.breakpointDecorator = new BreakpointDecoratorManager(editor);
            this.breakpointDecorator.setCurrentFileName(ev.filename);
            this.registerDebuggerFunctions();

            this.retrieveBreakpoints();

            let marginTimeout = 0;
            editor.onMouseMove((e) => {
                var targetZone = e.target.toString();
                if (targetZone.indexOf("GUTTER_GLYPH_MARGIN") != -1) {
                    clearTimeout(marginTimeout);
                    var line = e.target.position.lineNumber;
                    this.breakpointDecorator.updateMarginHover(line);
                    marginTimeout = setTimeout(() => this.breakpointDecorator.removeMarginHover(), 500);
                } else {
                    clearTimeout(marginTimeout);
                    this.breakpointDecorator.removeMarginHover();
                }
            });

            editor.onMouseDown((e) => {
                var targetZone = e.target.toString();
                if (targetZone.indexOf("GUTTER_GLYPH_MARGIN") != -1) {
                    var line = e.target.position.lineNumber;
                    this.breakpointDecorator.toggleBreakpoint(ev.filename, line);
                    debuggerProxy.toggleBreakpoint(ev.filename, line);
                }
            });
        }
    }
}
