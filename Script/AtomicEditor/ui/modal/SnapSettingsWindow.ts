import ModalWindow = require("./ModalWindow");

class SnapSettingsWindow extends ModalWindow {

    constructor() {

        super();

        this.init("Snap Settings", "AtomicEditor/editor/ui/snapsettings.tb.txt");

        this.transXEditField = <Atomic.UIEditField>this.getWidget("trans_x");
        this.transYEditField = <Atomic.UIEditField>this.getWidget("trans_y");
        this.transZEditField = <Atomic.UIEditField>this.getWidget("trans_z");
        this.rotateEditField = <Atomic.UIEditField>this.getWidget("rotation");
        this.scaleEditField = <Atomic.UIEditField>this.getWidget("scale");

        this.refreshWidgets();

    }

    apply() {

        var userPrefs = ToolCore.toolSystem.project.userPrefs;
        userPrefs.snapTranslationX = Number(this.transXEditField.text);
        userPrefs.snapTranslationY = Number(this.transYEditField.text);
        userPrefs.snapTranslationZ = Number(this.transZEditField.text);
        userPrefs.snapRotation = Number(this.rotateEditField.text);
        userPrefs.snapScale = Number(this.scaleEditField.text);

        ToolCore.toolSystem.project.saveUserPrefs();

    }

    refreshWidgets() {

        var userPrefs = ToolCore.toolSystem.project.userPrefs;

        this.transXEditField.text = parseFloat(userPrefs.snapTranslationX.toFixed(5)).toString();
        this.transYEditField.text = parseFloat(userPrefs.snapTranslationY.toFixed(5)).toString();
        this.transZEditField.text = parseFloat(userPrefs.snapTranslationZ.toFixed(5)).toString();
        this.rotateEditField.text = parseFloat(userPrefs.snapRotation.toFixed(5)).toString();
        this.scaleEditField.text = parseFloat(userPrefs.snapScale.toFixed(5)).toString();

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var id = ev.target.id;

            if (id == "apply") {

                this.apply();
                this.hide();
                return true;

            }

            if (id == "cancel") {

                this.hide();

                return true;
            }

        }

    }

    transXEditField: Atomic.UIEditField;
    transYEditField: Atomic.UIEditField;
    transZEditField: Atomic.UIEditField;
    rotateEditField: Atomic.UIEditField;
    scaleEditField: Atomic.UIEditField;

}

export = SnapSettingsWindow;
