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

#include <Atomic/UI/UI.h>
#include <Atomic/UI/UIBatch.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Engine/Engine.h>
#include <Atomic/Graphics/Graphics.h>
#include <Atomic/Graphics/Camera.h>
#include <Atomic/Graphics/RenderPath.h>
#include <Atomic/Graphics/Renderer.h>
#include <Atomic/Core/CoreEvents.h>

#include "UIRenderer.h"
#include "UISceneView.h"

using namespace tb;

namespace Atomic
{

UISceneView::UISceneView(Context* context, bool createWidget) : UIWidget(context, false),
    rttFormat_(Graphics::GetRGBFormat()),
    autoUpdate_(false),
    size_(-1, -1),
    resizeRequired_(false)
{
    UI* ui= GetSubsystem<UI>();

    if (createWidget)
    {
        renderTexture_ = new Texture2D(context_);
        depthTexture_ = new Texture2D(context_);
        viewport_ = new Viewport(context_);

        widget_ = new SceneViewWidget();
        widget_->SetDelegate(this);
        widget_->SetGravity(WIDGET_GRAVITY_ALL);
        ((SceneViewWidget*)widget_)->sceneView_ = this;

        ui->WrapWidget(this, widget_);


    }

    renderer_ = ui->GetRenderer();

    SubscribeToEvent(E_ENDFRAME, ATOMIC_HANDLER(UISceneView, HandleEndFrame));
}

UISceneView::~UISceneView()
{

}

bool UISceneView::OnEvent(const TBWidgetEvent &ev)
{
    return UIWidget::OnEvent(ev);
}

void UISceneView::HandleEndFrame(StringHash eventType, VariantMap& eventData)
{
    if (resizeRequired_)
    {
        TBRect rect = widget_->GetRect();
        OnResize(IntVector2(rect.w, rect.h));
        resizeRequired_ = false;
    }

}


void UISceneView::OnResize(const IntVector2 &newSize)
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


void UISceneView::SetView(Scene* scene, Camera* camera)
{
    scene_ = scene;
    cameraNode_ = camera ? camera->GetNode() : 0;

    viewport_->SetScene(scene_);
    viewport_->SetCamera(camera);
    QueueUpdate();
}

void UISceneView::SetFormat(unsigned format)
{
    if (format != rttFormat_)
    {
        rttFormat_ = format;
    }
}

void UISceneView::SetAutoUpdate(bool enable)
{
    if (enable != autoUpdate_)
    {
        autoUpdate_ = enable;
        RenderSurface* surface = renderTexture_->GetRenderSurface();
        if (surface)
            surface->SetUpdateMode(autoUpdate_ ? SURFACE_UPDATEALWAYS : SURFACE_MANUALUPDATE);
    }
}

void UISceneView::QueueUpdate()
{
    if (!autoUpdate_)
    {
        RenderSurface* surface = renderTexture_->GetRenderSurface();
        if (surface)
            surface->QueueUpdate();
    }
}

Scene* UISceneView::GetScene() const
{
    return scene_;
}

Node* UISceneView::GetCameraNode() const
{
    return cameraNode_;
}

Texture2D* UISceneView::GetRenderTexture() const
{
    return renderTexture_;
}

Texture2D* UISceneView::GetDepthTexture() const
{
    return depthTexture_;
}

Viewport* UISceneView::GetViewport() const
{
    return viewport_;
}

SceneViewWidget::SceneViewWidget()
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


void SceneViewWidget::OnPaint(const PaintProps &paint_props)
{
    if (sceneView_.Null())
        return;

    TBRect rect = GetRect();
    rect.x = rect.y = 0;
    ConvertToRoot(rect.x, rect.y);
    IntVector2 size = sceneView_->GetSize();

    if (size.x_ != rect.w || size.y_ != rect.h)
    {
        size.x_ = rect.w;
        size.y_ = rect.h;
        sceneView_->SetResizeRequired();
        // early out here, responsible for flicker
        // https://github.com/AtomicGameEngine/AtomicGameEngine/issues/115
        return;
    }

    float* data = &vertexData_[0];

    float color;
    float fopacity = GetOpacity() * sceneView_->renderer_->GetOpacity();
    unsigned char opacity = (unsigned char) (fopacity* 255.0f);
    ((unsigned&)color) = (0x00FFFFFF + (((uint32)opacity) << 24));

    float x = (float) rect.x;
    float y = (float) rect.y;
    float w = (float) rect.w;
    float h = (float) rect.h;

#ifdef ATOMIC_PLATFORM_WINDOWS

#ifndef ATOMIC_D3D11
    //Direct3D9 Adjustment
    x += 0.5f;
    y += 0.5f;
#endif

#endif

    data[3] = color;
    data[9] = color;
    data[15] = color;
    data[21] = color;
    data[27] = color;
    data[33] = color;

    data[0] = x;
    data[1] = y;

    data[6] = x + w;
    data[7] =  y;

    data[12] = x + w;
    data[13] = y + h;

    data[18] = x;
    data[19] = y;

    data[24] = x + w;
    data[25] = y + h;

    data[30] = x;
    data[31] = y + h;

    sceneView_->GetSubsystem<UI>()->SubmitBatchVertexData(sceneView_->GetRenderTexture(), vertexData_);

}

}
