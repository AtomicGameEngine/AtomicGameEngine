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
    ATOMIC_OBJECT(Player, Object);

public:
    /// Construct.
    Player(Context* context);
    /// Destruct.
    virtual ~Player();

    /// Load a scene file with optional camera specified
    Scene* LoadScene(const String& filename, Camera* camera = NULL);

    /// Get the number of currently loaded scenes
    unsigned GetNumScenes() { return loadedScenes_.Size();  }

    /// Get the current scene
    Scene* GetCurrentScene() { return currentScene_; }

    /// Get the scene loaded at specified index
    Scene* GetScene(unsigned index) { return loadedScenes_[index]; }

    /// Set the current scene 
    void SetCurrentScene(Scene* scene, Camera* camera = NULL);

    /// Unload a scene
    void UnloadScene(Scene* scene);

    /// Unload all loaded scenes
    void UnloadAllScenes();    

private:

    void HandleExitRequested(StringHash eventType, VariantMap& eventData);

    // Strong reference
    SharedPtr<Scene> currentScene_;

    Vector<SharedPtr<Scene>> loadedScenes_;

    SharedPtr<Viewport> viewport_;

};

}
