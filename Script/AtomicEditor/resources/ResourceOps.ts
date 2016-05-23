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

import EditorEvents = require("../editor/EditorEvents");

class ResourceOps extends Atomic.ScriptObject {

    constructor() {

        super();

        this.subscribeToEvent("AssetImportError", (ev: ToolCore.AssetImportErrorEvent) => {

            resourceOps.sendEvent(EditorEvents.ModalError, { title: "Asset Import Error", message: ev.error });

        });

    }


}

var resourceOps = new ResourceOps();

export function CreateNewFolder(resourcePath: string, reportError: boolean = true): boolean {

    var title = "New Folder Error";

    var fs = Atomic.getFileSystem();

    if (fs.dirExists(resourcePath) || fs.fileExists(resourcePath)) {
        if (reportError)
            resourceOps.sendEvent(EditorEvents.ModalError, { title: title, message: "Already exists: " + resourcePath });
        return false;

    }

    if (!fs.createDir(resourcePath)) {

        if (reportError)
            resourceOps.sendEvent(EditorEvents.ModalError, { title: title, message: "Could not create " + resourcePath });

        return false;
    }

    var db = ToolCore.getAssetDatabase();
    db.scan();

    return true;

}

export function CreateNewComponent(resourcePath: string, componentName: string, template: Editor.Templates.FileTemplateDefinition, reportError: boolean = true): boolean {

    var title = "New Component Error";

    var fs = Atomic.fileSystem;

    if (fs.dirExists(resourcePath) || fs.fileExists(resourcePath)) {
        if (reportError)
            resourceOps.sendEvent(EditorEvents.ModalError, { title: title, message: "Already exists: " + resourcePath });
        return false;

    }

    var templateFilename = template.filename;
    var file = Atomic.cache.getFile(templateFilename);

    if (!file) {

        if (reportError)
            resourceOps.sendEvent(EditorEvents.ModalError, { title: title, message: "Failed to open template: " + templateFilename });
        return false;

    }

    var out = new Atomic.File(resourcePath, Atomic.FILE_WRITE);
    var success = out.copy(file);
    out.close();

    if (!success) {
        if (reportError)
            resourceOps.sendEvent(EditorEvents.ModalError, { title: title, message: "Failed template copy: " + templateFilename + " -> " + resourcePath });
        return false;
    }

    ToolCore.assetDatabase.scan();

    return true;

}

export function CreateNewScript(resourcePath: string, scriptName: string, template: Editor.Templates.FileTemplateDefinition, reportError: boolean = true): boolean {

    var title = "New Script Error";

    var fs = Atomic.fileSystem;

    if (fs.dirExists(resourcePath) || fs.fileExists(resourcePath)) {
        if (reportError)
            resourceOps.sendEvent(EditorEvents.ModalError, { title: title, message: "Already exists: " + resourcePath });
        return false;

    }

    var templateFilename = template.filename;
    var file = Atomic.cache.getFile(templateFilename);

    if (!file) {

        if (reportError)
            resourceOps.sendEvent(EditorEvents.ModalError, { title: title, message: "Failed to open template: " + templateFilename });
        return false;

    }

    var out = new Atomic.File(resourcePath, Atomic.FILE_WRITE);
    var success = out.copy(file);
    out.close();

    if (!success) {
        if (reportError)
            resourceOps.sendEvent(EditorEvents.ModalError, { title: title, message: "Failed template copy: " + templateFilename + " -> " + resourcePath });
        return false;
    }

    ToolCore.assetDatabase.scan();

    return true;

}

export function CreateNewScene(resourcePath: string, sceneName: string, reportError: boolean = true): boolean {

    var title = "New Scene Error";

    var fs = Atomic.fileSystem;

    if (fs.dirExists(resourcePath) || fs.fileExists(resourcePath)) {
        if (reportError)
            resourceOps.sendEvent(EditorEvents.ModalError, { title: title, message: "Already exists: " + resourcePath });
        return false;

    }

    var templateFilename = "AtomicEditor/templates/template_scene.scene";
    var file = Atomic.cache.getFile(templateFilename);

    if (!file) {

        if (reportError)
            resourceOps.sendEvent(EditorEvents.ModalError, { title: title, message: "Failed to open template: " + templateFilename });
        return false;

    }

    var out = new Atomic.File(resourcePath, Atomic.FILE_WRITE);
    var success = out.copy(file);
    out.close();

    if (!success) {
        if (reportError)
            resourceOps.sendEvent(EditorEvents.ModalError, { title: title, message: "Failed template copy: " + templateFilename + " -> " + resourcePath });
        return false;
    }

    ToolCore.assetDatabase.scan();

    return true;

}

export function CreateNewMaterial(resourcePath: string, materialName: string, reportError: boolean = true): boolean {

    var title = "New Material Error";

    var fs = Atomic.fileSystem;

    if (fs.dirExists(resourcePath) || fs.fileExists(resourcePath)) {
        if (reportError)
            resourceOps.sendEvent(EditorEvents.ModalError, { title: title, message: "Already exists: " + resourcePath });
        return false;

    }

    var templateFilename = "AtomicEditor/templates/template_material.material";
    var file = Atomic.cache.getFile(templateFilename);

    if (!file) {

        if (reportError)
            resourceOps.sendEvent(EditorEvents.ModalError, { title: title, message: "Failed to open template: " + templateFilename });
        return false;

    }

    var out = new Atomic.File(resourcePath, Atomic.FILE_WRITE);
    var success = out.copy(file);
    out.close();

    if (!success) {
        if (reportError)
            resourceOps.sendEvent(EditorEvents.ModalError, { title: title, message: "Failed template copy: " + templateFilename + " -> " + resourcePath });
        return false;
    }

    ToolCore.assetDatabase.scan();

    return true;

}
