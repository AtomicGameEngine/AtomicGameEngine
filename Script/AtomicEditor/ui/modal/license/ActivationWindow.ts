
import EditorEvents = require("editor/EditorEvents");
import EditorUI = require("ui/EditorUI");
import ModalWindow = require("../ModalWindow");
import ProgressModal = require("../ProgressModal");

class ActivationWidow extends ModalWindow {

    constructor() {

        super();

        this.init("Product Activation", "AtomicEditor/editor/ui/activation.tb.txt");

        this.licenseKeyEdit = <Atomic.UIEditField> this.getWidget("license_key");

        this.subscribeToEvent("LicenseActivationError", (eventData) => this.handleLicenseActivationError(eventData));
        this.subscribeToEvent("LicenseActivationSuccess", (eventData) => this.handleLicenseActivationSuccess(eventData));

        this.progressModal = new ProgressModal("Activation", "Activating, please wait...");

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

                this.sendEvent(EditorEvents.Quit);
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
    progressModal:ProgressModal;
    licenseKeyEdit: Atomic.UIEditField;
}

export = ActivationWidow;
