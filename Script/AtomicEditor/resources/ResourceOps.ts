import EditorEvents = require("../editor/EditorEvents");

class ResourceOps extends Atomic.ScriptObject {


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

    resourceOps.sendEvent(EditorEvents.ResourceFolderCreated, { path: resourcePath});

    return true;

}
