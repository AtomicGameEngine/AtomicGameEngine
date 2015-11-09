//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import ScriptWidget = require("ui/ScriptWidget");

class InspectorWidget extends ScriptWidget {

    constructor() {

        super();

        var fd = this.attrFontDesc = new Atomic.UIFontDescription();
        fd.id = "Vera";
        fd.size = 11;

        var nlp = new Atomic.UILayoutParams();
        nlp.width = 310;

        var layout = this.rootLayout = new Atomic.UILayout();
        layout.spacing = 4;

        layout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;
        layout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        layout.layoutParams = nlp;
        layout.axis = Atomic.UI_AXIS_Y;

        this.gravity = Atomic.UI_GRAVITY_ALL;
        this.addChild(layout);

        this.subscribeToEvent("WidgetEvent", (data) => this.handleWidgetEvent(data));

    }

    onApply() {
      console.log("Apply Pressed!");
    }

    createAttrName(name:string):Atomic.UITextField {

      var nameField = new Atomic.UITextField();
      nameField.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
      nameField.skinBg = "InspectorTextAttrName";
      nameField.text = name;
      nameField.fontDescription = this.attrFontDesc;
      return nameField;
    }

    createSection(parent:Atomic.UIWidget, text:string, expanded:number):Atomic.UILayout {

      var section = new Atomic.UISection();

      section.text = text;
      section.value = expanded;
      section.fontDescription = this.attrFontDesc;

      var layout = this.createVerticalAttrLayout();
      parent.addChild(section);
      section.contentRoot.addChild(layout);

      return layout;

    }

    createVerticalAttrLayout():Atomic.UILayout {

      var layout = new Atomic.UILayout(Atomic.UI_AXIS_Y);
      layout.spacing = 3;
      layout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
      layout.layoutSize = Atomic.UI_LAYOUT_SIZE_AVAILABLE;

      return layout;

    }

    createApplyButton():Atomic.UIButton {

      var button = new Atomic.UIButton();
      button.fontDescription = this.attrFontDesc;
      button.gravity = Atomic.UI_GRAVITY_RIGHT;
      button.text = "Apply";

      button.onClick = function() {

        this.onApply();

      }.bind(this);

      return button;

    }

    createAttrCheckBox(name:string, parent:Atomic.UIWidget):Atomic.UICheckBox {

      var attrLayout = new Atomic.UILayout();
      attrLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;

      var _name = this.createAttrName(name);
      attrLayout.addChild(_name);

      var box = new Atomic.UICheckBox();
      box.skinBg = "TBGreyCheckBox";
      attrLayout.addChild(box);
      parent.addChild(attrLayout);

      return box;

    }

    createAttrEditField(name:string, parent:Atomic.UIWidget):Atomic.UIEditField {

      var attrLayout = new Atomic.UILayout();
      attrLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;

      var _name = this.createAttrName(name);
      attrLayout.addChild(_name);

      var edit = new Atomic.UIEditField();
      edit.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
      edit.skinBg = "TBAttrEditorField";
      edit.fontDescription = this.attrFontDesc;
      var lp = new Atomic.UILayoutParams();
      lp.width = 140;
      edit.layoutParams = lp;

      attrLayout.addChild(edit);
      parent.addChild(attrLayout);

      return edit;

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent):boolean {

      return false;

    }

    rootLayout:Atomic.UILayout;
    attrFontDesc:Atomic.UIFontDescription;
}

export  = InspectorWidget;
