//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

class InspectorUtils {

  private static Ctor = (() => {

    var fd = InspectorUtils.attrFontDesc = new Atomic.UIFontDescription();
    fd.id = "Vera";
    fd.size = 11;

  })();

  static createSeparator(parent:Atomic.UIWidget):Atomic.UISeparator {

    var sep = new Atomic.UISeparator();

    sep.gravity = Atomic.UI_GRAVITY_LEFT_RIGHT;
    sep.skinBg = "AESeparator";

    parent.addChild(sep);
    return sep;

  }

  static createContainer():Atomic.UIContainer {

    var container = new Atomic.UIContainer();

    container.skinBg = "AEContainer";

    return container;

  }

  static createAttrName(name:string):Atomic.UITextField {

    var nameField = new Atomic.UITextField();
    nameField.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
    nameField.skinBg = "InspectorTextAttrName";
    nameField.text = name;
    nameField.fontDescription = InspectorUtils.attrFontDesc;

    // atttribute name layout param
    var atlp = new Atomic.UILayoutParams();
    atlp.width = 120;
    nameField.layoutParams = atlp;

    return nameField;
  }

  static createEditField():Atomic.UIEditField {

    var edit = new Atomic.UIEditField();
    edit.id = "editfield";
    edit.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
    edit.skinBg = "TBAttrEditorField";
    edit.fontDescription = InspectorUtils.attrFontDesc;
    var lp = new Atomic.UILayoutParams();
    lp.width = 160;
    lp.height = 24;
    edit.layoutParams = lp;

    return edit;

  }

  static createAttrEditField(name:string, parent:Atomic.UIWidget):Atomic.UIEditField {

    var attrLayout = new Atomic.UILayout();

    attrLayout.layoutSize = Atomic.UI_LAYOUT_SIZE_AVAILABLE;
    attrLayout.gravity = Atomic.UI_GRAVITY_LEFT_RIGHT;
    attrLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;

    var _name = InspectorUtils.createAttrName(name);
    attrLayout.addChild(_name);

    var edit = InspectorUtils.createEditField();

    attrLayout.addChild(edit);
    parent.addChild(attrLayout);

    return edit;

  }

  static createAttrCheckBox(name:string, parent:Atomic.UIWidget):{ textField:Atomic.UITextField, checkBox: Atomic.UICheckBox} {

    var attrLayout = new Atomic.UILayout();

    attrLayout.layoutSize = Atomic.UI_LAYOUT_SIZE_AVAILABLE;
    attrLayout.gravity = Atomic.UI_GRAVITY_LEFT_RIGHT;
    attrLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;

    var _name = InspectorUtils.createAttrName(name);
    attrLayout.addChild(_name);

    var checkBox = new Atomic.UICheckBox();

    attrLayout.addChild(checkBox);
    parent.addChild(attrLayout);

    return {textField: _name, checkBox : checkBox};

  }


  static createAttrEditFieldWithSelectButton(name:string, parent:Atomic.UIWidget):{editField:Atomic.UIEditField, selectButton:Atomic.UIButton} {

    var attrLayout = new Atomic.UILayout();
    attrLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_POSITION_LEFT_TOP;

    if (name) {
      var _name = InspectorUtils.createAttrName(name);
      attrLayout.addChild(_name);
    }

    var fieldLayout = new Atomic.UILayout();
    fieldLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_POSITION_LEFT_TOP;

    var edit = InspectorUtils.createEditField();

    var selectButton = new Atomic.UIButton();
    selectButton.text = "...";
    selectButton.fontDescription = InspectorUtils.attrFontDesc;

    fieldLayout.addChild(edit);
    fieldLayout.addChild(selectButton);

    attrLayout.addChild(fieldLayout);
    parent.addChild(attrLayout);

    return {editField:edit, selectButton:selectButton};

  }


  // "static constructor"
  static attrFontDesc:Atomic.UIFontDescription;

}

export = InspectorUtils;
