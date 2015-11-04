//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//


#pragma once

#include <Atomic/Core/Object.h>
#include <Atomic/Resource/JSONFile.h>

using namespace Atomic;

namespace ToolCore
{

class NETToolSystem : public Object
{
    OBJECT(NETToolSystem);

public:

    NETToolSystem(Context* context);
    virtual ~NETToolSystem();

    bool InspectAssembly(const String& pathToAssembly, JSONValue& json);

private:

    void HandleProjectLoaded(StringHash eventType, VariantMap& eventData);

    typedef const char* (*InpectAssemblyFuctionPtr)(const char* path);
    InpectAssemblyFuctionPtr inspectAssemblyFunction_;

};

}
