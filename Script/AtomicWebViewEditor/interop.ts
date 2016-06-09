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

/**
 * Promise version of atomic query
 * @param  {string} message the query to use to pass to atomicQuery.  If there is no payload, this will be passed directly, otherwise it will be passed in a data object
 * @param  {any} payload optional data to send
 * @return {Promise}
 */
function atomicQueryPromise(message: any): Promise<{}> {
    return new Promise(function(resolve, reject) {
        let queryMessage = message;

        // if message is coming in as an object then let's stringify it
        if (typeof (message) != "string") {
            queryMessage = JSON.stringify(message);
        }

        window.atomicQuery({
            request: queryMessage,
            persistent: false,
            onSuccess: resolve,
            onFailure: (error_code, error_message) => reject({ error_code: error_code, error_message: error_message })
        });
    });
}

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
            atomicQueryPromise({
                message: HostInteropType.EDITOR_GET_USER_PREFS
            });
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
        return atomicQueryPromise({
            message: HostInteropType.EDITOR_SAVE_CODE,
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
        const fileExt = filename.indexOf(".") != -1 ? filename.split(".").pop() : "";
        return atomicQueryPromise({
            message: HostInteropType.EDITOR_SAVE_FILE,
            filename: filename,
            payload: fileContents
        }).then(() => {
            editorCommands.codeSaved(filename, fileExt, fileContents);
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
        atomicQueryPromise(HostInteropType.EDITOR_LOAD_COMPLETE);
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
        atomicQueryPromise(HostInteropType.EDITOR_CHANGE).catch((e: Editor.ClientExtensions.AtomicErrorMessage) => {
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
     * Host is notifying client that there are preferences to load and passing us the path
     * of the prefs.
     * @param  {string} prefUrl
     */
    loadPreferences(prefUrl: string) {
        // load prefs
        this.getResource(prefUrl).then((prefsJson: string) => {
            let prefs = JSON.parse(prefsJson);
            editorCommands.loadPreferences(prefs);
        }).catch((e: Editor.ClientExtensions.AtomicErrorMessage) => {
            console.log("Error loading preferences: " + e.error_message);
        });
    }
}
