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

            if (id == "purchase") {

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
