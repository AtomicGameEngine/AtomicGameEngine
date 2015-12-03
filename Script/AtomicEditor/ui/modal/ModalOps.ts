//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import EditorUI = require("../EditorUI");
import ModalWindow = require("./ModalWindow");
import About = require("./About");
import NewProject = require("./NewProject");
import CreateProject = require("./CreateProject");

import EULAWindow = require("./license/EULAWindow");
import ActivationWindow = require("./license/ActivationWindow");
import ActivationSuccessWindow = require("./license/ActivationSuccessWindow");
import ManageLicense = require("./license/ManageLicense");
import Pro3DWindow = require("./license/Pro3DWindow");
import ProPlatformWindow = require("./license/ProPlatformWindow");

import BuildWindow = require("./build/BuildWindow");
import BuildOutput = require("./build/BuildOutput");
import BuildSettingsWindow = require("./build/BuildSettingsWindow");

import ResourceSelection = require("./ResourceSelection");

import UIResourceOps = require("./UIResourceOps");

import SnapSettingsWindow = require("./SnapSettingsWindow");

class ModalOps extends Atomic.ScriptObject {

    constructor() {

        super();

        this.dimmer = new Atomic.UIDimmer();

    }

    showCreateProject(projectTemplateFolder: string, imagePath: string = "") {

        if (this.show()) {

            this.opWindow = new CreateProject(projectTemplateFolder, imagePath);

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


    showResourceSelection(windowText: string, importerType: string, resourceType:string, callback: (retObject: any, args: any) => void, args: any = undefined) {

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

    showActivationWindow() {

        if (this.show()) {

            this.opWindow = new ActivationWindow();

        }

    }

    showManageLicense() {

        if (this.show()) {

            this.opWindow = new ManageLicense();

        }

    }

    showPro3DWindow() {

        if (this.show()) {

            this.opWindow = new Pro3DWindow();

        }

    }

    showProPlatformWindow() {

        if (this.show()) {

            this.opWindow = new ProPlatformWindow();

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


    showActivationSuccessWindow() {

        if (this.show()) {

            this.opWindow = new ActivationSuccessWindow();

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
