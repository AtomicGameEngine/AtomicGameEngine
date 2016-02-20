//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//
// Based upon the TypeScript language services example at https://github.com/Microsoft/TypeScript/wiki/Using-the-Compiler-API#incremental-build-support-using-the-language-services

import * as ts from "../../../modules/typescript";
import {TypescriptLanguageService, FileSystemInterface} from "./TypescriptLanguageService";

interface TSConfigFile {
    compilerOptions?: ts.CompilerOptions;
    files: Array<string>;
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
    /**
     * Perform a full compile on save, or just transpile the current file
     * @type {boolean}
     */
    fullCompile: boolean = true;

    /**
     * The language service that will handle building
     * @type {TypescriptLanguageService}
     */
    languageService: TypescriptLanguageService = null;

    fs: WebFileSystem; // needed?

    /**
    * Inject this language service into the registry
     * @param  {Editor.ClientExtensions.ClientServiceLocator} serviceLocator
     */
    initialize(serviceLocator: Editor.ClientExtensions.ClientServiceLocator) {
        // initialize the language service
        this.serviceLocator = serviceLocator;
        this.fs = new WebFileSystem();
        this.languageService = new TypescriptLanguageService(this.fs);
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
     * Seed the language service with all of the relevant files in the project
     */
    private loadProjectFiles() {
        this.serviceLocator.getHostInterop().getFileResource("resources/tsconfig.atomic").then((jsonTsConfig: string) => {
            let promises: PromiseLike<void>[] = [];
            let tsConfig: TSConfigFile = JSON.parse(jsonTsConfig);

            if (tsConfig.compilerOptions) {
                this.languageService.compilerOptions = tsConfig.compilerOptions;
            };

            tsConfig.files.forEach((f) => {
                promises.push(this.serviceLocator.getHostInterop().getFileResource(f).then((code: string) => {
                    this.languageService.addProjectFile(f, code);
                }));
            });
            return Promise.all(promises);
        }).then(() => {
            // Let's seed the compiler state
            this.languageService.compile([this.filename]);
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
            editor.completers = [this.buildWordCompleter(ev.filename, this.languageService)];

            // for now, we will handle project stuff in the same thread.  In the future we need to share this between sessions
            this.languageService.reset();
            this.loadProjectFiles();
        }
    }

    /**
     * Builds the word completer for the Ace Editor.  This will handle determining which items to display in the popup and in which order
     * @param  {string} filename the filename of the current file
     * @param  {TypescriptLanguageService} langService The language service that handles compilation, completion resolution, etc.
     * @param  {FileSystemInterface} fs the interface into the file system
     * @return {[type]} returns a completer
     */
    private buildWordCompleter(filename: string, langService: TypescriptLanguageService): {
        getDocTooltip?: (selected: any) => { docText?: string, docHTML?: string },
        getCompletions: (editor, session, pos, prefix, callback) => void
    } {
        //let langTools = ace.require("ace/ext/language_tools");
        let wordCompleter = {
            getDocTooltip: function(selected: any): { docText?: string, docHTML?: string } {
                let details = langService.getCompletionEntryDetails(filename, selected.pos, selected.caption);
                if (details) {
                    return {
                        docHTML: details.displayParts.map(part => part.text).join("")
                        + "<br/>"
                        + details.documentation.map(part => part.text).join("")
                    };
                } else {
                    return null;
                }
            },

            getCompletions: function(editor, session, pos, prefix, callback) {
                try {
                    let sourceFile = langService.updateProjectFile(filename, editor.session.getValue());

                    //langService.compile([ev.filename]);
                    let newpos = langService.getPositionOfLineAndCharacter(sourceFile, pos.row, pos.column);
                    let completions = langService.getCompletions(filename, newpos);
                    if (completions) {
                        callback(null, completions.entries.map(function(completion: ts.CompletionEntry) {
                            return {
                                caption: completion.name,
                                value: completion.name,
                                score: 100 - parseInt(completion.sortText, 0),
                                meta: completion.kind,
                                pos: newpos
                            };
                        }));
                    } else {
                        console.log("No completions available: " + prefix);
                    }
                } catch (e) {
                    console.log("Failure completing " + filename);
                    console.log(e);
                }
            }
        };
        return wordCompleter;
    }

    /*** ResourceService implementation ****/

    /**
     * Called once a resource has been saved
     * @param  {Editor.EditorEvents.SaveResourceEvent} ev
     */
    save(ev: Editor.EditorEvents.SaveResourceEvent) {
        if (this.isValidFiletype(ev.path)) {
            console.log(`${this.name}: received a save resource event for ${ev.path}`);
            if (this.fullCompile) {
                this.languageService.compile([ev.path]);
            } else {
                this.languageService.transpile([ev.path]);
            }
        }
    }

    /**
     * Handle the delete.  This should delete the corresponding javascript file
     * @param  {Editor.EditorEvents.DeleteResourceEvent} ev
     */
    delete(ev: Editor.EditorEvents.DeleteResourceEvent) {
        if (this.isValidFiletype(ev.path)) {
            console.log(`${this.name}: received a delete resource event`);

            // notify the typescript language service that the file has been deleted
            this.languageService.deleteProjectFile(ev.path);
        }
    }

    /**
     * Handle the rename.  Should rename the corresponding .js file
     * @param  {Editor.EditorEvents.RenameResourceEvent} ev
     */
    rename(ev: Editor.EditorEvents.RenameResourceEvent) {
        if (this.isValidFiletype(ev.path)) {
            console.log(`${this.name}: received a rename resource event`);

            // notify the typescript language service that the file has been renamed
            this.languageService.renameProjectFile(ev.path, ev.newPath);
        }
    }
}
