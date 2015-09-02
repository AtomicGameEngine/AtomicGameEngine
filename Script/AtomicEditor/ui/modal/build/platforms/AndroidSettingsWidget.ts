
class AndroidSettingsWidget extends Atomic.UIWidget {

    constructor() {

        super();

        this.load("AtomicEditor/editor/ui/buildsettings_android.tb.txt");

        this.settings = ToolCore.toolSystem.project.buildSettings.androidBuildSettings;

        this.sdkTargetSelect = <Atomic.UISelectDropdown>this.getWidget("sdk_target_select");
        this.appNameEdit = <Atomic.UIEditField>this.getWidget("app_name");
        this.packageNameEdit = <Atomic.UIEditField>this.getWidget("app_package");
        this.productNameEdit = <Atomic.UIEditField>this.getWidget("product_name");
        this.companyNameEdit = <Atomic.UIEditField>this.getWidget("company_name");

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

            } else if (ev.target.id == "refresh_sdk_targets") {

                this.refreshAndroidTargets();

            }

        }

        return false;
    }

    refreshAndroidTargets() {

        var platform = <ToolCore.PlatformAndroid>ToolCore.toolSystem.getPlatformByName("ANDROID");

        platform.refreshAndroidTargets();

        this.subscribeToEvent(platform, "AndroidTargetsRefreshed", (ev) => {

            this.sdkTargetSource.clear();

            var targets: string[] = platform.androidTargets;

            for (var i in targets) {

                this.sdkTargetSource.addItem(new Atomic.UISelectItem(targets[i]));
            }

            this.sdkTargetSelect.source = this.sdkTargetSource;

            // force a refresh
            this.sdkTargetSelect.value = -1;
            this.sdkTargetSelect.value = 0;

        });

    }

    refreshWidgets() {

        var sdkPathEdit = <Atomic.UIEditField>this.getWidget("sdk_path");
        var antPathEdit = <Atomic.UIEditField>this.getWidget("ant_path");
        var jdkRootEdit = <Atomic.UIEditField>this.getWidget("jdk_root");

        var toolPrefs = ToolCore.toolEnvironment.toolPrefs;

        sdkPathEdit.text = toolPrefs.androidSDKPath;
        antPathEdit.text = toolPrefs.antPath;
        jdkRootEdit.text = toolPrefs.jDKRootPath;

        this.appNameEdit.text = this.settings.appName;
        this.packageNameEdit.text = this.settings.packageName;
        this.productNameEdit.text = this.settings.productName;
        this.companyNameEdit.text = this.settings.companyName;

        this.sdkTargetSelect.text = this.settings.sDKVersion;

    }

    storeValues() {

      this.settings.appName = this.appNameEdit.text;
      this.settings.packageName = this.packageNameEdit.text;
      this.settings.productName = this.productNameEdit.text;
      this.settings.companyName = this.companyNameEdit.text;
      
      this.settings.sDKVersion = this.sdkTargetSelect.text;

    }

    settings: ToolCore.AndroidBuildSettings;
    sdkTargetSource: Atomic.UISelectItemSource = new Atomic.UISelectItemSource();
    sdkTargetSelect: Atomic.UISelectDropdown;

    appNameEdit: Atomic.UIEditField;
    packageNameEdit: Atomic.UIEditField;
    productNameEdit: Atomic.UIEditField;
    companyNameEdit: Atomic.UIEditField;


}

export = AndroidSettingsWidget;
