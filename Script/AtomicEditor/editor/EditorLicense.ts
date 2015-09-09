
import EditorUI = require("../ui/EditorUI");

class EditorLicense extends Atomic.ScriptObject {

    constructor() {

        super();

        this.subscribeToEvent("LicenseEulaRequired", (eventData) => this.handleLicenseEulaRequired(eventData));
        this.subscribeToEvent("LicenseActivationRequired", (eventData) => this.handleLicenseActivationRequired(eventData));
        this.subscribeToEvent("LicenseSuccess", (eventData) => this.handleLicenseSuccess(eventData));

        this.subscribeToEvent("IPCMessage", (eventData:Atomic.IPCMessageEvent) => this.handleIPCMessage(eventData));

    }

    handleIPCMessage(eventData:Atomic.IPCMessageEvent) {

      if (eventData.message == "3D Module License Required") {

        var ops = EditorUI.getModelOps();
        ops.showPro3DWindow();

      }

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


    }

}

export = EditorLicense;
