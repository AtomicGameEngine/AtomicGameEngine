class DataBinding {

    constructor(object: Atomic.Serializable, attrInfo: Atomic.AttributeInfo, widget: Atomic.UIWidget) {

        this.object = object;
        this.attrInfo = attrInfo;
        this.widget = widget;

    }

    static createBinding(object: Atomic.Serializable, attrInfo: Atomic.AttributeInfo): DataBinding {

        var widget: Atomic.UIWidget = null;

        var fd = new Atomic.UIFontDescription();
        fd.id = "Vera";
        fd.size = 11;

        if (attrInfo.type == Atomic.VAR_BOOL) {

            var box = new Atomic.UICheckBox();
            box.skinBg = "TBGreyCheckBox";
            widget = box;

        } else if (attrInfo.type == Atomic.VAR_STRING) {

            var field = new Atomic.UIEditField();
            field.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
            field.skinBg = "TBAttrEditorField";;
            field.fontDescription = fd;
            var lp = new Atomic.UILayoutParams();
            lp.width = 140;
            field.layoutParams = lp;

            widget = field;
        }
        else if (attrInfo.type == Atomic.VAR_VECTOR3 || attrInfo.type == Atomic.VAR_QUATERNION)
        {
            var layout = new Atomic.UILayout();
            widget = layout;
            layout.spacing = 0;

            var lp = new Atomic.UILayoutParams();
            lp.width = 100;

            for (var i = 0; i < 3; i++)
            {
                var select = new Atomic.UIInlineSelect();
                select.id = String(i + 1);
                select.fontDescription = fd;
                select.skinBg = "InspectorTextAttrName";
                select.setLimits(-10000000, 10000000);
                var editlp = new Atomic.UILayoutParams();
                editlp.minWidth = 60;
                select.editFieldLayoutParams = editlp;
                select.layoutParams = lp;
                layout.addChild(select);
            }

        }

        if (widget) {

            var binding = new DataBinding(object, attrInfo, widget);
            return binding;

        }

        return null;

    }

    setObjectValueFromWidget(srcWidget:Atomic.UIWidget) {

      if (this.objectLocked)
          return;

      this.objectLocked = true;

      var type = this.attrInfo.type;

      if (type == Atomic.VAR_BOOL)
      {
          var box = <Atomic.UICheckBox> this.widget;

          this.object.setAttribute(this.attrInfo.name, box.value ? true : false);
      }



      this.objectLocked = false;

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) : boolean {

      if (this.objectLocked)
          return false;

      if (ev.type == Atomic.UI_EVENT_TYPE_CHANGED)
      {
          if (this.widget == ev.target || this.widget.isAncestorOf(ev.target))
          {
              this.setObjectValueFromWidget(ev.target);
              return true;
          }
      }

      return false;

    }

    object: Atomic.Serializable;
    objectLocked:boolean = false;
    attrInfo: Atomic.AttributeInfo;
    widget: Atomic.UIWidget;

}

export = DataBinding;
