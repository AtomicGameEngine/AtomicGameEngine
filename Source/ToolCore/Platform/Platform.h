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

class BuildBase;
class Project;

enum PlatformID
{
    PLATFORMID_UNDEFINED,
    PLATFORMID_WINDOWS,
    PLATFORMID_MAC,
    PLATFORMID_ANDROID,
    PLATFORMID_IOS,
    PLATFORMID_WEB
};

class Platform : public Object
{
    OBJECT(Platform);

public:

    Platform(Context* context);
    virtual ~Platform();

    virtual bool GetLicense() = 0;

    virtual String GetName() = 0;
    virtual PlatformID GetPlatformID() = 0;

    virtual BuildBase* NewBuild(Project* project) = 0;

private:

    bool validLicense_;

};

}
