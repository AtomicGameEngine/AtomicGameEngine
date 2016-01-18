//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//


class SelectionPrefabWidget extends Atomic.UILayout {

    widgetLayout: Atomic.UILayout;
    noticeLayout: Atomic.UILayout;
    node: Atomic.Node;

    constructor() {

        super();

        var fd = new Atomic.UIFontDescription();
        fd.id = "Vera";
        fd.size = 11;

        var widgetLayout = this.widgetLayout = new Atomic.UILayout();
        var noticeLayout = this.noticeLayout = new Atomic.UILayout();

        this.axis = Atomic.UI_AXIS_Y;
        widgetLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;
        noticeLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;

        var name = new Atomic.UITextField();
        name.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
        name.skinBg = "InspectorTextAttrName";
        name.text = "Prefab";
        name.fontDescription = fd;

        var saveButton = new Atomic.UIButton();
        saveButton.text = "Save";
        saveButton.fontDescription = fd;

        saveButton.onClick = () => {

            this.node.scene.sendEvent("SceneEditPrefabSave", {node : this.node});
            return true;
        };

        var undoButton = new Atomic.UIButton();
        undoButton.text = "Revert";
        undoButton.fontDescription = fd;

        undoButton.onClick = () => {

            this.node.scene.sendEvent("SceneEditPrefabRevert", {node : this.node});
            return true;

        };

        var breakButton = new Atomic.UIButton();
        breakButton.text = "Break";
        breakButton.fontDescription = fd;

        breakButton.onClick = () => {

            this.node.scene.sendEvent("SceneEditPrefabBreak", {node : this.node});
            return true;
        };

        var noticeName = new Atomic.UITextField();
        noticeName.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
        noticeName.skinBg = "InspectorTextAttrName";
        noticeName.text = "Prefab";
        noticeName.fontDescription = fd;

        var noticeText = new Atomic.UITextField();
        noticeText.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
        noticeText.skinBg = "InspectorTextAttrName";
        noticeText.text = "Multiple Selection";
        noticeText.fontDescription = fd;

        noticeLayout.addChild(noticeName);
        noticeLayout.addChild(noticeText);

        widgetLayout.addChild(name);
        widgetLayout.addChild(saveButton);
        widgetLayout.addChild(undoButton);
        widgetLayout.addChild(breakButton);

        this.addChild(this.widgetLayout);
        this.addChild(this.noticeLayout);

        this.visibility = Atomic.UI_WIDGET_VISIBILITY_GONE;

    }

    detectPrefab(node: Atomic.Node): boolean {

        if (node.getComponent("PrefabComponent"))
            return true;

        if (node.parent)
            return this.detectPrefab(node.parent);

        return false;

    }


    updateSelection(nodes: Atomic.Node[]) {

        var hasPrefab = false;
        this.node = null;

        for (var i in nodes) {

            var node = nodes[i];
            if (this.detectPrefab(node)) {
                hasPrefab = true;
                break;
            }

        }

        if (!hasPrefab) {
            this.visibility = Atomic.UI_WIDGET_VISIBILITY_GONE;
            return;
        }

        this.visibility = Atomic.UI_WIDGET_VISIBILITY_VISIBLE;

        if (nodes.length > 1) {
            this.noticeLayout.visibility = Atomic.UI_WIDGET_VISIBILITY_VISIBLE;
            this.widgetLayout.visibility = Atomic.UI_WIDGET_VISIBILITY_GONE;
            return;
        }

        this.noticeLayout.visibility = Atomic.UI_WIDGET_VISIBILITY_GONE;
        this.widgetLayout.visibility = Atomic.UI_WIDGET_VISIBILITY_VISIBLE;
        this.node = nodes[0];

    }


}

export = SelectionPrefabWidget;
