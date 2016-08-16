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
#include "../Assets/AssetDatabase.h"
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

    void NETProjectSystem::OpenSolution()
    {
        if (!visualStudioPath_.Length())
            return;

        FileSystem* fileSystem = GetSubsystem<FileSystem>();

        if (!fileSystem->FileExists(solutionPath_))
        {
            if (!GenerateSolution())
                return;
        }

        OpenSourceFile(String::EMPTY);

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

            if (sourceFilePath.Length())
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
            if (!sourceFilePath.Length())
                return;

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
            ATOMIC_LOGINFOF("NETBuild Success for project");
        }
        else
        {
            const String& errorText = eventData[P_ERRORTEXT].GetString();

            ATOMIC_LOGERRORF("\n%s\n", errorText.CString());
            ATOMIC_LOGERRORF("NETBuild Error for project");
        }

    }

    void NETProjectSystem::BuildAtomicProject()
    {
        FileSystem* fileSystem = GetSubsystem<FileSystem>();

        if (!fileSystem->FileExists(solutionPath_))
        {
            if (!GenerateSolution())
            {
                ATOMIC_LOGERRORF("NETProjectSystem::BuildAtomicProject - solutionPath does not exist: %s", solutionPath_.CString());
                return;
            }
        }

#ifdef ATOMIC_PLATFORM_WINDOWS

        Project* project = GetSubsystem<ToolSystem>()->GetProject();
        NETBuildSystem* buildSystem = GetSubsystem<NETBuildSystem>();

        if (buildSystem)
        {
            NETBuild* build = buildSystem->BuildAtomicProject(project);

            if (build)
            {
                build->SubscribeToEvent(E_NETBUILDRESULT, ATOMIC_HANDLER(NETProjectSystem, HandleNETBuildResult));
            }

        }
#endif

    }

    bool NETProjectSystem::GenerateSolution()
    {
        ToolSystem* tsystem = GetSubsystem<ToolSystem>();
        Project* project = tsystem->GetProject();

        if (!project)
        {
            ATOMIC_LOGERRORF("NETProjectSystem::GenerateSolution - No Project Loaded");
            return false;
        }

        SharedPtr<NETProjectGen> gen(new NETProjectGen(context_));

        gen->SetScriptPlatform("WINDOWS");

        if (!gen->LoadProject(project))
        {
            ATOMIC_LOGERRORF("NETProjectSystem::GenerateSolution - Unable to Load Project");
            return false;
        }

        if (!gen->Generate())
        {
            ATOMIC_LOGERRORF("NETProjectSystem::GenerateSolution - Unable to Generate Project");
            return false;
        }

        return true;

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
            // set to false in case of error, we don't want to keep trying to 
            // rebuild, TODO: better error handling
            solutionDirty_ = false;
            GenerateSolution();
        }

        if (projectAssemblyDirty_)
        {        
            BuildAtomicProject();
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
        projectAssemblyPath_ = AddTrailingSlash(projectPath) + "Resources/AtomicProject.dll";

        FileSystem* fileSystem = GetSubsystem<FileSystem>();

        // TODO: We need a better way of marking C# projects
        StringVector results;
        fileSystem->ScanDir(results, AddTrailingSlash(projectPath) + "Resources", "*.cs", SCAN_FILES, true);
        if (!results.Size())
        {
            fileSystem->ScanDir(results, AddTrailingSlash(projectPath) + "Resources", "*.dll", SCAN_FILES, true);
            if (!results.Size())
                return;
        }            

        // if the solution or project assemblies don't exist mark as dirty

        if (!fileSystem->FileExists(solutionPath_))
            solutionDirty_ = true;

        if (!fileSystem->FileExists(projectAssemblyPath_))
            projectAssemblyDirty_ = true;

    }

    void NETProjectSystem::HandleAssetScanBegin(StringHash eventType, VariantMap& eventData)
    {

    }

    void NETProjectSystem::HandleAssetScanEnd(StringHash eventType, VariantMap& eventData)
    {
        if (solutionDirty_)
        {
            // set to false in case of error, we don't want to keep trying to 
            // rebuild, TODO: better error handling
            solutionDirty_ = false;
            GenerateSolution();
        }
    }
    
    void NETProjectSystem::HandleProjectUnloaded(StringHash eventType, VariantMap& eventData)
    {
        Clear();
    }

    void NETProjectSystem::HandleFileChanged(StringHash eventType, VariantMap& eventData)
    {

    }

    void NETProjectSystem::HandleAssetNew(StringHash eventType, VariantMap& eventData)
    {
        using namespace ResourceAdded;

        const String& guid = eventData[P_GUID].ToString();

        Asset* asset = GetSubsystem<AssetDatabase>()->GetAssetByGUID(guid);

        if (asset->GetExtension() == ".cs")
            solutionDirty_ = true;
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

        SubscribeToEvent(E_UPDATE, ATOMIC_HANDLER(NETProjectSystem, HandleUpdate));

        SubscribeToEvent(E_PROJECTLOADED, ATOMIC_HANDLER(NETProjectSystem, HandleProjectLoaded));
        SubscribeToEvent(E_PROJECTUNLOADED, ATOMIC_HANDLER(NETProjectSystem, HandleProjectUnloaded));

        SubscribeToEvent(E_ASSETSCANBEGIN, ATOMIC_HANDLER(NETProjectSystem, HandleAssetScanBegin));
        SubscribeToEvent(E_ASSETSCANEND, ATOMIC_HANDLER(NETProjectSystem, HandleAssetScanEnd));

        SubscribeToEvent(E_FILECHANGED, ATOMIC_HANDLER(NETProjectSystem, HandleFileChanged));

        SubscribeToEvent(E_RESOURCEADDED, ATOMIC_HANDLER(NETProjectSystem, HandleResourceAdded));
        SubscribeToEvent(E_RESOURCEREMOVED, ATOMIC_HANDLER(NETProjectSystem, HandleResourceRemoved));

        SubscribeToEvent(E_ASSETNEW, ATOMIC_HANDLER(NETProjectSystem, HandleAssetNew));
        SubscribeToEvent(E_ASSETRENAMED, ATOMIC_HANDLER(NETProjectSystem, HandleAssetRenamed));
        SubscribeToEvent(E_ASSETMOVED, ATOMIC_HANDLER(NETProjectSystem, HandleAssetMoved));

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
