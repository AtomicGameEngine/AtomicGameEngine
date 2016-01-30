//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//
// Based upon the TypeScript language services example at https://github.com/Microsoft/TypeScript/wiki/Using-the-Compiler-API#incremental-build-support-using-the-language-services

import * as ts from "modules/typescript";

/**
 * Abstraction over the file system
 */
export interface FileSystemInterface {
    /**
     * Deterimine if the particular file exists in the resources
     * @param  {string} filename
     * @return {boolean}
     */
    fileExists(filename: string): boolean;
    /**
     * Grab the contents of the file
     * @param  {string} filename
     * @return {string}
     */
    getFile(filename: string): string;
    /**
     * Write the contents to the file specified
     * @param  {string} filename
     * @param  {string} contents
     */
    writeFile(filename: string, contents: string);
}

/**
 * Resource extension that handles compiling or transpling typescript on file save.
 */
export class TypescriptLanguageService {

    constructor(fs: FileSystemInterface) {
        this.fs = fs;
    }

    private fs: FileSystemInterface = null;
    private languageService: ts.LanguageService = null;

    name: string = "TypescriptLanguageService";

    /**
     * Perform a full compile on save, or just transpile the current file
     * @type {boolean}
     */
    fullCompile: boolean = true;

    private projectFiles: string[] = [];
    private versionMap: ts.Map<{ version: number, snapshot?: ts.IScriptSnapshot }> = {};

    /**
     * Adds a file to the internal project cache
     * @param  {string} file the full path of the file to add
     */
    addProjectFile(file: string) {
        if (this.projectFiles.indexOf(file) == -1) {
            console.log("ADDED: " + file);
            this.versionMap[file] = { version: 0 };
            this.projectFiles.push(file);
        }
    }

    /**
     * Simply transpile the typescript file.  This is much faster and only checks for syntax errors
     * @param {string[]}           fileNames array of files to transpile
     * @param {ts.CompilerOptions} options   compiler options
     */
    transpile(fileNames: string[], options: ts.CompilerOptions): void {
        fileNames.forEach((fileName) => {
            console.log(`${this.name}:  Transpiling ${fileName}`);
            let script = this.fs.getFile(fileName);
            let diagnostics: ts.Diagnostic[] = [];
            let result = ts.transpile(script, options, fileName, diagnostics);
            if (diagnostics.length) {
                this.logErrors(diagnostics);
            }

            if (diagnostics.length == 0) {
                this.fs.writeFile(fileName.replace(".ts", ".js"), result);
            }
        });
    }

    /**
     * Compile the provided file to javascript with full type checking etc
     * @param  {string}  a list of file names to compile
     * @param  {ts.CompilerOptions} options for the compiler
     */
    compile(files: string[], options: ts.CompilerOptions): void {
        let start = new Date().getTime();

        //Make sure we have these files in the project
        files.forEach((file) => {
            this.addProjectFile(file);
        });

        let errors: ts.Diagnostic[] = [];

        if (!this.languageService) {
            // This is the first time in.  Need to create a language service

            // Create the language service host to allow the LS to communicate with the host
            const servicesHost: ts.LanguageServiceHost = {
                getScriptFileNames: () => this.projectFiles,
                getScriptVersion: (fileName) => this.versionMap[fileName] && this.versionMap[fileName].version.toString(),
                getScriptSnapshot: (fileName) => {
                    const scriptVersion = this.versionMap[fileName];
                    if (this.fs.fileExists(fileName)) {
                        if (scriptVersion) {
                            delete this.versionMap[fileName];
                            let idx = this.projectFiles.indexOf(fileName);
                            if (idx > -1) {
                                this.projectFiles.splice(idx, 1);
                            }
                        }
                        return undefined;
                    }

                    // Grab the cached version
                    if (scriptVersion) {
                        if (scriptVersion.snapshot) {
                            console.log(`cache hit snapshot for ${fileName}`);
                            return scriptVersion.snapshot;
                        } else {
                            let script = this.fs.getFile(fileName);
                            scriptVersion.snapshot = ts.ScriptSnapshot.fromString(script);
                            return scriptVersion.snapshot;
                        }
                    } else {
                        console.log(`no script version for ${fileName}`);
                    }
                },
                getCurrentDirectory: () => ToolCore.toolSystem.project.resourcePath,
                getCompilationSettings: () => options,
                getDefaultLibFileName: (options) => undefined
            };

            // Create the language service files
            this.languageService = ts.createLanguageService(servicesHost, ts.createDocumentRegistry());

            // This is the first time in, let's compile all the files
            this.projectFiles.forEach(filename => {
                errors = errors.concat(this.compileFile(filename));
            });
        } else {
            // Only compile the files that are newly edited
            files.forEach(filename => {
                // increment the version number since we changed
                this.versionMap[filename].version++;
                this.versionMap[filename].snapshot = null;
                errors = errors.concat(this.compileFile(filename));
            });
        }

        if (errors.length) {
            this.logErrors(errors);
        }

        console.log(`${this.name}: Compiling complete after ${new Date().getTime() - start} ms`);
    }

    /**
     * Delete a file from the project store
     * @param  {string} filepath
     */
    deleteProjectFile(filepath: string) {
        if (this.versionMap[filepath]) {
            delete this.versionMap[filepath];
        }
        let idx = this.projectFiles.indexOf(filepath);
        if (idx > -1) {
            this.projectFiles.splice(idx, 1);
        }
    }

    /**
     * rename a file in the project store
     * @param  {string} filepath the old path to the file
     * @param  {string} newpath the new path to the file
     */
    renameProjectFile(filepath: string, newpath: string): void {
        let oldFile = this.versionMap[filepath];
        if (oldFile) {
            delete this.versionMap[filepath];
            this.versionMap[newpath] = oldFile;
        }
        let idx = this.projectFiles.indexOf(filepath);
        if (idx > -1) {
            this.projectFiles[idx] = newpath;
        }
    }

    /**
     * clear out any caches, etc.
     */
    reset() {
        this.projectFiles = [];
        this.versionMap = {};
        this.languageService = null;
    }

    /**
     * Compile an individual file
     * @param  {string} filename the file to compile
     * @return {[ts.Diagnostic]} a list of any errors
     */
    private compileFile(filename: string): ts.Diagnostic[] {
        console.log(`${this.name}: Compiling version ${this.versionMap[filename].version} of ${filename}`);
        //if (!filename.match("\.d\.ts$")) {
        try {
            return this.emitFile(filename);
        } catch (err) {
            console.log(`${this.name}: problem encountered compiling ${filename}: ${err}`);
            return [];
            // console.log(err.stack);
        }
        //}
    }

    /**
     * writes out a file from the compile process
     * @param  {string} filename [description]
     * @return {[ts.Diagnostic]} a list of any errors
     */
    private emitFile(filename: string): ts.Diagnostic[] {
        let output = this.languageService.getEmitOutput(filename);
        let allDiagnostics: ts.Diagnostic[] = [];
        if (output.emitSkipped) {
            console.log(`${this.name}: Failure Emitting ${filename}`);
            allDiagnostics = this.languageService.getCompilerOptionsDiagnostics()
                .concat(this.languageService.getSyntacticDiagnostics(filename))
                .concat(this.languageService.getSemanticDiagnostics(filename));
        }

        output.outputFiles.forEach(o => {
            this.fs.writeFile(o.name, o.text);
        });
        return allDiagnostics;
    }

    /**
     * Logs errors from the diagnostics returned from the compile/transpile process
     * @param {ts.Diagnostic} diagnostics information about the errors
     * @return {[type]}          [description]
     */
    private logErrors(diagnostics: ts.Diagnostic[]) {
        let msg = [];

        diagnostics.forEach(diagnostic => {
            let message = ts.flattenDiagnosticMessageText(diagnostic.messageText, "\n");
            if (diagnostic.file) {
                let { line, character } = diagnostic.file.getLineAndCharacterOfPosition(diagnostic.start);
                msg.push(`${this.name}:  Error ${diagnostic.file.fileName} (${line + 1},${character + 1}): ${message}`);
            }
            else {
                msg.push(`${this.name}  Error: ${message}`);
            }
        });
        console.log(`TypeScript Errors:\n${msg.join("\n") }`);
        throw new Error(`TypeScript Errors:\n${msg.join("\n") }`);
    }

}
