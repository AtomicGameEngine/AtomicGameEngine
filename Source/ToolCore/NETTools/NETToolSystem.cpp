//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
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

#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>
#include <AtomicNET/NETCore/NETCore.h>

#include "../Project/ProjectEvents.h"
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
