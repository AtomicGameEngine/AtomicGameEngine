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

import * as EditorEvents from "../../editor/EditorEvents";

/**
 * Default compiler options to use for compilation.  If there
 * is a compiler option block in a tsconfig.json located in the project,
 * then the one in the project will overwrite these
 * @type {ts.CompilerOptions}
 */
const defaultCompilerOptions = {
    noEmitOnError: true,
    noImplicitAny: false,
    target: "es5",
    module: "commonjs",
    declaration: false,
    inlineSourceMap: false,
    removeComments: false,
    noLib: true,
    allowNonTsExtensions: true,
    allowJs: true
};

/**
 * Resource extension that supports the web view typescript extension
 */
export default class TypescriptLanguageExtension implements Editor.HostExtensions.ResourceServicesEventListener, Editor.HostExtensions.ProjectServicesEventListener {
    name: string = "HostTypeScriptLanguageExtension";
    description: string = "This service supports the typscript webview extension.";

    /**
     * Indicates if this project contains typescript files.
     * @type {Boolean}
     */
    private isTypescriptProject = false;
    private serviceRegistry: Editor.HostExtensions.HostServiceLocator = null;

    private menuCreated = false;
    /** Reference to the compileOnSaveMenuItem */
    private compileOnSaveMenuItem: Atomic.UIMenuItem;

    /**
     * Determines if the file name/path provided is something we care about
     * @param  {string} path
     * @return {boolean}
     */
    private isValidFiletype(path: string): boolean {
        const ext = Atomic.getExtension(path);
        if (ext == ".ts" || ext == ".js") {
            return true;
        }
    }

    /**
     * Build an in-memory config file to be sent down to the web view client.  This will scan the resources directory
     * and generate a file list
     */
    private buildTsConfig(): any {
        // only build out a tsconfig.atomic if we actually have typescript files in the project
        let projectFiles: Array<string> = [];


        //scan all the files in the project for any typescript files and add them to the project
        Atomic.fileSystem.scanDir(ToolCore.toolSystem.project.resourcePath, "*.ts", Atomic.SCAN_FILES, true).forEach(filename => {
            projectFiles.push(Atomic.addTrailingSlash(ToolCore.toolSystem.project.resourcePath) + filename);
            if (!this.isTypescriptProject) {
                this.isTypescriptProject = true;
            }
        });

        let compilerOptions = defaultCompilerOptions;
        Atomic.fileSystem.scanDir(ToolCore.toolSystem.project.resourcePath, "*.js", Atomic.SCAN_FILES, true).forEach(filename => {
            let fn = Atomic.addTrailingSlash(ToolCore.toolSystem.project.resourcePath) + filename;
            projectFiles.push(Atomic.addTrailingSlash(ToolCore.toolSystem.project.resourcePath) + filename);
        });

        // First we need to load in a copy of the lib.core.d.ts that is necessary for the hosted typescript compiler
        projectFiles.push(Atomic.addTrailingSlash(Atomic.addTrailingSlash(ToolCore.toolEnvironment.toolDataDir) + "TypeScriptSupport") + "lib.core.d.ts");

        const slashedProjectPath = Atomic.addTrailingSlash(ToolCore.toolSystem.project.projectPath);

        const tsconfigFn = Atomic.addTrailingSlash(ToolCore.toolSystem.project.projectPath) + "tsconfig.json";
        // Let's look for a tsconfig.json file in the project root and add any additional files
        if (Atomic.fileSystem.fileExists(tsconfigFn)) {
            // load up the tsconfig file and parse out the files block and compare it to what we have
            // in resources
            const file = new Atomic.File(tsconfigFn, Atomic.FILE_READ);
            try {
                const savedTsConfig = JSON.parse(file.readText());
                if (savedTsConfig["files"]) {
                    savedTsConfig["files"].forEach((file: string) => {
                        let newFile = Atomic.addTrailingSlash(ToolCore.toolSystem.project.projectPath) + file;
                        let exists = false;
                        if (Atomic.fileSystem.fileExists(newFile)) {
                            exists = true;
                            file = newFile;
                        } else if (Atomic.fileSystem.exists(file)) {
                            exists = true;
                        }
                        if (exists && projectFiles.indexOf(file) == -1) {
                            projectFiles.push(file);
                        }
                    });
                }

                // override the default options if the tsconfig contains them
                if (savedTsConfig["compilerOptions"]) {
                    compilerOptions = savedTsConfig["compilerOptions"];
                }
            } finally {
                file.close();
            }
        };

        // Then see if we have a copy of Atomic.d.ts in the project directory.  If we don't then we should load it up from the tool environment
        let found = false;
        projectFiles.forEach((file) => {
            if (file.toLowerCase().indexOf("atomic.d.ts") != -1) {
                found = true;
            }
        });

        if (!found) {
            // Load up the Atomic.d.ts from the tool core
            projectFiles.push(Atomic.addTrailingSlash(Atomic.addTrailingSlash(ToolCore.toolEnvironment.toolDataDir) + "TypeScriptSupport") + "Atomic.d.ts");
        }

        let tsConfig = {
            compilerOptions: compilerOptions,
            files: projectFiles
        };
        return tsConfig;
    }

    /**
     * Configures the project to be a Typescript Project
     * @return {[type]}
     */
    private configureTypescriptProjectMenu() {
        if (this.isTypescriptProject && !this.menuCreated) {
            const isCompileOnSave = this.serviceRegistry.projectServices.getUserPreference(this.name, "CompileOnSave", false);

            // Build the menu - First build up an empty menu then manually add the items so we can have reference to them
            const menu = this.serviceRegistry.uiServices.createPluginMenuItemSource("TypeScript", {});
            this.compileOnSaveMenuItem = new Atomic.UIMenuItem(`Compile on Save: ${isCompileOnSave ? "On" : "Off"}`, `${this.name}.compileonsave`);
            menu.addItem(this.compileOnSaveMenuItem);
            menu.addItem(new Atomic.UIMenuItem("Compile Project", `${this.name}.compileproject`));
            this.menuCreated = true;
        }
    }

    /**
     * Inject this language service into the registry
     * @return {[type]}             True if successful
     */
    initialize(serviceLocator: Editor.HostExtensions.HostServiceLocator) {
        // We care about both resource events as well as project events
        serviceLocator.resourceServices.register(this);
        serviceLocator.projectServices.register(this);
        serviceLocator.uiServices.register(this);
        this.serviceRegistry = serviceLocator;
    }

    /**
     * Handle when a new file is loaded and we have not yet configured the editor for TS.
     * This could be when someone adds a TS file to a vanilla project
     * @param  {Editor.EditorEvents.EditResourceEvent} ev
     */
    edit(ev: Editor.EditorEvents.EditResourceEvent) {
        if (this.isValidFiletype(ev.path)) {
            // update ts config in case we have a new resource
            let tsConfig = this.buildTsConfig();
            this.setTsConfigOnWebView(tsConfig);

            if (this.isTypescriptProject) {
                this.configureTypescriptProjectMenu();
            }
        }
    }

    /**
     * Handle the delete.  This should delete the corresponding javascript file
     * @param  {Editor.EditorEvents.DeleteResourceEvent} ev
     */
    delete(ev: Editor.EditorEvents.DeleteResourceEvent) {
        if (this.isValidFiletype(ev.path)) {
            // console.log(`${this.name}: received a delete resource event`);

            // Delete the corresponding js file
            let jsFile = ev.path.replace(/\.ts$/, ".js");
            let jsFileAsset = ToolCore.assetDatabase.getAssetByPath(jsFile);
            if (jsFileAsset) {
                console.log(`${this.name}: deleting corresponding .js file`);
                ToolCore.assetDatabase.deleteAsset(jsFileAsset);

                let eventData: EditorEvents.DeleteResourceEvent = {
                    path: jsFile
                };

                this.setTsConfigOnWebView(this.buildTsConfig());
                this.serviceRegistry.sendEvent(EditorEvents.DeleteResourceNotification, eventData);
            }
        }
    }

    /**
     * Handle the rename.  Should rename the corresponding .js file
     * @param  {Editor.EditorEvents.RenameResourceEvent} ev
     */
    rename(ev: Editor.EditorEvents.RenameResourceEvent) {
        if (this.isValidFiletype(ev.path)) {
            // console.log(`${this.name}: received a rename resource event`);

            // Rename the corresponding js file
            let jsFile = ev.path.replace(/\.ts$/, ".js");
            let jsFileNew = ev.newPath.replace(/\.ts$/, ".js"); // rename doesn't want extension
            let jsFileAsset = ToolCore.assetDatabase.getAssetByPath(jsFile);
            if (jsFileAsset) {
                console.log(`${this.name}: renaming corresponding .js file`);
                jsFileAsset.rename(ev.newName);

                let eventData: EditorEvents.RenameResourceEvent = {
                    path: jsFile,
                    newPath: jsFileNew,
                    newName: ev.newName,
                    asset: jsFileAsset
                };

                this.setTsConfigOnWebView(this.buildTsConfig());
                this.serviceRegistry.sendEvent(EditorEvents.RenameResourceNotification, eventData);
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
                this.setTsConfigOnWebView(this.buildTsConfig());
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
        this.isTypescriptProject = false;
        //scan all the files in the project for any typescript files so we can determine if this is a typescript project
        if (Atomic.fileSystem.scanDir(ToolCore.toolSystem.project.resourcePath, "*.ts", Atomic.SCAN_FILES, true).length > 0) {
            this.isTypescriptProject = true;
            this.configureTypescriptProjectMenu();
        };
    }


    /**
     * Called when the project is unloaded
     */
    projectUnloaded() {
        // Clean up
        this.serviceRegistry.uiServices.removePluginMenuItemSource("TypeScript");
        this.compileOnSaveMenuItem = null;
        this.menuCreated = false;
        this.isTypescriptProject = false;
    }

    /*** UIService implementation ***/

    /**
     * Called when a plugin menu item is clicked
     * @param  {string} refId
     * @return {boolean}
     */
    menuItemClicked(refId: string): boolean {
        let [extension, action] = refId.split(".");
        if (extension == this.name) {
            switch (action) {
                case "compileonsave":
                    let isCompileOnSave = this.serviceRegistry.projectServices.getUserPreference(this.name, "CompileOnSave", false);
                    // Toggle
                    isCompileOnSave = !isCompileOnSave;
                    this.serviceRegistry.projectServices.setUserPreference(this.name, "CompileOnSave", isCompileOnSave);
                    this.compileOnSaveMenuItem.string = `Compile on Save: ${isCompileOnSave ? "On" : "Off"}`;
                    return true;
                case "compileproject":
                    this.doFullCompile();
                    return true;
            }
        }
    }

    /**
     * Handle messages that are submitted via Atomic.Query from within a web view editor.
     * @param message The message type that was submitted to be used to determine what the data contains if present
     * @param data any additional data that needs to be submitted with the message
     */
    handleWebMessage(messageType: string, data: any) {
        switch (messageType) {
            case "TypeScript.DisplayCompileResults":
                this.displayCompileResults(data);
                break;
        }
    }

    setTsConfigOnWebView(tsConfig: any) {
        WebView.WebBrowserHost.setGlobalStringProperty("TypeScriptLanguageExtension", "tsConfig", JSON.stringify(tsConfig));
    }

    /**
     * Perform a full compile of the TypeScript
     */
    doFullCompile() {
        const editor = this.serviceRegistry.uiServices.getCurrentResourceEditor();
        if (editor && editor.typeName == "JSResourceEditor" && this.isValidFiletype(editor.fullPath)) {
            const jsEditor = <Editor.JSResourceEditor>editor;
            jsEditor.webView.webClient.executeJavaScript(`TypeScript_DoFullCompile('${JSON.stringify(this.buildTsConfig())}');`);
        } else {
            this.serviceRegistry.uiServices.showModalError("TypeScript Compilation", "Please open a TypeScript file in the editor before attempting to do a full compile.");
        }

        // Ideally, we would want to either launch up a background web view, or shell out to node or something and not
        // need to have an editor open.  Still researching this
        /*
            const url = `atomic://${ToolCore.toolEnvironment.toolDataDir}CodeEditor/Editor.html`;
            const webClient = new WebView.WebClient();
            this.webClient = webClient;
            //this.webClient.loadURL(url);

            const webTexture = new WebView.WebTexture2D();
            webClient.webRenderHandler = webTexture;

            // doesn't work because atomicquery doesn't seem to be exposed to WebView.WebClient instances
            webClient.subscribeToEvent(EditorEvents.WebMessage, (data) => {
                switch (data.message) {
                    case "editorLoadComplete":
                        webClient.unsubscribeFromEvent(EditorEvents.WebMessage);
                        webClient.executeJavaScript(`TypeScript_DoFullCompile();`);
                        break;
                }
            });

            webClient.createBrowser(url, 1, 1);
        */
    }

    /**
     * Display the results of the compilation step
     * @param  {any[]} annotations
     */
    displayCompileResults(results: {
        annotations: any[],
        compilerOptions: any,
        duration: number
    }) {
        // get the name of the resources directory without preceding path
        let resourceDir = ToolCore.toolSystem.project.resourcePath.replace(Atomic.addTrailingSlash(ToolCore.toolSystem.project.projectPath), "");
        let messageArray = results.annotations.filter(result => {
            // If we are compiling the lib.d.ts or some other built-in library and it was successful, then
            // we really don't need to display that result since it's just noise.  Only display it if it fails
            if (result.type == "success") {
                return result.file.indexOf(ToolCore.toolSystem.project.projectPath) == 0;
            }
            return true;
        }).map(result => {

            // Clean up the path for display
            let file = result.file.replace(ToolCore.toolSystem.project.projectPath, "");
            let message = `<color #888888>${file}: </color>`;
            if (result.type == "success") {
                message += `<color #00ff00>${result.text}</color>`;
            } else {
                message += `<color #e3e02b>${result.text} at line ${result.row} col ${result.column}</color>`;
            }
            return message;
        }).join("\n");

        if (messageArray.length == 0) {
            messageArray = "Success";
        }

        let message = [
            "Compiler Options: ",
            JSON.stringify(results.compilerOptions, null, 2),
            "",
            messageArray,
            "",
            `Compilation Completed in ${results.duration}ms`
        ].join("\n");

        this.serviceRegistry.uiServices.showModalError("TypeScript Compilation Results", message);
    }
}
