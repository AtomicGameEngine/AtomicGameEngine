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

class WebSettingsWidget extends Atomic.UIWidget implements BuildSettingsWindow.BuildSettingsWidget {

  constructor() {

      super();

      this.settings = ToolCore.toolSystem.project.buildSettings.webBuildSettings;

      this.load("AtomicEditor/editor/ui/buildsettings_html5.tb.txt");

      this.appNameEdit = <Atomic.UIEditField>this.getWidget("app_name");
      this.radioDark = <Atomic.UICheckBox>this.getWidget("web_dark");
      this.radioLight = <Atomic.UICheckBox>this.getWidget("web_light");
      this.radioAtomic = <Atomic.UICheckBox>this.getWidget("web_atomic");
      this.radioLake = <Atomic.UICheckBox>this.getWidget("web_lake");
      this.radioFireworks = <Atomic.UICheckBox>this.getWidget("web_fireworks");
      this.radioRetro = <Atomic.UICheckBox>this.getWidget("web_retro");
      this.faviconName = <Atomic.UIEditField>this.getWidget("favicon_name");
      this.gameWidth = <Atomic.UIEditField>this.getWidget("web_game_width");
      this.gameHeight = <Atomic.UIEditField>this.getWidget("web_game_height");

      this.refreshWidgets();

      this.subscribeToEvent(this, Atomic.UIWidgetEvent((ev) => this.handleWidgetEvent(ev)));
    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        if (ev.target.id == "favicon_choose") {
            var fileUtils = new Editor.FileUtils();
            var path = fileUtils.findFile("", "");
            if ( path.length > 0 )
                this.faviconName.text = path;
            return true;
        }

        if (ev.type == Atomic.UI_EVENT_TYPE.UI_EVENT_TYPE_CLICK) {

            if (ev.target.id == "web_dark") {
                this.settings.pageTheme = 0;
            }
            if (ev.target.id == "web_light") {
                this.settings.pageTheme = 1;
            }
            if (ev.target.id == "web_atomic") {
                this.settings.pageTheme = 2;
            }
            if (ev.target.id == "web_lake") {
                this.settings.pageTheme = 3;
            }
            if (ev.target.id == "web_fireworks") {
                this.settings.pageTheme = 4;
            }
            if (ev.target.id == "web_retro") {
                this.settings.pageTheme = 5;
            }
            return true;
        }
         return false;
   }

    refreshWidgets() {

        this.appNameEdit.text = this.settings.appName;
        this.faviconName.text = this.settings.faviconName;
        this.gameWidth.text = this.settings.gameWidth;
        this.gameHeight.text = this.settings.gameHeight;
        if ( this.settings.pageTheme == 0 ) this.radioDark.value = 1;
            else this.radioDark.value = 0;
        if ( this.settings.pageTheme == 1 ) this.radioLight.value = 1;
            else this.radioLight.value = 0;
        if ( this.settings.pageTheme == 2 ) this.radioAtomic.value = 1;
            else this.radioAtomic.value = 0;
        if ( this.settings.pageTheme == 3 ) this.radioLake.value = 1;
            else this.radioLake.value = 0;
        if ( this.settings.pageTheme == 4 ) this.radioFireworks.value = 1;
            else this.radioFireworks.value = 0;
        if ( this.settings.pageTheme == 5 ) this.radioRetro.value = 1;
            else this.radioRetro.value = 0;
    }

    storeValues() {

        this.settings.appName = this.appNameEdit.text;
        this.settings.faviconName = this.faviconName.text;
        this.settings.gameWidth = this.gameWidth.text;
        this.settings.gameHeight = this.gameHeight.text;
        if ( this.radioDark.value  == 1 ) this.settings.pageTheme = 0;
        if ( this.radioLight.value == 1 ) this.settings.pageTheme = 1;
        if ( this.radioAtomic.value == 1 ) this.settings.pageTheme = 2;
        if ( this.radioLake.value == 1 ) this.settings.pageTheme = 3;
        if ( this.radioFireworks.value == 1 ) this.settings.pageTheme = 4;
        if ( this.radioRetro.value == 1 ) this.settings.pageTheme = 5;
    }

    appNameEdit: Atomic.UIEditField;
    radioRetro: Atomic.UICheckBox;
    radioDark: Atomic.UICheckBox;
    radioLight: Atomic.UICheckBox;
    radioAtomic: Atomic.UICheckBox;
    radioLake: Atomic.UICheckBox;
    radioFireworks: Atomic.UICheckBox;
    faviconChoose: Atomic.UIButton;
    faviconName: Atomic.UIEditField;
    gameWidth: Atomic.UIEditField;
    gameHeight: Atomic.UIEditField;

    settings: ToolCore.WebBuildSettings;

}

export = WebSettingsWidget;
