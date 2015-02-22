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

bool InspectorFrame::OnEvent(const TBWidgetEvent &ev)
{
    for (unsigned i = 0; i < dataBindings_.Size(); i++)
        dataBindings_[i]->OnEvent(ev);

    return false;
}


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
