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

import EditorEvents = require("../../editor/EditorEvents");
import EditorUI = require("../EditorUI");
import ModalWindow = require("./ModalWindow");

import ProjectTemplates = require("../../resources/ProjectTemplates");

class CreateProject extends ModalWindow {

    constructor(projectTemplate: ProjectTemplates.ProjectTemplateDefinition) {

        super();

        this.projectTemplate = projectTemplate;

        this.init("Create Project", "AtomicEditor/editor/ui/createproject.tb.txt");

        this.projectPathField = <Atomic.UIEditField>this.getWidget("project_path");
        this.projectNameField = <Atomic.UIEditField>this.getWidget("project_name");
        this.projectLanguageField = <Atomic.UISelectDropdown>this.getWidget("project_language");
        this.image = <Atomic.UIImageWidget>this.getWidget("project_image");

        if (!projectTemplate.screenshot)
            this.image.visibility = Atomic.UI_WIDGET_VISIBILITY_GONE;
        else
            this.image.image = projectTemplate.screenshot;


        var fileSystem = Atomic.getFileSystem();

        var userDocuments = fileSystem.userDocumentsDir;

        if (Atomic.platform == "MacOSX") {

            userDocuments += "Documents/AtomicProjects";

        } else {

            userDocuments += "AtomicProjects";

        }

        this.projectPathField.text = userDocuments;
        this.populateLanguageSelectionList();

        // Need to manually set the focus so the contents get auto-selected
        this.projectNameField.setFocus();

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

            // Determine if we have a language template for the selected language.
            let templateDetail: ProjectTemplates.ProjectTemplateDetail;
            let selectedLanguage = this.projectLanguageField.text;

            for (let i = 0; i < this.projectTemplate.templates.length; i++) {
                if (this.projectTemplate.templates[i].language === selectedLanguage) {
                    templateDetail = this.projectTemplate.templates[i];
                    break;
                }
            }

            // Do the creation!
            if (templateDetail && fileSystem.dirExists(templateDetail.folder)) {

                if (!fileSystem.copyDir(templateDetail.folder, folder)) {
                  var message = "Unable to copy folder: " + templateDetail.folder + " to " + folder;
                  EditorUI.showModalError("New Project Editor Error", message);
                  return false;
                }

                var utils = new Editor.FileUtils();

                utils.createDirs(folder + "Cache");

                if (!fileSystem.dirExists(folder)) {
                    var message = "Unable to create folder: " + folder + "\n\nPlease choose a different root folder or project name";
                    EditorUI.showModalError("New Project Editor Error", message);
                    return false;
                }

                // Look for the .atomic project file and if it exists, then rename it
                let fileResults = fileSystem.scanDir(folder, "*.atomic", Atomic.SCAN_FILES, false);
                if (fileResults.length === 1) {
                    fileSystem.rename(folder + fileResults[0], folder + name + ".atomic");
                } else {
                    // Just create the file.  We either don't have one existing, or we have more than one and don't know which one to rename
                    var file = new Atomic.File(folder + name + ".atomic", Atomic.FILE_WRITE);
                    file.close();
                }

                // Look for a .userprefs file and if it exists, then rename it
                fileResults = fileSystem.scanDir(folder, "*.userprefs", Atomic.SCAN_FILES, false);
                if (fileResults.length === 1) {
                    fileSystem.rename(folder + fileResults[0], folder + name + ".userprefs");
                }

                this.hide();

                this.sendEvent(EditorEvents.LoadProject, { path: folder });
                return true;
            } else {
                let message = [
                    "Unable to create project for:",
                    "",
                    `language: ${selectedLanguage}`,
                    `template: ${templateDetail.folder}`,
                    "",
                    "Please choose a different language."
                ].join("\n");

                EditorUI.showModalError("New Project Editor Error", message);
                return false;
            }
        }
        return false;
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

    /**
     * Queries the json file for languages that are available to this template and populates the
     * list.
     */
    populateLanguageSelectionList() {
        this.projectLanguageFieldSource.clear();

        this.projectTemplate.templates.forEach(templateDetail => {
            this.projectLanguageFieldSource.addItem(new Atomic.UISelectItem(templateDetail.language));
        });

        this.projectLanguageField.source = this.projectLanguageFieldSource;
        this.projectLanguageField.value = 0;
    }

    projectPathField: Atomic.UIEditField;
    projectNameField: Atomic.UIEditField;
    projectLanguageField: Atomic.UISelectDropdown;
    projectLanguageFieldSource: Atomic.UISelectItemSource = new Atomic.UISelectItemSource();
    image: Atomic.UIImageWidget;

    projectTemplate: ProjectTemplates.ProjectTemplateDefinition;
}


export = CreateProject;
