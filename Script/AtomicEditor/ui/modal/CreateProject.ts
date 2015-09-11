//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import EditorEvents = require("../../editor/EditorEvents");
import EditorUI = require("../EditorUI");
import ModalWindow = require("./ModalWindow");

class CreateProject extends ModalWindow {

    constructor(templateSourceDir: string, imagePath: string = "") {

        super();

        this.templateSourceDir = templateSourceDir;

        this.init("Create Project", "AtomicEditor/editor/ui/createproject.tb.txt");

        this.projectPathField = <Atomic.UIEditField> this.getWidget("project_path");
        this.projectNameField = <Atomic.UIEditField> this.getWidget("project_name");
        this.image = <Atomic.UIImageWidget> this.getWidget("project_image");

        if (!imagePath)
            this.image.visibility = Atomic.UI_WIDGET_VISIBILITY_GONE;
        else
            this.image.image = imagePath;


        var fileSystem = Atomic.getFileSystem();

        var userDocuments = fileSystem.userDocumentsDir;

        if (Atomic.platform == "MacOSX") {

            userDocuments += "Documents/AtomicProjects";

        } else {

            userDocuments += "AtomicProjects";

        }

        this.projectPathField.text = userDocuments;

        this.resizeToFitContent();
        this.center();

    }

    tryProjectCreate(): boolean {

        var name = this.projectNameField.text.trim();

        if (!name) {
            EditorUI.showModalError("New Project Editor Error", "Please enter a project name");
            return false;
        }

        var folder = this.projectPathField.text.trim();

        if (!folder) {
            EditorUI.showModalError("New Project Editor Error", "Please choose a root project folder");
            return false;
        }

        folder += "/" + name;

        var fileSystem = Atomic.getFileSystem();

        if (fileSystem.dirExists(folder) || fileSystem.fileExists(folder)) {

            var message = folder + " exists\n\nPlease choose a different root folder or project name";
            EditorUI.showModalError("New Project Editor Error", message);
            return false;

        }


        folder = Atomic.addTrailingSlash(folder);

        if (!fileSystem.dirExists(folder)) {

            var utils = new Editor.FileUtils();
            utils.createDirs(folder);

            utils.createDirs(folder + "Cache");

            if (!fileSystem.dirExists(folder)) {
                var message = "Unable to create folder: " + folder + "\n\nPlease choose a different root folder or project name";
                EditorUI.showModalError("New Project Editor Error", message);
                return false;
            }
        }

        // Do the creation!

        fileSystem.copyDir(this.templateSourceDir + "Resources", folder + "Resources");

        var file = new Atomic.File(folder + name + ".atomic", Atomic.FILE_WRITE);
        file.close();

        this.hide();

        this.sendEvent(EditorEvents.LoadProject, { path: folder });

        return true;

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var id = ev.target.id;

            if (id == "cancel") {
                this.hide();
                return true;
            }
            else if (id == "choose_path") {
                var utils = new Editor.FileUtils();
                var path = utils.newProjectFileDialog();

                if (path.length)
                    this.projectPathField.text = path;

                return true;
            }
            else if (id == "create") {

                this.tryProjectCreate();

                return true;

            }
        }
    }

    projectPathField: Atomic.UIEditField;
    projectNameField: Atomic.UIEditField;
    image: Atomic.UIImageWidget;

    templateSourceDir: string;
}


export = CreateProject;
