// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <TurboBadger/tb_editfield.h>
#include <Atomic/Core/Context.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/ResourceEvents.h>

#include <Atomic/Scene/Component.h>

#include <Atomic/UI/TBUI.h>

#include "AEEditor.h"
#include "AEEvents.h"

#include "UIInspectorDataBinding.h"
#include "UIInspectorFrame.h"


#include "UI/Modal/UIModalOps.h"

using namespace tb;

namespace AtomicEditor
{

InspectorFrame::InspectorFrame(Context* context) :
    AEWidget(context)
{
    TBUI* tbui = GetSubsystem<TBUI>();
    tbui->LoadResourceFile(delegate_, "AtomicEditor/editor/ui/inspectorframe.tb.txt");

    delegate_->SetID(TBIDC("projectframe_delegate"));

    delegate_->SetGravity(WIDGET_GRAVITY_TOP_BOTTOM);

    inspectorContainer_ = delegate_->GetWidgetByIDAndType<TBLayout>(TBIDC("inspectorcontainer"));
    assert(inspectorContainer_);

    SubscribeToEvent(E_EDITORACTIVENODECHANGE, HANDLER(InspectorFrame, HandleEditorActiveNodeChange));

}

InspectorFrame::~InspectorFrame()
{

}

/*
TBLayout* InspectorFrame::CreateComponentLayout(Component* component)
{
    TBLayout* componentLayout = new TBLayout(AXIS_Y);
    componentLayout->SetLayoutPosition(LAYOUT_POSITION_LEFT_TOP);

    TBFontDescription fd;
    fd.SetID(TBIDC("Vera"));
    fd.SetSize(11);

    TBTextField* name = new TBTextField();
    name->SetFontDescription(fd);
    name->SetText(component->GetTypeName().CString());
    componentLayout->AddChild(name);

    const Vector<AttributeInfo>* attr = component->GetAttributes();

    if (attr)
    {
        for (unsigned i = 0; i < attr->Size(); i++)
        {
            TBLayout* attrLayout = new TBLayout();
            attrLayout->SetLayoutDistribution(LAYOUT_DISTRIBUTION_GRAVITY);

            const AttributeInfo& info = attr->At(i);
            TBTextField* attrName = new TBTextField();
            attrName->SetFontDescription(fd);

            String name = info.name_;
            if (name == "Is Enabled")
                name = "Enabled";

            attrName->SetText(name.CString());

            attrLayout->AddChild(attrName);

            TBWidget* widget = NULL;

            if (info.enumNames_)
            {
                TBButton* button = new TBButton();
                widget = button;
                int value = component->GetAttribute(i).GetInt();
                button->SetText(info.enumNames_[value]);
            }

            if (info.type_ == VAR_BOOL)
            {
                TBCheckBox* box = new TBCheckBox();
                widget = box;
                box->SetValue(component->GetAttribute(i).GetBool() ? 1 : 0);
            }

            if (info.type_ == VAR_FLOAT)
            {
                TBEditField* editField = new TBEditField();
                editField->SetFontDescription(fd);
                widget = editField;
                editField->SetEditType(EDIT_TYPE_NUMBER);
                editField->SetText(ToString("%f", component->GetAttribute(i).GetFloat()).CString());
            }

            if (info.type_ == VAR_COLOR)
            {
                TBLayout* colorLayout = new TBLayout();

                LayoutParams lp;
                lp.SetWidth(32);

                TBEditField* red = new TBEditField();
                TBEditField* green = new TBEditField();
                TBEditField* blue = new TBEditField();
                TBEditField* alpha = new TBEditField();

                red->SetEditType(EDIT_TYPE_NUMBER);
                green->SetEditType(EDIT_TYPE_NUMBER);
                blue->SetEditType(EDIT_TYPE_NUMBER);
                alpha->SetEditType(EDIT_TYPE_NUMBER);

                red->SetLayoutParams(lp);
                green->SetLayoutParams(lp);
                blue->SetLayoutParams(lp);
                alpha->SetLayoutParams(lp);

                red->SetFontDescription(fd);
                green->SetFontDescription(fd);
                blue->SetFontDescription(fd);
                alpha->SetFontDescription(fd);

                Color color = component->GetAttribute(i).GetColor();

                red->SetText(ToString("%f", color.r_).CString());
                green->SetText(ToString("%f", color.g_).CString());
                blue->SetText(ToString("%f", color.b_).CString());
                alpha->SetText(ToString("%f", color.a_).CString());

                colorLayout->AddChild(red);
                colorLayout->AddChild(green);
                colorLayout->AddChild(blue);
                colorLayout->AddChild(alpha);

                widget = colorLayout;

            }

            if (widget)
            {

                attrLayout->AddChild(widget);

                DataBinding binding;
                binding.widget = widget;
                binding.component = component;
                binding.attrIndex = i;
                dataBindings_.Push(binding);
            }

            componentLayout->AddChild(attrLayout);
        }
    }

    return componentLayout;
}

*/

bool InspectorFrame::OnEvent(const TBWidgetEvent &ev)
{
    for (unsigned i = 0; i < dataBindings_.Size(); i++)
        dataBindings_[i]->OnEvent(ev);

    /*
    if (ev.type == EVENT_TYPE_KEY_DOWN)
    {
        for (unsigned i = 0; i < dataBindings_.Size(); i++)
        {
            const DataBinding& binding = dataBindings_[i];

            if (binding.widget == ev.target || binding.widget->IsAncestorOf(ev.target))
            {
                const AttributeInfo& info = binding.component->GetAttributes()->At(binding.attrIndex);
                return true;
            }
        }
    }

    if (ev.type == EVENT_TYPE_CHANGED)
    {
        if (refreshing_)
            return true;

        for (unsigned i = 0; i < dataBindings_.Size(); i++)
        {
            const DataBinding& binding = dataBindings_[i];

            if (binding.widget == ev.target || binding.widget->IsAncestorOf(ev.target))
            {
                const AttributeInfo& info = binding.component->GetAttributes()->At(binding.attrIndex);

                if (info.type_ == VAR_FLOAT)
                {
                    TBEditField* edit = (TBEditField*) ev.target;
                    float v = ToFloat(edit->GetText());
                    binding.component->SetAttribute(binding.attrIndex, Variant(v));

                }
                else if (info.type_ == VAR_COLOR)
                {
                    Color c = binding.component->GetAttribute(binding.attrIndex).GetColor();

                    if (ev.target == binding.widget->GetChildFromIndex(0))
                    {
                        TBEditField* red = (TBEditField*) ev.target;
                        c.r_ = ToFloat(red->GetText());
                    }
                    else if (ev.target == binding.widget->GetChildFromIndex(1))
                    {
                        TBEditField* green = (TBEditField*) ev.target;
                        c.g_ = ToFloat(green->GetText());
                    }
                    else if (ev.target == binding.widget->GetChildFromIndex(2))
                    {
                        TBEditField* blue = (TBEditField*) ev.target;
                        c.b_ = ToFloat(blue->GetText());
                    }

                    binding.component->SetAttribute(binding.attrIndex, Variant(c));

                }

                return true;

            }
        }

        return true;
    }

    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.target->GetID() == TBIDC("enum popup"))
        {
            int value = (int) (ev.ref_id & 0xFFFF);
            int bindingIdx = (int) (ev.ref_id >> 16);

            const DataBinding& binding = dataBindings_[bindingIdx];
            Variant v(value);
            binding.component->SetAttribute(binding.attrIndex, v);
            const AttributeInfo& info = binding.component->GetAttributes()->At(binding.attrIndex);
            binding.widget->SetText(info.enumNames_[value]);

        }
        else
        {
            for (unsigned i = 0; i < dataBindings_.Size(); i++)
            {
                const DataBinding& binding = dataBindings_[i];

                if (binding.widget == ev.target)
                {
                    const AttributeInfo& info = binding.component->GetAttributes()->At(binding.attrIndex);

                    if (info.type_ == VAR_BOOL)
                    {
                        TBCheckBox* box = (TBCheckBox *) binding.widget;
                        Variant v(box->GetValue() ? true : false);
                        binding.component->SetAttribute(binding.attrIndex, v);
                    }
                    else if (info.enumNames_)
                    {
                        TBMenuWindow *menu = new TBMenuWindow(ev.target, TBIDC("enum popup"));
                        TBGenericStringItemSource* source = menu->GetList()->GetDefaultSource();
                        const char** enumPtr = info.enumNames_;
                        unsigned v = i << 16;
                        while (*enumPtr)
                        {
                            source->AddItem(new TBGenericStringItem(*enumPtr, TBID(v++)));
                            enumPtr++;
                        }
                        menu->Show(source, TBPopupAlignment());

                    }

                }
            }
        }

    }
    */

    return false;
}

/*
void InspectorFrame::CreateTransformLayout()
{
    // transform

    TBFontDescription fd;
    fd.SetID(TBIDC("Vera"));
    fd.SetSize(11);

    transformLayout_ = new TBLayout(AXIS_Y);
    transformLayout_->SetLayoutPosition(LAYOUT_POSITION_LEFT_TOP);

    // POSITION
    TBTextField* posText = new TBTextField();
    posText->SetText("Position");
    posText->SetFontDescription(fd);
    transformLayout_->AddChild(posText);

    TBLayout* positionLayout = new TBLayout();
    positionLayout->SetSpacing(0);

    posXSelect_ = new TBInlineSelect();
    posXSelect_->SetFontDescription(fd);
    posXSelect_->SetLimits(-10000000, 10000000);
    positionLayout->AddChild(posXSelect_);

    posYSelect_ = new TBInlineSelect();
    posYSelect_->SetFontDescription(fd);
    posYSelect_->SetLimits(-10000000, 10000000);
    positionLayout->AddChild(posYSelect_);

    posZSelect_ = new TBInlineSelect();
    posZSelect_->SetFontDescription(fd);
    posZSelect_->SetLimits(-10000000, 10000000);
    positionLayout->AddChild(posZSelect_);

    transformLayout_->AddChild(positionLayout);

    // ROTATION
    TBTextField* rotText = new TBTextField();
    rotText->SetText("Rotation");
    rotText->SetFontDescription(fd);
    transformLayout_->AddChild(rotText);

    TBLayout* rotLayout = new TBLayout();
    rotLayout->SetSpacing(0);

    rotXSelect_ = new TBInlineSelect();
    rotXSelect_->SetFontDescription(fd);
    rotXSelect_->SetLimits(-10000000, 10000000);
    rotLayout->AddChild(rotXSelect_);

    rotYSelect_ = new TBInlineSelect();
    rotYSelect_->SetFontDescription(fd);
    rotYSelect_->SetLimits(-10000000, 10000000);
    rotLayout->AddChild(rotYSelect_);

    rotZSelect_ = new TBInlineSelect();
    rotZSelect_->SetFontDescription(fd);
    rotZSelect_->SetLimits(-10000000, 10000000);
    rotLayout->AddChild(rotZSelect_);

    transformLayout_->AddChild(rotLayout);

    // SCALE
    TBTextField* scaleText = new TBTextField();
    scaleText->SetText("Scale");
    scaleText->SetFontDescription(fd);
    transformLayout_->AddChild(scaleText);

    TBLayout* scaleLayout = new TBLayout();
    scaleLayout->SetSpacing(0);

    scaleXSelect_ = new TBInlineSelect();
    scaleXSelect_->SetFontDescription(fd);
    scaleXSelect_->SetLimits(-10000000, 10000000);
    scaleLayout->AddChild(scaleXSelect_);

    scaleYSelect_ = new TBInlineSelect();
    scaleYSelect_->SetFontDescription(fd);
    scaleYSelect_->SetLimits(-10000000, 10000000);
    scaleLayout->AddChild(scaleYSelect_);

    scaleZSelect_ = new TBInlineSelect();
    scaleZSelect_->SetFontDescription(fd);
    scaleZSelect_->SetLimits(-10000000, 10000000);
    scaleLayout->AddChild(scaleZSelect_);

    transformLayout_->AddChild(scaleLayout);

}
*/

void InspectorFrame::InspectNode(Node* node)
{
    if (node_ == node)
        return;

    node_ = node;

    inspectorContainer_->DeleteAllChildren();

    for (unsigned i = 0; i < dataBindings_.Size(); i++)
        delete dataBindings_[i];

    dataBindings_.Clear();

    if (!node_)
    {
        return;
    }
    else
    {
        TBFontDescription fd;
        fd.SetID(TBIDC("Vera"));
        fd.SetSize(11);

        LayoutParams nlp;
        nlp.SetWidth(304);
        TBLayout* nodeLayout = new TBLayout(AXIS_Y);

        nodeLayout->SetLayoutDistribution(LAYOUT_DISTRIBUTION_GRAVITY);
        nodeLayout->SetLayoutPosition(LAYOUT_POSITION_LEFT_TOP);
        nodeLayout->SetLayoutParams(nlp);

        TBContainer* nodeContainer = new TBContainer();
        nodeContainer->SetGravity(WIDGET_GRAVITY_ALL);

        TBLayout* attrsVerticalLayout = new TBLayout(AXIS_Y);
        attrsVerticalLayout->SetGravity(WIDGET_GRAVITY_ALL);
        attrsVerticalLayout->SetLayoutPosition(LAYOUT_POSITION_LEFT_TOP);
        nodeContainer->AddChild(attrsVerticalLayout);

        const Vector<AttributeInfo>* attrs = node->GetAttributes();

        for (unsigned i = 0; i < attrs->Size(); i++)
        {
            const AttributeInfo* attr = &attrs->At(i);

            InspectorDataBinding*  binding = InspectorDataBinding::Create(node, attr);

            if (binding)
            {
                dataBindings_.Push(binding);

                TBLayout* attrLayout = new TBLayout();

                attrLayout->SetLayoutDistribution(LAYOUT_DISTRIBUTION_GRAVITY);
                TBTextField* name = new TBTextField();

                String bname = attr->name_;
                if (bname == "Is Enabled")
                    bname = "Enabled";

                name->SetText(bname.CString());
                name->SetFontDescription(fd);

                attrLayout->AddChild(name);
                TBWidget* bwidget = binding->GetWidget();
                attrLayout->AddChild(bwidget);

                attrsVerticalLayout->AddChild(attrLayout);
            }

        }

        nodeLayout->AddChild(nodeContainer);

        const Vector<SharedPtr<Component> > components = node->GetComponents();
        for (unsigned i = 0; i < components.Size(); i++)
        {
            Component* c = components[i];

            TBContainer* componentContainer = new TBContainer();
            componentContainer->SetGravity(WIDGET_GRAVITY_ALL);

            TBLayout* attrsVerticalLayout = new TBLayout(AXIS_Y);
            attrsVerticalLayout->SetGravity(WIDGET_GRAVITY_ALL);
            attrsVerticalLayout->SetLayoutPosition(LAYOUT_POSITION_LEFT_TOP);

            TBTextField* cnameField = new TBTextField();
            cnameField->SetText(c->GetTypeName().CString());
            //cnameField->SetFontDescription(fd);
            attrsVerticalLayout->AddChild(cnameField);


            componentContainer->AddChild(attrsVerticalLayout);

            const Vector<AttributeInfo>* attrs = c->GetAttributes();

            if (attrs)
                for (unsigned i = 0; i < attrs->Size(); i++)
                {
                    const AttributeInfo* attr = &attrs->At(i);

                    InspectorDataBinding*  binding = InspectorDataBinding::Create(c, attr);

                    if (binding)
                    {
                        dataBindings_.Push(binding);

                        TBLayout* attrLayout = new TBLayout();

                        attrLayout->SetLayoutDistribution(LAYOUT_DISTRIBUTION_GRAVITY);
                        TBTextField* name = new TBTextField();

                        String bname = attr->name_;
                        if (bname == "Is Enabled")
                            bname = "Enabled";

                        name->SetText(bname.CString());
                        name->SetFontDescription(fd);

                        attrLayout->AddChild(name);
                        TBWidget* bwidget = binding->GetWidget();
                        attrLayout->AddChild(bwidget);

                        attrsVerticalLayout->AddChild(attrLayout);
                    }

                }

            nodeLayout->AddChild(componentContainer);

        }


        /*


        // enabled and name
        TBLayout* nameLayout = new TBLayout();
        TBCheckBox* checkBox = new TBCheckBox();
        TBEditField* nameField = new TBEditField();
        String name = node->GetName();
        if (!name.Length())
            name = "(Anonymous)";

        nameField->SetText(name.CString());
        nameField->SetGravity(WIDGET_GRAVITY_LEFT_RIGHT);
        nameLayout->AddChild(checkBox);
        nameLayout->AddChild(nameField);
        nodeLayout->AddChild(nameLayout);

        nodeLayout->AddChild(transformLayout_);

        TBSeparator* sep = new TBSeparator();
        sep->SetGravity(WIDGET_GRAVITY_LEFT_RIGHT);
        sep->SetSkinBg("AESeparator");
        nodeLayout->AddChild(sep);

        const Vector<SharedPtr<Component> > components = node->GetComponents();
        for (unsigned i = 0; i < components.Size(); i++)
        {
            Component* c = components[i];
            TBLayout* clayout = CreateComponentLayout(c);
            nodeLayout->AddChild(clayout);

            TBSeparator* sep = new TBSeparator();
            sep->SetGravity(WIDGET_GRAVITY_LEFT_RIGHT);
            sep->SetSkinBg("AESeparator");
            nodeLayout->AddChild(sep);
        }



        RefreshTransform();
        */

        inspectorContainer_->AddChild(nodeLayout);

        for (unsigned i = 0; i < dataBindings_.Size(); i++)
            dataBindings_[i]->SetWidgetValueFromObject();
    }

}

void InspectorFrame::HandleEditorActiveNodeChange(StringHash eventType, VariantMap& eventData)
{
    Node* node = (Node*) (eventData[EditorActiveNodeChange::P_NODE].GetPtr());

    InspectNode(node);
}

}
