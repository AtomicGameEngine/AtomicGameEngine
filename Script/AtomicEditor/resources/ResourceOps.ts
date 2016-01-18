//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
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

export function CreateNewComponent(resourcePath: string, componentName: string, reportError: boolean = true): boolean {

    var title = "New Component Error";

    var fs = Atomic.fileSystem;

    if (fs.dirExists(resourcePath) || fs.fileExists(resourcePath)) {
        if (reportError)
            resourceOps.sendEvent(EditorEvents.ModalError, { title: title, message: "Already exists: " + resourcePath });
        return false;

    }

    var templateFilename = "AtomicEditor/templates/template_component.js";
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

export function CreateNewScript(resourcePath: string, scriptName: string, reportError: boolean = true): boolean {

    var title = "New Script Error";

    var fs = Atomic.fileSystem;

    if (fs.dirExists(resourcePath) || fs.fileExists(resourcePath)) {
        if (reportError)
            resourceOps.sendEvent(EditorEvents.ModalError, { title: title, message: "Already exists: " + resourcePath });
        return false;

    }

    var templateFilename = "AtomicEditor/templates/template_script.js";
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
