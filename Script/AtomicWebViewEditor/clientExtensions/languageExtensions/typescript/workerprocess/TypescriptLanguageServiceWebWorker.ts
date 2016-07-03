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
 * Typescript Language Worker - Handles bringing down the source, providing completions, and compiling.
 * since this is a shared web worker, each editor tab will be sharing the same data.
 */
import * as ts from "../../../../modules/typescript";
import {TypescriptLanguageService, FileSystemInterface} from "./TypescriptLanguageService";
import * as WorkerProcessTypes from "./workerProcessTypes";
import ClientExtensionEventNames from "../../../ClientExtensionEventNames";

interface TSConfigFile {
    compilerOptions?: ts.CompilerOptions;
    files: Array<string>;
}

/**
 * Queries the host for a particular resource and returns it in a promise
 * @param  {string} codeUrl
 * @return {Promise}
 */
function getResource(codeUrl: string): Promise<{}> {
    return new Promise(function(resolve, reject) {
        const xmlHttp = new XMLHttpRequest();
        xmlHttp.onreadystatechange = () => {
            if (xmlHttp.readyState == 4 && xmlHttp.status == 200) {
                resolve(xmlHttp.responseText);
            }
        };
        xmlHttp.open("GET", codeUrl, true); // true for asynchronous
        xmlHttp.send(null);
    });
}

/**
 * Returns a file resource from the resources directory
 * @param  {string} filename name and path of file under the project directory or a fully qualified file name
 * @return {Promise}
 */
function getFileResource(filename: string): Promise<{}> {
    return getResource(`atomic://${filename}`);
}

/**
 * Class that provides access to the Atomic filesystem routines
 */
class WebFileSystem implements FileSystemInterface {

    private fileCache = {};
    private communicationPort: MessagePort;

    /**
     * Sets the port used to communicate with the primary window.
     * This is needed since AtomicQuery can"t be called from a webworker
     * @param  {MessagePort} port
     */
    setCommunicationPort(port: MessagePort) {
        this.communicationPort = port;
    }

    /**
     * Deterimine if the particular file exists in the resources
     * @param  {string} filename
     * @return {boolean}
     */
    fileExists(filename: string): boolean {
        return this.fileCache[filename] != null;
    }

    /**
     * Cache a file in the filesystem
     * @param  {string} filename
     * @param  {string} file
     */
    cacheFile(filename: string, file: string) {
        this.fileCache[filename] = file;
    }

    /**
     * Grab the contents of the file
     * @param  {string} filename
     * @return {string}
     */
    getFile(filename: string): string {
        // console.log("FS.GETFILE!!");
        // return HostInterop.getResource("atomic:" + filename);
        return this.fileCache[filename];
    }

    /**
     * Write the contents to the file specified
     * @param  {string} filename
     * @param  {string} contents
     */
    writeFile(filename: string, contents: string) {
        if (this.communicationPort) {
            const fileExt = filename.indexOf(".") != -1 ? filename.split(".").pop() : "";
            let message: WorkerProcessTypes.SaveMessageData = {
                command: WorkerProcessTypes.SaveFile,
                filename: filename,
                code: contents,
                fileExt: fileExt,
                editor: null
            };

            this.communicationPort.postMessage(message);
        }
    }

    /**
     * Returns the current directory / root of the source tree
     * @return {string}
     */
    getCurrentDirectory(): string {
        return "";
    }

}

export default class TypescriptLanguageServiceWebWorker {
    /**
     * Number of editors connected to this shared worker
     * @type {Number}
     */
    connections = 0;

    /**
     * The language service that will handle building
     * @type {TypescriptLanguageService}
     */
    languageService: TypescriptLanguageService = null;

    fs: WebFileSystem; // needed?

    projectLoaded = false;

    options = {
        compileOnSave: false
    };

    tsConfig: TSConfigFile = null;

    constructor() {
        this.fs = new WebFileSystem();
    }

    /**
     * Handle when an editor connects to this worker
     * @param  {any} e
     */
    connect(e) {
        let port: MessagePort = e.ports[0];
        this.connections++;

        port.addEventListener("message", (e: WorkerProcessTypes.WorkerProcessMessage<any>) => {
            try {
                switch (e.data.command) {
                    case WorkerProcessTypes.Connect:
                        this.handleHELO(port, e.data);
                        break;
                    case WorkerProcessTypes.Disconnect:
                        this.handleCLOSE(port, e.data);
                        break;
                    case ClientExtensionEventNames.CodeSavedEvent:
                        this.handleSave(port, e.data);
                        break;
                    case ClientExtensionEventNames.ResourceRenamedEvent:
                        this.handleRename(port, e.data);
                        break;
                    case ClientExtensionEventNames.ResourceDeletedEvent:
                        this.handleDelete(port, e.data);
                        break;
                    case WorkerProcessTypes.GetAnnotations:
                        this.handleGetAnnotations(port, e.data);
                        break;
                    case WorkerProcessTypes.SetPreferences:
                        this.setPreferences(port, e.data);
                        break;
                    case WorkerProcessTypes.DoFullCompile:
                        this.doFullCompile(port, e.data);
                        break;

                    /* Monaco */
                    case WorkerProcessTypes.MonacoProvideCompletionItems:
                        this.monacoHandleProvideCompletionItems(port, e.data);
                        break;
                    case WorkerProcessTypes.MonacoResolveCompletionItem:
                        this.monacoHandleResolveCompletionItem(port, e.data);
                        break;
                    case WorkerProcessTypes.MonacoGetQuickInfo:
                        this.monacoGetQuickInfo(port, e.data);
                        break;
                    case WorkerProcessTypes.MonacoGetSignature:
                        this.monacoGetSignature(port, e.data);
                        break;
                }
            } catch (e) {
                port.postMessage({ command: WorkerProcessTypes.Message, message: `Error in TypescriptLanguageServiceWebWorker: ${e}\n${e.stack}` });
            }
        }, false);

        port.start();
    }

    /**
     * Called when we want to reset the language service
     * @return {[type]}
     */
    reset() {
        this.languageService.reset();
        this.projectLoaded = false;
    }

    /**
     * Seed the language service with all of the relevant files in the project
     * @return {Promise}
     */
    private loadProjectFiles() {
        // Let"s query the backend and get a list of the current files
        // and delete any that may be been removed and sync up
        let promises: PromiseLike<void>[] = [];

        let existingFiles = this.languageService.getProjectFiles();

        // see if anything was deleted
        existingFiles.forEach((f) => {
            if (this.tsConfig.files.indexOf(f) == -1) {
                this.languageService.deleteProjectFile(f);
            }
        });

        // load up any new files that may have been added
        this.tsConfig.files.forEach((f) => {
            if (existingFiles.indexOf(f) == -1) {
                promises.push(getFileResource(f).then((code: string) => {
                    this.languageService.addProjectFile(f, code);
                }));
            }
        });

        return Promise.all(promises).then(() => {
            this.projectLoaded = true;
        });
    }

    /**
     * Handle initial tab connection
     * @param  {MessagePort} port
     * @param  {any} eventData
     */
    handleHELO(port: MessagePort, eventData: any | {
        sender: string,
        filename: string,
        tsConfig: any,
        code: string
    }) {
        //port.postMessage({ command: WorkerProcessTypes.Message, message: "Hello " + eventData.sender + " (port #" + this.connections + ")" });
        this.tsConfig = eventData.tsConfig;
        if (!this.languageService) {
            this.languageService = new TypescriptLanguageService(this.fs, this.tsConfig);
        }

        // Check to see if the file coming in is already in the
        // tsconfig.  The file coming in won"t have a full path
        // so, compare the ends
        const fn = this.resolvePartialFilename(eventData.filename);

        // Add this file right away, and then add the rest of the project files
        this.languageService.addProjectFile(fn, eventData.code);

        this.loadProjectFiles().then(() => {
            //let diagnostics = this.languageService.compile([fn]);
            this.handleGetAnnotations(port, eventData);
        });
    }

    /**
     * Handle when a tab closes
     * @param  {MessagePort} port
     * @param  {any} eventData
     */
    handleCLOSE(port: MessagePort, eventData: any) {
        this.connections--;
        if (this.connections <= 0) {
            this.reset();
        }
    }

    /**
     * Look up the filename in the tsconfig and see if it exists.  The filename
     * may contain a partial path and the tsconfig contains full paths, so check
     * the ends
     * @param  {string} partial
     * @return {string}
     */
    resolvePartialFilename(partial: string): string {
        let result = this.tsConfig.files.find(fn => fn.endsWith(partial));
        return result || partial;
    }

    /**
     * Get any errors or warnings
     * @param  {MessagePort} port
     * @param  {WorkerProcessTypes.GetAnnotationsMessageData} eventData
     */
    handleGetAnnotations(port: MessagePort, eventData: WorkerProcessTypes.GetAnnotationsMessageData) {
        let filename = this.resolvePartialFilename(eventData.filename);
        this.languageService.updateProjectFile(filename, eventData.code);
        let message: WorkerProcessTypes.GetAnnotationsResponseMessageData = {
            command: WorkerProcessTypes.AnnotationsUpdated,
            annotations: this.languageService.getDiagnostics(filename)
        };

        port.postMessage(message);
    }

    /**
     * Called when the file has been saved.  This will also send back annotations to the caller
     * @param  {MessagePort} port
     * @param  {WorkerProcessCommands.SaveMessageData} eventData
     */
    handleSave(port: MessagePort, eventData: WorkerProcessTypes.SaveMessageData) {
        let filename = this.resolvePartialFilename(eventData.filename);

        this.languageService.updateProjectFile(filename, eventData.code);
        this.handleGetAnnotations(port, eventData);

        if (this.options.compileOnSave) {
            this.fs.setCommunicationPort(port);
            let results = this.languageService.compile([filename]);
            this.fs.setCommunicationPort(null);
        }
    }


    /**
     * Perform a full compile of the typescript
     * @param  {MessagePort} port
     */
    doFullCompile(port: MessagePort, eventData: WorkerProcessTypes.FullCompileMessageData) {
        this.tsConfig = eventData.tsConfig;
        this.languageService.setTsConfig(eventData.tsConfig);

        this.fs.setCommunicationPort(port);

        let results = [];
        let start = Date.now();
        this.languageService.compile([], (filename, errors) => {
            if (errors.length > 0) {
                results = results.concat(errors.map(diagnostic => {
                    let row = 0;
                    let char = 0;
                    let filename = "";
                    if (diagnostic.file) {
                        let lineChar = diagnostic.file.getLineAndCharacterOfPosition(diagnostic.start);
                        row = lineChar.line;
                        char = lineChar.character;
                        filename = diagnostic.file.fileName;
                    }

                    let message = ts.flattenDiagnosticMessageText(diagnostic.messageText, "\n");
                    return {
                        file: filename,
                        row: row,
                        column: char,
                        text: message,
                        type: diagnostic.category == 1 ? "error" : "warning"
                    };
                }));
            } else {
                results.push({
                    file: filename,
                    row: 0,
                    column: 0,
                    text: "Success",
                    type: "success"
                });
            }
        });
        let duration = Date.now() - start;

        let message: WorkerProcessTypes.FullCompileResultsMessageData = {
            command: WorkerProcessTypes.DisplayFullCompileResults,
            annotations: results,
            compilerOptions: this.tsConfig.compilerOptions,
            duration: duration
        };
        this.fs.setCommunicationPort(null);
        port.postMessage(message);
    }

    /*  Monaco Routines */

    /**
     * Get completions
     * @param  {MessagePort} port
     * @param  {WorkerProcessCommands.GetCompletionsMessage} eventData
     */
    monacoHandleProvideCompletionItems(port: MessagePort, eventData: WorkerProcessTypes.MonacoProvideCompletionItemsMessageData) {
        // filename may not include the entire path, so let"s find it in the tsconfig
        let filename = this.resolvePartialFilename(eventData.uri);
        let sourceFile = this.languageService.updateProjectFile(filename, eventData.source);
        let completions = this.languageService.getCompletions(filename, eventData.positionOffset);

        let message: WorkerProcessTypes.MonacoProvideCompletionItemsResponseMessageData = {
            command: WorkerProcessTypes.MonacoProvideCompletionItemsResponse,
            completions: []
        };

        if (completions) {
            message.completions = completions.entries.map((completion: ts.CompletionEntry) => {
                let completionItem: WorkerProcessTypes.MonacoWordCompletion = {
                    label: completion.name,
                    uri: eventData.uri,
                    sortText: completion.sortText,
                    completionKind: completion.kind,
                    kind: -1,
                    positionOffset: eventData.positionOffset
                };
                return completionItem;
            });
        };

        port.postMessage(message);
    }


    monacoHandleResolveCompletionItem(port: MessagePort, data: WorkerProcessTypes.MonacoResolveCompletionItemMessageData) {
        let filename = this.resolvePartialFilename(data.item.uri);
        const details = this.languageService.getCompletionEntryDetails(filename, data.item.positionOffset, data.item.label);
        let message: WorkerProcessTypes.MonacoResolveCompletionItemResponseMessageData = {
            command: WorkerProcessTypes.MonacoResolveCompletionItemResponse,
            label: data.item.label,
            kind: data.item.kind,
            detail: "",
            documentation: ""
        };

        if (details) {
            message.label = details.name;
            message.kind = data.item.kind;
            message.detail = ts.displayPartsToString(details.displayParts);
            message.documentation = ts.displayPartsToString(details.documentation);
        }

        port.postMessage(message);
    }

    monacoGetQuickInfo(port: MessagePort, eventData: WorkerProcessTypes.MonacoGetQuickInfoMessageData) {
        let filename = this.resolvePartialFilename(eventData.uri);
        let quickInfo = this.languageService.getQuickInfoAtPosition(filename, eventData.positionOffset);

        let message: WorkerProcessTypes.MonacoGetQuickInfoResponseMessageData = {
            command: WorkerProcessTypes.MonacoGetQuickInfoResponse,
        };

        if (quickInfo) {
            message.contents = quickInfo.contents;
            message.textSpan = quickInfo.range;
        }

        port.postMessage(message);
    }

    monacoGetSignature(port: MessagePort, eventData: WorkerProcessTypes.MonacoGetSignatureMessageData) {
        let filename = this.resolvePartialFilename(eventData.uri);
        let sourceFile = this.languageService.updateProjectFile(filename, eventData.source);
        let signatureInfo = this.languageService.getSignatureHelpItems(filename, eventData.positionOffset);

        let message: WorkerProcessTypes.MonacoGetSignatureMessageDataResponse = {
            command: WorkerProcessTypes.MonacoGetSignatureResponse,
        };

        if (signatureInfo) {
            message.selectedItemIndex = signatureInfo.selectedItemIndex;
            message.argumentIndex = signatureInfo.argumentIndex;
            message.signatures = [];

            signatureInfo.items.forEach(item => {
                let signature: monaco.languages.SignatureInformation = {
                    label: "",
                    documentation: null,
                    parameters: []
                };

                signature.label += ts.displayPartsToString(item.prefixDisplayParts);

                item.parameters.forEach((p, i, a) => {
                    let label = ts.displayPartsToString(p.displayParts);
                    let parameter: monaco.languages.ParameterInformation = {
                        label: label,
                        documentation: ts.displayPartsToString(p.documentation)
                    };

                    signature.label += label;
                    signature.parameters.push(parameter);
                    if (i < a.length - 1) {
                        signature.label += ts.displayPartsToString(item.separatorDisplayParts);
                    }
                });

                signature.label += ts.displayPartsToString(item.suffixDisplayParts);
                message.signatures.push(signature);
            });
        }

        port.postMessage(message);
    }

    /**
     * Called when a file has been deleted
     * @param  {MessagePort} port
     * @param  {WorkerProcessCommands.DeleteMessageData} eventData
     */
    handleDelete(port: MessagePort, eventData: WorkerProcessTypes.DeleteMessageData) {
        let filename = this.resolvePartialFilename(eventData.path);
        this.languageService.deleteProjectFile(filename);
    }

    /**
     * Called when a file has been renamed
     * @param  {MessagePort} port
     * @param  {WorkerProcessCommands.RenameMessageData} eventData
     */
    handleRename(port: MessagePort, eventData: WorkerProcessTypes.RenameMessageData) {
        let fromFn = this.resolvePartialFilename(eventData.path);
        let toFn = fromFn.replace(eventData.path, eventData.newPath);
        this.languageService.renameProjectFile(fromFn, toFn);
    }

    setPreferences(port: MessagePort, eventData: WorkerProcessTypes.SetPreferencesMessageData) {
        this.options.compileOnSave = eventData.preferences.compileOnSave;
    }
}
