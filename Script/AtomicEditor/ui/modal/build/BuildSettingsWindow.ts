
import EditorEvents = require("editor/EditorEvents");
import EditorUI = require("ui/EditorUI");
import ModalWindow = require("../ModalWindow");
import ProgressModal = require("../ProgressModal");

import WindowsSettingsWidget = require("./WindowsSettingsWidget");
import MacSettingsWidget = require("./MacSettingsWidget");
import AndroidSettingsWidget = require("./AndroidSettingsWidget");
import IOSSettingsWidget = require("./IOSSettingsWidget");
import WebSettingsWidget = require("./WebSettingsWidget");

class BuildSettingsWindow extends ModalWindow {

    constructor() {

        super();

        this.init("Build Settings", "AtomicEditor/editor/ui/buildsettings.tb.txt");

        this.settingsContainer = <Atomic.UILayout>this.getWidget("settingscontainer");
        var platformcontainer = <Atomic.UILayout>this.getWidget("platformcontainer");
        this.platformIndicator = <Atomic.UISkinImage>this.getWidget("current_platform_indicator");

        var platformSelect = this.platformSelect = new Atomic.UISelectList();

        var platformSource = new Atomic.UISelectItemSource();

        platformSource.addItem(new Atomic.UISelectItem("Windows", "WindowsBuildSettings", "LogoWindows"));
        platformSource.addItem(new Atomic.UISelectItem("Mac", "MacBuildSettings", "LogoMac"));
        platformSource.addItem(new Atomic.UISelectItem("Android", "AndroidBuildSettings", "LogoAndroid"));
        platformSource.addItem(new Atomic.UISelectItem("iOS", "iOSBuildSettings", "LogoIOS"));
        platformSource.addItem(new Atomic.UISelectItem("WebGL", "WebGLBuildSettings", "LogoHTML5"));

        platformSelect.setSource(platformSource);

        var lp = new Atomic.UILayoutParams();

        lp.minWidth = 160;
        lp.minHeight = 370;
        lp.maxHeight = 370;

        platformSelect.layoutParams = lp;
        platformSelect.gravity = Atomic.UI_GRAVITY_ALL;

        platformcontainer.addChild(platformSelect);

        this.addPlatformWidget("WINDOWS", new WindowsSettingsWidget(), "LogoWindows", 0);
        this.addPlatformWidget("MAC", new MacSettingsWidget(), "LogoMac", 1);
        this.addPlatformWidget("ANDROID", new AndroidSettingsWidget(), "LogoAndroid", 2);
        this.addPlatformWidget("IOS", new IOSSettingsWidget(), "LogoIOS", 3);
        this.addPlatformWidget("WEB", new WebSettingsWidget(), "LogoHTML5", 4);

        var currentPlatform = ToolCore.toolSystem.currentPlatform;
        this.setDisplayPlatform(currentPlatform);

        this.platformIndicator.skinBg = this.platformInfo[currentPlatform.name].logo;

        this.resizeToFitContent();
        this.center();

        this.subscribeToEvent("PlatformChanged", (ev: ToolCore.PlatformChangedEvent) => {

            this.platformIndicator.skinBg = this.platformInfo[ev.platform.name].logo;

        });

        this.subscribeToEvent(this, "WidgetEvent", (ev) => this.handleWidgetEvent(ev));
    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var toolSystem = ToolCore.toolSystem;

            if (ev.target.id == "build") {

                var buildSystem = ToolCore.buildSystem;
                var toolSystem = ToolCore.toolSystem;

                if (Atomic.platform == "Windows")
                    buildSystem.buildPath = "C:/Users/Josh/Desktop/MyBuilds/";
                else
                    buildSystem.buildPath = "/Users/josh/Desktop/MyBuilds/";
                    
                var project = toolSystem.project;
                var platform = toolSystem.currentPlatform;

                var buildBase = platform.newBuild(project);
                buildSystem.queueBuild(buildBase);
                buildSystem.startNextBuild();

            }

            if (ev.target.id == "set_current_platform") {

                var index = this.platformSelect.value;

                for (var name in this.platformInfo) {

                    var info: { widget: Atomic.UIWidget, index: number, logo: string } = this.platformInfo[name];

                    if (info.index == index) {

                        toolSystem.setCurrentPlatform(toolSystem.getPlatformByName(name).platformID);

                        return true;
                    }

                }

                return true;
            }

            if (ev.refid == "WindowsBuildSettings") {
                this.setDisplayPlatform(toolSystem.getPlatformByName("WINDOWS"));
                return true;
            }
            if (ev.refid == "MacBuildSettings") {
                this.setDisplayPlatform(toolSystem.getPlatformByName("MAC"));
                return true;
            }
            if (ev.refid == "AndroidBuildSettings") {
                this.setDisplayPlatform(toolSystem.getPlatformByName("ANDROID"));
                return true;
            }
            if (ev.refid == "iOSBuildSettings") {
                this.setDisplayPlatform(toolSystem.getPlatformByName("IOS"));
                return true;
            }
            if (ev.refid == "WebGLBuildSettings") {
                this.setDisplayPlatform(toolSystem.getPlatformByName("WEB"));
                return true;
            }

        }

        return false;


    }

    addPlatformWidget(name: string, widget: Atomic.UIWidget, logo: string, index: number) {

        this.platformInfo[name] = { widget: widget, index: index, logo: logo };
        this.settingsContainer.addChild(widget);

    }

    setDisplayPlatform(platform: ToolCore.Platform) {

        for (var name in this.platformInfo) {

            this.platformInfo[name].widget.visibility = Atomic.UI_WIDGET_VISIBILITY_GONE;

        }

        if (!platform) return;

        var info: { widget: Atomic.UIWidget, index: number, logo: string } = this.platformInfo[platform.name];
        info.widget.visibility = Atomic.UI_WIDGET_VISIBILITY_VISIBLE;
        if (this.platformSelect.value != info.index)
            this.platformSelect.value = info.index;

    }

    platformInfo: {} = {};

    settingsContainer: Atomic.UILayout;
    platformSelect: Atomic.UISelectList;
    platformIndicator: Atomic.UISkinImage;

}

export = BuildSettingsWindow;
