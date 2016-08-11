//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

import BuildSettingsWindow = require("../BuildSettingsWindow");

class AndroidSettingsWidget extends Atomic.UIWidget implements BuildSettingsWindow.BuildSettingsWidget {

    constructor() {

        super();

        this.load("AtomicEditor/editor/ui/buildsettings_android.tb.txt");

        this.settings = ToolCore.toolSystem.project.buildSettings.androidBuildSettings;

        this.sdkPathEdit = <Atomic.UIEditField>this.getWidget("sdk_path");
        this.sdkTargetSelect = <Atomic.UISelectDropdown>this.getWidget("sdk_target_select");
        this.appNameEdit = <Atomic.UIEditField>this.getWidget("app_name");
        this.packageNameEdit = <Atomic.UIEditField>this.getWidget("app_package");
        this.productNameEdit = <Atomic.UIEditField>this.getWidget("product_name");
        this.companyNameEdit = <Atomic.UIEditField>this.getWidget("company_name");


        this.jdkRootChooseButton = <Atomic.UIButton>this.getWidget("choose_jdk_root");
        this.jdkRootEdit = <Atomic.UIEditField>this.getWidget("jdk_root");

        var jdkRootText = <Atomic.UITextField>this.getWidget("jdk_root_text");
        var antPathText = <Atomic.UITextField>this.getWidget("ant_path_text");

        this.releaseChooseButton = <Atomic.UIButton>this.getWidget("choose_and_auth");
        this.releaseNameEdit = <Atomic.UIEditField>this.getWidget("auth_root");
        this.releaseCheck = <Atomic.UICheckBox>this.getWidget("and_auth_check");
        this.iconNameEdit = <Atomic.UIEditField>this.getWidget("icon_root");
        this.iconChooseButton = <Atomic.UIButton>this.getWidget("choose_icon");
        this.iconImage = <Atomic.UIImageWidget>this.getWidget("and_icon");

        if (Atomic.platform == "Windows") {

            jdkRootText.text = "JDK Root: (Ex. C:\\Program Files\\Java\\jdk1.8.0_31)";
            antPathText.text = "Ant Path: (The folder that contains ant.bat)";

        }

        this.antPathEdit = <Atomic.UIEditField>this.getWidget("ant_path");

        this.refreshWidgets();

        this.subscribeToEvent(this, "WidgetEvent", (ev) => this.handleWidgetEvent(ev));

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            if (ev.target.id == "choose_sdk_path") {

                var fileUtils = new Editor.FileUtils();
                var currsdk = this.sdkPathEdit.text;
                var path = fileUtils.findPath("Please choose the root folder of your Android SDK" , currsdk );
                if ( path.length > 0 )
                    this.sdkPathEdit.text = path;

                return true;

            } else if (ev.target.id == "choose_ant_path") {

                var fileUtils = new Editor.FileUtils();
                var currant = this.antPathEdit.text;
                var path = fileUtils.getAntPath(currant);
                if ( path.length > 0 )
                    this.antPathEdit.text = path;

                return true;

            }  else if (ev.target.id == "choose_jdk_root") {

                var fileUtils = new Editor.FileUtils();
                var currjdk = this.jdkRootEdit.text;
                var path = fileUtils.findPath("Please choose the root folder of your JDK" , currjdk );
                if ( path.length > 0 )
                    this.jdkRootEdit.text = path;

                return true;

            } else if (ev.target.id == "refresh_sdk_targets") {

                this.refreshAndroidTargets();

            }  else if (ev.target.id == "choose_and_auth") {
                var fileUtils = new Editor.FileUtils();
                var currauth = this.releaseNameEdit.text;
                var path = fileUtils.findPath( "Please choose the folder of your ant.properties", currauth );
                if ( path.length > 0 )
                    this.releaseNameEdit.text = path;
                return true;

            }  else if (ev.target.id == "choose_icon") {
                var fileUtils = new Editor.FileUtils();
                var curricon = this.iconNameEdit.text;
                var path = fileUtils.findPath("Please choose the folder with drawable folders" , curricon);
                if ( path.length > 0 ) {
                    this.iconNameEdit.text = path;
                    this.updateIconButton();
               }
                return true;

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

    updateIconButton() {

        var fileSystem = Atomic.getFileSystem();

        if ( this.iconNameEdit.text.length > 0 ) {

            let myicon = this.iconNameEdit.text + "/drawable-ldpi/icon.png";
            if ( fileSystem.fileExists(myicon) ) {

                this.iconImage.setImage( myicon );
                return;
            }
        }

        let defaulticon = fileSystem.getProgramDir() + "Resources/ToolData/Deployment/Android/res/drawable-ldpi/icon.png";
        this.iconImage.setImage( defaulticon );

    }


    refreshWidgets() {

        var toolPrefs = ToolCore.toolEnvironment.toolPrefs;

        this.sdkPathEdit.text = toolPrefs.androidSDKPath;
        this.antPathEdit.text = toolPrefs.antPath;
        this.jdkRootEdit.text = toolPrefs.jDKRootPath;
        this.releaseNameEdit.text = toolPrefs.releasePath;
        this.releaseCheck.value = toolPrefs.releaseCheck;

        this.appNameEdit.text = this.settings.appName;
        this.packageNameEdit.text = this.settings.packageName;
        this.productNameEdit.text = this.settings.productName;
        this.companyNameEdit.text = this.settings.companyName;
        this.iconNameEdit.text = this.settings.iconPath;

        this.sdkTargetSelect.text = this.settings.sDKVersion;

        this.updateIconButton();
    }

    storeValues() {

        this.settings.appName = this.appNameEdit.text;
        this.settings.packageName = this.packageNameEdit.text;
        this.settings.productName = this.productNameEdit.text;
        this.settings.companyName = this.companyNameEdit.text;
        this.settings.sDKVersion = this.sdkTargetSelect.text;
        this.settings.iconPath = this.iconNameEdit.text;

        ToolCore.toolEnvironment.toolPrefs.antPath = this.antPathEdit.text;
        ToolCore.toolEnvironment.toolPrefs.androidSDKPath = this.sdkPathEdit.text;
        ToolCore.toolEnvironment.toolPrefs.jDKRootPath = this.jdkRootEdit.text;
        ToolCore.toolEnvironment.toolPrefs.releasePath = this.releaseNameEdit.text;
        ToolCore.toolEnvironment.toolPrefs.releaseCheck = this.releaseCheck.value;

        ToolCore.toolEnvironment.saveToolPrefs();
    }

    settings: ToolCore.AndroidBuildSettings;
    sdkTargetSource: Atomic.UISelectItemSource = new Atomic.UISelectItemSource();
    sdkTargetSelect: Atomic.UISelectDropdown;
    sdkPathEdit: Atomic.UIEditField;

    jdkRootChooseButton: Atomic.UIButton;
    jdkRootEdit: Atomic.UIEditField;

    antPathEdit: Atomic.UIEditField;

    appNameEdit: Atomic.UIEditField;
    packageNameEdit: Atomic.UIEditField;
    productNameEdit: Atomic.UIEditField;
    companyNameEdit: Atomic.UIEditField;

    releaseNameEdit : Atomic.UIEditField;
    releaseChooseButton : Atomic.UIButton;
    releaseCheck : Atomic.UICheckBox;
    iconNameEdit : Atomic.UIEditField;
    iconChooseButton : Atomic.UIButton;
    iconImage : Atomic.UIImageWidget;
}

export = AndroidSettingsWidget;
