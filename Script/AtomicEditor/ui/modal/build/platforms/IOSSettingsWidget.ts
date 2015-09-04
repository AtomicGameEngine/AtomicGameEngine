

class IOSSettingsWidget extends Atomic.UIWidget {

    constructor() {

        super();

        this.load("AtomicEditor/editor/ui/buildsettings_ios.tb.txt");

        this.settings = ToolCore.toolSystem.project.buildSettings.iOSBuildSettings;

        this.appNameEdit = <Atomic.UIEditField>this.getWidget("app_name");
        this.packageNameEdit = <Atomic.UIEditField>this.getWidget("app_package");
        this.productNameEdit = <Atomic.UIEditField>this.getWidget("product_name");
        this.companyNameEdit = <Atomic.UIEditField>this.getWidget("company_name");

        this.provisionPathEdit = <Atomic.UIEditField>this.getWidget("provision_path");
        this.appIDPrefixField = <Atomic.UITextField>this.getWidget("appid_prefix");

        this.refreshWidgets();

        this.subscribeToEvent(this, "WidgetEvent", (ev) => this.handleWidgetEvent(ev));

    }

    refreshWidgets() {

        this.appNameEdit.text = this.settings.appName;
        this.packageNameEdit.text = this.settings.packageName;
        this.productNameEdit.text = this.settings.productName;
        this.companyNameEdit.text = this.settings.companyName;

        this.provisionPathEdit.text = this.settings.provisionFile;
        this.appIDPrefixField.text = this.settings.appIDPrefix;

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            if (ev.target.id == "choose_provision_path") {

              var fileUtils = new Editor.FileUtils();
              var path = fileUtils.getMobileProvisionPath();

              if (path.length) {

                  var platform = <ToolCore.PlatformIOS> ToolCore.toolSystem.getPlatformByName("IOS");
                  var prefix = platform.parseProvisionAppIdentifierPrefix(path);

                  if (prefix.length) {

                    this.provisionPathEdit.text = path;
                    this.appIDPrefixField.text = prefix;

                  }

                  this.refreshWidgets();

              }

              return true;

            }
        }

        return true;
    }

    storeValues() {

        this.settings.appName = this.appNameEdit.text;
        this.settings.packageName = this.packageNameEdit.text;
        this.settings.productName = this.productNameEdit.text;
        this.settings.companyName = this.companyNameEdit.text;

        this.settings.provisionFile = this.provisionPathEdit.text;
        this.settings.appIDPrefix = this.appIDPrefixField.text;

    }

    settings: ToolCore.IOSBuildSettings;

    appNameEdit: Atomic.UIEditField;
    packageNameEdit: Atomic.UIEditField;
    productNameEdit: Atomic.UIEditField;
    companyNameEdit: Atomic.UIEditField;

    provisionPathEdit: Atomic.UIEditField;
    appIDPrefixField: Atomic.UITextField;

}

export = IOSSettingsWidget;
