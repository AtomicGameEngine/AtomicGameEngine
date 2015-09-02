
class AndroidSettingsWidget extends Atomic.UIWidget {

    constructor() {

        super();

        this.load("AtomicEditor/editor/ui/buildsettings_android.tb.txt");

        this.refreshWidgets();

        this.subscribeToEvent(this, "WidgetEvent", (ev) => this.handleWidgetEvent(ev));

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            if (ev.target.id == "choose_sdk_path") {

                var fileUtils = new Editor.FileUtils();

                var path = fileUtils.getAndroidSDKPath("");

                if (path.length) {

                    var toolPrefs = ToolCore.toolEnvironment.toolPrefs;
                    if (toolPrefs.androidSDKPath != path) {
                        toolPrefs.androidSDKPath = path;
                        toolPrefs.save();
                    }

                    this.refreshWidgets();

                }

                return true;

            }

        }

        return false;
    }

    refreshWidgets() {

        var sdkPathEdit = <Atomic.UIEditField>this.getWidget("sdk_path");
        var antPathEdit = <Atomic.UIEditField>this.getWidget("ant_path");
        var jdkRootEdit = <Atomic.UIEditField>this.getWidget("jdk_root");

        var toolPrefs = ToolCore.toolEnvironment.toolPrefs;

        sdkPathEdit.text = toolPrefs.androidSDKPath;
        antPathEdit.text = toolPrefs.antPath;
        jdkRootEdit.text = toolPrefs.jDKRootPath;

        /*
        appNameEdit_->SetText(settings.appName.CString());
        appPackageEdit_->SetText(settings.package.CString());
        productNameEdit_->SetText(settings.productName.CString());
        companyNameEdit_->SetText(settings.companyName.CString());
        sdkTargetSelect_->SetText(settings.targetSDKVersion.CString());
        */

    }

    storeValues() {

    }

}

export = AndroidSettingsWidget;
