//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import InspectorWidget = require("./InspectorWidget");
import ArrayEditWidget = require("./ArrayEditWidget");
import InspectorUtils = require("./InspectorUtils");

class AssemblyInspector extends InspectorWidget {

    constructor() {

        super();

        this.subscribeToEvent(this, "WidgetEvent", (data) => this.handleWidgetEvent(data));

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent):boolean {

      return false;

    }

    inspect(asset: ToolCore.Asset) {

        this.asset = asset;
        this.importer = <ToolCore.NETAssemblyImporter> asset.importer;

        // node attr layout
        var rootLayout = this.rootLayout;

        // Assembly Section
        var assemblyLayout = this.createSection(rootLayout, "NETAssembly", 1);

        var assemblyFile = <AtomicNET.CSComponentAssembly> asset.importer.getResource();

        var container = InspectorUtils.createContainer();
        container.gravity = Atomic.UI_GRAVITY_ALL;
        assemblyLayout.addChild(container);

        var panel = new Atomic.UILayout();
        panel.axis = Atomic.UI_AXIS_Y;
        panel.layoutSize = Atomic.UI_LAYOUT_SIZE_PREFERRED;
        panel.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        container.addChild(panel);

        var label = InspectorUtils.createAttrName("CSComponents:");
        panel.addChild(label);

        for (var index in assemblyFile.classNames) {
            var classname = assemblyFile.classNames[index];

            label = InspectorUtils.createAttrName(classname);
            panel.addChild(label);
        }

    }


    asset: ToolCore.Asset;
    importer:ToolCore.NETAssemblyImporter;

}

export = AssemblyInspector;
