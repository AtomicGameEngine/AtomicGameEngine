
import EditorEvents = require("editor/EditorEvents");
import EditorUI = require("ui/EditorUI");
import ModalWindow = require("../ModalWindow");
import ProgressModal = require("../ProgressModal");

import WindowsSettingsWidget = require("./platforms/WindowsSettingsWidget");
import MacSettingsWidget = require("./platforms/MacSettingsWidget");
import AndroidSettingsWidget = require("./platforms/AndroidSettingsWidget");
import IOSSettingsWidget = require("./platforms/IOSSettingsWidget");
import WebSettingsWidget = require("./platforms/WebSettingsWidget");

class BuildWindow extends ModalWindow {

    constructor() {

        super();

        this.init("Build Settings", "AtomicEditor/editor/ui/build.tb.txt");

        this.buildPathField = <Atomic.UITextField>this.getWidget("build_path");
        this.platformIndicator = <Atomic.UISkinImage>this.getWidget("current_platform_indicator");

        var currentPlatform = ToolCore.toolSystem.currentPlatform;

        this.buildPathField.text = "/Users/josh/Desktop/MyBuilds/";

        switch (currentPlatform.name) {

            case "WINDOWS": this.platformIndicator.skinBg = "LogoWindows"; break;
            case "MAC": this.platformIndicator.skinBg = "LogoMac"; break;
            case "ANDROID": this.platformIndicator.skinBg = "LogoAndroid"; break;
            case "IOS": this.platformIndicator.skinBg = "LogoIOS"; break;
            case "WEB": this.platformIndicator.skinBg = "LogoHTML5"; break;

        }

        this.subscribeToEvent(this, "WidgetEvent", (ev) => this.handleWidgetEvent(ev));
    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            if (ev.target.id == "cancel") {
                this.hide();
                return true;
            }

            if (ev.target.id == "choose_path") {

                var utils = new Editor.FileUtils();
                var buildPath = utils.getBuildPath("");
                if (buildPath && buildPath.length)
                    this.buildPathField.text = buildPath;

                return true;
            }


        }

    }

    build() {

        /*
        var buildSystem = ToolCore.buildSystem;
        var toolSystem = ToolCore.toolSystem;

        buildSystem.buildPath = "/Users/josh/Desktop/MyBuilds/";

        var project = toolSystem.project;
        var platform = toolSystem.currentPlatform;

        var buildBase = platform.newBuild(project);
        buildSystem.queueBuild(buildBase);
        buildSystem.startNextBuild();
        */

    }

    buildPathField: Atomic.UITextField;
    platformIndicator: Atomic.UISkinImage;
}

export = BuildWindow;

/*

buildPathField_ = delegate_->GetWidgetByIDAndType<TBTextField>(TBIDC("build_path"));
assert(buildPathField_);

String buildPath = project->GetLastBuildPath();
buildPathField_->SetText(buildPath.CString());

window_->ResizeToFitContent();
Center();

AEEditorPlatform platform = editor->GetCurrentPlatform();
TBSkinImage* platformIndicator = window_->GetContentRoot()->GetWidgetByIDAndType<TBSkinImage>(TBIDC("current_platform_indicator"));
assert(platformIndicator);

if (platform == AE_PLATFORM_MAC)
    platformIndicator->SetSkinBg(TBIDC("LogoMac"));
else if (platform == AE_PLATFORM_WINDOWS)
    platformIndicator->SetSkinBg(TBIDC("LogoWindows"));
else if (platform == AE_PLATFORM_ANDROID)
    platformIndicator->SetSkinBg(TBIDC("LogoAndroid"));
else if (platform == AE_PLATFORM_HTML5)
    platformIndicator->SetSkinBg(TBIDC("LogoHTML5"));
else if (platform == AE_PLATFORM_IOS)
    platformIndicator->SetSkinBg(TBIDC("LogoIOS"));


*/
