
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
