//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>
#include <AtomicNET/NETCore/NETCore.h>

#include "../ToolEvents.h"
#include "../ToolEnvironment.h"

#include "NETToolSystem.h"
#include "NETProjectGen.h"

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

        SubscribeToEvent(E_PROJECTLOADED, HANDLER(NETToolSystem, HandleProjectLoaded));
    }

}

NETToolSystem::~NETToolSystem()
{

}

void NETToolSystem::HandleProjectLoaded(StringHash eventType, VariantMap& eventData)
{
    FileSystem* fileSystem = GetSubsystem<FileSystem>();

    String projectPath = eventData[ProjectLoaded::P_PROJECTPATH].GetString();

    String pathName, fileName, ext;

    SplitPath(projectPath, pathName, fileName, ext);

    String netJSONPath = AddTrailingSlash(pathName) + "AtomicNET.json";

    if (fileSystem->FileExists(netJSONPath))
    {
        SharedPtr<NETProjectGen> gen(new NETProjectGen(context_));

#ifdef ATOMIC_PLATFORM_OSX
        gen->SetScriptPlatform("MACOSX");
#else
        gen->SetScriptPlatform("WINDOWS");
#endif
        gen->LoadProject(netJSONPath, true);

        gen->Generate();

    }

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
