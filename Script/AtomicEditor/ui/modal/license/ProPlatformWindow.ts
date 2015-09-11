//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import EditorEvents = require("editor/EditorEvents");
import EditorUI = require("ui/EditorUI");
import ModalWindow = require("../ModalWindow");
import ProgressModal = require("../ProgressModal");

class ProPlatformWindow extends ModalWindow {

    constructor() {

        super(false);

        this.init("Platform License Required", "AtomicEditor/editor/ui/platformsinfo.tb.txt");

        var editField = <Atomic.UIEditField> this.getWidget("info");

        var text = "\nAtomic Game Engine Pro is required to deploy apps to this platform.\n\n<color #D4FB79>Installed platforms:</color>\n\n";

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

        text += installedText + "\n\n\n";

        if (!licenseSystem.licenseIOS || !licenseSystem.licenseAndroid || !licenseSystem.licenseModule3D) {
          text += "<color #76D6FF>Available platforms:</color>\n\n";
          text += availableText + "\n\n\n";
      }

      editField.text = text;

      this.resizeToFitContent();
      this.center();

      editField.reformat();


    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var id = ev.target.id;

            if (id == 'purchase') {

                Atomic.fileSystem.systemOpen("https://store.atomicgameengine.com/site");

            } else if (id == "ok") {

                this.hide();

                return true;
            }

            return false;

        }
    }

}

export = ProPlatformWindow;
