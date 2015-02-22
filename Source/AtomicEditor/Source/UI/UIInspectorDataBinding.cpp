
#include <TurboBadger/tb_widgets_common.h>
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

        // refresh widget
        SetWidgetValueFromObject();
    }
    else if (type == VAR_VECTOR3 && srcWidget)
    {
        Vector3 value = object_->GetAttribute(attrInfo_->name_).GetVector3();

        if (srcWidget->GetID() == TBID(unsigned(1)))
            value.x_ = srcWidget->GetValue();
        else if (srcWidget->GetID() == TBID(unsigned(2)))
            value.y_ = srcWidget->GetValue();
        else if (srcWidget->GetID() == TBID(unsigned(3)))
            value.z_ = srcWidget->GetValue();

        object_->SetAttribute(attrInfo_->name_, value);
    }
    else if (type == VAR_QUATERNION && srcWidget)
    {
        Quaternion q = object_->GetAttribute(attrInfo_->name_).GetQuaternion();

        Vector3 value = q.EulerAngles();

        if (srcWidget->GetID() == TBID(unsigned(1)))
            value.x_ = srcWidget->GetValue();
        else if (srcWidget->GetID() == TBID(unsigned(2)))
            value.y_ = srcWidget->GetValue();
        else if (srcWidget->GetID() == TBID(unsigned(3)))
            value.z_ = srcWidget->GetValue();

        q.FromEulerAngles(value.x_, value.y_, value.z_);

        object_->SetAttribute(attrInfo_->name_, q);
    }


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
            select->SetValue(value.x_);
        select = widget_->GetWidgetByIDAndType<TBInlineSelect>(TBID(unsigned(2)));
        if (select)
            select->SetValue(value.y_);
        select = widget_->GetWidgetByIDAndType<TBInlineSelect>(TBID(unsigned(3)));
        if (select)
            select->SetValue(value.z_);
    }
    else if (attrInfo_->type_ == VAR_QUATERNION)
    {
        Vector3 value = object_->GetAttribute(attrInfo_->name_).GetQuaternion().EulerAngles();

        TBInlineSelect* select = widget_->GetWidgetByIDAndType<TBInlineSelect>(TBID(unsigned(1)));
        if (select)
            select->SetValue(value.x_);
        select = widget_->GetWidgetByIDAndType<TBInlineSelect>(TBID(unsigned(2)));
        if (select)
            select->SetValue(value.y_);
        select = widget_->GetWidgetByIDAndType<TBInlineSelect>(TBID(unsigned(3)));
        if (select)
            select->SetValue(value.z_);
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

    widgetLocked_ = false;

}

bool InspectorDataBinding::OnEvent(const TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (objectLocked_)
            return false;

        if (widget_ == ev.target || widget_->IsAncestorOf(ev.target))
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
        widget = box;
    }
    else if (attrInfo->type_ == VAR_STRING)
    {
        TBEditField* field = new TBEditField();
        field->SetFontDescription(fd);
        widget = field;
    }
    else if (attrInfo->type_ == VAR_FLOAT)
    {
        TBEditField* field = new TBEditField();
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
            field->SetFontDescription(fd);
            widget = field;
        }
    }
    else if (attrInfo->type_ == VAR_VECTOR3 || attrInfo->type_ == VAR_QUATERNION)
    {
        TBLayout* layout = new TBLayout();
        widget = layout;
        layout->SetSpacing(0);

        LayoutParams lp;
        lp.SetWidth(90);

        for (unsigned i = 0; i < 3; i++)
        {
            TBInlineSelect* select = new TBInlineSelect();
            select->SetID(TBID(i + 1));
            select->SetFontDescription(fd);
            select->SetLimits(-10000000, 10000000);
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
