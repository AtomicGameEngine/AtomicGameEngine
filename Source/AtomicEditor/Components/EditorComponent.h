//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Scene/Component.h>

using namespace Atomic;

namespace AtomicEditor
{

class SceneEditor3D;

/// A component that requires the editor, for example CubemapGenerator requires a renderer
/// IMPORTANT: Other tool related components, which don't require the editor, should be in ToolCore
class EditorComponent : public Component
{
    OBJECT(EditorComponent);

public:

    /// Construct.
    EditorComponent(Context* context);

    /// Destruct.
    virtual ~EditorComponent();

    /// Register object factory.
    static void RegisterObject(Context* context);

protected:

    SceneEditor3D* GetSceneEditor();


};

}
