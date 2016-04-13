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

import editor from "./editor";
import serviceLocator from "../clientExtensions/ServiceLocator";
import HostInterop from "../interop";
import ClientExtensionEventNames from "../clientExtensions/ClientExtensionEventNames";


/**
 * Set the editor theme and configuration based upon the file extension
 * @param  {string} fileExt
 */
export function configure(fileExt: string, filename: string) {

    // set a default theme
    editor.setTheme("ace/theme/monokai");

    // set a default mode
    editor.session.setMode("ace/mode/javascript");

    // give the language extensions the opportunity to configure the editor based upon the file type
    serviceLocator.sendEvent(ClientExtensionEventNames.ConfigureEditorEvent, {
        fileExt: fileExt,
        filename: filename,
        editor: editor
    });
}

/**
 * Returns the text in the editor instance
 * @return {string}
 */
export function getSourceText() : string {
    return editor.session.getValue();
}

/**
 * Loads a file of code into the editor and wires up the change events
 * @param  {string} code
 * @param {string} filename
 * @param  {string} fileExt
 */
export function loadCodeIntoEditor(code: string, filename: string, fileExt: string) {
    editor.session.setValue(code);
    editor.gotoLine(0);

    editor.getSession().on("change", function(e) {
        HostInterop.getInstance().notifyEditorChange();
    });

    serviceLocator.sendEvent(ClientExtensionEventNames.CodeLoadedEvent, {
        code: code,
        filename: filename,
        fileExt: fileExt,
        editor: editor
    });

}

/**
 * Called when the project is getting unloaded
 */
export function projectUnloaded() {
    serviceLocator.sendEvent(ClientExtensionEventNames.ProjectUnloadedEvent, null);
}

/**
 * Called when a resource is getting renamed
 * @param  {string} path
 * @param  {string} newPath
 */
export function resourceRenamed(path: string, newPath: string) {
    let data:Editor.EditorEvents.RenameResourceEvent = {
        path: path,
        newPath: newPath
    };
    serviceLocator.sendEvent(ClientExtensionEventNames.ResourceRenamedEvent, data);
}

/**
 * Called when a resource is getting deleted
 * @param  {string} path
 */
export function resourceDeleted(path: string) {
    let data:Editor.EditorEvents.DeleteResourceEvent = {
        path: path
    };
    serviceLocator.sendEvent(ClientExtensionEventNames.ResourceDeletedEvent, data);
}

/**
 * Called when a resource is saved
 * @param  {string} path
 * @param {string} fileExt
 * @param {string} contents
 */
export function codeSaved(path: string, fileExt: string, contents: string) {
    let data:Editor.EditorEvents.CodeSavedEvent = {
        filename: path,
        fileExt: fileExt,
        editor: editor,
        code: contents
    };
    serviceLocator.sendEvent(ClientExtensionEventNames.CodeSavedEvent, data);
}
