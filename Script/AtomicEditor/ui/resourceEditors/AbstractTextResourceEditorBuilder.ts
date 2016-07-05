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

import EditorEvents = require("../../editor/EditorEvents");
export abstract class AbstractTextResourceEditorBuilder implements Editor.Extensions.ResourceEditorBuilder {

    abstract canHandleResource(resourcePath: string): boolean;

    /**
     * Full path is the fully qualified path from the root of the filesystem.  In order to take advantage
     * of the resource caching system, let's trim it down to just the path inside the resources directory
     * including the Resources directory so that the casing is correct
     */
    private getNormalizedPath(path: string) {
        const RESOURCES_MARKER = "resources/";
        return path.substring(path.toLowerCase().indexOf(RESOURCES_MARKER));
    }

    /**
     * Returns the URL to load into the web view.  Override this to return a custom url
     * @return {string}
     */
    getEditorUrl(): string {
        return `atomic://${ToolCore.toolEnvironment.toolDataDir}CodeEditor/MonacoEditor.html`;
    }

    getEditor(resourceFrame: Atomic.UIWidget, resourcePath: string, tabContainer: Atomic.UITabContainer): Editor.ResourceEditor {
        const editor = new Editor.JSResourceEditor(resourcePath, tabContainer, this.getEditorUrl());

        // one time subscriptions waiting for the web view to finish loading.  This event
        // actually hits the editor instance before we can hook it, so listen to it on the
        // frame and then unhook it
        editor.subscribeToEvent(EditorEvents.WebViewLoadEnd, (data) => {
            editor.unsubscribeFromEvent(EditorEvents.WebViewLoadEnd);
            this.loadCode(<Editor.JSResourceEditor>editor, resourcePath);
        });

        // Cannot subscribe to WebMessage until the .ts side can return a Handler.Success() message
        // editor.subscribeToEvent(EditorEvents.WebMessage, (data) => {
        //     console.log("editor Got a web message: " + data.request);
        //     for (let x in data) {
        //         console.log(x + ":" + data[x]);
        //     }
        //     const request = JSON.parse(data.request);
        //     switch (request.message) {
        //         case "editorGetUserPrefs":
        //             this.getUserPrefs(editor);
        //             return true;
        //     }
        // });

        editor.subscribeToEvent(EditorEvents.DeleteResourceNotification, (data) => {
            const webClient = editor.webView.webClient;
            webClient.executeJavaScript(`HOST_resourceDeleted("${this.getNormalizedPath(data.path)}");`);
        });

        editor.subscribeToEvent(EditorEvents.UserPreferencesChangedNotification, (data: EditorEvents.UserPreferencesChangedEvent) => {
            const webClient = editor.webView.webClient;
            webClient.executeJavaScript(`HOST_preferencesChanged('${data.projectPreferences}','${data.applicationPreferences}');`);
        });

        return editor;
    }

    /**
     * Send the url of the code to the web view so that it can request it
     */
    loadCode(editor: Editor.JSResourceEditor, resourcePath: string) {
        const webClient = editor.webView.webClient;
        webClient.executeJavaScript(`HOST_loadCode("atomic://${this.getNormalizedPath(editor.fullPath)}");`);
    }
}
