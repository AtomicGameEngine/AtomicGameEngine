//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import EditorUI = require("../EditorUI");
import ModalWindow = require("./ModalWindow");

class About extends ModalWindow {

    constructor() {

        super();

        // we're not calling this.init here as it calls resizeToFitContent
        // and center, which screw up the generated About text being resized

        this.text = "About the Atomic Game Engine";
        this.load("AtomicEditor/editor/ui/about.tb.txt");

        this.age_license = <Atomic.UIEditField>this.getWidget("age_license");
        this.age_runtimelicense = <Atomic.UIEditField> this.getWidget("age_runtime_license");
        this.thirdparty_license = <Atomic.UIEditField>this.getWidget("thirdparty_license");
        this.externaltool_license = <Atomic.UIEditField>this.getWidget("externaltool_license");
        this.about_text = <Atomic.UIEditField>this.getWidget("about_text");

        var cache = Atomic.cache;

        var file = cache.getFile("AtomicEditor/eulas/atomic_game_engine_eula.txt");
        this.age_license.text = file.readText();

        file = cache.getFile("AtomicEditor/eulas/atomic_runtime_eula.txt");
        this.age_runtimelicense.text = file.readText();

        file = cache.getFile("AtomicEditor/eulas/atomic_thirdparty_eula.txt");
        this.thirdparty_license.text = file.readText();

        file = cache.getFile("AtomicEditor/eulas/atomic_external_tools_eula.txt");
        this.externaltool_license.text = file.readText();

        this.about_text.text = this.generateAboutText();

        var get_pro = <Atomic.UIButton>this.getWidget("purchase_pro");
        if (get_pro) {
            get_pro.onClick = function() {
                Atomic.fileSystem.systemOpen("https://store.atomicgameengine.com/site");
            };
        }

        this.resizeToFitContent();
        this.center();

        this.getWidget("tabcontainer").value = 0;

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var id = ev.target.id;

            if (id == "ok") {
                this.hide();
                return true;
            }
        }
    }


    generateAboutText(): string {

        var text = "";

        text += "<widget TBImageWidget: filename: 'AtomicEditor/editor/images/atomic_logo.png'>\n\n";
        text += "<color #D4FB79>Version  0.1.p0</color>\n\n";
        text += "(c) 2014-2015 THUNDERBEAST GAMES LLC\n\n\n";

        text += "<color #D4FB79>Installed platforms and modules:</color>\n\n";

        var licenseSystem = ToolCore.licenseSystem;

        var installedText = "    <widget TBSkinImage: skin: 'LogoMac-Small'> <widget TBSkinImage: skin: 'LogoWindows-Small'> <widget TBSkinImage: skin: 'LogoHTML5-Small'> ";
        var availableText = "   ";

        if (licenseSystem.licenseAndroid)
            installedText += "<widget TBSkinImage: skin: 'LogoAndroid-Small'> ";
        else
            availableText += "<widget TBSkinImage: skin: 'LogoAndroid-Small'> ";

            if (licenseSystem.licenseIOS)
                installedText += "<widget TBSkinImage: skin: 'LogoIOS-Small'> ";
            else
                availableText += "<widget TBSkinImage: skin: 'LogoIOS-Small'> ";

        installedText += "<widget TBSkinImage: skin: 'Module2D-Small'> ";

        if (licenseSystem.licenseModule3D)
            installedText += "<widget TBSkinImage: skin: 'Module3D-Small'> ";
        else
            availableText += "<widget TBSkinImage: skin: 'Module3D-Small'> ";

        text += installedText + "\n\n\n";

        if (!licenseSystem.licenseIOS || !licenseSystem.licenseAndroid || !licenseSystem.licenseModule3D) {
          text += "<color #76D6FF>Available platforms and modules:</color>\n\n";
          text += availableText + "\n\n\n";
      }


        text += "<color #76D6FF>Special Thanks:</color>\n\n";
        text += "    The Urho3D Project - http://urho3d.github.io\n\n";
        text += "    Sami Vaarala - http://www.duktape.org";

        return text;

    }

    age_license: Atomic.UIEditField;
    age_runtimelicense: Atomic.UIEditField;
    thirdparty_license: Atomic.UIEditField;
    externaltool_license: Atomic.UIEditField;
    about_text: Atomic.UIEditField;

}

export = About;
