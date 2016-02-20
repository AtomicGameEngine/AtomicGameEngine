//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

/**
 * Resource extension that supports the web view typescript extension
 */
export default class TypescriptLanguageExtension implements Editor.HostExtensions.ResourceService, Editor.HostExtensions.ProjectService {
    name: string = "HostTypeScriptLanguageExtension";
    description: string = "This service supports the typscript webview extension.";

    /**
     * Indicates if this project contains typescript files.
     * @type {Boolean}
     */
    private isTypescriptProject = false;

    /**
     * Determines if the file name/path provided is something we care about
     * @param  {string} path
     * @return {boolean}
     */
    private isValidFiletype(path: string): boolean {
        if (this.isTypescriptProject) {
            const ext = Atomic.getExtension(path);
            if (ext == ".ts") {
                return true;
            }
        }
        return false;
    }

    /**
     * Seed the language service with all of the relevant files in the project.  This updates the tsconifg.atomic file in
     * the root of the resources directory.
     */
    private loadProjectFiles() {
        let projectFiles: Array<string> = [];

        //scan all the files in the project for any typescript files so we can determine if this is a typescript project
        Atomic.fileSystem.scanDir(ToolCore.toolSystem.project.resourcePath, "*.ts", Atomic.SCAN_FILES, true).forEach(filename => {
            projectFiles.push(Atomic.addTrailingSlash(ToolCore.toolSystem.project.resourcePath) + filename);
            this.isTypescriptProject = true;
        });

        // only build out a tsconfig.atomic if we actually have typescript files in the project
        if (this.isTypescriptProject) {
            // First we need to load in a copy of the lib.core.d.ts that is necessary for the hosted typescript compiler
            projectFiles.push(Atomic.addTrailingSlash(Atomic.addTrailingSlash(ToolCore.toolEnvironment.toolDataDir) + "TypeScriptSupport") + "lib.core.d.ts");

            // Load up a copy of the duktape.d.ts
            projectFiles.push(Atomic.addTrailingSlash(Atomic.addTrailingSlash(ToolCore.toolEnvironment.toolDataDir) + "TypeScriptSupport") + "duktape.d.ts");


            // Look in a 'typings' directory for any typescript definition files
            const typingsDir = Atomic.addTrailingSlash(ToolCore.toolSystem.project.projectPath) + "typings";
            Atomic.fileSystem.scanDir(typingsDir, "*.d.ts", Atomic.SCAN_FILES, true).forEach(filename => {
                projectFiles.push(Atomic.addTrailingSlash(typingsDir) + filename);
            });

            let files = projectFiles.map((f: string) => {
                if (f.indexOf(ToolCore.toolSystem.project.resourcePath) != -1) {
                    // if we are in the resources directory, just pass back the path from resources down
                    return f.replace(Atomic.addTrailingSlash(ToolCore.toolSystem.project.projectPath), "");
                } else {
                    // otherwise return the full path
                    return f;
                }
            });

            let tsConfig = {
                files: files
            };

            let filename = Atomic.addTrailingSlash(ToolCore.toolSystem.project.resourcePath) + "tsconfig.atomic";
            let script = new Atomic.File(filename, Atomic.FILE_WRITE);
            try {
                script.writeString(JSON.stringify(tsConfig));
                script.flush();
            } finally {
                script.close();
            }
        }
    }

    /**
     * Inject this language service into the registry
     * @return {[type]}             True if successful
     */
    initialize(serviceRegistry: Editor.HostExtensions.HostServiceLocator) {
        // We care about both resource events as well as project events
        serviceRegistry.resourceServices.register(this);
        serviceRegistry.projectServices.register(this);
    }

    /**
     * Handle the delete.  This should delete the corresponding javascript file
     * @param  {Editor.EditorEvents.DeleteResourceEvent} ev
     */
    delete(ev: Editor.EditorEvents.DeleteResourceEvent) {
        if (this.isValidFiletype(ev.path)) {
            console.log(`${this.name}: received a delete resource event`);

            // Delete the corresponding js file
            let jsFile = ev.path.replace(/\.ts$/, ".js");
            let jsFileAsset = ToolCore.assetDatabase.getAssetByPath(jsFile);
            if (jsFileAsset) {
                console.log(`${this.name}: deleting corresponding .js file`);
                ToolCore.assetDatabase.deleteAsset(jsFileAsset);
            }
        }
    }

    /**
     * Handle the rename.  Should rename the corresponding .js file
     * @param  {Editor.EditorEvents.RenameResourceEvent} ev
     */
    rename(ev: Editor.EditorEvents.RenameResourceEvent) {
        if (this.isValidFiletype(ev.path)) {
            console.log(`${this.name}: received a rename resource event`);

            // Rename the corresponding js file
            let jsFile = ev.path.replace(/\.ts$/, ".js");
            let jsFileNew = ev.newPath.replace(/\.ts$/, ".js");
            let jsFileAsset = ToolCore.assetDatabase.getAssetByPath(jsFile);
            if (jsFileAsset) {
                console.log(`${this.name}: renaming corresponding .js file`);
                jsFileAsset.rename(jsFileNew);
            }
        }
    }

    /**
     * Handles the save event and detects if a typescript file has been added to a non-typescript project
     * @param  {Editor.EditorEvents.SaveResourceEvent} ev
     * @return {[type]}
     */
    save(ev: Editor.EditorEvents.SaveResourceEvent) {
        // let's check to see if we have created a typescript file
        if (!this.isTypescriptProject) {
            if (Atomic.getExtension(ev.path) == ".ts") {
                this.isTypescriptProject = true;
                this.loadProjectFiles();
            }
        }
    }

    /*** ProjectService implementation ****/

    /**
     * Called when the project is being loaded to allow the typscript language service to reset and
     * possibly compile
     */
    projectLoaded(ev: Editor.EditorEvents.LoadProjectEvent) {
        // got a load, we need to reset the language service
        console.log(`${this.name}: received a project loaded event for project at ${ev.path}`);
        this.loadProjectFiles();
    }
}
