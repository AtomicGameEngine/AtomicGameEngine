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

import EditorEvents = require("editor/EditorEvents");
import EditorUI = require("ui/EditorUI");
import ModalWindow = require("../ModalWindow");
import ProgressModal = require("../ProgressModal");

import WindowsSettingsWidget = require("./platforms/WindowsSettingsWidget");
import MacSettingsWidget = require("./platforms/MacSettingsWidget");
import AndroidSettingsWidget = require("./platforms/AndroidSettingsWidget");
import IOSSettingsWidget = require("./platforms/IOSSettingsWidget");
import WebSettingsWidget = require("./platforms/WebSettingsWidget");

export interface BuildSettingsWidget {

    storeValues();

}

export class BuildSettingsWindow extends ModalWindow {

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

    commitBuildSettings() {

        for (var name in this.platformInfo) {

            <BuildSettingsWidget>(this.platformInfo[name].widget).storeValues();
        }

        ToolCore.toolSystem.project.saveBuildSettings();


    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var toolSystem = ToolCore.toolSystem;

            if (ev.target.id == "cancel") {
                this.hide();
                return true;
            }

            if (ev.target.id == "ok") {

                this.commitBuildSettings();
                this.hide();
                return true;
            }

            if (ev.target.id == "build") {

                this.commitBuildSettings();
                this.hide();
                EditorUI.getModelOps().showBuild();
                return true;

            }

            if (ev.target.id == "set_current_platform") {

                var index = this.platformSelect.value;

                for (var name in this.platformInfo) {

                    var info: { widget: Atomic.UIWidget, index: number, logo: string } = this.platformInfo[name];

                    if (info.index == index) {

                        var platform = toolSystem.getPlatformByName(name);

                        if (platform.platformID == ToolCore.PLATFORMID_IOS) {

                            if (Atomic.platform == "Windows") {

                                var message = "\niOS Deployment requires running the Atomic Editor on MacOSX\n\n";
                                new Atomic.UIMessageWindow(this, "modal_error").show("MacOSX Required", message, Atomic.UI_MESSAGEWINDOW_SETTINGS_OK, true, 640, 260);
                                return true;

                            }

                        }

                        toolSystem.setCurrentPlatform(platform.platformID);

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
