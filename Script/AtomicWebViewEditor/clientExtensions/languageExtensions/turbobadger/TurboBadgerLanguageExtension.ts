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

/**
 * Resource extension that handles configuring the editor for Javascript
 */
export default class TurboBadgerLanguageExtension implements Editor.ClientExtensions.WebViewServiceEventListener {
    name: string = "ClientTurboBadgerLanguageExtension";
    description: string = "TurboBadger language services for the editor.";

    private serviceLocator: Editor.ClientExtensions.ClientServiceLocator;

    /**
    * Initialize the language service
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
        return path.toLowerCase().endsWith(".tb.txt") || path.toLowerCase().endsWith(".tb");
    }

    /**
     * Called when the editor needs to be configured for a particular file
     * @param  {Editor.EditorEvents.EditorFileEvent} ev
     */
    configureEditor(ev: Editor.EditorEvents.EditorFileEvent) {
        if (this.isValidFiletype(ev.filename)) {

            monaco.languages.register({ id: "turbobadger" });
            // TODO: set up syntax hilighter
            // monaco.languages.setMonarchTokensProvider("turbobadger", this.getTokensProvider());

            let editor = <monaco.editor.IStandaloneCodeEditor>ev.editor;
            monaco.editor.setModelLanguage(editor.getModel(), "turbobadger");
            editor.updateOptions({
                renderWhitespace: true,
                useTabStops: true,
            });
        }
    }

    /**
     * Called when code is first loaded into the editor
     * @param  {CodeLoadedEvent} ev
     */
    codeLoaded(ev: Editor.EditorEvents.CodeLoadedEvent) {
        if (this.isValidFiletype(ev.filename)) {
            let editor = <monaco.editor.IStandaloneCodeEditor>ev.editor;
            editor.getModel().updateOptions({
                insertSpaces: false
            });
        }
    }
}
