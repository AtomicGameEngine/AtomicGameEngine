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

#include <Poco/Environment.h>

#include <Atomic/Core/CoreEvents.h>

#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/ResourceEvents.h>

#include "../ToolSystem.h"
#include "../Assets/AssetEvents.h"
#include "../Project/Project.h"
#include "../Project/ProjectEvents.h"

#include "../Subprocess/SubprocessSystem.h"

#include "NETProjectGen.h"
#include "NETBuildSystem.h"
#include "NETProjectSystem.h"

namespace ToolCore
{

    NETProjectSystem::NETProjectSystem(Context* context) :
        Object(context)
    {
        Initialize();
    }

    NETProjectSystem::~NETProjectSystem()
    {

    }

    void NETProjectSystem::Clear()
    {
        quietPeriod_ = 0.0f;
        solutionPath_.Clear();
        projectAssemblyPath_.Clear();
        solutionDirty_ = false;
        projectAssemblyDirty_ = false;
    }

    void NETProjectSystem::OpenSourceFile(const String& sourceFilePath)
    {
        if (!visualStudioPath_.Length())
            return;

        StringVector args;

        if (vsSubprocess_.Expired())
        {
            SubprocessSystem* subs = GetSubsystem<SubprocessSystem>();
            vsSubprocess_ = 0;

            args.Push(solutionPath_);
            args.Push(sourceFilePath);

            try
            {
                vsSubprocess_ = subs->Launch(visualStudioPath_, args);
            }
            catch (Poco::SystemException)
            {
                vsSubprocess_ = 0;
            }

        }
        else
        {
            try
            {
                std::vector<std::string> args;
                args.push_back("/edit");
                args.push_back(sourceFilePath.CString());
                Poco::Process::launch(visualStudioPath_.CString(), args);

            }
            catch (Poco::SystemException)
            {
            }

        }

    }

    void NETProjectSystem::HandleNETBuildResult(StringHash eventType, VariantMap& eventData)
    {
        using namespace NETBuildResult;

        if (eventData[P_SUCCESS].GetBool())
        {
            LOGINFOF("NETBuild Success for project");
        }
        else
        {
            const String& errorText = eventData[P_ERRORTEXT].GetString();

            LOGERRORF("\n%s\n", errorText.CString());
            LOGERRORF("NETBuild Error for project");
        }

    }


    void NETProjectSystem::HandleUpdate(StringHash eventType, VariantMap& eventData)
    {
        using namespace Update;

        float delta = eventData[P_TIMESTEP].GetFloat();

        quietPeriod_ -= delta;

        if (quietPeriod_ < 0.0f)
            quietPeriod_ = 0.0f;

        if (quietPeriod_ > 0.0f)
            return;

        if (solutionDirty_)
        {
            ToolSystem* tsystem = GetSubsystem<ToolSystem>();
            Project* project = tsystem->GetProject();

            SharedPtr<NETProjectGen> gen(new NETProjectGen(context_));

            gen->SetScriptPlatform("WINDOWS");

            if (!gen->LoadProject(project))
            {
                return;
            }

            if (!gen->Generate())
            {
                return;
            }

            solutionDirty_ = false;

        }

        if (projectAssemblyDirty_)
        {
            Project* project = GetSubsystem<ToolSystem>()->GetProject();
            NETBuild* build = GetSubsystem<NETBuildSystem>()->BuildAtomicProject(project);

            if (build)
            {
                build->SubscribeToEvent(E_NETBUILDRESULT, HANDLER(NETProjectSystem, HandleNETBuildResult));
            }

            projectAssemblyDirty_ = false;

        }

    }

    void NETProjectSystem::HandleProjectLoaded(StringHash eventType, VariantMap& eventData)
    {
        using namespace ProjectLoaded;

        String projectPath = eventData[P_PROJECTPATH].GetString();
        
        if (GetExtension(projectPath) == ".atomic")
            projectPath = GetParentPath(projectPath);

        solutionPath_ = AddTrailingSlash(projectPath) + "AtomicNET/Solution/AtomicProject.sln";
        projectAssemblyPath_ = AddTrailingSlash(projectPath) + "Resource/AtomicProject.dll";

        FileSystem* fileSystem = GetSubsystem<FileSystem>();

        // if the solution or project assemblies don't exist mark as dirty

        if (!fileSystem->FileExists(solutionPath_))
            solutionDirty_ = true;

        if (!fileSystem->FileExists(projectAssemblyPath_))
            projectAssemblyDirty_ = true;

    }
    
    void NETProjectSystem::HandleProjectUnloaded(StringHash eventType, VariantMap& eventData)
    {
        Clear();
    }

    void NETProjectSystem::HandleFileChanged(StringHash eventType, VariantMap& eventData)
    {

    }

    void NETProjectSystem::HandleResourceAdded(StringHash eventType, VariantMap& eventData)
    {

    }

    void NETProjectSystem::HandleResourceRemoved(StringHash eventType, VariantMap& eventData)
    {

    }

    void NETProjectSystem::HandleAssetRenamed(StringHash eventType, VariantMap& eventData)
    {

    }

    void NETProjectSystem::HandleAssetMoved(StringHash eventType, VariantMap& eventData)
    {

    }

    void NETProjectSystem::Initialize()    
    {
        Clear();

#ifdef ATOMIC_PLATFORM_WINDOWS

        SubscribeToEvent(E_UPDATE, HANDLER(NETProjectSystem, HandleUpdate));

        SubscribeToEvent(E_PROJECTLOADED, HANDLER(NETProjectSystem, HandleProjectLoaded));
        SubscribeToEvent(E_PROJECTUNLOADED, HANDLER(NETProjectSystem, HandleProjectUnloaded));

        SubscribeToEvent(E_FILECHANGED, HANDLER(NETProjectSystem, HandleFileChanged));

        SubscribeToEvent(E_RESOURCEADDED, HANDLER(NETProjectSystem, HandleResourceAdded));
        SubscribeToEvent(E_RESOURCEREMOVED, HANDLER(NETProjectSystem, HandleResourceRemoved));

        SubscribeToEvent(E_ASSETRENAMED, HANDLER(NETProjectSystem, HandleAssetRenamed));
        SubscribeToEvent(E_ASSETMOVED, HANDLER(NETProjectSystem, HandleAssetMoved));

        FileSystem* fileSystem = GetSubsystem<FileSystem>();

        // Query for Visual Studio 2015 path
        visualStudioPath_ = Poco::Environment::get("VS140COMNTOOLS").c_str();

        if (visualStudioPath_.Length())
        {
            visualStudioPath_.Replace("Tools\\", "IDE\\devenv.exe");

            if (!fileSystem->FileExists(visualStudioPath_))
                visualStudioPath_.Clear();
        }

#endif

    }


}
