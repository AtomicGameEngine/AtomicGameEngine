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

import * as internalEditor from "./editor";
import serviceLocator from "../clientExtensions/ServiceLocator";
import HostInterop from "../interop";
import ClientExtensionEventNames from "../clientExtensions/ClientExtensionEventNames";


/**
 * Set the editor theme and configuration based upon the file extension
 * @param  {string} fileExt
 */
export function configure(fileExt: string, filename: string) {

    let monacoEditor = <monaco.editor.IStandaloneCodeEditor>internalEditor.getInternalEditor();
    monacoEditor.updateOptions({
        theme: serviceLocator.clientServices.getApplicationPreference("codeEditor", "theme", "vs-dark"),
        renderWhitespace: serviceLocator.clientServices.getApplicationPreference("codeEditor", "showInvisibles", false),
        mouseWheelScrollSensitivity: 2,
        fontSize: serviceLocator.clientServices.getApplicationPreference("codeEditor", "fontSize", 12),
        fontFamily: serviceLocator.clientServices.getApplicationPreference("codeEditor", "fontFamily", "")
    });

    // give the language extensions the opportunity to configure the editor based upon the file type
    serviceLocator.sendEvent(ClientExtensionEventNames.ConfigureEditorEvent, {
        fileExt: fileExt,
        filename: filename,
        editor: monacoEditor
    });
}

/**
 * Returns the text in the editor instance
 * @return {string}
 */
export function getSourceText(): string {
    return internalEditor.getInternalEditor().getModel().getValue();
}

/**
 * Loads a file of code into the editor and wires up the change events
 * @param  {string} code
 * @param {string} filename
 * @param  {string} fileExt
 */
export function loadCodeIntoEditor(code: string, filename: string, fileExt: string) {

    let monacoEditor = internalEditor.getInternalEditor();
    let model = monaco.editor.createModel(code, null, monaco.Uri.file(filename));

    model.updateOptions({
        insertSpaces: serviceLocator.clientServices.getApplicationPreference("codeEditor", "useSoftTabs", true),
        tabSize: serviceLocator.clientServices.getApplicationPreference("codeEditor", "tabSize", 4)
    });

    monacoEditor.setModel(model);
    serviceLocator.sendEvent(ClientExtensionEventNames.CodeLoadedEvent, {
        code: code,
        filename: filename,
        fileExt: fileExt,
        editor: monacoEditor
    });

    monacoEditor.onDidChangeModelContent((listener) => {
        HostInterop.getInstance().notifyEditorChange();
    });
}

/**
 * Called when a resource is getting renamed
 * @param  {string} path
 * @param  {string} newPath
 */
export function resourceRenamed(path: string, newPath: string) {
    let data: Editor.EditorEvents.RenameResourceEvent = {
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
    let data: Editor.EditorEvents.DeleteResourceEvent = {
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
    let data: Editor.EditorEvents.CodeSavedEvent = {
        filename: path,
        fileExt: fileExt,
        editor: internalEditor.getInternalEditor(),
        code: contents
    };
    serviceLocator.sendEvent(ClientExtensionEventNames.CodeSavedEvent, data);
}

/**
 * Called when the editor has finished it's initial load
 */
export function editorLoaded() {
    // let's grab the prefs and seed the service locator
    serviceLocator.clientServices.setPreferences(JSON.parse(window.HOST_Preferences.ProjectPreferences), JSON.parse(window.HOST_Preferences.ApplicationPreferences));
}

/**
 * Called when new preferences are available (or initially with current prefs)
 */
export function preferencesChanged(prefs: Editor.ClientExtensions.PreferencesChangedEventData) {
    serviceLocator.clientServices.setPreferences(prefs.projectPreferences, prefs.applicationPreferences);
    serviceLocator.sendEvent(ClientExtensionEventNames.PreferencesChangedEvent, prefs);
}

export function setEditor(editor: any) {
    internalEditor.setInternalEditor(editor);
}
