//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import EditorUI = require("ui/EditorUI");

class CSComponentClassSelector extends Atomic.UIWindow {

    constructor(editField: Atomic.UIEditField, component: AtomicNET.CSComponent) {

        super();

        var assemblyFile = component.assemblyFile;

        this.text = "Select Class: " + assemblyFile.name;

        this.rect = [0, 0, 400, 512];

        var mainLayout = new Atomic.UILayout();
        mainLayout.gravity = Atomic.UI_GRAVITY_ALL;
        mainLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_AVAILABLE;
        mainLayout.axis = Atomic.UI_AXIS_Y;
        this.contentRoot.addChild(mainLayout);

        // really want a grid container
        var scrollContainer = new Atomic.UIScrollContainer();
        scrollContainer.gravity = Atomic.UI_GRAVITY_ALL;
        scrollContainer.scrollMode = Atomic.UI_SCROLL_MODE_Y_AUTO;
        scrollContainer.adaptContentSize = true;

        var scrollLayout = new Atomic.UILayout();
        scrollLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        scrollLayout.layoutDistributionPosition = Atomic.UI_LAYOUT_DISTRIBUTION_POSITION_LEFT_TOP;
        scrollLayout.axis = Atomic.UI_AXIS_Y;

        scrollContainer.contentRoot.addChild(scrollLayout);

        var window = this;

        for (var i in assemblyFile.classNames) {

            var classname = assemblyFile.classNames[i];
            var button = new Atomic.UIButton();
            button.text = classname;

            button.onClick = function() {
                editField.text = this.text;
                component.componentClassName = this.text;
                window.close();
            }.bind(button);

            scrollLayout.addChild(button);
        }

        mainLayout.addChild(scrollContainer);

        EditorUI.getMainFrame().addChild(this);

        this.center();

        this.subscribeToEvent("WidgetEvent", (data) => this.handleWidgetEvent(data));

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            if (ev.target != this && !this.isAncestorOf(ev.target)) {

                //this.close();

            }

        }

        return false;

    }

}

export = CSComponentClassSelector;
