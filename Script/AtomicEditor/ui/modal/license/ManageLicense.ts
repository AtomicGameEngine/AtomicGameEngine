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

class ManageLicense extends ModalWindow {

    constructor() {

        super();

        this.init("Product Activation", "AtomicEditor/editor/ui/managelicense.tb.txt");

        this.progressModal = new ProgressModal("License Management", "Returning license, please wait...");

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var id = ev.target.id;

            if (id == "ok") {

                this.hide();
                return true;
            }

            if (ev.refid == "TBMessageWindow.ok") {

                if (ToolCore.licenseSystem.deactivate()) {

                    this.progressModal.show();

                    this.subscribeToEvent("LicenseDeactivationSuccess", (ev) => {

                        this.progressModal.hide();
                        this.hide();

                        EditorUI.getModelOps().showActivationWindow();

                    });

                    this.subscribeToEvent("LicenseDeactivationError", (ev: ToolCore.LicenseDeactivationErrorEvent) => {

                        this.progressModal.hide();

                        EditorUI.showModalError("Deactivation Error", ev.message);

                    });

                }

                return true;
            }


            if (id == "return_activation") {

                if (ToolCore.toolSystem.project) {
                    EditorUI.showModalError("Project Open",
                        "Please close the current project before deactivating license");

                }
                else {
                    var confirm = new Atomic.UIMessageWindow(this, "confirm_license_return");
                    confirm.show("Return License", "Are you sure you want to return the installed license?", Atomic.UI_MESSAGEWINDOW_SETTINGS_OK_CANCEL, true, 300, 140);
                }

                return true;
            }

        }
    }

    progressModal: ProgressModal;
}

export = ManageLicense;
