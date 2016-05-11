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

/// <reference path="../../../TypeScript/duktape.d.ts" />

import * as EditorEvents from "../../editor/EditorEvents";

/**
 * Resource extension that supports the web view typescript extension
 */
export default class ProjectBasedExtensionLoader implements Editor.HostExtensions.ProjectServicesEventListener {
    name: string = "ProjectBasedExtensionLoader";
    description: string = "This service supports loading extensions that reside in the project under {ProjectRoot}/Editor and named '*.Service.js'.";

    private serviceRegistry: Editor.HostExtensions.HostServiceLocator = null;
    private modSearchRewritten = false;

    /**
     * Prefix to use to detect "special" require paths
     * @type {String}
     */
    private static duktapeRequirePrefix = "project:";

    /**
     * Inject this language service into the registry
     * @return {[type]}             True if successful
     */
    initialize(serviceLocator: Editor.HostExtensions.HostServiceLocator) {

        // Let's rewrite the mod search
        this.rewriteModSearch();

        // We care project events
        serviceLocator.projectServices.register(this);
        this.serviceRegistry = serviceLocator;
    }

    /**
     * Rewrite the duktape modSearch routine so that we can intercept any
     * require calls with a "project:" prefix.  Duktape will fail if it receives
     * a require call with a fully qualified path starting with a "/" (at least on OSX and Linux),
     * so we will need to detect any of these project level requires and allow Atomic to go to the
     * file system and manually pull these in to provide to duktape
     */
    private rewriteModSearch() {
        Duktape.modSearch = (function(origModSearch) {
            return function(id: string, require, exports, module) {
                let system = ToolCore.getToolSystem();
                if (id.indexOf(ProjectBasedExtensionLoader.duktapeRequirePrefix) == 0) {
                    let path = id.substr(ProjectBasedExtensionLoader.duktapeRequirePrefix.length) + ".js";

                    // For safety, only allow bringing modules in from the project directory.  This could be
                    // extended to look for some global extension directory to pull extensions from such as
                    // ~/.atomicExtensions/...
                    if (system.project && path.indexOf(system.project.projectPath) == 0) {
                        console.log(`Searching for project based include: ${path}`);
                        // we have a project based require
                        if (Atomic.fileSystem.fileExists(path)) {
                            let include = new Atomic.File(path, Atomic.FILE_READ);
                            try {
                                return include.readText();
                            } finally {
                                include.close();
                            }
                        } else {
                            throw new Error(`Cannot find project module: ${path}`);
                        }
                    } else {
                        throw new Error(`Extension at ${path} does not reside in the project directory ${system.project.projectPath}`);
                    }
                } else {
                    return origModSearch(id, require, exports, module);
                }
            };
        })(Duktape.modSearch);
    }
    /**
     * Called when the project is being loaded to allow the typscript language service to reset and
     * possibly compile
     */
    projectLoaded(ev: Editor.EditorEvents.LoadProjectEvent) {
        // got a load, we need to reset the language service
        console.log(`${this.name}: received a project loaded event for project at ${ev.path}`);
        let system = ToolCore.getToolSystem();
        if (system.project) {
            let fileSystem = Atomic.getFileSystem();
            let editorScriptsPath = Atomic.addTrailingSlash(system.project.resourcePath) + "EditorData/";
            if (fileSystem.dirExists(editorScriptsPath)) {
                let filenames = fileSystem.scanDir(editorScriptsPath, "*.js", Atomic.SCAN_FILES, true);
                filenames.forEach((filename) => {
                    // Filtered search in Atomic doesn't due true wildcarding, only handles extension filters
                    // in the future this may be better handled with some kind of manifest file
                    if (filename.toLowerCase().lastIndexOf(".plugin.js") >= 0) {
                        var extensionPath = editorScriptsPath + filename;
                        extensionPath = extensionPath.substring(0, extensionPath.length - 3);

                        console.log(`Detected project extension at: ${extensionPath} `);
                        // Note: duktape does not yet support unloading modules,
                        // but will return the same object when passed a path the second time.
                        let resourceServiceModule = require(ProjectBasedExtensionLoader.duktapeRequirePrefix + extensionPath);

                        // Handle situation where the service is either exposed by a typescript default export
                        // or as the module.export (depends on if it is being written in typescript, javascript, es6, etc.)
                        let resourceService: Editor.HostExtensions.HostEditorService = null;
                        if (resourceServiceModule.default) {
                            resourceService = resourceServiceModule.default;
                        } else {
                            resourceService = resourceServiceModule;
                        }
                        this.serviceRegistry.loadService(resourceService);
                    }
                });
            }
        }
    }
}
