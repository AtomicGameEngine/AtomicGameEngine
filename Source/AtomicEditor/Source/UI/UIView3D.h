// Portions Copyright (c) 2008-2015 the Urho3D project.

// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Graphics/Texture2D.h>
#include <Atomic/Graphics/Viewport.h>
#include <Atomic/Scene/Scene.h>

#include "AEWidget.h"

using namespace Atomic;

namespace AtomicEditor
{

class View3D;

class View3DWidget : public TBWidget
{
    friend class View3D;

public:
    // For safe typecasting
    TBOBJECT_SUBCLASS(View3DWidget, TBWidget);

    View3DWidget();

    virtual void OnPaint(const PaintProps &paint_props);

private:

    WeakPtr<View3D> view3D_;
    PODVector<float> vertexData_;

};


class View3D : public AEWidget
{
    OBJECT(View3D);

public:
    /// Construct.
    View3D(Context* context);
    /// Destruct.
    virtual ~View3D();

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

    const IntVector2& GetSize() const { return size_; }

    virtual bool OnEvent(const TBWidgetEvent &ev);

protected:

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

    IntVector2 size_;

    View3DWidget* view3DWidget_;


};

}
