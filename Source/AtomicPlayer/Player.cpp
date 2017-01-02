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
#include <Atomic/Graphics/Renderer.h>
#include <Atomic/Graphics/Camera.h>

#include "PlayerEvents.h"
#include "Player.h"

namespace AtomicPlayer
{

Player::Player(Context* context) :
    Object(context)
{
    viewport_ = new Viewport(context_);
    GetSubsystem<Renderer>()->SetViewport(0, viewport_);

    SubscribeToEvent(E_EXITREQUESTED, ATOMIC_HANDLER(Player, HandleExitRequested));
}

Player::~Player()
{

}

void Player::HandleExitRequested(StringHash eventType, VariantMap& eventData)
{
    currentScene_ = 0;
    viewport_ = 0;

}

Scene* Player::LoadScene(const String& filename, Camera *camera)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    SharedPtr<File> file = cache->GetFile(filename);

    if (!file || !file->IsOpen())
    {
        return 0;
    }

    Scene* scene = new Scene(context_);

    VariantMap eventData;

    eventData[PlayerSceneLoadBegin::P_SCENE] = scene;

    scene->SendEvent(E_PLAYERSCENELOADBEGIN, eventData);

    if (!scene->LoadXML(*file))
    {
        eventData[PlayerSceneLoadEnd::P_SCENE] = scene;
        eventData[PlayerSceneLoadEnd::P_SUCCESS] = false;
        scene->SendEvent(E_PLAYERSCENELOADEND, eventData);
        scene->ReleaseRef();
        return 0;
    }

    eventData[PlayerSceneLoadEnd::P_SCENE] = scene;
    eventData[PlayerSceneLoadEnd::P_SUCCESS] = true;
    scene->SendEvent(E_PLAYERSCENELOADEND, eventData);

    loadedScenes_.Push(SharedPtr<Scene>(scene));

    if (currentScene_.Null())
    {
        SetCurrentScene(scene, camera);
    }

    return scene;
}

void Player::SetCurrentScene(Scene* scene, Camera* camera)
{
    Vector<SharedPtr<Scene>>::ConstIterator citr = loadedScenes_.Find(SharedPtr<Scene>(scene));

    if (citr == loadedScenes_.End())
    {
        ATOMIC_LOGERROR("Player::UnloadScene - unknown scene");
        return;
    }

    currentScene_ = scene;

    if (!camera)
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

void Player::UnloadScene(Scene* scene)
{
    SharedPtr<Scene> keepalive(scene);

    if (currentScene_ == scene)
    {
        viewport_->SetScene(0);
        viewport_->SetCamera(0);
        currentScene_ = 0;
    }

    Vector<SharedPtr<Scene>>::ConstIterator citr = loadedScenes_.Find(keepalive);
    
    if (citr == loadedScenes_.End())
    {
        ATOMIC_LOGERROR("Player::UnloadScene - unknown scene");
        return;
    }

    VariantMap eventData;
    eventData[PlayerSceneUnload::P_SCENE] = scene;

    scene->SendEvent(E_PLAYERSCENEUNLOAD, eventData);

    loadedScenes_.Remove(keepalive);

}

void Player::UnloadAllScenes()
{
    Vector<SharedPtr<Scene>> scenes = loadedScenes_;

    for (unsigned i = 0; i < scenes.Size(); i++)
    {
        UnloadScene(scenes[i]);
    }

    assert(loadedScenes_.Size() == 0);
    
}



}
