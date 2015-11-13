//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
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
