//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/IO/Log.h>
#include <AtomicNET/NETCore/NETCore.h>

#include "NETToolSystem.h"

namespace ToolCore
{

NETToolSystem::NETToolSystem(Context* context) : Object(context)
{
    if (context->GetEditorContext())
    {
        NETCore* core = GetSubsystem<NETCore>();
        if (!core->CreateDelegate("AtomicNETTools", "AtomicTools.AtomicTools", "InspectAssembly", (void**) &inspectAssemblyFunction_))
        {
            LOGERROR("NETToolSystem::NETToolSystem - Unable to resolve delagate AtomicNETTools.InspectAssembly");
        }
    }
}

NETToolSystem::~NETToolSystem()
{

}

bool NETToolSystem::InspectAssembly(const String& pathToAssembly, JSONValue &json)
{
    json.SetType(JSON_NULL);

    if (!inspectAssemblyFunction_)
        return false;

    String jsonString = inspectAssemblyFunction_(pathToAssembly.CString());

    if (!jsonString.Length())
        return false;

    return JSONFile::ParseJSON(jsonString, json);

}

}
