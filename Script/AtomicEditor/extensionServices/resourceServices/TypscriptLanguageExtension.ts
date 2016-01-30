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
import {TypescriptLanguageService, FileSystemInterface} from "./TypescriptLanguageService";

import * as metrics from "modules/metrics";

/**
 * Class that provides access to the Atomic filesystem routines
 */
class AtomicFileSystem implements FileSystemInterface {
    /**
     * Deterimine if the particular file exists in the resources
     * @param  {string} filename
     * @return {boolean}
     */
    fileExists(filename: string): boolean {
        return !Atomic.fileSystem.exists(filename);

    }

    /**
     * Grab the contents of the file
     * @param  {string} filename
     * @return {string}
     */
    getFile(filename: string): string {
        let script = new Atomic.File(filename, Atomic.FILE_READ);
        try {
            return script.readText();
        } finally {
            script.close();
        }
    }

    /**
     * Write the contents to the file specified
     * @param  {string} filename
     * @param  {string} contents
     */
    writeFile(filename: string, contents: string) {
        let script = new Atomic.File(filename, Atomic.FILE_WRITE);
        try {
            script.writeString(contents);
            script.flush();
        } finally {
            script.close();
        }
    }

}

/**
 * Resource extension that handles compiling or transpling typescript on file save.
 */
export default class TypescriptLanguageExtension implements ExtensionServices.ResourceService, ExtensionServices.ProjectService {
    name: string = "TypeScriptResourceService";
    description: string = "This service transpiles TypeScript into JavaScript on save.";

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

    /**
     * Determines if the file name/path provided is something we care about
     * @param  {string} path
     * @return {boolean}
     */
    private isValidFiletype(path: string): boolean {
        const ext = Atomic.getExtension(path);
        if (ext == ".ts") {
            return true;
        }
        return false;
    }

    /**
     * Seed the language service with all of the relevant files in the project
     */
    private loadProjectFiles() {
        // First we need to load in a copy of the lib.core.d.ts that is necessary for the hosted typescript compiler
        this.languageService.addProjectFile(Atomic.addTrailingSlash(Atomic.addTrailingSlash(ToolCore.toolEnvironment.toolDataDir) + "TypeScriptSupport") + "lib.core.d.ts");

        // Load up a copy of the duktape.d.ts
        this.languageService.addProjectFile(Atomic.addTrailingSlash(Atomic.addTrailingSlash(ToolCore.toolEnvironment.toolDataDir) + "TypeScriptSupport") + "duktape.d.ts");

        //scan all the files in the project
        Atomic.fileSystem.scanDir(ToolCore.toolSystem.project.resourcePath, "*.ts", Atomic.SCAN_FILES, true).forEach(filename => {
            this.languageService.addProjectFile(Atomic.addTrailingSlash(ToolCore.toolSystem.project.resourcePath) + filename);
        });

        // Look in a 'typings' directory for any typescript definition files
        const typingsDir = Atomic.addTrailingSlash(ToolCore.toolSystem.project.projectPath) + "typings";
        Atomic.fileSystem.scanDir(typingsDir, "*.d.ts", Atomic.SCAN_FILES, true).forEach(filename => {
            this.languageService.addProjectFile(Atomic.addTrailingSlash(typingsDir) + filename);
        });
    }

    /**
     * Inject this language service into the registry
     * @return {[type]}             True if successful
     */
    initialize(serviceRegistry: ExtensionServices.ServiceLocatorType) {
        // initialize the language service
        this.languageService = new TypescriptLanguageService(new AtomicFileSystem());

        // We care about both resource events as well as project events
        serviceRegistry.resourceServices.register(this);
        serviceRegistry.projectServices.register(this);
    }

    /*** ResourceService implementation ****/
    /**
     * Can this service extension handle the save event for the resource?
     * @param  {EditorEvents.SaveResourceEvent} ev the
     * @return {boolean}                return true if this service can handle the resource
     */
    canSave(ev: EditorEvents.SaveResourceEvent): boolean {
        return this.isValidFiletype(ev.path);
    }

    /**
     * Called once a resource has been saved
     * @param  {EditorEvents.SaveResourceEvent} ev
     */
    save(ev: EditorEvents.SaveResourceEvent) {
        console.log(`${this.name}: received a save resource event for ${ev.path}`);
        if (this.fullCompile) {
            this.languageService.compile([ev.path], {
                noEmitOnError: true,
                noImplicitAny: false,
                target: ts.ScriptTarget.ES5,
                module: ts.ModuleKind.CommonJS,
                noLib: true
            });
        } else {
            this.languageService.transpile([ev.path], {
                noEmitOnError: false,
                noImplicitAny: false,
                target: ts.ScriptTarget.ES5,
                module: ts.ModuleKind.CommonJS,
                noLib: true
            });
        }
        metrics.logMetrics();
    }

    /**
     * Determine if we care if an asset has been deleted
     * @param  {EditorEvents.DeleteResourceEvent} ev
     * @return {boolean} true if we care
     */
    canDelete(ev: EditorEvents.DeleteResourceEvent): boolean {
        return this.isValidFiletype(ev.path);
    }

    /**
     * Handle the delete.  This should delete the corresponding javascript file
     * @param  {EditorEvents.DeleteResourceEvent} ev
     */
    delete(ev: EditorEvents.DeleteResourceEvent) {
        console.log(`${this.name}: received a delete resource event`);

        // notify the typescript language service that the file has been deleted
        this.languageService.deleteProjectFile(ev.path);

        // Delete the corresponding js file
        let jsFile = ev.path.replace(/\.ts$/, ".js");
        let jsFileAsset = ToolCore.assetDatabase.getAssetByPath(jsFile);
        if (jsFileAsset) {
            console.log(`${this.name}: deleting corresponding .js file`);
            ToolCore.assetDatabase.deleteAsset(jsFileAsset);
        }

    }

    /**
     * Determine if we want to respond to resource renames
     * @param  {EditorEvents.RenameResourceEvent} ev
     * @return {boolean} true if we care
     */
    canRename(ev: EditorEvents.RenameResourceEvent): boolean {
        return this.isValidFiletype(ev.path);
    }

    /**
     * Handle the rename.  Should rename the corresponding .js file
     * @param  {EditorEvents.RenameResourceEvent} ev
     */
    rename(ev: EditorEvents.RenameResourceEvent) {
        console.log(`${this.name}: received a rename resource event`);

        // notify the typescript language service that the file has been renamed
        this.languageService.renameProjectFile(ev.path, ev.newPath);

        // Rename the corresponding js file
        let jsFile = ev.path.replace(/\.ts$/, ".js");
        let jsFileNew = ev.newPath.replace(/\.ts$/, ".js");
        let jsFileAsset = ToolCore.assetDatabase.getAssetByPath(jsFile);
        if (jsFileAsset) {
            console.log(`${this.name}: renaming corresponding .js file`);
            jsFileAsset.rename(jsFileNew);
        }
    }

    /*** ProjectService implementation ****/

    /**
     * Called when the project is being unloaded to allow the typscript language service to reset
     */
    projectUnloaded() {
        // got an unload, we need to reset the language service
        console.log(`${this.name}: received a project unloaded event`);
        this.languageService.reset();
    }

    /**
     * Called when the project is being loaded to allow the typscript language service to reset and
     * possibly compile
     */
    projectLoaded(ev: EditorEvents.LoadProjectEvent) {
        // got a load, we need to reset the language service
        console.log(`${this.name}: received a project loaded event for project at ${ev.path}`);
        this.languageService.reset();
        this.loadProjectFiles();

        //TODO: do we want to run through and compile at this point?
    }


    /**
     * Called when the player is launched
     */
    playerStarted() {
        console.log(`${this.name}: received a player started event for project`);

        // Make sure the project has the latest files
        this.loadProjectFiles();
        if (this.fullCompile) {
            this.languageService.compile(null, {
                noEmitOnError: true,
                noImplicitAny: false,
                target: ts.ScriptTarget.ES5,
                module: ts.ModuleKind.CommonJS,
                noLib: true
            });
        }
    }
}
