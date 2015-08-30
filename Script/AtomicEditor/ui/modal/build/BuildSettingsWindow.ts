
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

        this.addPlatformWidget("WINDOWS", new WindowsSettingsWidget(), 0);
        this.addPlatformWidget("MAC", new MacSettingsWidget(), 1);
        this.addPlatformWidget("ANDROID", new AndroidSettingsWidget(), 2);
        this.addPlatformWidget("IOS", new IOSSettingsWidget(), 3);
        this.addPlatformWidget("WEB", new WebSettingsWidget(), 4);

        this.setDisplayPlatform(ToolCore.toolSystem.currentPlatform);

        this.resizeToFitContent();
        this.center();

        this.subscribeToEvent(this, "WidgetEvent", (ev) => this.handleWidgetEvent(ev));
    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {


        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            if (ev.refid == "WindowsBuildSettings") {
                this.setDisplayPlatform(ToolCore.toolSystem.getPlatformByName("WINDOWS"));
                return true;
            }
            if (ev.refid == "MacBuildSettings") {
                this.setDisplayPlatform(ToolCore.toolSystem.getPlatformByName("MAC"));
                return true;
            }
            if (ev.refid == "AndroidBuildSettings") {
                this.setDisplayPlatform(ToolCore.toolSystem.getPlatformByName("ANDROID"));
                return true;
            }
            if (ev.refid == "iOSBuildSettings") {
                this.setDisplayPlatform(ToolCore.toolSystem.getPlatformByName("IOS"));
                return true;
            }
            if (ev.refid == "WebGLBuildSettings") {
                this.setDisplayPlatform(ToolCore.toolSystem.getPlatformByName("WEB"));
                return true;
            }

        }

        return false;


    }

    addPlatformWidget(name: string, widget: Atomic.UIWidget, index: number) {

        this.allPlatformWidgets[name] = { widget: widget, index: index };
        this.settingsContainer.addChild(widget);

    }

    setDisplayPlatform(platform: ToolCore.Platform) {

        for (var name in this.allPlatformWidgets) {

            this.allPlatformWidgets[name].widget.visibility = Atomic.UI_WIDGET_VISIBILITY_GONE;

        }

        if (!platform) return;

        var info: { widget: Atomic.UIWidget, index: number } = this.allPlatformWidgets[platform.name];
        info.widget.visibility = Atomic.UI_WIDGET_VISIBILITY_VISIBLE;
        if (this.platformSelect.value != info.index)
            this.platformSelect.value = info.index;

    }

    allPlatformWidgets: {} = {};

    settingsContainer: Atomic.UILayout;
    platformSelect: Atomic.UISelectList;
    platformIndicator: Atomic.UISkinImage;

}

export = BuildSettingsWindow;
