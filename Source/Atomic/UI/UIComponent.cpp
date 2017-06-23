//
// Copyright (c) 2014-2017, THUNDERBEAST GAMES LLC All rights reserved
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

#include "../Core/Context.h"
#include "../IO/Log.h"
#include "../Resource/ResourceCache.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Texture2D.h"
#include "../Graphics/Technique.h"
#include "../Graphics/Material.h"
#include "../Graphics/Octree.h"
#include "../Graphics/OctreeQuery.h"
#include "../Graphics/Camera.h"
#include "../Graphics/BillboardSet.h"
#include "../Graphics/StaticModel.h"
#include "../Graphics/Renderer.h"
#include "../Scene/Node.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneEvents.h"

#include "UIView.h"
#include "UIComponent.h"

#include <Atomic/Input/Input.h>
#include <Atomic/Input/InputEvents.h>
#include <Atomic/UI/UI.h>
#include <Atomic/UI/UIEvents.h>

namespace Atomic
{

static const IntVector2 DEFAULT_UICOMPONENT_SIZE(UIVIEW_DEFAULT_TEXTURE_SIZE, UIVIEW_DEFAULT_TEXTURE_SIZE);

UIComponent::UIComponent(Context* context) :
    Component(context),
    viewSize_(IntVector2(UIVIEW_DEFAULT_TEXTURE_SIZE, UIVIEW_DEFAULT_TEXTURE_SIZE)),
    mouseScreenRect_(IntRect::ZERO)
{

}

UIComponent::~UIComponent()
{

}

void UIComponent::RegisterObject(Context* context)
{
    context->RegisterFactory<UIComponent>();

    ATOMIC_ACCESSOR_ATTRIBUTE("Size", GetSizeAttr, SetSizeAttr, IntVector2, DEFAULT_UICOMPONENT_SIZE, AM_DEFAULT);
}

const IntVector2& UIComponent::GetSizeAttr() const
{
    return viewSize_;
}

void UIComponent::SetSizeAttr(const IntVector2& value)
{
    viewSize_ = value;

    if (uiView_.NotNull())
    {
        uiView_->SetSize(viewSize_.x_, viewSize_.y_);
    }
}

void UIComponent::SetStaticModel(StaticModel* staticModel)
{
    staticModel_ = staticModel_;
}

StaticModel* UIComponent::GetStaticModel() const
{
    return staticModel_;
}

void UIComponent::SetUIView(UIView* view)
{
    uiView_ = view;
    viewSize_.x_ = uiView_->GetWidth();
    viewSize_.y_ = uiView_->GetHeight();
    view->SetRenderToTexture(true, viewSize_.x_, viewSize_.y_);
}

void UIComponent::OnSetEnabled()
{
    Component::OnSetEnabled();
    UpdateEventSubscriptions(IsEnabledEffective());
}

void UIComponent::UpdateMouseEventSubscriptions(bool unsubscribe)
{
    if (!unsubscribe)
    {
        SubscribeToEvent(E_MOUSEBUTTONDOWN, ATOMIC_HANDLER(UIComponent, HandleMouseButtonDown));
        SubscribeToEvent(E_MOUSEBUTTONUP, ATOMIC_HANDLER(UIComponent, HandleMouseButtonUp));
        SubscribeToEvent(E_MOUSEMOVE, ATOMIC_HANDLER(UIComponent, HandleMouseMove));
        SubscribeToEvent(E_MOUSEWHEEL, ATOMIC_HANDLER(UIComponent, HandleMouseWheel));
    }
    else
    {
        UnsubscribeFromEvent(E_MOUSEBUTTONDOWN);
        UnsubscribeFromEvent(E_MOUSEBUTTONUP);
        UnsubscribeFromEvent(E_MOUSEMOVE);
        UnsubscribeFromEvent(E_MOUSEWHEEL);
    }

}
void UIComponent::UpdateEventSubscriptions(bool subscribe)
{
    Scene* scene = GetScene();

    if (subscribe && scene)
    {
        SubscribeToEvent(scene, E_SCENEPOSTUPDATE, ATOMIC_HANDLER(UIComponent, HandleScenePostUpdate));
        UpdateMouseEventSubscriptions();
    }
    else
    {
        UpdateMouseEventSubscriptions(true);
        UnsubscribeFromEvent(E_SCENEPOSTUPDATE);
    }
}

void UIComponent::HandleScenePostUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace ScenePostUpdate;
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    if (!IsEnabledEffective())
    {
        return;
    }

    // TODO: check visibility

    // early out if nothing to render
    if (staticModel_.Null())
    {
        staticModel_ = node_->GetComponent<StaticModel>();

        if (staticModel_.Null())
        {
            return;
        }
    }

    // if we don't have a view, let's make it
    if (uiView_.Null())
    {
        UIView* view = new UIView(context_);
        view->SetSize(viewSize_.x_, viewSize_.y_);
        SetUIView(view);
    }

    if (!staticModel_->GetMaterial())
    {
        if (defaultMaterial_.Null())
        {
            ResourceCache* cache = GetSubsystem<ResourceCache>();
            defaultMaterial_ = new Material(context_);
            defaultMaterial_->SetTechnique(0, cache->GetResource<Technique>("Techniques/Diff.xml"));
            defaultMaterial_->SetTexture(Atomic::TU_DIFFUSE, uiView_->GetRenderTexture());
        }

        staticModel_->SetMaterial(defaultMaterial_);
    }

}

Viewport* UIComponent::GetViewport() const
{
    if (!GetScene())
        return 0;

    Renderer* renderer = GetSubsystem<Renderer>();
    for (unsigned i = 0; i < renderer->GetNumViewports(); ++i)
    {
        Viewport* viewport = renderer->GetViewport(i);

        // Find viewport with same scene
        // TODO: support multiple viewports
        if (viewport && viewport->GetScene() == GetScene())
        {
            return viewport;
        }
    }

    return 0;

}

bool UIComponent::CalcUIViewPos(const IntVector2& screenPos, IntVector2& viewPos)
{

    if (staticModel_.Null() || uiView_.Null())
    {
        return false;
    }

    Viewport* viewport = GetViewport();

    if (!viewport)
    {
        return false;
    }

    Scene* scene = GetScene();
    Camera* camera = viewport->GetCamera();
    Octree* octree = scene->GetComponent<Octree>();

    bool rectIsDefault = mouseScreenRect_ == IntRect::ZERO;

    if (!camera || !octree || (!rectIsDefault && !mouseScreenRect_.IsInside(screenPos)))
    {
        return false;
    }

    IntRect vpRect = viewport->GetRect();

    if (vpRect == IntRect::ZERO)
    {
        Graphics* graphics = GetSubsystem<Graphics>();

        vpRect.right_ = graphics->GetWidth();
        vpRect.bottom_ = graphics->GetHeight();
    }

    Vector2 normPos(screenPos.x_ - mouseScreenRect_.left_, screenPos.y_ - mouseScreenRect_.top_);
    normPos /= rectIsDefault ? Vector2(vpRect.Width(), vpRect.Height()) : Vector2(mouseScreenRect_.Width(), mouseScreenRect_.Height());

    Ray ray(camera->GetScreenRay(normPos.x_, normPos.y_));
    PODVector<RayQueryResult> queryResultVector;
    RayOctreeQuery query(queryResultVector, ray, RAY_TRIANGLE_UV, M_INFINITY, DRAWABLE_GEOMETRY, DEFAULT_VIEWMASK);

    octree->Raycast(query);

    if (queryResultVector.Empty())
        return false;

    for (unsigned i = 0; i < queryResultVector.Size(); i++)
    {
        RayQueryResult& queryResult = queryResultVector[i];

        if (queryResult.drawable_ != staticModel_)
        {
            // ignore billboard sets by default
            if (queryResult.drawable_->GetTypeInfo()->IsTypeOf(BillboardSet::GetTypeStatic()))
            {
                continue;
            }

            return false;
        }

        Vector2& uv = queryResult.textureUV_;
        viewPos = IntVector2(uv.x_ * uiView_->GetWidth(), uv.y_ * uiView_->GetHeight());
        return true;

    }

    return false;

}

bool UIComponent::FilterInput() const
{
    if (uiView_.Null() || staticModel_.Null() || !uiView_->GetMouseEnabled() || !uiView_->GetFocus())
    {
        return true;
    }

    return false;
}

void UIComponent::HandleMouseMove(StringHash eventType, VariantMap& eventData)
{    
    if (FilterInput())
    {
        return;
    }

    using namespace MouseMove;

    IntVector2 screenPos(eventData[P_X].GetInt(), eventData[P_Y].GetInt());
    IntVector2 viewPos;

    if (!CalcUIViewPos(screenPos, viewPos))
    {
        return;
    }

    tb::TBWidget *widget = uiView_->GetInternalWidget();
    widget->InvokePointerMove(viewPos.x_, viewPos.y_, tb::TB_MODIFIER_NONE, false);
}

// this is duplicated in UIView.cpp, don't want to expose in header
// possible need a static TB <-> Atomic UI helper class to encapsulate
static tb::MODIFIER_KEYS GetModifierKeys(int qualifiers, bool superKey)
{
    tb::MODIFIER_KEYS code = tb::TB_MODIFIER_NONE;
    if (qualifiers & QUAL_ALT)    code |= tb::TB_ALT;
    if (qualifiers & QUAL_CTRL)    code |= tb::TB_CTRL;
    if (qualifiers & QUAL_SHIFT)    code |= tb::TB_SHIFT;
    if (superKey)    code |= tb::TB_SUPER;
    return code;
}

void UIComponent::HandleMouseButtonDown(StringHash eventType, VariantMap& eventData)
{
    if (FilterInput())
    {
        return;
    }

    using namespace MouseButtonDown;

    Input* input = GetSubsystem<Input>();

    unsigned button = eventData[P_BUTTON].GetUInt();

    IntVector2 screenPos = GetSubsystem<Input>()->GetMousePosition();
    IntVector2 viewPos;

    if (!CalcUIViewPos(screenPos, viewPos))
    {
        return;
    }

    int qualifiers = input->GetQualifiers();

#ifdef ATOMIC_PLATFORM_WINDOWS
    bool superdown = input->GetKeyDown(KEY_LCTRL) || input->GetKeyDown(KEY_RCTRL);
#else
    bool superdown = input->GetKeyDown(KEY_LGUI) || input->GetKeyDown(KEY_RGUI);
#endif

    tb::TBWidget *widget = uiView_->GetInternalWidget();

    const int clickCount = 1;

    tb::MODIFIER_KEYS mod = GetModifierKeys(qualifiers, superdown);

    if (button == MOUSEB_RIGHT)
        widget->InvokeRightPointerDown(viewPos.x_, viewPos.y_, clickCount, mod);
    else
        widget->InvokePointerDown(viewPos.x_, viewPos.y_, clickCount, mod, false);

}


void UIComponent::HandleMouseButtonUp(StringHash eventType, VariantMap& eventData)
{
    if (FilterInput())
    {
        return;
    }

    using namespace MouseButtonUp;

    Input* input = GetSubsystem<Input>();

    unsigned button = eventData[P_BUTTON].GetUInt();

    IntVector2 screenPos = GetSubsystem<Input>()->GetMousePosition();
    IntVector2 viewPos;

    if (!CalcUIViewPos(screenPos, viewPos))
    {
        return;
    }

    int qualifiers = input->GetQualifiers();

#ifdef ATOMIC_PLATFORM_WINDOWS
    bool superdown = input->GetKeyDown(KEY_LCTRL) || input->GetKeyDown(KEY_RCTRL);
#else
    bool superdown = input->GetKeyDown(KEY_LGUI) || input->GetKeyDown(KEY_RGUI);
#endif

    tb::MODIFIER_KEYS mod = GetModifierKeys(qualifiers, superdown);

    tb::TBWidget *widget = uiView_->GetInternalWidget();

    if (button == MOUSEB_RIGHT)
        widget->InvokeRightPointerUp(viewPos.x_, viewPos.y_, mod);
    else
        widget->InvokePointerUp(viewPos.x_, viewPos.y_, mod, false);

}

void UIComponent::HandleMouseWheel(StringHash eventType, VariantMap& eventData)
{
    if (FilterInput())
    {
        return;
    }

    using namespace MouseWheel;

    if (uiView_.Null() || staticModel_.Null())
    {
        return;
    }

    int delta = eventData[P_WHEEL].GetInt();

    Input* input = GetSubsystem<Input>();

    tb::TBWidget *widget = uiView_->GetInternalWidget();

    widget->InvokeWheel(input->GetMousePosition().x_, input->GetMousePosition().y_, 0, -delta, tb::TB_MODIFIER_NONE);
}


void UIComponent::OnSceneSet(Scene* scene)
{
    Component::OnSceneSet(scene);
    if (!scene)
    {
        UpdateEventSubscriptions(false);
    }
    else
    {
        UpdateEventSubscriptions(IsEnabledEffective());
    }
}

void UIComponent::OnNodeSet(Node* node)
{
    Component::OnNodeSet(node);
}

}
