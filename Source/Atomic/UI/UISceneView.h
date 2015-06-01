
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
    OBJECT(UISceneView)

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

};

}
