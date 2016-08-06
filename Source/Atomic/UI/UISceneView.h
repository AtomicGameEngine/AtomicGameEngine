//
// Copyright (c) 2008-2015 the Urho3D project.
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
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

#include "UIWidget.h"

#include <Atomic/Graphics/Texture2D.h>
#include <Atomic/Graphics/Viewport.h>
#include <Atomic/Scene/Scene.h>

#include <TurboBadger/tb_widgets.h>

using namespace tb;

namespace Atomic
{

class UISceneView;
class UIRenderer;

class SceneViewWidget : public tb::TBWidget
{
    friend class UISceneView;

public:
    // For safe typecasting
    TBOBJECT_SUBCLASS(SceneViewWidget, tb::TBWidget);

    SceneViewWidget();

    virtual void OnPaint(const PaintProps &paint_props);

private:

    WeakPtr<UISceneView> sceneView_;
    PODVector<float> vertexData_;

};


class UISceneView : public UIWidget
{

    friend class SceneViewWidget;

    ATOMIC_OBJECT(UISceneView, UIWidget)

public:

    UISceneView(Context* context, bool createWidget = true);
    virtual ~UISceneView();

    /// React to resize.
    void OnResize(const IntVector2& newSize);

    /// Define the scene and camera to use in rendering. When ownScene is true the View3D will take ownership of them with shared pointers.
    void SetView(Scene* scene, Camera* camera);
    /// Set render texture pixel format. Default is RGB.
    void SetFormat(unsigned format);
    /// Set render target auto update mode. Default is true.
    void SetAutoUpdate(bool enable);
    /// Queue manual update on the render texture.
    void QueueUpdate();

    /// Return render texture pixel format.
    unsigned GetFormat() const { return rttFormat_; }
    /// Return whether render target updates automatically.
    bool GetAutoUpdate() const { return autoUpdate_; }
    /// Return scene.
    Scene* GetScene() const;
    /// Return camera scene node.
    Node* GetCameraNode() const;
    /// Return render texture.
    Texture2D* GetRenderTexture() const;
    /// Return depth stencil texture.
    Texture2D* GetDepthTexture() const;
    /// Return viewport.
    Viewport* GetViewport() const;

    void SetResizeRequired() {resizeRequired_ = true;}
    const IntVector2& GetSize() const { return size_; }

protected:

    void HandleEndFrame(StringHash eventType, VariantMap& eventData);

    /// Renderable texture.
    SharedPtr<Texture2D> renderTexture_;
    /// Depth stencil texture.
    SharedPtr<Texture2D> depthTexture_;
    /// Viewport.
    SharedPtr<Viewport> viewport_;
    /// Scene.
    SharedPtr<Scene> scene_;
    /// Camera scene node.
    SharedPtr<Node> cameraNode_;
    /// Render texture format.
    unsigned rttFormat_;
    /// Render texture auto update mode.
    bool autoUpdate_;

    bool resizeRequired_;

    IntVector2 size_;

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

    UIRenderer* renderer_;

};

}
