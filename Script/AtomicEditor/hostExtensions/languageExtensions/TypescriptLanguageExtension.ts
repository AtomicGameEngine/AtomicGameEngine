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
 * Default compiler options to use for compilation.  If there
 * is a compiler option block in a tsconfig.json located in the project,
 * then the one in the project will overwrite these
 * @type {ts.CompilerOptions}
 */
function getDefaultCompilerOptions() {
    return {
        noEmitOnError: true,
        noImplicitAny: false,
        target: "es5",
        module: "commonjs",
        declaration: false,
        inlineSourceMap: false,
        sourceMap: false,
        removeComments: false,
        noLib: false,
        forceConsistentCasingInFileNames: true,
        allowJs: true,
        lib: ["es5"]
    };
}

/**
 * Resource extension that supports the web view typescript extension
 */
export default class TypescriptLanguageExtension extends Atomic.ScriptObject implements Editor.HostExtensions.ResourceServicesEventListener, Editor.HostExtensions.ProjectServicesEventListener {
    name: string = "HostTypeScriptLanguageExtension";
    description: string = "This service supports the typescript webview extension.";

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
        let hasJsFiles = false;
        const slashedProjectPath = Atomic.addTrailingSlash(ToolCore.toolSystem.project.projectPath);

        //scan all the files in the project for any typescript files and add them to the project
        Atomic.fileSystem.scanDir(ToolCore.toolSystem.project.resourcePath, "*.ts", Atomic.SCAN_FILES, true).forEach(filename => {
            // Don't grab d.ts files yet.  We'll do that in a separate pass
            if (filename.search(/d.ts$/i) == -1) {
                projectFiles.push(Atomic.addTrailingSlash(ToolCore.toolSystem.project.resourcePath) + filename);
                if (!this.isTypescriptProject) {
                    this.isTypescriptProject = true;
                }
            }
        });

        //Scan for any d.ts files in the root
        Atomic.fileSystem.scanDir(ToolCore.toolSystem.project.projectPath, "*.d.ts", Atomic.SCAN_FILES, true).forEach(filename => {
            projectFiles.push(Atomic.addTrailingSlash(ToolCore.toolSystem.project.projectPath) + filename);
            if (!this.isTypescriptProject) {
                this.isTypescriptProject = true;
            }
        });

        let compilerOptions = getDefaultCompilerOptions();
        Atomic.fileSystem.scanDir(ToolCore.toolSystem.project.resourcePath, "*.js", Atomic.SCAN_FILES, true).forEach(filename => {
            let fn = Atomic.addTrailingSlash(ToolCore.toolSystem.project.resourcePath) + filename;
            // if the .js file matches up to a .ts file already loaded, then skip it
            let tsfn = filename.replace(/\.js$/, ".ts");
            if (projectFiles.indexOf(tsfn) == -1) {
                hasJsFiles = true;
                projectFiles.push(fn);
            }
        });

        // First we need to load in a copy of the lib.es6.d.ts that is necessary for the hosted typescript compiler
        projectFiles.push(Atomic.addTrailingSlash(Atomic.addTrailingSlash(ToolCore.toolEnvironment.toolDataDir) + "TypeScriptSupport") + "lib.es5.d.ts");


        const tsconfigFn = Atomic.addTrailingSlash(ToolCore.toolSystem.project.projectPath) + "tsconfig.json";
        // Let's look for a tsconfig.json file in the project root and add any additional files
        if (Atomic.fileSystem.fileExists(tsconfigFn)) {
            // load up the tsconfig file and parse out the files block and compare it to what we have
            // in resources
            const file = new Atomic.File(tsconfigFn, Atomic.FileMode.FILE_READ);
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

        // set the base url
        compilerOptions["baseUrl"] = ToolCore.toolSystem.project.resourcePath;

        // enable JS files in the TS Compiler if we have js files
        compilerOptions.allowJs = hasJsFiles;

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
    private configureProjectMenu() {
        if (!this.menuCreated) {
            if (this.isTypescriptProject) {
                const isCompileOnSave = this.serviceRegistry.projectServices.getUserPreference(this.name, "CompileOnSave", false);

                // Build the menu - First build up an empty menu then manually add the items so we can have reference to them
                const menu = this.serviceRegistry.uiServices.createPluginMenuItemSource("TypeScript", {});
                this.compileOnSaveMenuItem = new Atomic.UIMenuItem(`Compile on Save: ${isCompileOnSave ? "On" : "Off"}`, `${this.name}.compileonsave`);
                menu.addItem(this.compileOnSaveMenuItem);
                menu.addItem(new Atomic.UIMenuItem("Compile Project", `${this.name}.compileproject`));
                menu.addItem(new Atomic.UIMenuItem("Generate External Editor Project", `${this.name}.generateexternalproject`));
                this.menuCreated = true;
            } else {
                // Build the menu - First build up an empty menu then manually add the items so we can have reference to them
                const menu = this.serviceRegistry.uiServices.createPluginMenuItemSource("JavaScript", {});
                menu.addItem(new Atomic.UIMenuItem("Generate External Editor Project", `${this.name}.generateexternalproject`));
                this.menuCreated = true;
            }
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
     * @param  {Editor.EditorEditResourceEvent} ev
     */
    edit(ev: Editor.EditorEditResourceEvent) {
        if (this.isValidFiletype(ev.path)) {
            // update ts config in case we have a new resource
            let tsConfig = this.buildTsConfig();
            this.setTsConfigOnWebView(tsConfig);
            this.configureProjectMenu();
        }
    }

    /**
     * Handle the delete.  This should delete the corresponding javascript file
     * @param  {Editor.EditorDeleteResourceEvent} ev
     */
    delete(ev: Editor.EditorDeleteResourceEvent) {
        if (this.isValidFiletype(ev.path)) {
            // console.log(`${this.name}: received a delete resource event`);

            // Delete the corresponding js file
            let jsFile = ev.path.replace(/\.ts$/, ".js");
            let jsFileAsset = ToolCore.assetDatabase.getAssetByPath(jsFile);
            if (jsFileAsset) {
                console.log(`${this.name}: deleting corresponding .js file`);
                ToolCore.assetDatabase.deleteAsset(jsFileAsset);

                let eventData: Editor.EditorDeleteResourceNotificationEvent = {
                    path: jsFile
                };

                this.setTsConfigOnWebView(this.buildTsConfig());
                this.sendEvent(Editor.EditorDeleteResourceNotificationEventData(eventData));
            }
        }
    }

    /**
     * Handle the rename.  Should rename the corresponding .js file
     * @param  {Editor.EditorRenameResourceNotificationEvent} ev
     */
    rename(ev: Editor.EditorRenameResourceNotificationEvent) {
        if (this.isValidFiletype(ev.path)) {
            // console.log(`${this.name}: received a rename resource event`);

            // Rename the corresponding js file
            let jsFile = ev.path.replace(/\.ts$/, ".js");
            let jsFileNew = ev.newPath.replace(/\.ts$/, ".js"); // rename doesn't want extension
            let jsFileAsset = ToolCore.assetDatabase.getAssetByPath(jsFile);
            if (jsFileAsset) {
                console.log(`${this.name}: renaming corresponding .js file`);
                jsFileAsset.rename(ev.newName);

                let eventData: Editor.EditorRenameResourceNotificationEvent = {
                    path: jsFile,
                    newPath: jsFileNew,
                    newName: ev.newName,
                    asset: jsFileAsset
                };

                this.setTsConfigOnWebView(this.buildTsConfig());
                this.sendEvent(Editor.EditorRenameResourceNotificationEventData(eventData));
            }
        }
    }

    /**
     * Handles the save event and detects if a typescript file has been added to a non-typescript project
     * @param  ev
     */
    save(ev: Editor.EditorSaveResourceEvent) {
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
     * Called when the project is being loaded to allow the typescript language service to reset and
     * possibly compile
     */
    projectLoaded(ev: Editor.EditorLoadProjectEvent) {
        // got a load, we need to reset the language service
        this.isTypescriptProject = false;
        //scan all the files in the project for any typescript files so we can determine if this is a typescript project
        if (Atomic.fileSystem.scanDir(ToolCore.toolSystem.project.resourcePath, "*.ts", Atomic.SCAN_FILES, true).length > 0) {
            this.isTypescriptProject = true;
        };

        this.configureProjectMenu();
    }


    /**
     * Called when the project is unloaded
     */
    projectUnloaded() {
        // Clean up
        if (this.isTypescriptProject) {
            this.serviceRegistry.uiServices.removePluginMenuItemSource("TypeScript");
        } else {
            this.serviceRegistry.uiServices.removePluginMenuItemSource("JavaScript");
        }

        this.compileOnSaveMenuItem = null;
        this.menuCreated = false;
        this.isTypescriptProject = false;
        this.unsubscribeFromAllEvents();
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
                case "generateexternalproject":
                    this.generateExternalProject();
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

    generateExternalProject() {
        const projectDir = ToolCore.toolSystem.project.projectPath;

        // Create the typings folder in project root
        const projectDirTypings = Atomic.addTrailingSlash(projectDir + "typings/ambient/atomicgameengine");
        Atomic.getFileSystem().createDir(projectDirTypings);

        // Copy the Atomic.d.ts definition file to the typings folder
        const toolDataDir = ToolCore.toolEnvironment.toolDataDir;
        const typescriptSupportDir = Atomic.addTrailingSlash(toolDataDir + "TypeScriptSupport");
        Atomic.getFileSystem().copy(typescriptSupportDir + "Atomic.d.ts", projectDirTypings + "Atomic.d.ts");

        // Generate a tsconfig.json file
        if (this.isTypescriptProject) {
            const tsconfigFile = new Atomic.File(projectDir + "tsconfig.json", Atomic.FileMode.FILE_WRITE);

            let tsconfig = {
                compilerOptions: getDefaultCompilerOptions()
            };

            // Don't use fully qualified path in the persistent tsconfig file, just use a relative path from the tsconfig
            tsconfig.compilerOptions["baseUrl"] = "./Resources";
            tsconfig.compilerOptions.allowJs = false;
            tsconfig.compilerOptions.sourceMap = true;

            tsconfigFile.writeString(JSON.stringify(tsconfig, null, 4));
            tsconfigFile.close();
        } else {
            const jsconfigFile = new Atomic.File(projectDir + "jsconfig.json", Atomic.FileMode.FILE_WRITE);
            let jsconfig = {
                "compilerOptions": {
                    "target": "es5"
                }
            };

            // Don't use fully qualified path in the persistent jsconfig file, just use a relative path from the jsconfig
            jsconfig.compilerOptions["baseUrl"] = "./Resources";

            jsconfigFile.writeString(JSON.stringify(jsconfig, null, 4));
            jsconfigFile.close();
        }

        this.generateVsCodeFiles();
    }

    /**
     * Generates the extra vscode files for launching the debugger, the player, etc
     */
    generateVsCodeFiles() {
        const projectDir = ToolCore.toolSystem.project.projectPath;
        const vscodeDir = Atomic.addTrailingSlash(projectDir + ".vscode");
        Atomic.fileSystem.createDir(vscodeDir);

        // Build out the vscode tasks.json
        const taskFile = {
            "version": "0.1.0",
            "tasks": []
        };

        if (this.isTypescriptProject) {
            taskFile.tasks.push({
                "taskName": "Build",
                "command": "tsc",
                "isShellCommand": true,
                "args": [
                    "-p",
                    "."
                ],
                "showOutput": "always",
                "problemMatcher": "$tsc",
                "isBuildCommand": true
            });
        };

        taskFile.tasks.push({
            "taskName": "Debug Atomic Player",
            "command": `${ToolCore.toolEnvironment.editorBinary}`,
            "args": [
                "--player",
                "--debug",
                "--project",
                "${workspaceRoot}"
            ],
            "isBackground": true
        });

        taskFile.tasks.push({
            "taskName": "Launch Atomic Player",
            "command": `${ToolCore.toolEnvironment.editorBinary}`,
            "args": [
                "--player",
                "--project",
                "${workspaceRoot}"
            ],
            "isBackground": true
        });

        const tasksFile = new Atomic.File(vscodeDir + "tasks.json", Atomic.FileMode.FILE_WRITE);
        tasksFile.writeString(JSON.stringify(taskFile, null, 4));
        tasksFile.close();

        // Build vscode launch.json
        const launch = {
            "version": "0.2.0",
            "configurations": [
                {
                    "name": "Attach",
                    "type": "duk",
                    "request": "attach",
                    "address": "localhost",
                    "port": 9091,
                    "localRoot": "${workspaceRoot}/Resources",
                    "sourceMaps": this.isTypescriptProject, // turn on source maps if we are a TypeScript project
                    "outDir": "${workspaceRoot}/Resources",
                    "stopOnEntry": false,
                    "debugLog": true
                }
            ]
        };

        const launchFile = new Atomic.File(vscodeDir + "launch.json", Atomic.FileMode.FILE_WRITE);
        launchFile.writeString(JSON.stringify(launch, null, 4));
        launchFile.close();
    }

    /**
     * Perform a full compile of the TypeScript
     */
    doFullCompile() {
        const editor = this.serviceRegistry.uiServices.getCurrentResourceEditor();
        if (editor && editor.typeName == "JSResourceEditor" && this.isValidFiletype(editor.fullPath)) {
            this.sendEvent(Editor.EditorModalEventData({
                type: Editor.EDITOR_MODALINFO,
                title: "Compiling TypeScript",
                message: "Compiling TypeScript..."
            }));

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
        let links = {};
        let linkId = 0;

        // choose different colors based upon if we are in a dark theme or light theme
        let successColor = "#00ff00";
        let errorColor = "#e3e02b";
        let infoColor = "#888888";
        const currentSkin = this.serviceRegistry.projectServices.getApplicationPreference("uiData", "skinPath", "");
        if (currentSkin.indexOf("light") != -1) {
            successColor = "#006600";
            errorColor = "#7f5f04";
            infoColor = "#333333";
        }

        let errors = false;
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
            let message = `<color ${infoColor}>${file}: </color>`;
            if (result.type == "success") {
                message += `<color ${successColor}>${result.text}</color>`;
            } else {
                message += `<color ${errorColor}>${result.text} at line ${result.row + 1} col ${result.column}</color> <widget TBSkinImage: skin: MagnifierBitmap, text:"..." id: link${linkId}>`;
                links["link" + linkId] = result;
                linkId++;
                errors = true;
            }
            return message;
        }).join("\n");

        if (messageArray.length == 0) {
            messageArray = "Success";
        }

        if (errors) {
            this.sendEvent(Editor.EditorModalEventData({
                type: Editor.EDITOR_MODALINFO,
                title: "Compiling TypeScript",
                message: "Errors detected while compiling TypeScript."
            }));
        } else {
            this.sendEvent(Editor.EditorModalEventData({
                type: Editor.EDITOR_MODALINFO,
                title: "Compiling TypeScript",
                message: "Successfully compiled TypeScript."
            }));
        }

        let message = [
            "Compiler Options: ",
            JSON.stringify(results.compilerOptions, null, 2),
            "",
            messageArray,
            "",
            `Compilation Completed in ${results.duration}ms`
        ].join("\n");

        let window = this.serviceRegistry.uiServices.showNonModalWindow("TypeScript Compilation Results", "AtomicEditor/editor/ui/typescriptresults.tb.txt", (ev: Atomic.UIWidgetEvent) => {
            if (ev.type == Atomic.UI_EVENT_TYPE.UI_EVENT_TYPE_CLICK) {
                if (ev.target.id == "close") {
                    window.close();
                } else {
                    let diag = links[ev.target.id];
                    if (diag) {
                        console.log(`Load editor: ${diag.file}:${diag.row + 1}`);
                        this.serviceRegistry.uiServices.loadResourceEditor(diag.file, diag.row + 1);
                    }
                }
            }
        });

        let textField = window.getWidget<Atomic.UIEditField>("msg");
        textField.setText(message);
        textField.reformat(true);
    }
}
