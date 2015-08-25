
import EditorUI = require("../ui/EditorUI");

class EditorLicense extends Atomic.ScriptObject {

    constructor() {

        super();

        this.subscribeToEvent("LicenseEulaRequired", (eventData) => this.handleLicenseEulaRequired(eventData));
        this.subscribeToEvent("LicenseActivationRequired", (eventData) => this.handleLicenseActivationRequired(eventData));
        this.subscribeToEvent("LicenseSuccess", (eventData) => this.handleLicenseSuccess(eventData));

    }

    handleLicenseEulaRequired(eventData) {

        var ops = EditorUI.getModelOps();
        ops.showEULAWindow();

    }

    handleLicenseActivationRequired(eventData) {

        var ops = EditorUI.getModelOps();
        ops.showActivationWindow();

    }

    handleLicenseSuccess(eventData) {

        if (ToolCore.licenseSystem.sourceBuild && ToolCore.licenseSystem.isStandardLicense()) {

          var ops = EditorUI.getModelOps();
          ops.showProWindow("AtomicEditor/editor/ui/sourceinfo.tb.txt");

        }

    }

}

export = EditorLicense;
