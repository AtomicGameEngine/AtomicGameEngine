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

import * as ts from "../../../modules/typescript";
import * as WorkerProcessTypes from "./workerprocess/workerProcessTypes";
import ClientExtensionEventNames from "../../ClientExtensionEventNames";

/**
 * Resource extension that handles compiling or transpling typescript on file save.
 */
export default class TypescriptLanguageExtension implements Editor.ClientExtensions.WebViewService {
    name: string = "ClientTypescriptLanguageExtension";
    description: string = "This extension handles typescript language features such as completion, compilation, etc.";

    /**
     * current filename
     * @type {string}
     */
    private filename: string;

    private serviceLocator: Editor.ClientExtensions.ClientServiceLocator;

    private worker: SharedWorker.SharedWorker;

    private editor;

    /**
     * Perform a full compile on save, or just transpile the current file
     * @type {boolean}
     */
    fullCompile: boolean = true;

    /**
    * Inject this language service into the registry
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
        return ext == "ts";
    }

    /**
     * Utility function that handles sending a request to the worker process and then when
     * a response is received pass it back to the caller.  Since this is all handled async,
     * it will be facilitated by passing back a promise
     * @param  {string} responseChannel The unique string name of the response message channel
     * @param  {any} message
     * @return {PromiseLike}
     */
    private workerRequest(responseChannel: string, message: any): PromiseLike<{}> {
        let worker = this.worker;

        return new Promise((resolve, reject) => {
            const responseCallback = function(e: WorkerProcessTypes.WorkerProcessMessage<any>) {
                if (e.data.command == responseChannel) {
                    worker.port.removeEventListener("message", responseCallback);
                    resolve(e.data);
                }
            };
            this.worker.port.addEventListener("message", responseCallback);
            this.worker.port.postMessage(message);
        });
    }

    /**
     * Called when the editor needs to be configured for a particular file
     * @param  {Editor.EditorEvents.EditorFileEvent} ev
     */
    configureEditor(ev: Editor.EditorEvents.EditorFileEvent) {
        if (this.isValidFiletype(ev.filename)) {
            let editor = <AceAjax.Editor>ev.editor;
            editor.session.setMode("ace/mode/typescript");

            editor.setOptions({
                enableBasicAutocompletion: true,
                enableLiveAutocompletion: true
            });

            this.editor = editor; // cache this so that we can reference it later
        }
    }

    /**
     * Called when code is first loaded into the editor
     * @param  {CodeLoadedEvent} ev
     * @return {[type]}
     */
    codeLoaded(ev: Editor.EditorEvents.CodeLoadedEvent) {
        if (this.isValidFiletype(ev.filename)) {
            this.filename = ev.filename;

            let editor = ev.editor;

            // we only want the typescript completer, otherwise we get a LOT of noise
            editor.completers = [this.buildWordCompleter(ev.filename)];

            // Build our worker
            this.buildWorker();

            // post a message to the shared web worker
            this.worker.port.postMessage({ command: WorkerProcessTypes.Connect, sender: "Typescript Language Extension", filename: ev.filename });
        }
    }

    /**
     * Handler for any messages initiated by the worker process
     * @param  {WorkerProcessTypes.WorkerProcessMessage} e
     */
    handleWorkerMessage(e: WorkerProcessTypes.WorkerProcessMessage<any>) {
        switch (e.data.command) {
            case WorkerProcessTypes.Message:
                console.log(e.data.message);
                break;
            case WorkerProcessTypes.Alert:
                alert(e.data.message);
                break;
            case WorkerProcessTypes.AnnotationsUpdated:
                this.setAnnotations(e.data);
                break;
        }
    }

    /**
     * Set annotations based upon issues reported by the typescript language service
     * @param  {WorkerProcessTypes.GetAnnotationsResponseMessageData} event
     */
    setAnnotations(event: WorkerProcessTypes.GetAnnotationsResponseMessageData) {
        // grab the existing annotations and filter out any TS annotations
        let oldAnnotations = this.editor.session.getAnnotations().filter(ann => !ann.tsAnnotation);
        this.editor.session.clearAnnotations();

        // Mark these annotations as special
        event.annotations.forEach(ann => ann.tsAnnotation = true);
        this.editor.session.setAnnotations(oldAnnotations.concat(event.annotations));
    }

    /**
     * Build/Attach to the shared web worker
     */
    buildWorker() {

        this.worker = new SharedWorker("./source/editorCore/clientExtensions/languageExtensions/typescript/workerprocess/workerLoader.js");

        // hook up the event listener
        this.worker.port.addEventListener("message", this.handleWorkerMessage.bind(this), false);

        // Tell the SharedWorker we're closing
        addEventListener("beforeunload", () => {
            this.worker.port.postMessage({ command: WorkerProcessTypes.Disconnect });
        });

        this.worker.port.start();
    }

    /**
     * Builds the word completer for the Ace Editor.  This will handle determining which items to display in the popup and in which order
     * @param  {string} filename the filename of the current file
     * @return {[type]} returns a completer
     */
    private buildWordCompleter(filename: string): {
        getDocTooltip?: (selected: WorkerProcessTypes.WordCompletion) => void,
        getCompletions: (editor, session, pos, prefix, callback) => void
    } {
        let extension = this;
        let wordCompleter = {
            getDocTooltip: function(selected: WorkerProcessTypes.WordCompletion) {
                const message: WorkerProcessTypes.GetDocTooltipMessageData = {
                    command: WorkerProcessTypes.GetDocTooltip,
                    filename: extension.filename,
                    completionItem: selected,
                    pos: selected.pos
                };

                // Since the doc tooltip built in function of Ace doesn't support async calls to retrieve the tootip,
                // we need to go ahead and call the worker and then force the display of the tooltip when we get
                // a result back
                extension.workerRequest(WorkerProcessTypes.DocTooltipResponse, message)
                    .then((e: WorkerProcessTypes.GetDocTooltipResponseMessageData) => {
                    extension.editor.completer.showDocTooltip(e);
                });
            },

            getCompletions: function(editor, session, pos, prefix, callback) {
                const message: WorkerProcessTypes.GetCompletionsMessageData = {
                    command: WorkerProcessTypes.GetCompletions,
                    filename: extension.filename,
                    pos: pos,
                    sourceText: editor.session.getValue(),
                    prefix: prefix
                };

                extension.workerRequest(WorkerProcessTypes.CompletionResponse, message)
                    .then((e: WorkerProcessTypes.GetCompletionsResponseMessageData) => {
                    callback(null, e.completions);
                });
            }
        };
        return wordCompleter;
    }

    /**
     * Called once a resource has been saved
     * @param  {Editor.EditorEvents.SaveResourceEvent} ev
     */
    save(ev: Editor.EditorEvents.CodeSavedEvent) {
        if (this.isValidFiletype(ev.filename)) {
            console.log(`${this.name}: received a save resource event for ${ev.filename}`);

            const message: WorkerProcessTypes.SaveMessageData = {
                command: ClientExtensionEventNames.CodeSavedEvent,
                filename: ev.filename,
                fileExt: ev.fileExt,
                code: ev.code,
                editor: null // cannot send editor across the boundary
            };

            this.worker.port.postMessage(message);
        }
    }

    /**
     * Handle the delete.  This should delete the corresponding javascript file
     * @param  {Editor.EditorEvents.DeleteResourceEvent} ev
     */
    delete(ev: Editor.EditorEvents.DeleteResourceEvent) {
        if (this.isValidFiletype(ev.path)) {
            console.log(`${this.name}: received a delete resource event for ${ev.path}`);

            // notify the typescript language service that the file has been deleted
            const message: WorkerProcessTypes.DeleteMessageData = {
                command: ClientExtensionEventNames.ResourceDeletedEvent,
                path: ev.path
            };

            this.worker.port.postMessage(message);
        }
    }

    /**
     * Handle the rename.  Should rename the corresponding .js file
     * @param  {Editor.EditorEvents.RenameResourceEvent} ev
     */
    rename(ev: Editor.EditorEvents.RenameResourceEvent) {
        if (this.isValidFiletype(ev.path)) {
            console.log(`${this.name}: received a rename resource event for ${ev.path} -> ${ev.newPath}`);

            // notify the typescript language service that the file has been renamed
            const message: WorkerProcessTypes.RenameMessageData = {
                command: ClientExtensionEventNames.ResourceRenamedEvent,
                path: ev.path,
                newPath: ev.newPath
            };

            this.worker.port.postMessage(message);
        }
    }

    /**
     * Handle when the project is unloaded so that resources can be freed
     */
    projectUnloaded() {
        if (this.worker) {

            console.log(`${this.name}: received a project unloaded event`);

            const message: WorkerProcessTypes.WorkerProcessMessageData = {
                command: ClientExtensionEventNames.ProjectUnloadedEvent
            };

            this.worker.port.postMessage(message);
        }
    }
}
