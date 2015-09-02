
class AndroidSettingsWidget extends Atomic.UIWidget {

    constructor() {

        super();

        this.load("AtomicEditor/editor/ui/buildsettings_android.tb.txt");

        this.sdkTargetSelect = <Atomic.UISelectDropdown>this.getWidget("sdk_target_select");

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

          var targets:string[] = platform.androidTargets;

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

    sdkTargetSource:Atomic.UISelectItemSource = new Atomic.UISelectItemSource();
    sdkTargetSelect:Atomic.UISelectDropdown;

}

export = AndroidSettingsWidget;
