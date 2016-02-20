//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine

/**
 * Resource extension that handles configuring the editor for Javascript
 */
export default class JavascriptLanguageExtension implements Editor.ClientExtensions.WebViewService {
    name: string = "ClientJavascriptLanguageExtension";
    description: string = "Javascript language services for the editor.";

    private serviceLocator: Editor.ClientExtensions.ClientServiceLocator;

    /**
    * Initialize the language service
     * @param  {Editor.ClientExtensions.ClientServiceLocator} serviceLocator
     */
    initialize(serviceLocator: Editor.ClientExtensions.ClientServiceLocator) {
        // initialize the language service
        this.serviceLocator = serviceLocator;
    }

    /**
     * Determines if the file name/path provided is something we care about
     * @param  {string} path
     * @return {boolean}
     */
    private isValidFiletype(path: string): boolean {
        let ext = path.split(".").pop();
        return ext == "js";
    }

    /**
     * Called when the editor needs to be configured for a particular file
     * @param  {Editor.EditorEvents.EditorFileEvent} ev
     */
    configureEditor(ev: Editor.EditorEvents.EditorFileEvent) {
        if (this.isValidFiletype(ev.filename)) {
            let editor = <AceAjax.Editor>ev.editor;
            editor.session.setMode("ace/mode/javascript");
        }
    }
}
