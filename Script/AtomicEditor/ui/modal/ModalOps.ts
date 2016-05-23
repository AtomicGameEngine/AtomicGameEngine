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
import About = require("./About");
import NewProject = require("./NewProject");
import CreateProject = require("./CreateProject");

import EULAWindow = require("./license/EULAWindow");
import NewBuildWindow = require("./license/NewBuildWindow");

import BuildWindow = require("./build/BuildWindow");
import BuildOutput = require("./build/BuildOutput");
import BuildSettingsWindow = require("./build/BuildSettingsWindow");

import ResourceSelection = require("./ResourceSelection");

import UIResourceOps = require("./UIResourceOps");

import SnapSettingsWindow = require("./SnapSettingsWindow");

import ExtensionWindow = require("./ExtensionWindow");

import ProjectTemplates = require("../../resources/ProjectTemplates");


class ModalOps extends Atomic.ScriptObject {

    constructor() {

        super();

        this.dimmer = new Atomic.UIDimmer();

    }

    showCreateProject(projectTemplateDefinition: ProjectTemplates.ProjectTemplateDefinition) {

        if (this.show()) {

            this.opWindow = new CreateProject(projectTemplateDefinition);

        }

    }

    showCreateFolder(resourcePath: string) {

        if (this.show()) {

            this.opWindow = new UIResourceOps.CreateFolder(resourcePath);

        }

    }

    showCreateComponent(resourcePath: string) {

        if (this.show()) {

            this.opWindow = new UIResourceOps.CreateComponent(resourcePath);

        }

    }

    showCreateScript(resourcePath: string) {

        if (this.show()) {

            this.opWindow = new UIResourceOps.CreateScript(resourcePath);

        }

    }

    showCreateScene(resourcePath: string) {

        if (this.show()) {

            this.opWindow = new UIResourceOps.CreateScene(resourcePath);

        }

    }

    showCreateMaterial(resourcePath: string) {

        if (this.show()) {

            this.opWindow = new UIResourceOps.CreateMaterial(resourcePath);

        }

    }


    showResourceDelete(asset: ToolCore.Asset) {

        if (this.show()) {

            this.opWindow = new UIResourceOps.ResourceDelete(asset);

        }

    }

    showRenameAsset(asset: ToolCore.Asset) {

        if (this.show()) {

            this.opWindow = new UIResourceOps.RenameAsset(asset);

        }

    }


    showResourceSelection(windowText: string, importerType: string, resourceType: string, callback: (retObject: any, args: any) => void, args: any = undefined) {

        if (this.show()) {

            this.opWindow = new ResourceSelection(windowText, importerType, resourceType, callback, args);

        }

    }

    showNewProject() {

        if (this.show()) {

            this.opWindow = new NewProject();

        }
    }

    showEULAWindow() {

        if (this.show()) {

            this.opWindow = new EULAWindow();

        }

    }

    showNewBuildWindow() {

        if (this.show()) {

            this.opWindow = new NewBuildWindow();

        }

    }

    showAbout() {

        if (this.show()) {

            this.opWindow = new About();

        }

    }

    showBuild() {

        if (!ToolCore.toolSystem.project)
            return;

        if (this.show()) {

            this.opWindow = new BuildWindow();

        }

    }


    showBuildSettings() {

        if (!ToolCore.toolSystem.project)
            return;

        if (this.show()) {

            this.opWindow = new BuildSettingsWindow.BuildSettingsWindow();

        }

    }

    showBuildOutput(buildBase: ToolCore.BuildBase) {

        if (this.show()) {

            this.opWindow = new BuildOutput(buildBase);

        }

    }

    showSnapSettings() {

        // only show snap settings if we have a project loaded
        if (!ToolCore.toolSystem.project)
            return;

        if (this.show()) {

            this.opWindow = new SnapSettingsWindow();

        }

    }

    // TODO: standardize  to same pattern as other modal windows
    showError(windowText: string, message: string) {
        var view = EditorUI.getView();
        var window = new Atomic.UIMessageWindow(view, "modal_error");
        window.show(windowText, message, Atomic.UI_MESSAGEWINDOW_SETTINGS_OK, true, 640, 360);
    }

    showExtensionWindow(windowText: string, uifilename: string, handleWidgetEventCB: (ev: Atomic.UIWidgetEvent) => void): Editor.Modal.ExtensionWindow {
        if (this.show()) {

            this.opWindow = new ExtensionWindow(windowText, uifilename, handleWidgetEventCB);
            return this.opWindow;
        }
    }

    private show(): boolean {

        if (this.dimmer.parent) {

            console.log("WARNING: attempting to show modal while dimmer is active");
            return false;

        }

        if (this.opWindow) {

            console.log("WARNING: attempting to show modal while another opWindow is active");
            return false;

        }

        var view = EditorUI.getView();
        view.addChild(this.dimmer);

        return true;

    }

    hide() {

        if (this.opWindow) {

            var window = this.opWindow;
            this.opWindow = null;

            if (window.parent)
                window.parent.removeChild(window, false);

            var view = EditorUI.getView();
            view.setFocusRecursive();

        }

        if (this.dimmer.parent) {

            this.dimmer.parent.removeChild(this.dimmer, false);

        }


    }

    dimmer: Atomic.UIDimmer;
    opWindow: ModalWindow;

}

export = ModalOps;
