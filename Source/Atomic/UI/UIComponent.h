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

#pragma once

#include "../Scene/Component.h"

namespace Atomic
{

class Viewport;
class UIView;
class StaticModel;
class Material;

// UI Component which can be attached to a scene node with a StaticModel
class ATOMIC_API UIComponent : public Component
{
    ATOMIC_OBJECT(UIComponent, Component)

public:

    /// Construct.
    UIComponent(Context* context);
    /// Destruct.
    virtual ~UIComponent();

    /// Register object factory.
    static void RegisterObject(Context* context);

    /// Handle enabled/disabled state change.
    virtual void OnSetEnabled();

    /// Set the UIView for this UIComponent, note this is optional as one will be autocreated
    void SetUIView(UIView* view);
    /// Get the UIView for this UIComponent
    UIView* GetUIView() const { return uiView_; }

    /// Get the viewport associated with this UIComponent
    Viewport* GetViewport() const;

    /// Set the static model used for rendering and ray casting
    void SetStaticModel(StaticModel* staticModel);
    /// Get the static model used for rendering and ray casting
    StaticModel* GetStaticModel() const;

    /// Calculate position in UI space given a screen position
    bool CalcUIViewPos(const IntVector2& screenPos, IntVector2& viewPos);

protected:

    /// Handle scene node being assigned at creation.
    virtual void OnNodeSet(Node* node);

    /// Handle scene being assigned. This may happen several times during the component's lifetime. Scene-wide subsystems and events are subscribed to here.
    virtual void OnSceneSet(Scene* scene);

private:

    const IntVector2& GetSizeAttr() const;
    void SetSizeAttr(const IntVector2& value);

    void UpdateEventSubscriptions(bool subscribe);
    void HandleScenePostUpdate(StringHash eventType, VariantMap& eventData);

    bool FilterInput() const;

    void UpdateMouseEventSubscriptions(bool unsubscribe = false);
    void HandleMouseButtonDown(StringHash eventType, VariantMap& eventData);
    void HandleMouseButtonUp(StringHash eventType, VariantMap& eventData);
    void HandleMouseMove(StringHash eventType, VariantMap& eventData);
    void HandleMouseWheel(StringHash eventType, VariantMap& eventData);

    /// WeakPtr to UIView as we don't want to hold after UI subsystem has released
    WeakPtr<UIView> uiView_;
    WeakPtr<StaticModel> staticModel_;

    IntVector2 viewSize_;

    IntRect mouseScreenRect_;

    SharedPtr<Material> defaultMaterial_;

};

}
