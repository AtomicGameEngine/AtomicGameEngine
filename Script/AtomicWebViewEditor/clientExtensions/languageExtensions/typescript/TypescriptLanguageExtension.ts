//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//
// Based upon the TypeScript language services example at https://github.com/Microsoft/TypeScript/wiki/Using-the-Compiler-API#incremental-build-support-using-the-language-services

import * as ts from "../../../modules/typescript";
import * as WorkerProcessCommands from "./workerprocess/workerProcessCommands";

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
            const responseCallback = function(e: WorkerProcessCommands.WorkerProcessMessage<any>) {
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
            this.editor = <AceAjax.Editor>ev.editor;
            this.editor.session.setMode("ace/mode/typescript");
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
            this.worker.port.postMessage({ command: WorkerProcessCommands.Connect, sender: "Typescript Language Extension", filename: ev.filename });
        }
    }

    /**
     * Handler for any messages initiated by the worker process
     * @param  {any} e
     * @return {[type]}
     */
    handleWorkerMessage(e: WorkerProcessCommands.WorkerProcessMessage<any>) {
        switch (e.data.command) {
            case WorkerProcessCommands.Message:
                console.log(e.data.message);
                break;
            case WorkerProcessCommands.Alert:
                alert(e.data.message);
                break;
        }
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
            this.worker.port.postMessage({ command: WorkerProcessCommands.Disconnect });
        });

        this.worker.port.start();
    }

    /**
     * Builds the word completer for the Ace Editor.  This will handle determining which items to display in the popup and in which order
     * @param  {string} filename the filename of the current file
     * @param  {TypescriptLanguageService} langService The language service that handles compilation, completion resolution, etc.
     * @param  {FileSystemInterface} fs the interface into the file system
     * @return {[type]} returns a completer
     */
    private buildWordCompleter(filename: string): {
        getDocTooltip?: (selected: WorkerProcessCommands.WordCompletion) => void,
        getCompletions: (editor, session, pos, prefix, callback) => void
    } {
        let extension = this;
        let wordCompleter = {
            getDocTooltip: function(selected: WorkerProcessCommands.WordCompletion) {
                const message: WorkerProcessCommands.GetDocTooltipMessageData = {
                    command: WorkerProcessCommands.GetDocTooltip,
                    filename: extension.filename,
                    completionItem: selected,
                    pos: selected.pos
                };

                // Since the doc tooltip built in function of Ace doesn't support async calls to retrieve the tootip,
                // we need to go ahead and call the worker and then force the display of the tooltip when we get
                // a result back
                extension.workerRequest(WorkerProcessCommands.DocTooltipResponse, message)
                    .then((e: WorkerProcessCommands.GetDocTooltipResponseMessageData) => {
                    extension.editor.completer.showDocTooltip(e);
                });
            },

            getCompletions: function(editor, session, pos, prefix, callback) {
                const message: WorkerProcessCommands.GetCompletionsMessageData = {
                    command: WorkerProcessCommands.GetCompletions,
                    filename: extension.filename,
                    pos: pos,
                    sourceText: editor.session.getValue(),
                    prefix: prefix
                };

                extension.workerRequest(WorkerProcessCommands.CompletionResponse, message)
                    .then((e: WorkerProcessCommands.GetCompletionsResponseMessageData) => {
                    callback(null, e.completions);
                });
            }
        };
        return wordCompleter;
    }

    /*** ResourceService implementation ****/

    /**
     * Called once a resource has been saved
     * @param  {Editor.EditorEvents.SaveResourceEvent} ev
     */
    // save(ev: Editor.EditorEvents.SaveResourceEvent) {
    //     if (this.isValidFiletype(ev.path)) {
    //         console.log(`${this.name}: received a save resource event for ${ev.path}`);
    //         this.worker.port.postMessage({
    //             command: WorkerProcessCommands.Save,
    //             path: ev.path
    //         });
    //     }
    // }

    /**
     * Handle the delete.  This should delete the corresponding javascript file
     * @param  {Editor.EditorEvents.DeleteResourceEvent} ev
     */
    // delete(ev: Editor.EditorEvents.DeleteResourceEvent) {
    //     if (this.isValidFiletype(ev.path)) {
    //         console.log(`${this.name}: received a delete resource event`);
    //
    //         // notify the typescript language service that the file has been deleted
    //         this.worker.port.postMessage({
    //             command: WorkerProcessCommands.Delete,
    //             path: ev.path
    //         });
    //     }
    // }

    /**
     * Handle the rename.  Should rename the corresponding .js file
     * @param  {Editor.EditorEvents.RenameResourceEvent} ev
     */
    // rename(ev: Editor.EditorEvents.RenameResourceEvent) {
    //     if (this.isValidFiletype(ev.path)) {
    //         console.log(`${this.name}: received a rename resource event`);
    //
    //         // notify the typescript language service that the file has been renamed
    //         this.worker.port.postMessage({
    //             command: WorkerProcessCommands.Rename,
    //             path: ev.path,
    //             newPath: ev.newPath
    //         });
    //     }
    // }
}
