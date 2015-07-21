
#include <Atomic/IO/Log.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Graphics/Renderer.h>
#include <Atomic/Graphics/Camera.h>

#include "Player.h"

namespace AtomicPlayer
{

Player::Player(Context* context) :
    Object(context)
{
    viewport_ = new Viewport(context_);
    GetSubsystem<Renderer>()->SetViewport(0, viewport_);
}

Player::~Player()
{

}

Scene* Player::LoadScene(const String& filename, Camera *camera)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    SharedPtr<File> file = cache->GetFile(filename);

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

    return scene;
}

void Player::SayHi()
{

    LOGINFO("The Player says Hi!");

}


}
