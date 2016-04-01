//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
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
