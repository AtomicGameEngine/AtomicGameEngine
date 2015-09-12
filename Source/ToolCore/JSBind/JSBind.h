//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace ToolCore
{

class JSBind : public Object
{

    OBJECT(JSBind)

public:

    JSBind(Context* context);
    virtual ~JSBind();

    bool GenerateBindings(const String& sourceRootFolder, const String& packageFolder, const String& platform);

    const String& GetSourceRootFolder() { return sourceRootFolder_; }
    const String& GetPackageFolder() { return packageFolder_; }
    const String& GetPlatform() { return platform_; }

private:

    String sourceRootFolder_;
    String packageFolder_;
    String platform_;

};


}
