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

import EditorEvents = require("editor/EditorEvents");
import EditorUI = require("ui/EditorUI");
import ModalWindow = require("../ModalWindow");
import ProgressModal = require("../ProgressModal");

class ActivationWindow extends ModalWindow {

    constructor() {

        super(true);

        this.init("Product Activation", "AtomicEditor/editor/ui/activation.tb.txt");

        this.licenseKeyEdit = <Atomic.UIEditField>this.getWidget("license_key");

        this.subscribeToEvent("LicenseActivationError", (eventData) => this.handleLicenseActivationError(eventData));
        this.subscribeToEvent("LicenseActivationSuccess", (eventData) => this.handleLicenseActivationSuccess(eventData));

        this.progressModal = new ProgressModal("Activation", "Activating, please wait...");

        if (ToolCore.licenseSystem.sourceBuild) {

            var message = "\nThis is a <color #76D6FF>development snapshot</color> of the Atomic Editor intended for testing new features.\n\n";
            message += "Please report issues to the Atomic Forums or GitHub issue tracker.";
            new Atomic.UIMessageWindow(this, "modal_error").show("Atomic Editor - Development Snapshot", message, Atomic.UI_MESSAGEWINDOW_SETTINGS_OK, true, 640, 260);

        }

    }

    handleLicenseActivationError(ev) {

        this.progressModal.hide();

        EditorUI.showModalError("Activation Error",
            ev.message);

    }

    handleLicenseActivationSuccess(ev) {

        this.progressModal.hide();

        this.hide();

        EditorUI.getModelOps().showActivationSuccessWindow();


    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var id = ev.target.id;

            if (id == "quit") {

                this.sendEvent("ExitRequested");
                return true;

            } else if (id == "get_key") {

                var fileSystem = Atomic.getFileSystem();
                fileSystem.systemOpen("https://store.atomicgameengine.com/site");

            } else if (id == "activate") {

                var key = this.licenseKeyEdit.text.trim().toUpperCase();
                var licenseSystem = ToolCore.getLicenseSystem();
                if (!licenseSystem.validateKey(key)) {

                    EditorUI.showModalError("Invalid Product Key",
                        "The key entered is invalid\n\nProduct keys are in the form of ATOMIC-XXXX-XXXX-XXXX-XXXX");

                    return true;
                }

                this.progressModal.show();

                licenseSystem.requestServerActivation(key);

            }


        }

    }
    progressModal: ProgressModal;
    licenseKeyEdit: Atomic.UIEditField;
}

export = ActivationWindow;
