//
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

#include <Atomic/IO/Log.h>

#include <Atomic/Input/InputEvents.h>

#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Graphics/Graphics.h>
#include <Atomic/Graphics/Renderer.h>
#include <Atomic/Graphics/Camera.h>
#include <Atomic/Graphics/Texture2D.h>

#include "Player.h"

namespace AtomicPlayer
{

Player::Player(Context* context) :
    Object(context),
    renderWidth_(0),
    renderHeight_(0)
{
    viewport_ = new Viewport(context_);

    SubscribeToEvent(E_EXITREQUESTED, HANDLER(Player, HandleExitRequested));
}

Player::~Player()
{

}

void Player::HandleExitRequested(StringHash eventType, VariantMap& eventData)
{
    currentScene_ = 0;
    viewport_ = 0;

}

void Player::SetRenderToTexture(unsigned width, unsigned height)
{
    renderWidth_ = width;
    renderHeight_ = height;

    renderTexture_ = new Texture2D(context_);
    depthTexture_ = new Texture2D(context_);

    renderTexture_->SetGPUShared(true);

    viewport_->SetRect(IntRect(0, 0, width, height));
    renderTexture_->SetSize(width, height, Graphics::GetRGBFormat(), TEXTURE_RENDERTARGET);
    depthTexture_->SetSize(width, height, Graphics::GetDepthStencilFormat(), TEXTURE_DEPTHSTENCIL);

    RenderSurface* surface = renderTexture_->GetRenderSurface();
    surface->SetViewport(0, viewport_);
    surface->SetUpdateMode(SURFACE_UPDATEALWAYS );
    surface->SetLinkedDepthStencil(depthTexture_->GetRenderSurface());
}

Scene* Player::LoadScene(const String& filename, Camera *camera)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    SharedPtr<File> file = cache->GetFile(filename);

    // If we're not rendering to texture and we haven't set the viewport, set it
    if (renderTexture_.Null() && GetSubsystem<Renderer>()->GetViewport(0) != viewport_)
    {
        GetSubsystem<Renderer>()->SetViewport(0, viewport_);
    }

    if (!file->IsOpen())
    {
        return 0;
    }

    Scene* scene = new Scene(context_);

    if (!scene->LoadXML(*file))
    {
        scene->ReleaseRef();
        return 0;
    }

    if (currentScene_.Null())
    {
        currentScene_ = scene;

        if(!camera)
        {
            PODVector<Node*> cameraNodes;
            scene->GetChildrenWithComponent(cameraNodes, Camera::GetTypeStatic(), true);
            if (cameraNodes.Size())
            {
                camera = cameraNodes[0]->GetComponent<Camera>();
            }
        }

        viewport_->SetScene(scene);

        if (camera)
            viewport_->SetCamera(camera);

    }

    return scene;
}


}
