
#pragma once

#include <Atomic/Core/Object.h>
#include <Atomic/Graphics/Viewport.h>
#include <Atomic/Scene/Scene.h>

using namespace Atomic;

namespace AtomicPlayer
{

/// Player subsystem
class Player : public Object
{
    OBJECT(Player);

public:
    /// Construct.
    Player(Context* context);
    /// Destruct.
    virtual ~Player();

    Scene* LoadScene(const String& filename, Camera* camera = NULL);

    Scene* GetCurrentScene() { return currentScene_; }

private:

    void HandleExitRequested(StringHash eventType, VariantMap& eventData);

    // Strong reference
    SharedPtr<Scene> currentScene_;

    SharedPtr<Viewport> viewport_;

};

}
