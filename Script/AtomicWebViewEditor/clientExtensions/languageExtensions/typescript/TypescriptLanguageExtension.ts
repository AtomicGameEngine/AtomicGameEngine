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
import * as tsLanguageSupport from "./tsLanguageSupport";

/**
 * Resource extension that handles compiling or transpling typescript on file save.
 */
export default class TypescriptLanguageExtension implements Editor.ClientExtensions.WebViewServiceEventListener {
    name: string = "ClientTypescriptLanguageExtension";
    description: string = "This extension handles typescript language features such as completion, compilation, etc.";

    /**
     * current filename
     * @type {string}
     */
    filename: string;

    /**
     * Is this instance of the extension active?  Only true if the current editor
     * is a typescript file.
     * @type {Boolean}
     */
    private active = false;

    private serviceLocator: Editor.ClientExtensions.ClientServiceLocator;

    private worker: SharedWorker.SharedWorker;

    private editor: monaco.editor.IStandaloneCodeEditor;

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
        serviceLocator.clientServices.register(this);
    }

    /**
     * Determines if the file name/path provided is something we care about
     * @param  {string} path
     * @return {boolean}
     */
    private isValidFiletype(path: string): boolean {
        let ext = path.split(".").pop();
        return ext == "ts" || ext == "js";
    }

    /**
     * Returns true if this is a javascript file
     * @param  {string} path
     * @return {boolean}
     */
    private isJsFile(path: string): boolean {
        let ext = path.split(".").pop();
        return ext == "js";
    }

    /**
     * Checks to see if this is a transpiled Javascript file
     * @param  {string} path
     * @param  {[type]} tsconfig
     * @return {boolean}
     */
    private isTranspiledJsFile(path: string, tsconfig): boolean {
        if (this.isJsFile(path)) {
            const tsFilename = path.replace(/\.js$/, ".ts");
            return tsconfig.files.find(f => f.endsWith(tsFilename)) != null;
        }
        return false;
    }

    /**
     * Utility function that handles sending a request to the worker process and then when
     * a response is received pass it back to the caller.  Since this is all handled async,
     * it will be facilitated by passing back a promise
     * @param  {string} responseChannel The unique string name of the response message channel
     * @param  {any} message
     * @return {PromiseLike}
     */
    workerRequest(responseChannel: string, message: any): PromiseLike<{}> {
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
            this.active = true;

            this.overrideBuiltinServiceProviders();

            // Hook in the routine to allow the host to perform a full compile
            this.serviceLocator.clientServices.getHostInterop().addCustomHostRoutine("TypeScript_DoFullCompile", (jsonTsConfig: string) => {
                let tsConfig = JSON.parse(jsonTsConfig);
                this.doFullCompile(tsConfig);
            });

            this.filename = ev.filename;

            let editor = ev.editor as monaco.editor.IStandaloneCodeEditor;
            this.editor = editor; // cache this so that we can reference it later
            // Let's turn some things off in the editor.  These will be provided by the shared web worker
            if (this.isJsFile(ev.filename)) {
                monaco.languages.typescript.javascriptDefaults.setCompilerOptions({
                    noEmit: true,
                    noResolve: true,
                    allowNonTsExtensions: true,
                    noLib: true,
                    target: monaco.languages.typescript.ScriptTarget.ES5
                });

                monaco.languages.typescript.javascriptDefaults.setDiagnosticsOptions({
                    noSemanticValidation: true,
                    noSyntaxValidation: true
                });

                // Register editor feature providers
                monaco.languages.registerCompletionItemProvider("javascript", new CustomCompletionProvider(this));
                monaco.languages.registerHoverProvider("javascript", new CustomHoverProvider(this));
                monaco.languages.registerSignatureHelpProvider("javascript", new CustomSignatureProvider(this));
            } else {
                monaco.languages.register({
                    id: "atomic-ts"
                });
                monaco.languages.typescript.typescriptDefaults.setCompilerOptions({
                    noEmit: true,
                    noResolve: true,
                    noLib: true,
                    target: monaco.languages.typescript.ScriptTarget.ES5
                });

                monaco.languages.typescript.typescriptDefaults.setDiagnosticsOptions({
                    noSemanticValidation: true,
                    noSyntaxValidation: true
                });

                // Register editor feature providers
                monaco.languages.registerCompletionItemProvider("typescript", new CustomCompletionProvider(this));
                monaco.languages.registerSignatureHelpProvider("typescript", new CustomSignatureProvider(this));
                monaco.languages.registerHoverProvider("typescript", new CustomHoverProvider(this));
            }
        }
    }

    /**
     * Monkeypatch the TypeScript language provider so that our service providers get loaded
     * and not the built-in ones.  Ours use a shared web worker so that state can be shared
     * across tabs.
     */
    private overrideBuiltinServiceProviders() {
        monaco.languages.registerSignatureHelpProvider = ((original) => {
            return function(languageId: string, provider: monaco.languages.SignatureHelpProvider): monaco.IDisposable {
                if (provider["isOverride"]) {
                    return original(languageId, provider);
                }
            };
        })(monaco.languages.registerSignatureHelpProvider);

        monaco.languages.registerCompletionItemProvider = ((original) => {
            return function(languageId: string, provider: monaco.languages.CompletionItemProvider): monaco.IDisposable {
                if (provider["isOverride"]) {
                    return original(languageId, provider);
                }
            };
        })(monaco.languages.registerCompletionItemProvider);

        monaco.languages.registerHoverProvider = ((original) => {
            return function(languageId: string, provider: monaco.languages.HoverProvider): monaco.IDisposable {
                if (provider["isOverride"]) {
                    return original(languageId, provider);
                }
            };
        })(monaco.languages.registerHoverProvider);
    }

    /**
     * Called when code is first loaded into the editor
     * @param  {CodeLoadedEvent} ev
     */
    codeLoaded(ev: Editor.EditorEvents.CodeLoadedEvent) {
        if (this.isValidFiletype(ev.filename)) {

            // Build our worker
            this.buildWorker();

            // Initial load, so the TSConfig on the window object should be the most current
            let tsConfig = JSON.parse(window["TypeScriptLanguageExtension"]["tsConfig"]);

            let model = this.editor.getModel();
            let handle: number;
            model.onDidChangeContent(() => {
                clearTimeout(handle);
                handle = setTimeout(() => this.getAnnotations(), 500);
            });

            // post a message to the shared web worker
            this.worker.port.postMessage({
                command: WorkerProcessTypes.Connect,
                sender: "Typescript Language Extension",
                filename: ev.filename,
                tsConfig: tsConfig,
                code: ev.code
            });

            // Configure based on prefs
            this.preferencesChanged();
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
            case WorkerProcessTypes.SaveFile:
                this.saveFile(e.data);
                break;
            case WorkerProcessTypes.DisplayFullCompileResults:
                this.displayFullCompileResults(e.data);
                break;
        }
    }

    /**
     * Saves a compiled file sent across from the worker service
     * @param  {WorkerProcessTypes.SaveMessageData} event
     */
    saveFile(event: WorkerProcessTypes.SaveMessageData) {
        if (this.active) {
            this.serviceLocator.clientServices.getHostInterop().saveFile(event.filename, event.code);
        }
    }

    /**
     * Request the markers to display
     */
    getAnnotations() {
        const message: WorkerProcessTypes.GetAnnotationsMessageData = {
            command: WorkerProcessTypes.GetAnnotations,
            code: this.editor.getModel().getValue(),
            filename: this.filename,
            fileExt: null,
            editor: null // cannot send editor across the boundary
        };

        this.worker.port.postMessage(message);
    }

    /**
     * Set annotations based upon issues reported by the typescript language service
     * @param  {WorkerProcessTypes.GetAnnotationsResponseMessageData} event
     */
    setAnnotations(event: WorkerProcessTypes.GetAnnotationsResponseMessageData) {
        let model = this.editor.getModel();
        let markers = event.annotations
            .filter(ann => ann.start != undefined)
            .map(ann => {
                return {
                    code: ann.code,
                    severity: monaco.Severity.Error,
                    message: ann.message,
                    //source?: string;
                    startLineNumber: model.getPositionAt(ann.start).lineNumber,
                    startColumn: model.getPositionAt(ann.start).column,
                    endLineNumber: model.getPositionAt(ann.start + ann.length).lineNumber,
                    endColumn: model.getPositionAt(ann.start + ann.length).column
                };
            });

        monaco.editor.setModelMarkers(this.editor.getModel(), "Atomic", markers);
    }

    /**
     * Build/Attach to the shared web worker
     */
    buildWorker() {
        if (!this.worker) {
            this.worker = new SharedWorker("./source/editorCore/clientExtensions/languageExtensions/typescript/workerprocess/workerLoader.js");

            // hook up the event listener
            this.worker.port.addEventListener("message", this.handleWorkerMessage.bind(this), false);

            // Tell the SharedWorker we're closing
            addEventListener("beforeunload", () => {
                this.worker.port.postMessage({ command: WorkerProcessTypes.Disconnect });
            });

            this.worker.port.start();
        }
    }

    /**
     * Called once a resource has been saved
     * @param  {Editor.EditorEvents.SaveResourceEvent} ev
     */
    save(ev: Editor.EditorEvents.CodeSavedEvent) {
        if (this.active && this.isValidFiletype(ev.filename)) {
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
        if (this.active && this.isValidFiletype(ev.path)) {
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
        if (this.active && this.isValidFiletype(ev.path)) {
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
     * Called when the user preferences have been changed (or initially loaded)
     * @return {[type]}
     */
    preferencesChanged() {
        if (this.active) {
            let compileOnSave = this.serviceLocator.clientServices.getUserPreference("HostTypeScriptLanguageExtension", "CompileOnSave", true);
            const message: WorkerProcessTypes.SetPreferencesMessageData = {
                command: WorkerProcessTypes.SetPreferences,
                preferences: {
                    compileOnSave: compileOnSave
                }
            };

            this.worker.port.postMessage(message);
        }
    }

    /**
     * Tell the language service to perform a full compile
     */
    doFullCompile(tsConfig: any) {
        if (this.active) {
            const message: WorkerProcessTypes.FullCompileMessageData = {
                command: WorkerProcessTypes.DoFullCompile,
                tsConfig: tsConfig
            };
            this.worker.port.postMessage(message);
        }
    }

    /**
     * Displays the results from a full compile
     * @param  {WorkerProcessTypes.FullCompileResultsMessageData} results
     */
    displayFullCompileResults(results: WorkerProcessTypes.FullCompileResultsMessageData) {
        let messageArray = results.annotations.map((result) => {
            return `${result.text} at line ${result.row} col ${result.column} in ${result.file}`;
        });
        window.atomicQueryPromise("TypeScript.DisplayCompileResults", results);
    }
}


/**
 * Class used as a base for our custom providers to let the patched loader
 * know to allow this through.
 */
class BuiltinServiceProviderOverride {

    constructor(extension: TypescriptLanguageExtension) {
        this.extension = extension;
    }
    protected extension: TypescriptLanguageExtension;

    /**
     * Used to tell the loader that we want to load this extension.  Others will be skipped
     * @type {Boolean}
     */
    public isOverride = true;
}

/**
 * Customized completion provider that will delegate to the shared web worker for it's completions
 */
class CustomCompletionProvider extends BuiltinServiceProviderOverride implements monaco.languages.CompletionItemProvider {
    get triggerCharacters(): string[] {
        return ["."];
    }

    provideCompletionItems(model: monaco.editor.IReadOnlyModel, position: monaco.Position, token: monaco.CancellationToken): monaco.languages.CompletionItem[] | monaco.Thenable<monaco.languages.CompletionItem[]> | monaco.languages.CompletionList | monaco.Thenable<monaco.languages.CompletionList> {
        const message: WorkerProcessTypes.MonacoProvideCompletionItemsMessageData = {
            command: WorkerProcessTypes.MonacoProvideCompletionItems,
            uri: this.extension.filename,
            source: model.getValue(),
            positionOffset: model.getOffsetAt(position)
        };

        return this.extension.workerRequest(WorkerProcessTypes.MonacoProvideCompletionItemsResponse, message)
            .then((e: WorkerProcessTypes.MonacoProvideCompletionItemsResponseMessageData) => {
                // Need to map the TS completion kind to the monaco completion kind
                return e.completions.map(completion => {
                    completion.kind = tsLanguageSupport.Kind.convertKind(completion.completionKind);
                    return completion;
                }).filter(completion => {
                    // Filter out built-in keywords since most of them don't apply to Atomic
                    return completion.kind != monaco.languages.CompletionItemKind.Keyword;
                });
            });
    }

    resolveCompletionItem(item: monaco.languages.CompletionItem, token: monaco.CancellationToken): monaco.languages.CompletionItem | monaco.Thenable<monaco.languages.CompletionItem> {
        const message: WorkerProcessTypes.MonacoResolveCompletionItemMessageData = {
            command: WorkerProcessTypes.MonacoResolveCompletionItem,
            item: item as WorkerProcessTypes.MonacoWordCompletion
        };

        return this.extension.workerRequest(WorkerProcessTypes.MonacoResolveCompletionItem, message)
            .then((e: WorkerProcessTypes.MonacoResolveCompletionItemResponseMessageData) => {
                return e;
            });
    }
}

class CustomHoverProvider extends BuiltinServiceProviderOverride implements monaco.languages.HoverProvider {
    protected _offsetToPosition(uri: monaco.Uri, offset: number): monaco.IPosition {
        let model = monaco.editor.getModel(uri);
        return model.getPositionAt(offset);
    }

    protected _textSpanToRange(uri: monaco.Uri, span: ts.TextSpan): monaco.IRange {
        let p1 = this._offsetToPosition(uri, span.start);
        let p2 = this._offsetToPosition(uri, span.start + span.length);
        let {lineNumber: startLineNumber, column: startColumn} = p1;
        let {lineNumber: endLineNumber, column: endColumn} = p2;
        return { startLineNumber, startColumn, endLineNumber, endColumn };
    }

    provideHover(model, position) {
        let resource = model.uri;
        const message: WorkerProcessTypes.MonacoGetQuickInfoMessageData = {
            command: WorkerProcessTypes.MonacoGetQuickInfo,
            uri: this.extension.filename,
            source: model.getValue(),
            positionOffset: model.getOffsetAt(position)
        };

        return this.extension.workerRequest(WorkerProcessTypes.MonacoGetQuickInfoResponse, message)
            .then((e: WorkerProcessTypes.MonacoGetQuickInfoResponseMessageData) => {
                if (e.contents) {
                    return {
                        range: this._textSpanToRange(resource, e.textSpan),
                        contents: [e.contents]
                    };
                }
            });
    }
}

export class CustomSignatureProvider extends BuiltinServiceProviderOverride implements monaco.languages.SignatureHelpProvider {
    public signatureHelpTriggerCharacters = ["(", ","];

    provideSignatureHelp(model: monaco.editor.IReadOnlyModel, position: monaco.Position) {
        let resource = model.uri;

        const message: WorkerProcessTypes.MonacoGetSignatureMessageData = {
            command: WorkerProcessTypes.MonacoGetSignature,
            uri: this.extension.filename,
            source: model.getValue(),
            positionOffset: model.getOffsetAt(position)
        };

        return this.extension.workerRequest(WorkerProcessTypes.MonacoGetSignatureResponse, message)
            .then((e: WorkerProcessTypes.MonacoGetSignatureMessageDataResponse) => {
                if (e.signatures) {
                    let result: monaco.languages.SignatureHelp = {
                        signatures: e.signatures,
                        activeSignature: e.selectedItemIndex,
                        activeParameter: e.argumentIndex
                    };

                    return result;
                }
            });
    }
}
