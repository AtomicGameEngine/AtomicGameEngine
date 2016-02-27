/**
 * Typescript Language Worker - Handles bringing down the source, providing completions, and compiling.
 * since this is a shared web worker, each editor tab will be sharing the same data.
 */
import * as ts from "../../../../modules/typescript";
import {TypescriptLanguageService, FileSystemInterface} from "./TypescriptLanguageService";
import * as WorkerProcessCommands from "./workerProcessCommands";

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

        port.addEventListener("message", (e:WorkerProcessCommands.WorkerProcessMessage<any>) => {
            switch (e.data.command) {
                case WorkerProcessCommands.Connect:
                    this.handleHELO(port, e.data);
                    break;
                case WorkerProcessCommands.Disconnect:
                    this.handleCLOSE(port, e.data);
                    break;
                case WorkerProcessCommands.GetCompletions:
                    this.handleGetCompletions(port, e.data);
                    break;
                case WorkerProcessCommands.GetDocTooltip:
                    this.handleGetDocTooltip(port, e.data);
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
        if (!this.projectLoaded) {
            return getFileResource("resources/tsconfig.atomic").then((jsonTsConfig: string) => {
                let promises: PromiseLike<void>[] = [];
                let tsConfig: TSConfigFile = JSON.parse(jsonTsConfig);

                if (tsConfig.compilerOptions) {
                    this.languageService.compilerOptions = tsConfig.compilerOptions;
                };

                tsConfig.files.forEach((f) => {
                    promises.push(getFileResource(f).then((code: string) => {
                        this.languageService.addProjectFile(f, code);
                    }));
                });
                return Promise.all(promises);
            }).then(() => {
                // Let's seed the compiler state
                // this.languageService.compile([this.filename]);
                this.projectLoaded = true;
            });
        } else {
            // just return a resolved promise.. we are done
            return Promise.resolve();
        }
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
        port.postMessage({ command: WorkerProcessCommands.Message, message: "Hello " + eventData.sender + " (port #" + this.connections + ")" });
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
    handleGetCompletions(port: MessagePort, eventData: WorkerProcessCommands.GetCompletionsMessageData) {
        let sourceFile = this.languageService.updateProjectFile(eventData.filename, eventData.sourceText);

        let newpos = this.languageService.getPositionOfLineAndCharacter(sourceFile, eventData.pos.row, eventData.pos.column);
        let completions = this.languageService.getCompletions(eventData.filename, newpos);

        let message: WorkerProcessCommands.GetCompletionsResponseMessageData = {
            command: WorkerProcessCommands.CompletionResponse,
            completions: []
        };
        let langService = this.languageService;

        if (completions) {
            message.completions = completions.entries.map((completion: ts.CompletionEntry) => {
                let value = completion.name;
                let completionItem: WorkerProcessCommands.WordCompletion = {
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
    handleGetDocTooltip(port: MessagePort, eventData: WorkerProcessCommands.GetDocTooltipMessageData) {
        let message: WorkerProcessCommands.GetDocTooltipResponseMessageData = {
            command: WorkerProcessCommands.DocTooltipResponse
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
}
