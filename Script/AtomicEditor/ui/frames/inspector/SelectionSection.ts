//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import SerializableEditType = require("./SerializableEditType");
import AttributeInfoEdit = require("./AttributeInfoEdit");
import SelectionSectionUI = require("./SelectionSectionUI");

import "./ComponentAttributeUI";

abstract class SelectionSection extends Atomic.UISection {

    hasDynamicAttr: boolean = false;
    editType: SerializableEditType;
    attrLayout: Atomic.UILayout;
    suppressed: boolean = false;
    customUI: SelectionSectionUI;

    attrEdits: { [name: string]: AttributeInfoEdit } = {};

    constructor(editType: SerializableEditType) {

        super();

        this.editType = editType;

        this.text = editType.typeName;

        this.createUI();

    }

    contains(serial:Atomic.Serializable):boolean {

        return this.editType.objects.indexOf(serial) == -1;

    }

    refresh() {

        for (var name in this.attrEdits) {

            this.attrEdits[name].refresh();

        }

        if (this.customUI)
            this.customUI.refresh();

    }

    suppress(value: boolean) {

        if (this.suppressed == value) {
            return;
        }

        this.suppressed = value;
        if (value) {
            this.visibility = Atomic.UI_WIDGET_VISIBILITY_GONE;
        } else {
            this.visibility = Atomic.UI_WIDGET_VISIBILITY_VISIBLE;
        }

    }

    updateDynamicAttrInfos(attrInfos: Atomic.AttributeInfo[]) {

        Atomic.ui.blockChangedEvents = true;

        this.editType.attrInfos = attrInfos;

        var attrEdit: AttributeInfoEdit;
        var remove: AttributeInfoEdit[] = [];

        var addWidget: Atomic.UIWidget;

        for (var name in this.attrEdits) {

            attrEdit = this.attrEdits[name];

            if (attrEdit.attrInfo.dynamic) {
                remove.push(attrEdit);
            } else {
                addWidget = attrEdit;
            }

        }

        for (var i in remove) {

            var attrEdit = remove[i];
            attrEdit.remove();
            delete this.attrEdits[attrEdit.attrInfo.name];

        }

        for (var i in attrInfos) {

            var attr = attrInfos[i];

            if (!attr.dynamic) {
                continue;
            }

            if (attr.mode & Atomic.AM_NOEDIT)
                continue;

            var attrEdit = AttributeInfoEdit.createAttrEdit(this.editType, attr);

            if (!attrEdit)
                continue;

            this.attrEdits[attr.name] = attrEdit;

            if (!addWidget) {
                this.attrLayout.addChild(attrEdit);
                addWidget = attrEdit;
            } else {
                this.attrLayout.addChildAfter(attrEdit, addWidget);
                addWidget = attrEdit;
            }

        }

        this.refresh();

        Atomic.ui.blockChangedEvents = false;

    }

    createUI() {

        var attrLayout = this.attrLayout = new Atomic.UILayout(Atomic.UI_AXIS_Y);
        attrLayout.spacing = 3;
        attrLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        attrLayout.layoutSize = Atomic.UI_LAYOUT_SIZE_AVAILABLE;

        this.contentRoot.addChild(attrLayout);

        for (var i in this.editType.attrInfos) {

            var attr = this.editType.attrInfos[i];

            if (attr.mode & Atomic.AM_NOEDIT)
                continue;

            var attrEdit = AttributeInfoEdit.createAttrEdit(this.editType, attr);

            if (!attrEdit)
                continue;

            this.attrEdits[attr.name] = attrEdit;

            attrLayout.addChild(attrEdit);

        }

        if (SelectionSection.customSectionUI[this.editType.typeName]) {

            this.customUI = new SelectionSection.customSectionUI[this.editType.typeName]();
            this.customUI.createUI(this.editType);
            attrLayout.addChild(this.customUI);

        }

    }

    static fontDesc: Atomic.UIFontDescription;

    static customSectionUI: { [typeName: string]: typeof SelectionSectionUI } = {};

    static registerCustomSectionUI(typeName: string, ui: typeof SelectionSectionUI) {

        SelectionSection.customSectionUI[typeName] = ui;

    }


    private static Ctor = (() => {

        var fd = SelectionSection.fontDesc = new Atomic.UIFontDescription();
        fd.id = "Vera";
        fd.size = 11;

    })();

}

export = SelectionSection;
