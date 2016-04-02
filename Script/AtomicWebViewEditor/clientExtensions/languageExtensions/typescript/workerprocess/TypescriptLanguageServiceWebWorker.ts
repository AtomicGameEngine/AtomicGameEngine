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
        console.log("FS.GETFILE!!");
        // return HostInterop.getResource("atomic:" + filename);
        return this.fileCache[filename];
    }

    /**
     * Write the contents to the file specified
     * @param  {string} filename
     * @param  {string} contents
     */
    writeFile(filename: string, contents: string) {
        //TODO:
        /*let script = new Atomic.File(filename, Atomic.FILE_WRITE);
        try {
            script.writeString(contents);
            script.flush();
        } finally {
            script.close();
        }
        */
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

    constructor() {
        this.fs = new WebFileSystem();
        this.languageService = new TypescriptLanguageService(this.fs);
    }

    /**
     * Handle when an editor connects to this worker
     * @param  {any} e
     */
    connect(e) {
        let port: MessagePort = e.ports[0];
        this.connections++;

        port.addEventListener("message", (e: WorkerProcessTypes.WorkerProcessMessage<any>) => {
            switch (e.data.command) {
                case WorkerProcessTypes.Connect:
                    this.handleHELO(port, e.data);
                    break;
                case WorkerProcessTypes.Disconnect:
                    this.handleCLOSE(port, e.data);
                    break;
                case WorkerProcessTypes.GetCompletions:
                    this.handleGetCompletions(port, e.data);
                    break;
                case WorkerProcessTypes.GetDocTooltip:
                    this.handleGetDocTooltip(port, e.data);
                    break;
                case ClientExtensionEventNames.ResourceSavedEvent:
                    this.handleSave(port, e.data);
                    break;
                case ClientExtensionEventNames.ResourceRenamedEvent:
                    this.handleRename(port, e.data);
                    break;
                case ClientExtensionEventNames.ResourceDeletedEvent:
                    this.handleDelete(port, e.data);
                    break;
                case ClientExtensionEventNames.ProjectUnloadedEvent:
                    this.handleProjectUnloaded(port);
                    break;
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
        // Let's query the backend and get a list of the current files
        // and delete any that may be been removed and sync up
        return getFileResource("resources/tsconfig.atomic").then((jsonTsConfig: string) => {
            let promises: PromiseLike<void>[] = [];
            let tsConfig: TSConfigFile = JSON.parse(jsonTsConfig);

            if (tsConfig.compilerOptions) {
                this.languageService.compilerOptions = tsConfig.compilerOptions;
            };

            let existingFiles = this.languageService.getProjectFiles();

            // see if anything was deleted
            existingFiles.forEach((f) => {
                if (tsConfig.files.indexOf(f) == -1) {
                    this.languageService.deleteProjectFile(f);
                }
            });

            // load up any new files that may have been added
            tsConfig.files.forEach((f) => {
                if (existingFiles.indexOf(f) == -1) {
                    promises.push(getFileResource(f).then((code: string) => {
                        this.languageService.addProjectFile(f, code);
                    }));
                }
            });
            return Promise.all(promises);
        }).then(() => {
            // Let's seed the compiler state
            // this.languageService.compile([this.filename]);
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
        filename: string
    }) {
        port.postMessage({ command: WorkerProcessTypes.Message, message: "Hello " + eventData.sender + " (port #" + this.connections + ")" });
        this.loadProjectFiles().then(() => {
            this.languageService.compile([eventData.filename]);
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
        console.log("Got a close");
    }

    /**
     * Get completions
     * @param  {MessagePort} port
     * @param  {WorkerProcessCommands.GetCompletionsMessage} eventData
     */
    handleGetCompletions(port: MessagePort, eventData: WorkerProcessTypes.GetCompletionsMessageData) {
        let sourceFile = this.languageService.updateProjectFile(eventData.filename, eventData.sourceText);

        let newpos = this.languageService.getPositionOfLineAndCharacter(sourceFile, eventData.pos.row, eventData.pos.column);
        let completions = this.languageService.getCompletions(eventData.filename, newpos);

        let message: WorkerProcessTypes.GetCompletionsResponseMessageData = {
            command: WorkerProcessTypes.CompletionResponse,
            completions: []
        };
        let langService = this.languageService;

        if (completions) {
            message.completions = completions.entries.map((completion: ts.CompletionEntry) => {
                let value = completion.name;
                let completionItem: WorkerProcessTypes.WordCompletion = {
                    caption: completion.name,
                    value: value,
                    score: 100 - parseInt(completion.sortText, 0),
                    meta: completion.kind,
                    pos: newpos
                };

                //completionItem.docTooltip = this.getDocTooltip(eventData.filename, newpos, completionItem);
                return completionItem;
            });
        }

        port.postMessage(message);
    }

    /**
     * Get the documentation popup information for the current completion item
     * @param  {MessagePort} port
     * @param  {WorkerProcessCommands.GetDocTooltipMessageData} eventData
     * @return {[type]}
     */
    handleGetDocTooltip(port: MessagePort, eventData: WorkerProcessTypes.GetDocTooltipMessageData) {
        let message: WorkerProcessTypes.GetDocTooltipResponseMessageData = {
            command: WorkerProcessTypes.DocTooltipResponse
        };
        const details = this.languageService.getCompletionEntryDetails(eventData.filename, eventData.pos, eventData.completionItem.caption);
        if (details) {
            let docs = details.displayParts.map(part => part.text).join("");
            if (details.documentation) {
                docs += "<br/" + details.documentation.map(part => part.text).join("");
            }

            message.docHTML = docs;
        }

        port.postMessage(message);
    }

    /**
     * Called when the file has been saved.
     * @param  {MessagePort} port
     * @param  {WorkerProcessCommands.SaveMessageData} eventData
     */
    handleSave(port: MessagePort, eventData: WorkerProcessTypes.SaveMessageData) {
        // let's reload the file
        getFileResource(eventData.path).then((code: string) => {
            this.languageService.updateProjectFile(eventData.path, code);
        });
    }

    /**
     * Called when a file has been deleted
     * @param  {MessagePort} port
     * @param  {WorkerProcessCommands.DeleteMessageData} eventData
     */
    handleDelete(port: MessagePort, eventData: WorkerProcessTypes.DeleteMessageData) {
        this.languageService.deleteProjectFile(eventData.path);
    }

    /**
     * Called when a file has been renamed
     * @param  {MessagePort} port
     * @param  {WorkerProcessCommands.RenameMessageData} eventData
     */
    handleRename(port: MessagePort, eventData: WorkerProcessTypes.RenameMessageData) {
        this.languageService.renameProjectFile(eventData.path, eventData.newPath);
    }

    /**
     * Called when the project has been closed
     * @param  {MessagePort} port
     */
    handleProjectUnloaded(port: MessagePort) {
        this.reset();
    }
}
