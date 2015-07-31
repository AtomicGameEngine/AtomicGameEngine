// Portions Copyright (c) 2008-2015 the Urho3D project.

// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"


#include <Atomic/UI/UI.h>
#include <Atomic/UI/UIBatch.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Engine/Engine.h>
#include <Atomic/Graphics/Graphics.h>
#include <Atomic/Graphics/Camera.h>
#include <Atomic/Graphics/RenderPath.h>
#include <Atomic/Graphics/Renderer.h>
#include <Atomic/Core/CoreEvents.h>

#include "UIView3D.h"

#include "AEEvents.h"
#include "AETypes.h"
#include "AEEditor.h"

using namespace tb;

namespace AtomicEditor
{


UIView3D::UIView3D(Context* context) :
    AEWidget(context),
    rttFormat_(Graphics::GetRGBFormat()),
    autoUpdate_(false),
    size_(-1, -1),
    resizeRequired_(false)
{
    renderTexture_ = new Texture2D(context_);
    depthTexture_ = new Texture2D(context_);
    viewport_ = new Viewport(context_);

    view3DWidget_ = new View3DWidget();
    view3DWidget_->SetGravity(WIDGET_GRAVITY_ALL);

    view3DWidget_->view3D_ = this;
    delegate_->AddChild(view3DWidget_);

   SubscribeToEvent(E_ENDFRAME, HANDLER(UIView3D, HandleEndFrame));
}

UIView3D::~UIView3D()
{
    // FIXME: need to refactor Light2D viewport handling
    if (viewport_.NotNull())
    {
        RenderPath* renderpath = viewport_->GetRenderPath();
        if (renderpath)
            renderpath->RemoveCommands("Light2D");
    }

}

bool UIView3D::OnEvent(const TBWidgetEvent &ev)
{
    return false;
}

void UIView3D::HandleEndFrame(StringHash eventType, VariantMap& eventData)
{
    if (resizeRequired_)
    {
        TBRect rect = view3DWidget_->GetRect();
        OnResize(IntVector2(rect.w, rect.h));
        resizeRequired_ = false;
    }

}


void UIView3D::OnResize(const IntVector2 &newSize)
{
    if (newSize.x_ == size_.x_ && newSize.y_ == size_.y_)
        return;

    int width = newSize.x_;
    int height = newSize.y_;

    if (width > 0 && height > 0)
    {
        viewport_->SetRect(IntRect(0, 0, width, height));
        renderTexture_->SetSize(width, height, rttFormat_, TEXTURE_RENDERTARGET);
        depthTexture_->SetSize(width, height, Graphics::GetDepthStencilFormat(), TEXTURE_DEPTHSTENCIL);

        RenderSurface* surface = renderTexture_->GetRenderSurface();
        surface->SetViewport(0, viewport_);
        surface->SetUpdateMode(autoUpdate_ ? SURFACE_UPDATEALWAYS : SURFACE_MANUALUPDATE);
        surface->SetLinkedDepthStencil(depthTexture_->GetRenderSurface());

        size_ = newSize;

    }
}


void UIView3D::SetView(Scene* scene, Camera* camera)
{
    scene_ = scene;
    cameraNode_ = camera ? camera->GetNode() : 0;

    viewport_->SetScene(scene_);
    viewport_->SetCamera(camera);
    QueueUpdate();
}

void UIView3D::SetFormat(unsigned format)
{
    if (format != rttFormat_)
    {
        rttFormat_ = format;
    }
}

void UIView3D::SetAutoUpdate(bool enable)
{
    if (enable != autoUpdate_)
    {
        autoUpdate_ = enable;
        RenderSurface* surface = renderTexture_->GetRenderSurface();
        if (surface)
            surface->SetUpdateMode(autoUpdate_ ? SURFACE_UPDATEALWAYS : SURFACE_MANUALUPDATE);
    }
}

void UIView3D::QueueUpdate()
{
    if (!autoUpdate_)
    {
        RenderSurface* surface = renderTexture_->GetRenderSurface();
        if (surface)
            surface->QueueUpdate();
    }
}

Scene* UIView3D::GetScene() const
{
    return scene_;
}

Node* UIView3D::GetCameraNode() const
{
    return cameraNode_;
}

Texture2D* UIView3D::GetRenderTexture() const
{
    return renderTexture_;
}

Texture2D* UIView3D::GetDepthTexture() const
{
    return depthTexture_;
}

Viewport* UIView3D::GetViewport() const
{
    return viewport_;
}

View3DWidget::View3DWidget()
{
    vertexData_.Resize(6 * UI_VERTEX_SIZE);
    float color;
    ((unsigned&)color) = 0xFFFFFFFF;

    float* data = &vertexData_[0];

    data[2] = 0; data[3] = color; data[4] = 0; data[5] = 0;
    data[8] = 0; data[9] = color; data[10] = 1; data[11] = 0;
    data[14] = 0; data[15] = color; data[16] = 1; data[17] = 1;
    data[20] = 0; data[21] = color; data[22] = 0; data[23] = 0;
    data[26] = 0; data[27] = color; data[28] = 1; data[29] = 1;
    data[32] = 0; data[33] = color; data[34] = 0; data[35] = 1;
}


void View3DWidget::OnPaint(const PaintProps &paint_props)
{
    if (view3D_.Null())
        return;

    TBRect rect = GetRect();
    ConvertToRoot(rect.x, rect.y);

    IntVector2 size = view3D_->GetSize();

    if (size.x_ != rect.w || size.y_ != rect.h)
    {
        size.x_ = rect.w;
        size.y_ = rect.h;
        view3D_->SetResizeRequired();
        return;
    }

    float* data = &vertexData_[0];

    data[0] = rect.x;
    data[1] = rect.y;

    data[6] = rect.x + rect.w;
    data[7] =  rect.y;

    data[12] = rect.x + rect.w;
    data[13] = rect.y + rect.h;

    data[18] = rect.x;
    data[19] = rect.y;

    data[24] = rect.x + rect.w;
    data[25] = rect.y + rect.h;

    data[30] = rect.x;
    data[31] = rect.y + rect.h;

    view3D_->GetSubsystem<UI>()->SubmitBatchVertexData(view3D_->GetRenderTexture(), vertexData_);

}

}

