//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Core/Object.h>
#include "BuildBase.h"

#include "../Platform/Platform.h"

using namespace Atomic;

namespace ToolCore
{

class BuildSystem : public Object
{
    OBJECT(BuildSystem);

public:
    /// Construct.
    BuildSystem(Context* context);
    /// Destruct.
    virtual ~BuildSystem();

    void SetBuildPath(const String& path) { buildPath_ = path; }

    const String& GetBuildPath() const { return buildPath_; }

    void QueueBuild(BuildBase* buildBase);

    bool StartNextBuild();

    void BuildComplete(PlatformID platform, const String& buildFolder, bool success = true, const String& buildMessage = String::EMPTY);

private:

    String buildPath_;

    List<SharedPtr<BuildBase>> queuedBuilds_;
    SharedPtr<BuildBase> currentBuild_;

};


}
