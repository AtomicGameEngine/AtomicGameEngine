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

import InspectorWidget = require("./InspectorWidget");
import ArrayEditWidget = require("./ArrayEditWidget");
import InspectorUtils = require("./InspectorUtils");

class PrefabInspector extends InspectorWidget {

    constructor() {

        super();

        this.fd.id = "Vera";
        this.fd.size = 11;

        this.subscribeToEvent(this, "WidgetEvent", (data) => this.handleWidgetEvent(data));

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent):boolean {

      return false;

    }

    onApply() {

        var nameText = this.nameEdit.text;
        if (nameText != this.asset.name) {
            if (!this.asset.rename(nameText)) {
                this.nameEdit.text = this.asset.name;
            }
        }
    }

    inspect(asset: ToolCore.Asset) {

        this.asset = asset;
        this.importer = <ToolCore.PrefabImporter>asset.importer;

        // node attr layout
        var rootLayout = this.rootLayout;

        // Assembly Section
        var prefabLayout = this.createSection(rootLayout, "Prefab", 1);

        var container = InspectorUtils.createContainer();
        container.gravity = Atomic.UI_GRAVITY_ALL;
        prefabLayout.addChild(container);

        var panel = new Atomic.UILayout();
        panel.axis = Atomic.UI_AXIS_Y;
        panel.layoutSize = Atomic.UI_LAYOUT_SIZE_PREFERRED;
        panel.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        container.addChild(panel);

        // Name Edit
        this.nameEdit = this.createAttrEditField("Name", panel);
        this.nameEdit.text = this.asset.name;

        var applyButton = this.createApplyButton();
        panel.addChild(applyButton);

        /*
        var editButton = new Atomic.UIButton();
        editButton.fontDescription = this.fd;
        editButton.text = "Edit";

        editButton.onClick = function() {

        }.bind(this);

        prefabLayout.addChild(editButton);
        */


    }

    nameEdit: Atomic.UIEditField;

    asset: ToolCore.Asset;
    importer: ToolCore.PrefabImporter;
    fd: Atomic.UIFontDescription = new Atomic.UIFontDescription();

}

export = PrefabInspector;
