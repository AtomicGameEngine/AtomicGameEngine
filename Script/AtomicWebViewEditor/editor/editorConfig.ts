//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import editor from "./editor";
import serviceLocator from "../clientExtensions/ServiceLocator";
import HostInterop from "../interop";

const CONFIGURE_EDITOR_EVENT = "ConfigureEditor";
const CODE_LOADED_NOTIFICATION = "CodeLoadedNotification";
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
    serviceLocator.sendEvent(CONFIGURE_EDITOR_EVENT, {
        fileExt: fileExt,
        filename: filename,
        editor: editor
    });
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

    serviceLocator.sendEvent(CODE_LOADED_NOTIFICATION, {
        code: code,
        filename: filename,
        fileExt: fileExt,
        editor: editor
    });

}
