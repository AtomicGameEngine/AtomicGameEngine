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

  static createColorWidget():Atomic.UIColorWidget {

    var colorWidget = new Atomic.UIColorWidget();
    colorWidget.id = "colorfield";

    var lp = new Atomic.UILayoutParams();
    lp.width = 160;
    lp.height = 24;
    colorWidget.layoutParams = lp;

    return colorWidget;

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

  static createAttrColorFieldWithSelectButton(name:string, parent:Atomic.UIWidget):{colorWidget:Atomic.UIColorWidget, selectButton:Atomic.UIButton} {

    var attrLayout = new Atomic.UILayout();
    attrLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_POSITION_LEFT_TOP;

    if (name) {
      var _name = InspectorUtils.createAttrName(name);
      attrLayout.addChild(_name);
    }

    var fieldLayout = new Atomic.UILayout();
    fieldLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_POSITION_LEFT_TOP;

    var colorWidget = InspectorUtils.createColorWidget();

    var selectButton = new Atomic.UIButton();
    selectButton.text = "...";
    selectButton.fontDescription = InspectorUtils.attrFontDesc;

    fieldLayout.addChild(colorWidget);
    fieldLayout.addChild(selectButton);

    attrLayout.addChild(fieldLayout);
    parent.addChild(attrLayout);

    return {colorWidget:colorWidget, selectButton:selectButton};

  }

  // "static constructor"
  static attrFontDesc:Atomic.UIFontDescription;

}

export = InspectorUtils;
