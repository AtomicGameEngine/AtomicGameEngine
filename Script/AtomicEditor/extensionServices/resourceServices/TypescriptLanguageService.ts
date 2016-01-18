//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

// Based upon the TypeScript language services example at https://github.com/Microsoft/TypeScript/wiki/Using-the-Compiler-API#incremental-build-support-using-the-language-services

import * as ExtensionServices from "../EditorExtensionServices";
import * as EditorEvents from "../../editor/EditorEvents";
import * as ts from "modules/typescript";

/**
 * Resource extension that handles compiling or transpling typescript on file save.
 */
export default class TypescriptLanguageService implements ExtensionServices.ResourceService {
    name: string = "TypeScriptResourceService";
    description: string = "This service transpiles TypeScript into JavaScript on save.";

    languageService: ts.LanguageService;
    projectFiles: string[];
    versionMap: ts.Map<{ version: number }> = {};

    /**
     * Perform a full compile on save, or just transpile the current file
     * @type {boolean}
     */
    fullCompile: boolean = true;

    /**
     * used by the compile to build a registery of all of the project files
     * @param {string[]} files optional list of files to refresh.  If not provided, then all files will be reloaded
     */
    refreshProjectFiles(files?: string[]) {
        if (!this.projectFiles || !files) {
            // First time in, let's index the entire project
            this.projectFiles = [];

            // First we need to load in a copy of the lib.core.d.ts that is necessary for the hosted typescript compiler
            this.projectFiles.push(Atomic.addTrailingSlash(Atomic.addTrailingSlash(ToolCore.toolEnvironment.toolDataDir) + "TypeScriptSupport") + "lib.core.d.ts");

            // Load up a copy of the duktape.d.ts
            this.projectFiles.push(Atomic.addTrailingSlash(Atomic.addTrailingSlash(ToolCore.toolEnvironment.toolDataDir) + "TypeScriptSupport") + "duktape.d.ts");

            //scan all the files in the project
            Atomic.fileSystem.scanDir(ToolCore.toolSystem.project.resourcePath, "*.ts", Atomic.SCAN_FILES, true).forEach(filename => {
                this.projectFiles.push(Atomic.addTrailingSlash(ToolCore.toolSystem.project.resourcePath) + filename);
            });

            // Look in a 'typings' directory for any typescript definition files
            const typingsDir = Atomic.addTrailingSlash(ToolCore.toolSystem.project.projectPath) + "typings";
            Atomic.fileSystem.scanDir(typingsDir, "*.d.ts", Atomic.SCAN_FILES, true).forEach(filename => {
                this.projectFiles.push(Atomic.addTrailingSlash(typingsDir) + filename);
            });

            // initialize the list of files
            this.projectFiles.forEach(fileName => {
                this.versionMap[fileName] = { version: 0 };
            });
        } else {
            //We already have a project, let's just add the files that are being saved if they are new
            files.forEach((file) => {
                if (!this.projectFiles.indexOf(file)) {
                    this.versionMap[file] = { version: 0 };
                    this.projectFiles.push(file);
                }
            });
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
            let script = new Atomic.File(fileName, Atomic.FILE_READ);
            try {
                let diagnostics: ts.Diagnostic[] = [];
                let result = ts.transpile(script.readText(), options, fileName, diagnostics);
                if (diagnostics.length) {
                    this.logErrors(diagnostics);
                }

                if (diagnostics.length == 0) {
                    let output = new Atomic.File(fileName.replace(".ts", ".js"), Atomic.FILE_WRITE);
                    try {
                        output.writeString(result);
                        output.flush();
                    } finally {
                        output.close();
                    }
                }
            } finally {
                script.close();
            }
        });
    }

    /**
     * Compile the provided file to javascript with full type checking etc
     * @param  {string}  a list of file names to compile
     * @param  {ts.CompilerOptions} options for the compiler
     */
    compile(files: string[], options: ts.CompilerOptions): void {

        //scan all the files in the project
        this.refreshProjectFiles(files);
        let errors: ts.Diagnostic[] = [];

        if (!this.languageService) {
            // This is the first time in.  Need to create a language service

            // Create the language service host to allow the LS to communicate with the host
            const servicesHost: ts.LanguageServiceHost = {
                getScriptFileNames: () => this.projectFiles,
                getScriptVersion: (fileName) => this.versionMap[fileName] && this.versionMap[fileName].version.toString(),
                getScriptSnapshot: (fileName) => {
                    if (!Atomic.fileSystem.exists(fileName)) {
                        return undefined;
                    }
                    let script = new Atomic.File(fileName, Atomic.FILE_READ);
                    try {
                        return ts.ScriptSnapshot.fromString(script.readText());
                    } finally {
                        script.close();
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
                errors = errors.concat(this.compileFile(filename));
            });
        }

        if (errors.length) {
            this.logErrors(errors);
        }
    }

    /**
     * Compile an individual file
     * @param  {string} filename the file to compile
     * @return {[ts.Diagnostic]} a list of any errors
     */
    compileFile(filename: string): ts.Diagnostic[] {
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
    emitFile(filename: string): ts.Diagnostic[] {
        let output = this.languageService.getEmitOutput(filename);
        let allDiagnostics: ts.Diagnostic[] = [];
        if (output.emitSkipped) {
            console.log(`${this.name}: Failure Emitting ${filename}`);
            allDiagnostics = this.languageService.getCompilerOptionsDiagnostics()
                .concat(this.languageService.getSyntacticDiagnostics(filename))
                .concat(this.languageService.getSemanticDiagnostics(filename));
        }

        output.outputFiles.forEach(o => {
            let script = new Atomic.File(o.name, Atomic.FILE_WRITE);
            try {
                script.writeString(o.text);
                script.flush();
            } finally {
                script.close();
            }
        });
        return allDiagnostics;
    }

    /**
     * Logs errors from the diagnostics returned from the compile/transpile process
     * @param {ts.Diagnostic} diagnostics information about the errors
     * @return {[type]}          [description]
     */
    logErrors(diagnostics: ts.Diagnostic[]) {
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

    /**
     * Inject this language service into the registry
     * @return {[type]}             True if successful
     */
    initialize(serviceRegistry: ExtensionServices.ServiceLocatorType) {
        serviceRegistry.resourceServices.register(this);
    }

    /**
     * Called once a resource has been saved
     * @param  {EditorEvents.SaveResourceEvent} ev
     */
    save(ev: EditorEvents.SaveResourceEvent) {
        console.log(`${this.name}: received a save resource event for ${ev.path}`);
        if (this.fullCompile) {
            this.compile([ev.path], {
                noEmitOnError: true,
                noImplicitAny: false,
                target: ts.ScriptTarget.ES5,
                module: ts.ModuleKind.CommonJS,
                noLib: true
            });
        } else {
            this.transpile([ev.path], {
                noEmitOnError: false,
                noImplicitAny: false,
                target: ts.ScriptTarget.ES5,
                module: ts.ModuleKind.CommonJS,
                noLib: true
            });
        }
    }

    /**
     * Can this service extension handle the save event for the resource?
     * @param  {EditorEvents.SaveResourceEvent} ev the
     * @return {boolean}                return true if this service can handle the resource
     */
    canSave(ev: EditorEvents.SaveResourceEvent): boolean {
        const ext = Atomic.getExtension(ev.path);
        //if (ext == ".ts" && !ev.path.match("\.d\.ts$")) {
        if (ext == ".ts") {
            return true;
        }
        return false;
    }
}
