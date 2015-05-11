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

#include <Atomic/UI/UI.h>

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
    UI* tbui = GetSubsystem<UI>();
    tbui->LoadResourceFile(delegate_, "AtomicEditor/editor/ui/inspectorframe.tb.txt");

    delegate_->SetID(TBIDC("projectframe_delegate"));

    delegate_->SetGravity(WIDGET_GRAVITY_TOP_BOTTOM);

    inspectorContainer_ = delegate_->GetWidgetByIDAndType<TBLayout>(TBIDC("inspectorcontainer"));
    assert(inspectorContainer_);

    InitializeSources();

    SubscribeToEvent(E_EDITORACTIVENODECHANGE, HANDLER(InspectorFrame, HandleEditorActiveNodeChange));

}

InspectorFrame::~InspectorFrame()
{

}

void InspectorFrame::InitializeSources()
{
    componentCreateSource.AddItem(new MenubarItem("Audio", &audioCreateSource));
    componentCreateSource.AddItem(new MenubarItem("Geometry", &geometryCreateSource));
    componentCreateSource.AddItem(new MenubarItem("Logic", &logicCreateSource));
    componentCreateSource.AddItem(new MenubarItem("Navigation", &navigationCreateSource));
    componentCreateSource.AddItem(new MenubarItem("Network", &networkCreateSource));
    componentCreateSource.AddItem(new MenubarItem("Physics", &physicsCreateSource));
    componentCreateSource.AddItem(new MenubarItem("Scene", &sceneCreateSource));
    componentCreateSource.AddItem(new MenubarItem("Subsystem", &subsystemCreateSource));

    audioCreateSource.AddItem(new MenubarItem("SoundListener", TBIDC("create component")));
    audioCreateSource.AddItem(new MenubarItem("SoundSource",  TBIDC("create component")));
    audioCreateSource.AddItem(new MenubarItem("SoundSource3D", TBIDC("create component")));

    geometryCreateSource.AddItem(new MenubarItem("AnimatedModel", TBIDC("create component")));
    geometryCreateSource.AddItem(new MenubarItem("BillboardSet",  TBIDC("create component")));
    geometryCreateSource.AddItem(new MenubarItem("CustomGeometry", TBIDC("create component")));
    geometryCreateSource.AddItem(new MenubarItem("ParticleEmitter", TBIDC("create component")));
    geometryCreateSource.AddItem(new MenubarItem("Skybox", TBIDC("create component")));
    geometryCreateSource.AddItem(new MenubarItem("StaticModel", TBIDC("create component")));
    geometryCreateSource.AddItem(new MenubarItem("StaticModelGroup", TBIDC("create component")));
    geometryCreateSource.AddItem(new MenubarItem("Terrain", TBIDC("create component")));
    geometryCreateSource.AddItem(new MenubarItem("Text3D", TBIDC("create component")));
    geometryCreateSource.AddItem(new MenubarItem("Water", TBIDC("create component")));

    logicCreateSource.AddItem(new MenubarItem("AnimationController", TBIDC("create component")));
    logicCreateSource.AddItem(new MenubarItem("Javascript Component", TBIDC("create component")));
    logicCreateSource.AddItem(new MenubarItem("SplinePath", TBIDC("create component")));

    navigationCreateSource.AddItem(new MenubarItem("Navigable", TBIDC("create component")));
    navigationCreateSource.AddItem(new MenubarItem("NavigationMesh", TBIDC("create component")));
    navigationCreateSource.AddItem(new MenubarItem("OffMeshConnection", TBIDC("create component")));

    networkCreateSource.AddItem(new MenubarItem("Network Priority", TBIDC("create component")));

    physicsCreateSource.AddItem(new MenubarItem("CollisionShape", TBIDC("create component")));
    physicsCreateSource.AddItem(new MenubarItem("Constraint", TBIDC("create component")));
    physicsCreateSource.AddItem(new MenubarItem("RigidBody", TBIDC("create component")));

    sceneCreateSource.AddItem(new MenubarItem("Camera", TBIDC("create component")));
    sceneCreateSource.AddItem(new MenubarItem("Light", TBIDC("create component")));
    sceneCreateSource.AddItem(new MenubarItem("Zone", TBIDC("create component")));

    subsystemCreateSource.AddItem(new MenubarItem("DebugRenderer", TBIDC("create component")));
    subsystemCreateSource.AddItem(new MenubarItem("Octree", TBIDC("create component")));
    subsystemCreateSource.AddItem(new MenubarItem("PhysicsWorld", TBIDC("create component")));

}

bool InspectorFrame::OnEvent(const TBWidgetEvent &ev)
{
    for (unsigned i = 0; i < dataBindings_.Size(); i++)
        dataBindings_[i]->OnEvent(ev);

    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.target->GetID() == TBIDC("create popup"))
        {
            if (ev.ref_id == TBIDC("create component"))
            {
                Editor* editor = GetSubsystem<Editor>();
                editor->PostModalInfo("Early Access - Not Implemented", "Component creation not implemented in this early access build");
            }

        }

        if (ev.target->GetID() == TBIDC("create button"))
        {
            if (TBMenuWindow *menu = new TBMenuWindow(ev.target, TBIDC("create popup")))
            {
                menu->Show(&componentCreateSource, TBPopupAlignment(TB_ALIGN_LEFT));
            }

            return true;
        }
    }

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
        nodeLayout->SetSpacing(4);

        nodeLayout->SetLayoutDistribution(LAYOUT_DISTRIBUTION_GRAVITY);
        nodeLayout->SetLayoutPosition(LAYOUT_POSITION_LEFT_TOP);
        nodeLayout->SetLayoutParams(nlp);

        TBContainer* nodeContainer = new TBContainer();
        nodeContainer->SetGravity(WIDGET_GRAVITY_ALL);

        nodeContainer->SetSkinBg("InspectorTopLayout");

        TBLayout* attrsVerticalLayout = new TBLayout(AXIS_Y);
        attrsVerticalLayout->SetGravity(WIDGET_GRAVITY_ALL);
        attrsVerticalLayout->SetLayoutPosition(LAYOUT_POSITION_LEFT_TOP);
        nodeContainer->AddChild(attrsVerticalLayout);

        TBTextField* nodeLabel = new TBTextField();
        nodeLabel->SetTextAlign(TB_TEXT_ALIGN_LEFT);
        nodeLabel->SetText("Node");
        nodeLabel->SetSkinBg(TBIDC("InspectorTextLabel"));
        attrsVerticalLayout->AddChild(nodeLabel);

        const Vector<AttributeInfo>* attrs = node->GetAttributes();

        for (unsigned i = 0; i < attrs->Size(); i++)
        {
            const AttributeInfo* attr = &attrs->At(i);

            InspectorDataBinding*  binding = InspectorDataBinding::Create(node, attr);

            if (binding)
            {
                dataBindings_.Push(binding);

                TBLayout* attrLayout = new TBLayout();

                if (attr->type_ == VAR_VECTOR3 || attr->type_ == VAR_COLOR || attr->type_ == VAR_QUATERNION)
                {
                    attrLayout->SetAxis(AXIS_Y);
                    attrLayout->SetLayoutPosition(LAYOUT_POSITION_LEFT_TOP);
                    attrLayout->SetSkinBg("InspectorVectorAttrLayout");
                }

                attrLayout->SetLayoutDistribution(LAYOUT_DISTRIBUTION_GRAVITY);

                TBTextField* name = new TBTextField();
                name->SetTextAlign(TB_TEXT_ALIGN_LEFT);
                name->SetSkinBg(TBIDC("InspectorTextAttrName"));

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

        TBLayout* componentsLayout = new TBLayout();

        TBTextField* componentsLabel = new TBTextField();
        componentsLabel->SetText("Components");
        componentsLabel->SetSkinBg(TBIDC("InspectorTextLabel"));
        componentsLabel->SetTextAlign(TB_TEXT_ALIGN_LEFT);
        componentsLayout->AddChild(componentsLabel);

        TBButton* create = new TBButton();
        LayoutParams createLP;
        createLP.SetHeight(20);
        create->SetLayoutParams(createLP);
        create->SetFontDescription(fd);
        create->SetText("Create");
        create->SetID(TBIDC("create button"));
        componentsLayout->AddChild(create);

        nodeLayout->AddChild(componentsLayout);

        const Vector<SharedPtr<Component> > components = node->GetComponents();
        for (unsigned i = 0; i < components.Size(); i++)
        {
            Component* c = components[i];

            TBContainer* componentContainer = new TBContainer();

            componentContainer->SetSkinBg("InspectorTopLayout");

            componentContainer->SetGravity(WIDGET_GRAVITY_ALL);

            TBLayout* attrsVerticalLayout = new TBLayout(AXIS_Y);
            attrsVerticalLayout->SetGravity(WIDGET_GRAVITY_ALL);
            attrsVerticalLayout->SetLayoutDistribution(LAYOUT_DISTRIBUTION_GRAVITY);
            attrsVerticalLayout->SetLayoutPosition(LAYOUT_POSITION_LEFT_TOP);

            TBTextField* cnameField = new TBTextField();
            cnameField->SetText(c->GetTypeName().CString());
            cnameField->SetSkinBg(TBIDC("InspectorTextLabel"));
            cnameField->SetTextAlign(TB_TEXT_ALIGN_LEFT);
            cnameField->SetFontDescription(fd);
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

                        if (attr->type_ == VAR_VECTOR3 || attr->type_ == VAR_COLOR || attr->type_ == VAR_QUATERNION)
                        {
                            attrLayout->SetAxis(AXIS_Y);
                            attrLayout->SetLayoutPosition(LAYOUT_POSITION_LEFT_TOP);
                            attrLayout->SetSkinBg("InspectorVectorAttrLayout");
                        }

                        TBTextField* name = new TBTextField();
                        String bname = attr->name_;
                        if (bname == "Is Enabled")
                            bname = "Enabled";

                        name->SetTextAlign(TB_TEXT_ALIGN_LEFT);
                        name->SetText(bname.CString());
                        name->SetFontDescription(fd);

                        name->SetSkinBg(TBIDC("InspectorTextAttrName"));

                        LayoutParams lp;
                        lp.SetWidth(140);
                        name->SetLayoutParams(lp);

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
