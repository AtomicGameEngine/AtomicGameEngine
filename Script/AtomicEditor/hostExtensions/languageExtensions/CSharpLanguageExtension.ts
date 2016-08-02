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
* Resource extension that supports the web view typescript extension
*/
export default class CSharpLanguageExtension implements Editor.HostExtensions.ResourceServicesEventListener, Editor.HostExtensions.ProjectServicesEventListener {
    name: string = "HostCSharpLanguageExtension";
    description: string = "This service supports the csharp language.";

    /**
    * Indicates if this project contains NET files.
    * @type {Boolean}
    */
    private isNETProject = false;
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
        if (ext == ".cs" || ext == ".dll") {
            return true;
        }
    }

    /**
    * Configures the project to be a Typescript Project
    * @return {[type]}
    */
    private configureNETProjectMenu() {

        if (this.isNETProject && !this.menuCreated) {

            // Build the menu - First build up an empty menu then manually add the items so we can have reference to them
            const menu = this.serviceRegistry.uiServices.createPluginMenuItemSource("AtomicNET", {});
            menu.addItem(new Atomic.UIMenuItem("Open Solution", `${this.name}.opensolution`));
            menu.addItem(new Atomic.UIMenuItem("Compile Project", `${this.name}.compileproject`));
            menu.addItem(new Atomic.UIMenuItem("Generate Solution", `${this.name}.generatesolution`));
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
    * This could be when someone adds a CS or assembly file to a vanilla project
    * @param  {Editor.EditorEvents.EditResourceEvent} ev
    */
    edit(ev: Editor.EditorEvents.EditResourceEvent) {
        if (this.isValidFiletype(ev.path)) {

            if (this.isNETProject) {
                this.configureNETProjectMenu();
            }
        }
    }

    /**
    * Handle the delete.  This should delete the corresponding javascript file
    * @param  {Editor.EditorEvents.DeleteResourceEvent} ev
    */
    delete(ev: Editor.EditorEvents.DeleteResourceEvent) {

    }

    /**
    * Handle the rename.  Should rename the corresponding .js file
    * @param  {Editor.EditorEvents.RenameResourceEvent} ev
    */
    rename(ev: Editor.EditorEvents.RenameResourceEvent) {

    }

    /**
    * Handles the save event and detects if a cs file has been added to a non-csharp project
    * @param  {Editor.EditorEvents.SaveResourceEvent} ev
    * @return {[type]}
    */
    save(ev: Editor.EditorEvents.SaveResourceEvent) {

        // let's check to see if we have created a csharp file
        if (!this.isNETProject) {
            if (Atomic.getExtension(ev.path) == ".cs") {
                this.isNETProject = true;
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
        this.isNETProject = false;

        let found = false;

        //scan all the files in the project for any csharp or assembly files so we can determine if this is a csharp project
        if (Atomic.fileSystem.scanDir(ToolCore.toolSystem.project.resourcePath, "*.cs", Atomic.SCAN_FILES, true).length > 0) {
            found = true;
        } else if (Atomic.fileSystem.scanDir(ToolCore.toolSystem.project.resourcePath, "*.dll", Atomic.SCAN_FILES, true).length > 0) {
            found = true;
        };

        if (found) {
            this.isNETProject = true;
            this.configureNETProjectMenu();
        }
    }


    /**
    * Called when the project is unloaded
    */
    projectUnloaded() {
        // Clean up
        this.serviceRegistry.uiServices.removePluginMenuItemSource("AtomicNET");
        this.menuCreated = false;
        this.isNETProject = false;
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
                case "compileproject":
                    this.doFullCompile();
                    return true;
                case "opensolution":
                    this.openSolution();
                    return true;
                case "generatesolution":
                    this.generateSolution();
                    return true;

            }
        }
    }

    /**
    * Perform a full compile of the solution
    */
    doFullCompile() {

        if (ToolCore.netProjectSystem)
            ToolCore.netProjectSystem.buildAtomicProject();

    }
    /**
    * Open Solution
    */
    openSolution() {

        if (ToolCore.netProjectSystem) {
            ToolCore.netProjectSystem.openSolution();
        }

    }

    /**
    * Generate Solution
    */
    generateSolution() {

        if (ToolCore.netProjectSystem) {
            ToolCore.netProjectSystem.generateSolution();
        }

    }


}
