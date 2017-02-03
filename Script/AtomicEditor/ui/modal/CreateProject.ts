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

import EditorUI = require("../EditorUI");
import ModalWindow = require("./ModalWindow");
import Preferences = require("../../editor/Preferences");

import ProjectTemplates = require("../../resources/ProjectTemplates");

class CreateProject extends ModalWindow {

    constructor(projectTemplate: ProjectTemplates.ProjectTemplateDefinition, projectPath?: string) {

        super();

        this.projectPath = projectPath;
        this.projectTemplate = projectTemplate;
        this.defaultPath = Preferences.getInstance().editorFeatures.defaultPath;
        this.defaultLanguage = Preferences.getInstance().editorFeatures.defaultLanguage;

        this.init("Create Project", "AtomicEditor/editor/ui/createproject.tb.txt");

        this.projectPathField = <Atomic.UIEditField>this.getWidget("project_path");
        this.projectNameField = <Atomic.UIEditField>this.getWidget("project_name");
        this.appIDField = <Atomic.UIEditField>this.getWidget("app_id");
        this.projectLanguageField = <Atomic.UISelectDropdown>this.getWidget("project_language");
        this.image = <Atomic.UIImageWidget>this.getWidget("project_image");

        this.desktopButton = this.addPlatformButton("desktop", "AtomicEditor/editor/images/Desktop128.png");
        this.desktopButton.value = 1;
        this.androidButton = this.addPlatformButton("android", "AtomicEditor/editor/images/Android128.png");
        this.iosButton = this.addPlatformButton("ios", "AtomicEditor/editor/images/iOS128.png");
        this.html5Button = this.addPlatformButton("html5", "AtomicEditor/editor/images/HTML5128.png");

        if (!projectTemplate.screenshot)
            this.image.visibility = Atomic.UI_WIDGET_VISIBILITY.UI_WIDGET_VISIBILITY_GONE;
        else
            this.image.image = projectTemplate.screenshot;

        // If we're specifying where to put the project (initially), then we are opening
        // an example directly so use the same name
        if (projectPath) {
            this.projectNameField.text = projectTemplate.name;
        }

        this.projectPathField.text = (projectPath) ? projectPath : this.defaultPath;
        this.populateLanguageSelectionList();

        // Need to manually set the focus so the contents get auto-selected
        this.projectNameField.setFocus();

        this.resizeToFitContent();
        this.center();

    }

    addPlatformButton(platformName:string, platformLogo:string):Atomic.UIButton {

        var platformcontainer = <Atomic.UILayout>this.getWidget("platformcontainer");

        // IMAGE BUTTON

        var id = platformName;
        var size = 92;

        var button = new Atomic.UIButton();
        button.id = id;
        button.toggleMode = true;

        var lp = new Atomic.UILayoutParams();
        lp.minWidth = size;
        lp.minHeight = size;

        button.layoutParams = lp;

        button.gravity = Atomic.UI_GRAVITY.UI_GRAVITY_ALL;

        var image = new Atomic.UIImageWidget();
        image.image = platformLogo;
        var rect = [0, 0, size, size];
        image.rect = rect;
        button.addChild(image);

        if (platformName != "desktop") {
            var greenplus = new Atomic.UIImageWidget();
            greenplus.image = "AtomicEditor/editor/images/green_plus.png";
            rect = [size - 18, 2, size - 2, 18];
            greenplus.rect = rect;
            greenplus.visibility = Atomic.UI_WIDGET_VISIBILITY.UI_WIDGET_VISIBILITY_INVISIBLE;
            button.addChild(greenplus);
            button["greenPlus"] = greenplus;
        }

        platformcontainer.addChild(button);

        return button;

    }


    tryProjectCreate(): boolean {

        var name = this.projectNameField.text.trim();

        if (!name) {
            EditorUI.showModalError("New Project Editor Error", "Please enter a project name");
            return false;
        }

        var folder = this.projectPathField.text.trim();

        var projectPath = this.projectPath;

        // if we changed the project path, clear it
        if (folder != this.projectPath) {
            projectPath = "";
        }

        if (!folder) {
            EditorUI.showModalError("New Project Editor Error", "Please choose a root project folder");
            return false;
        }

        var fileSystem = Atomic.getFileSystem();

        if (!projectPath) {

            folder += "/" + name;

            if (fileSystem.dirExists(folder) || fileSystem.fileExists(folder)) {

                var message = folder + " exists\n\nPlease choose a different root folder or project name";
                EditorUI.showModalError("New Project Editor Error", message);
                return false;

            }
        }

        folder = Atomic.addTrailingSlash(folder);

        // Determine if we have a language template for the selected language.
        let selectedLanguage = this.projectLanguageField.text;

        // Check whether we have a required IDE installed for C# projects
        var atomicNET = false;
        if (selectedLanguage == "CSharp" || selectedLanguage == "C#") {

            atomicNET = true;
            if (!ToolCore.netProjectSystem.getIDEAvailable()) {
                this.hide();
                EditorUI.getModelOps().showAtomicNETWindow();
                return false;
            }
        }

        let templateFolder = "";
        for (let i = 0; i < this.projectTemplate.languages.length; i++) {
            if (this.projectTemplate.languages[i] === selectedLanguage) {
                templateFolder = this.projectTemplate.folder + selectedLanguage + "/";
                break;
            }
        }

        // Do the creation!
        if (templateFolder != "" && fileSystem.dirExists(templateFolder)) {

            if (!projectPath) {
                if (!fileSystem.copyDir(templateFolder, folder)) {
                    var message = "Unable to copy folder: " + templateFolder + " to " + folder;
                    EditorUI.showModalError("New Project Editor Error", message);
                    return false;
                }
            }

            var utils = new Editor.FileUtils();

            utils.createDirs(folder + "Cache");
            utils.createDirs(folder + "Settings");

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
                var file = new Atomic.File(folder + name + ".atomic", Atomic.FileMode.FILE_WRITE);
                file.close();
            }

            // Look for a .userprefs file and if it exists, then rename it
            fileResults = fileSystem.scanDir(folder, "*.userprefs", Atomic.SCAN_FILES, false);
            if (fileResults.length === 1) {
                fileSystem.rename(folder + fileResults[0], folder + name + ".userprefs");
            }

            // create project settings

            var platforms = ["desktop"];

            if (this.androidButton.value == 1) {
                platforms.push("android");
            }

            if (this.iosButton.value == 1) {
                platforms.push("ios");
            }

            var projectSettings = {
                name : name,
                platforms : platforms
            };

            var jsonFile = new Atomic.File(folder + "Settings/Project.json", Atomic.FileMode.FILE_WRITE);
            if (jsonFile.isOpen()) {
                jsonFile.writeString(JSON.stringify(projectSettings, null, 2));
                jsonFile.flush();
                jsonFile.close();
            }

            // Generate AtomicNET project if necessary
            if (atomicNET) {
                if (!ProjectTemplates.generateAtomicNETProject({
                    name: name,
                    appID : this.appIDField.text,
                    platforms : platforms,
                    projectFolder : folder,
                    projectTemplate : this.projectTemplate
                })) {
                    var message = "Unable to generate AtomicNET project: " + folder;
                    EditorUI.showModalError("New Project Editor Error", message);
                    return false;
                }
            }

            this.hide();

            this.sendEvent(Editor.EditorLoadProjectEventData({ path: folder }));
            return true;
        } else {
            let message = [
                "Unable to create project for:",
                "",
                `language: ${selectedLanguage}`,
                `template: ${templateFolder}`,
                "",
                "Please choose a different language."
            ].join("\n");

            EditorUI.showModalError("New Project Editor Error", message);
            return false;
        }

    }

    handleLanguageSwitch(selectedLanguage:string) {

        if (selectedLanguage == "CSharp" || selectedLanguage == "C#") {

            this.html5Button["greenPlus"].visibility = Atomic.UI_WIDGET_VISIBILITY.UI_WIDGET_VISIBILITY_INVISIBLE;
            this.html5Button.value = 0;
            this.html5Button.disable();

        } else {

            this.html5Button.enable();
            this.html5Button["greenPlus"].visibility = this.html5Button.value == 1 ? Atomic.UI_WIDGET_VISIBILITY.UI_WIDGET_VISIBILITY_VISIBLE : Atomic.UI_WIDGET_VISIBILITY.UI_WIDGET_VISIBILITY_INVISIBLE;

        }

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE.UI_EVENT_TYPE_CLICK) {

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
                if (this.tryProjectCreate()) {
                    let needsWrite = false;
                    let prefs = Preferences.getInstance();
                    if (prefs.editorFeatures.defaultLanguage != this.projectLanguageField.text) {
                        prefs.editorFeatures.defaultLanguage = this.projectLanguageField.text;
                        needsWrite = true;
                    }
                    if (prefs.editorFeatures.defaultPath != this.projectPathField.text) {
                        prefs.editorFeatures.defaultPath = this.projectPathField.text;
                        needsWrite = true;
                    }
                    if (needsWrite) prefs.write();
                    this.hide();
                }

                return true;

            }
        } else if (ev.type == Atomic.UI_EVENT_TYPE.UI_EVENT_TYPE_CHANGED) {

            // handle language change
            if (ev.target.id == "project_language") {
                this.handleLanguageSwitch(this.projectLanguageField.text);
            }

            if (ev.target.id == "desktop") {

                // desktop is always selected
                this.desktopButton.value = 1;

            } else if (ev.target["greenPlus"]) {
                ev.target["greenPlus"].visibility = ev.target.value == 1 ? Atomic.UI_WIDGET_VISIBILITY.UI_WIDGET_VISIBILITY_VISIBLE : Atomic.UI_WIDGET_VISIBILITY.UI_WIDGET_VISIBILITY_INVISIBLE;
            }


        }
    }

    /**
     * Queries the json file for languages that are available to this template and populates the
     * list.
     */
    populateLanguageSelectionList() {

        this.projectLanguageFieldSource.clear();

        this.projectTemplate.languages.forEach(language => {
            this.projectLanguageFieldSource.addItem(new Atomic.UISelectItem(language));
        });

        this.projectLanguageField.source = this.projectLanguageFieldSource;
        this.projectLanguageField.value = 0;
        let defrank = -1;
        let jsrank = -1;
        let csrank = -1;
        let tsrank = -1;
        let ii = 0;
        for ( ii = 0; ii < this.projectLanguageFieldSource.getItemCount(); ii++ ) { // get rankings
            if ( this.projectLanguageFieldSource.getItemStr( ii ) == "JavaScript" ) jsrank = ii;
            if ( this.projectLanguageFieldSource.getItemStr( ii ) == "CSharp" ) csrank = ii;
            if ( this.projectLanguageFieldSource.getItemStr( ii ) == "TypeScript" ) tsrank = ii;
        }

        if ( this.defaultLanguage == "JavaScript" ) defrank = jsrank; // which is the default language
        if ( this.defaultLanguage == "CSharp" ) defrank = csrank;
        if ( this.defaultLanguage == "TypeScript" ) defrank = tsrank;

        if ( defrank > -1 ) this.projectLanguageField.value = defrank;  // the default language is present
        else if ( jsrank > -1 ) this.projectLanguageField.value = jsrank;  // js is present
        else if ( csrank > -1 ) this.projectLanguageField.value = csrank;  // cs is present
        else if ( tsrank > -1 ) this.projectLanguageField.value = tsrank;  // ts is present
    }

    projectPathField: Atomic.UIEditField;
    projectNameField: Atomic.UIEditField;
    appIDField: Atomic.UIEditField;
    projectLanguageField: Atomic.UISelectDropdown;
    projectLanguageFieldSource: Atomic.UISelectItemSource = new Atomic.UISelectItemSource();
    image: Atomic.UIImageWidget;

    desktopButton: Atomic.UIButton;
    androidButton: Atomic.UIButton;
    iosButton: Atomic.UIButton;
    html5Button: Atomic.UIButton;

    // if we have specified a projectPath, the dest will not be the combination of path + name
    projectPath: string;
    projectTemplate: ProjectTemplates.ProjectTemplateDefinition;
    defaultPath: string;
    defaultLanguage: string;
}


export = CreateProject;
