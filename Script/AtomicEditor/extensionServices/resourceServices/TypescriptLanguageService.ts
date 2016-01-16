//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//
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
    fullCompile: boolean = false;

    /**
     * used by the compile to build a registery of all of the project files
     * @return {[type]} [description]
     */
    refreshProjectFiles() {
        if (!this.projectFiles) {
            this.projectFiles = [];

            //scan all the files in the project
            Atomic.fileSystem.scanDir(ToolCore.toolSystem.project.resourcePath, "*.ts", Atomic.SCAN_FILES, true).forEach(filename => {
                this.projectFiles.push(Atomic.addTrailingSlash(ToolCore.toolSystem.project.resourcePath) + filename);
            });

            Atomic.fileSystem.scanDir(ToolCore.toolSystem.project.projectPath, "*.d.ts", Atomic.SCAN_FILES, true).forEach(filename => {
                if (filename.indexOf("node_modules") == -1) {
                    this.projectFiles.push(Atomic.addTrailingSlash(ToolCore.toolSystem.project.projectPath) + filename);
                }
            });

            // initialize the list of files
            this.projectFiles.forEach(fileName => {
                this.versionMap[fileName] = { version: 0 };
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
            let diagnostics: ts.Diagnostic[] = [];
            let result = ts.transpile(script.readText(), options, fileName, diagnostics);
            this.logErrors(fileName, diagnostics);

            if (diagnostics.length == 0) {
                let output = new Atomic.File(fileName.replace(".ts", ".js"), Atomic.FILE_WRITE);
                output.writeString(result);
                output.flush();
                output.close();
            }
            script.close();
        });
    }

    /**
     * Compile the provided file to javascript with full type checking etc
     * @param  {string}  filename the file name and path to compile
     * @return {boolean}          true on success
     */
    compile(fileNames: string[], options: ts.CompilerOptions): void {

        //scan all the files in the project
        this.refreshProjectFiles();

        // Create the language service host to allow the LS to communicate with the host
        const servicesHost: ts.LanguageServiceHost = {
            getScriptFileNames: () => this.projectFiles,
            getScriptVersion: (fileName) => this.versionMap[fileName] && this.versionMap[fileName].version.toString(),
            getScriptSnapshot: (fileName) => {
                if (!Atomic.fileSystem.exists(fileName)) {
                    console.log(`${fileName} does not exist`);
                    return undefined;
                }
                console.log(`${fileName} exists`);
                let script = new Atomic.File(fileName, Atomic.FILE_READ);
                return ts.ScriptSnapshot.fromString(script.readText());
            },
            getCurrentDirectory: () => ToolCore.toolSystem.project.resourcePath,
            getCompilationSettings: () => options,
            getDefaultLibFileName: (options) => undefined
        };

        // Create the language service files
        this.languageService = ts.createLanguageService(servicesHost, ts.createDocumentRegistry());

        fileNames.forEach(fileName => {
            try {
                this.emitFile(fileName);
            } catch (err) {
                console.log(`${this.name}: problem encountered compiling ${fileName}: ${err}`);
                console.log(err.stack);
            }
        });
    }

    /**
     * writes out a file from the compile process
     * @param  {string} fileName [description]
     * @return {[type]}          [description]
     */
    emitFile(fileName: string) {
        let output = this.languageService.getEmitOutput(fileName);

        if (!output.emitSkipped) {
            console.log(`${this.name}: Emitting ${fileName}`);
        }
        else {
            console.log(`${this.name}: Emitting ${fileName} failed`);
            let allDiagnostics = this.languageService.getCompilerOptionsDiagnostics()
                .concat(this.languageService.getSyntacticDiagnostics(fileName))
                .concat(this.languageService.getSemanticDiagnostics(fileName));
            this.logErrors(fileName, allDiagnostics);
        }

        output.outputFiles.forEach(o => {
            console.log(`${this.name}: Writing out ${o.name}`);
            let script = new Atomic.File(o.name, Atomic.FILE_WRITE);
            script.writeString(o.text);
            // fs.writeFileSync(o.name, o.text, "utf8");
        });
    }

    /**
     * Logs errors from the diagnostics returned from the compile/transpile process
     * @param  {string} fileName [description]
     * @return {[type]}          [description]
     */
    logErrors(fileName: string, diagnostics: ts.Diagnostic[]) {

        diagnostics.forEach(diagnostic => {
            let message = ts.flattenDiagnosticMessageText(diagnostic.messageText, "\n");
            if (diagnostic.file) {
                let { line, character } = diagnostic.file.getLineAndCharacterOfPosition(diagnostic.start);
                console.log(`${this.name}:  Error ${diagnostic.file.fileName} (${line + 1},${character + 1}): ${message}`);
            }
            else {
                console.log(`${this.name}  Error: ${message}`);
            }
        });
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
                noEmitOnError: false,
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
        if (ext == ".ts") {
            return true;
        }
        return false;
    }
}
