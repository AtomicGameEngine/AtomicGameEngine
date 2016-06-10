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

        var copyButton = new Atomic.UIButton();
        copyButton.text = "Copy";
        copyButton.fontDescription = fd;

        copyButton.onClick = () => {
            this.node.scene.sendEvent("SceneEditPrefabCopy", {node : this.node });
            return true;
        };

        var pasteButton = new Atomic.UIButton();
        pasteButton.text = "Paste";
        pasteButton.fontDescription = fd;

        pasteButton.onClick = () => {
            this.node.scene.sendEvent("SceneEditPrefabPaste", {node : this.node });
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
        widgetLayout.addChild(copyButton);
        widgetLayout.addChild(pasteButton);

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
