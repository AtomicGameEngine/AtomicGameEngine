//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import BuildSettingsWindow = require("../BuildSettingsWindow");

class WindowsSettingsWidget extends Atomic.UIWidget implements BuildSettingsWindow.BuildSettingsWidget {

  constructor() {

      super();

      this.settings = ToolCore.toolSystem.project.buildSettings.windowsBuildSettings;

      this.load("AtomicEditor/editor/ui/buildsettings_windows.tb.txt");

      this.appNameEdit = <Atomic.UIEditField>this.getWidget("app_name");
      this.packageNameEdit = <Atomic.UIEditField>this.getWidget("app_identifier");
      this.productNameEdit = <Atomic.UIEditField>this.getWidget("product_name");
      this.companyNameEdit = <Atomic.UIEditField>this.getWidget("company_name");

      this.refreshWidgets();

    }

    refreshWidgets() {

        this.appNameEdit.text = this.settings.appName;
        this.packageNameEdit.text = this.settings.packageName;
        this.productNameEdit.text = this.settings.productName;
        this.companyNameEdit.text = this.settings.companyName;
    }

    storeValues() {

        this.settings.appName = this.appNameEdit.text;
        this.settings.packageName = this.packageNameEdit.text;
        this.settings.productName = this.productNameEdit.text;
        this.settings.companyName = this.companyNameEdit.text;

    }

    appNameEdit: Atomic.UIEditField;
    packageNameEdit: Atomic.UIEditField;
    productNameEdit: Atomic.UIEditField;
    companyNameEdit: Atomic.UIEditField;

    settings: ToolCore.WindowsBuildSettings;

}

export = WindowsSettingsWidget;
