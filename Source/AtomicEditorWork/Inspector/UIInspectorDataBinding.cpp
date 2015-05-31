
#include <TurboBadger/tb_widgets_common.h>
#include <TurboBadger/tb_menu_window.h>
#include <TurboBadger/tb_inline_select.h>

#include "AtomicEditor.h"

#include <Atomic/Core/StringUtils.h>

#include "UIInspectorDataBinding.h"

namespace AtomicEditor
{

void InspectorDataBinding::SetObjectValueFromWidget(TBWidget *srcWidget)
{
    if (objectLocked_)
        return;

    objectLocked_ = true;

    VariantType type = attrInfo_->type_;

    if (type == VAR_BOOL)
    {
        TBCheckBox* box = (TBCheckBox *) widget_;
        Variant v(box->GetValue() ? true : false);
        object_->SetAttribute(attrInfo_->name_, v);
    }
    else if (type == VAR_INT)
    {
        TBEditField* field = (TBEditField*) widget_;
        Variant v(ToInt(field->GetText().CStr()));
        object_->SetAttribute(attrInfo_->name_, v);
    }
    else if (type == VAR_FLOAT)
    {
        TBEditField* field = (TBEditField*) widget_;
        Variant v(ToFloat(field->GetText().CStr()));
        object_->SetAttribute(attrInfo_->name_, v);
    }
    else if (type == VAR_STRING)
    {
        TBEditField* field = (TBEditField*) widget_;
        Variant v(String(field->GetText().CStr()));
        object_->SetAttribute(attrInfo_->name_, v);
    }
    else if (type == VAR_COLOR && srcWidget)
    {
        Color value = object_->GetAttribute(attrInfo_->name_).GetColor();

        TBInlineSelect* select = NULL;
        if (srcWidget->GetID() == TBID(unsigned(1)))
        {
            select = (TBInlineSelect*) srcWidget;
            value.r_ = (float ) select->GetValueDouble();
        }
        else if (srcWidget->GetID() == TBID(unsigned(2)))
        {
            select = (TBInlineSelect*) srcWidget;
            value.g_ = (float ) select->GetValueDouble();
        }
        else if (srcWidget->GetID() == TBID(unsigned(3)))
        {
            select = (TBInlineSelect*) srcWidget;
            value.b_ = (float ) select->GetValueDouble();
        }
        else if (srcWidget->GetID() == TBID(unsigned(4)))
        {
            select = (TBInlineSelect*) srcWidget;
            value.a_ = (float ) select->GetValueDouble();
        }

        object_->SetAttribute(attrInfo_->name_, value);
    }
    else if (type == VAR_VECTOR3 && srcWidget)
    {
        Vector3 value = object_->GetAttribute(attrInfo_->name_).GetVector3();

        if (srcWidget->GetID() == TBID(unsigned(1)))
            value.x_ = srcWidget->GetValueDouble();
        else if (srcWidget->GetID() == TBID(unsigned(2)))
            value.y_ = srcWidget->GetValueDouble();
        else if (srcWidget->GetID() == TBID(unsigned(3)))
            value.z_ = srcWidget->GetValueDouble();

        object_->SetAttribute(attrInfo_->name_, value);
    }
    else if (type == VAR_QUATERNION && srcWidget)
    {
        Quaternion q = object_->GetAttribute(attrInfo_->name_).GetQuaternion();

        Vector3 value = q.EulerAngles();

        if (srcWidget->GetID() == TBID(unsigned(1)))
            value.x_ = srcWidget->GetValueDouble();
        else if (srcWidget->GetID() == TBID(unsigned(2)))
            value.y_ = srcWidget->GetValueDouble();
        else if (srcWidget->GetID() == TBID(unsigned(3)))
            value.z_ = srcWidget->GetValueDouble();

        q.FromEulerAngles(value.x_, value.y_, value.z_);

        object_->SetAttribute(attrInfo_->name_, q);
    }

    // refresh widget
    //SetWidgetValueFromObject();

    objectLocked_ = false;

}

void InspectorDataBinding::SetWidgetValueFromObject()
{
    if (widgetLocked_)
        return;

    widgetLocked_ = true;

    if (attrInfo_->type_ == VAR_BOOL)
    {
        bool value = object_->GetAttribute(attrInfo_->name_).GetBool();
        widget_->SetValue(value ? 1 : 0);
            }
    else if (attrInfo_->type_ == VAR_VECTOR3)
    {
        Vector3 value = object_->GetAttribute(attrInfo_->name_).GetVector3();

        TBInlineSelect* select = widget_->GetWidgetByIDAndType<TBInlineSelect>(TBID(unsigned(1)));
        if (select)
            select->SetValueDouble(value.x_);
        select = widget_->GetWidgetByIDAndType<TBInlineSelect>(TBID(unsigned(2)));
        if (select)
            select->SetValueDouble(value.y_);
        select = widget_->GetWidgetByIDAndType<TBInlineSelect>(TBID(unsigned(3)));
        if (select)
            select->SetValueDouble(value.z_);
    }
    else if (attrInfo_->type_ == VAR_QUATERNION)
    {
        Vector3 value = object_->GetAttribute(attrInfo_->name_).GetQuaternion().EulerAngles();

        TBInlineSelect* select = widget_->GetWidgetByIDAndType<TBInlineSelect>(TBID(unsigned(1)));
        if (select)
            select->SetValueDouble(value.x_);
        select = widget_->GetWidgetByIDAndType<TBInlineSelect>(TBID(unsigned(2)));
        if (select)
            select->SetValueDouble(value.y_);
        select = widget_->GetWidgetByIDAndType<TBInlineSelect>(TBID(unsigned(3)));
        if (select)
            select->SetValueDouble(value.z_);
    }
    else if (attrInfo_->type_ == VAR_STRING)
    {
        String value = object_->GetAttribute(attrInfo_->name_).GetString();
        widget_->SetText(value.CString());
    }
    else if (attrInfo_->type_ == VAR_FLOAT)
    {
        float value = object_->GetAttribute(attrInfo_->name_).GetFloat();
        widget_->SetText(ToString("%f", value).CString());
    }
    else if (attrInfo_->type_ == VAR_INT)
    {
        int value = object_->GetAttribute(attrInfo_->name_).GetInt();

        if (attrInfo_->enumNames_)
        {
            widget_->SetText(attrInfo_->enumNames_[value]);
        }
        else
        {
            widget_->SetText(ToString("%i", value).CString());
        }

    }
    else if (attrInfo_->type_ == VAR_COLOR)
    {
        Color value = object_->GetAttribute(attrInfo_->name_).GetColor();

        TBInlineSelect* select = widget_->GetWidgetByIDAndType<TBInlineSelect>(TBID(unsigned(1)));
        if (select)
            select->SetValueDouble(value.r_);
        select = widget_->GetWidgetByIDAndType<TBInlineSelect>(TBID(unsigned(2)));
        if (select)
            select->SetValueDouble(value.g_);
        select = widget_->GetWidgetByIDAndType<TBInlineSelect>(TBID(unsigned(3)));
        if (select)
            select->SetValue(value.b_);
        select = widget_->GetWidgetByIDAndType<TBInlineSelect>(TBID(unsigned(4)));
        if (select)
            select->SetValueDouble(value.a_);

    }


    widgetLocked_ = false;

}

bool InspectorDataBinding::OnEvent(const TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (objectLocked_)
            return false;

        String id = attrInfo_->name_;
        id += " enum popup";

        if (ev.target->GetID() == TBIDC(id.CString()))
        {
            Variant value(int(ev.ref_id-1));
            object_->SetAttribute(attrInfo_->name_, value);
            SetWidgetValueFromObject();
        }
        else if (widget_ == ev.target && attrInfo_->enumNames_)
        {
            TBMenuWindow *menu = new TBMenuWindow(ev.target, TBIDC(id.CString()));
            TBGenericStringItemSource* source = menu->GetList()->GetDefaultSource();
            const char** enumPtr = attrInfo_->enumNames_;
            unsigned v = 1;
            while (*enumPtr)
            {
                source->AddItem(new TBGenericStringItem(*enumPtr, TBID(v++)));
                enumPtr++;
            }
            menu->Show(source, TBPopupAlignment());

        }
        else if (widget_ == ev.target || widget_->IsAncestorOf(ev.target))
        {
            SetObjectValueFromWidget(ev.target);
        }
    }

    if (ev.type == EVENT_TYPE_CHANGED)
    {
        if (objectLocked_)
            return false;

        if (widget_ == ev.target || widget_->IsAncestorOf(ev.target))
        {
            SetObjectValueFromWidget(ev.target);
        }
    }

    return false;
}

InspectorDataBinding* InspectorDataBinding::Create(Serializable* object, const AttributeInfo* attrInfo)
{

    if (attrInfo->mode_ & AM_NOEDIT)
        return NULL;

    TBWidget* widget = NULL;
    InspectorDataBinding* binding = NULL;

    TBFontDescription fd;
    fd.SetID(TBIDC("Vera"));
    fd.SetSize(11);

    if (attrInfo->type_ == VAR_BOOL)
    {
        TBCheckBox* box = new TBCheckBox();
        box->SetSkinBg(TBIDC("TBGreyCheckBox"));
        widget = box;
    }
    else if (attrInfo->type_ == VAR_STRING)
    {
        TBEditField* field = new TBEditField();
        field->SetTextAlign(TB_TEXT_ALIGN_LEFT);
        field->SetSkinBg(TBIDC("TBAttrEditorField"));
        field->SetFontDescription(fd);
        LayoutParams lp;
        lp.SetWidth(140);
        field->SetLayoutParams(lp);

        widget = field;
    }
    else if (attrInfo->type_ == VAR_FLOAT)
    {
        TBEditField* field = new TBEditField();
        field->SetTextAlign(TB_TEXT_ALIGN_CENTER);
        field->SetSkinBg(TBIDC("TBAttrEditorField"));
        field->SetFontDescription(fd);
        widget = field;
    }
    else if (attrInfo->type_ == VAR_COLOR)
    {
        TBLayout* layout = new TBLayout();
        widget = layout;
        layout->SetSpacing(0);

        LayoutParams lp;
        lp.SetWidth(70);

        for (unsigned i = 0; i < 4; i++)
        {
            TBInlineSelect* select = new TBInlineSelect();
            select->SetID(TBID(i + 1));
            select->SetFontDescription(fd);
            select->SetLimits(-10000000, 10000000);
            select->SetLayoutParams(lp);
            layout->AddChild(select);
        }

    }
    else if (attrInfo->type_ == VAR_INT)
    {
        if (attrInfo->enumNames_)
        {
            TBButton* field = new TBButton();
            field->SetFontDescription(fd);
            widget = field;
        }
        else
        {
            TBEditField* field = new TBEditField();
            field->SetTextAlign(TB_TEXT_ALIGN_CENTER);
            field->SetFontDescription(fd);
            field->SetSkinBg(TBIDC("TBAttrEditorField"));
            widget = field;
        }
    }
    else if (attrInfo->type_ == VAR_VECTOR3 || attrInfo->type_ == VAR_QUATERNION)
    {
        TBLayout* layout = new TBLayout();
        widget = layout;
        layout->SetSpacing(0);

        LayoutParams lp;
        lp.SetWidth(100);

        for (unsigned i = 0; i < 3; i++)
        {
            TBInlineSelect* select = new TBInlineSelect();
            select->SetID(TBID(i + 1));
            select->SetFontDescription(fd);
            select->SetSkinBg(TBIDC("InspectorTextAttrName"));
            select->SetLimits(-10000000, 10000000);
            LayoutParams editlp;
            editlp.min_w = 60;
            select->SetEditFieldLayoutParams(editlp);
            select->SetLayoutParams(lp);
            layout->AddChild(select);
        }

    }

    if (widget)
    {
        binding = new InspectorDataBinding();
        binding->object_ = object;
        binding->widget_ = widget;
        binding->attrInfo_ = attrInfo;
    }

    return binding;
}

}
