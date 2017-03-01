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

// This is the interop file, exposing functions that can be called by the host game engine
import "./utils/WindowExt";
import * as editorCommands from "./editor/editorCommands";

/**
 * Port to attach Chrome Dev Tools to
 * @type {Number}
 */
const DEBUG_PORT = 3335;

/**
 * Display "Attach dev tools now" alert on startup if this is set to true
 * @type {Boolean}
 */
const DEBUG_ALERT = false;

export default class HostInteropType {

    private static _inst: HostInteropType = null;
    private fileName: string = null;
    private fileExt: string = null;

    static getInstance(): HostInteropType {
        if (HostInteropType._inst == null) {
            HostInteropType._inst = new HostInteropType();
        }
        return HostInteropType._inst;
    }

    static EDITOR_SAVE_CODE = "editorSaveCode";
    static EDITOR_SAVE_FILE = "editorSaveFile";
    static EDITOR_LOAD_COMPLETE = "editorLoadComplete";
    static EDITOR_CHANGE = "editorChange";
    static EDITOR_GET_USER_PREFS = "editorGetUserPrefs";

    private setCodeLoaded;
    private editorReady = new Promise((resolve, reject) => {
        this.setCodeLoaded = resolve;
    });

    /**
     * Called from the host to notify the client what file to load
     * @param  {string} codeUrl
     */
    loadCode(codeUrl: string) {
        const fileExt = codeUrl.indexOf(".") != -1 ? codeUrl.split(".").pop() : "";
        const filename = codeUrl.replace("atomic://", "");

        // Keep track of our filename
        this.fileName = filename;
        this.fileExt = fileExt;

        // go ahead and set the theme prior to pulling the file across
        editorCommands.configure(fileExt, filename);

        // get the code
        this.getResource(codeUrl).then((src: string) => {
            editorCommands.loadCodeIntoEditor(src, filename, fileExt);
            return atomicHostEvent(HostInteropType.EDITOR_GET_USER_PREFS);
        }).then(() => {
            this.setCodeLoaded();
        }).catch((e: Editor.ClientExtensions.AtomicErrorMessage) => {
            console.log("Error loading code: " + e.error_message);
        });
    }

    /**
     * Save the contents of the editor
     * @return {Promise}
     */
    saveCode(): Promise<any> {
        let source = editorCommands.getSourceText();
        return atomicHostEvent(HostInteropType.EDITOR_SAVE_CODE, {
            payload: source
        }).then(() => {
            editorCommands.codeSaved(this.fileName, this.fileExt, source);
        });
    }

    /**
     * Save the contents of a file as filename
     * @param  {string} filename
     * @param  {string} fileContents
     * @return {Promise}
     */
    saveFile(filename: string, fileContents: string): Promise<any> {
        return atomicHostEvent(HostInteropType.EDITOR_SAVE_FILE, {
            filename: filename,
            payload: fileContents
        });
    }

    /**
     * Call this function when the client is fully loaded up.  This will notify the host that
     * it can start loading code
     */
    editorLoaded() {
        if (DEBUG_ALERT) {
            alert(`Attach chrome dev tools to this instance by navigating to http://localhost:${DEBUG_PORT}`);
        }
        editorCommands.editorLoaded();
        atomicHostEvent(HostInteropType.EDITOR_LOAD_COMPLETE);

        atomicHostRequest<string>("foo").then((d) => alert(d));
    }

    /**
     * Queries the host for a particular resource and returns it in a promise
     * @param  {string} codeUrl
     * @return {Promise}
     */
    getResource(codeUrl: string): Promise<{}> {
        return new Promise(function(resolve, reject) {
            const xmlHttp = new XMLHttpRequest();
            xmlHttp.onreadystatechange = () => {
                if (xmlHttp.readyState == 4 && xmlHttp.status == 200) {
                    resolve(xmlHttp.responseText);
                }
            };
            xmlHttp.open("GET", codeUrl, true); // true for asynchronous
            xmlHttp.send(null);
        });
    }

    /**
     * Returns a file resource from the resources directory
     * @param  {string} filename name and path of file under the project directory or a fully qualified file name
     * @return {Promise}
     */
    getFileResource(filename: string): Promise<{}> {
        return this.getResource(`atomic://${filename}`);
    }

    /**
     * Notify the host that the contents of the editor has changed
     */
    notifyEditorChange() {
        atomicHostEvent(HostInteropType.EDITOR_CHANGE).catch((e: Editor.ClientExtensions.AtomicErrorMessage) => {
            console.log("Error on change: " + e.error_message);
        });
    }

    /**
     * Notify that a resource has been renamed
     * @param  {string} path
     * @param  {string} newPath
     */
    resourceRenamed(path: string, newPath: string) {
        this.fileName = newPath;
        editorCommands.resourceRenamed(path, newPath);
    }

    /**
     * Notify that a resource has been deleted
     * @param  {string} path
     */
    resourceDeleted(path: string) {
        editorCommands.resourceDeleted(path);
    }

    /**
     * Host is notifying client that there are preferences to load and passing us JSON objects containing the prefs
     * of the prefs.
     */
    preferencesChanged(prefs: Editor.ClientExtensions.PreferencesChangedEventData) {
        editorCommands.preferencesChanged(prefs);
    }

    /**
     * This adds a global routine to the window object so that it can be called from the host
     * @param  {string} routineName
     * @param  {(} callback
     */
    addCustomHostRoutine(routineName: string, callback: (...args) => void) {
        window[routineName] = callback;
    }

    /**
     * Sets the editor instance
     * @param  {any} editor
     */
    setEditor(editor: any) {
        editorCommands.setEditor(editor);
    }

    /**
     * Sets the editor used for debugging
     * @param  {any} editor
     * @return {[type]}
     */
    setDebuggerEditor(editor: any) {
        editorCommands.setDebuggerEditor(editor);
    }

    /**
     * Called when a shortcut should be invoked, coming from the host editor
     * @param {Editor.EditorShortcutType} shortcut shortcut to be executed
     */
    invokeShortcut(shortcut: Editor.EditorShortcutType) {
       editorCommands.invokeShortcut(shortcut);
    }

    /**
     * Format the code inside the editor
     */
    formatCode() {
        editorCommands.formatCode();
    }

    /**
     * Jump to the provided line number
     */
    gotoLineNumber(lineNumber:number) {
        this.editorReady.then(() => {
            editorCommands.gotoLineNumber(lineNumber);
        });
    }

    /**
     * Jump to the provided position
     */
    gotoTokenPos(tokenPos:number) {
        editorCommands.gotoTokenPos(tokenPos);
    }
}
