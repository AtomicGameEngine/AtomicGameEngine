
import EditorUI = require("../ui/EditorUI");

class EditorLicense extends Atomic.ScriptObject {

    constructor() {

        super();

        this.subscribeToEvent("LicenseEulaRequired", (eventData) => this.handleLicenseEulaRequired(eventData));
        this.subscribeToEvent("LicenseActivationRequired", (eventData) => this.handleLicenseActivationRequired(eventData));

    }

    handleLicenseEulaRequired(eventData) {

        var ops = EditorUI.getModelOps();
        ops.showEULAWindow();

    }

    handleLicenseActivationRequired(eventData) {

        var ops = EditorUI.getModelOps();
        ops.showActivationWindow();

    }


}

export = EditorLicense;
