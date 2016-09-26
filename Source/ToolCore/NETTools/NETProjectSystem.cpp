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
#include "../Project/ProjectSettings.h"
#include "../Project/ProjectEvents.h"

#include "../Build/BuildSystem.h"

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
        if (!idePath_.Length())
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
        if (!idePath_.Length())
            return;

        String command = idePath_;
        StringVector args;

        if (ideSubprocess_.Expired())
        {
            SubprocessSystem* subs = GetSubsystem<SubprocessSystem>();
            ideSubprocess_ = 0;

#ifdef ATOMIC_PLATFORM_OSX

            command = "open";
            args.Push("-W");
            args.Push("-a");
            args.Push(idePath_);
#endif

            args.Push(solutionPath_);

            if (sourceFilePath.Length())
                args.Push(sourceFilePath);                       

            try
            {
                ideSubprocess_ = subs->Launch(command, args);
            }
            catch (Poco::SystemException)
            {
                ideSubprocess_ = 0;
            }

        }
        else
        {
            if (!sourceFilePath.Length())
                return;

            try
            {                
                std::vector<std::string> args;

#ifdef ATOMIC_PLATFORM_WINDOWS

                args.push_back("/edit");

#elif defined ATOMIC_PLATFORM_OSX

                command = "open";
                args.push_back("-a");
                args.push_back(idePath_.CString());

#elif defined ATOMIC_PLATFORM_LINUX

                args.push_back(idePath_.CString());

#endif
                args.push_back(sourceFilePath.CString());
                Poco::Process::launch(command.CString(), args);

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
    }

    bool NETProjectSystem::GenerateResourcePak()
    {
        ToolSystem* tsystem = GetSubsystem<ToolSystem>();
        Project* project = tsystem->GetProject();
        BuildSystem* buildSystem = GetSubsystem<BuildSystem>();

        // TODO: We just use WINDOWS platform for PAK generation for now
        Platform* platform = tsystem->GetPlatformByName("WINDOWS");

        buildSystem->SetBuildPath(project->GetProjectPath() + "AtomicNET/Resources/");

        SharedPtr<BuildBase> buildBase(platform->NewBuild(project));
        buildBase->SetResourcesOnly(true);
        buildBase->SetVerbose(true);
        buildSystem->QueueBuild(buildBase);
        buildSystem->StartNextBuild();

        if (buildBase->GetBuildFailed())
        {
            const StringVector& errors = buildBase->GetBuildErrors();
            ATOMIC_LOGERRORF("NETProjectSystem::GenerateSolution - Unable to Build Resources.pak: %s", errors.Size() ? errors[0].CString() : "Unknown Error");
            return false;
        }

        return true;

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

        // TODO: Generalize and move me
        if (project->GetSupportsPlatform("android") || project->GetSupportsPlatform("ios"))
        {
            FileSystem* fileSystem = GetSubsystem<FileSystem>();

            if (!fileSystem->FileExists(project->GetProjectPath() + "AtomicNET/Resources/AtomicResources.pak"))
            {
                if (!GenerateResourcePak())
                    return false;

            }

        }

        SharedPtr<NETProjectGen> gen(new NETProjectGen(context_));

        if (!gen->LoadAtomicProject(project->GetProjectPath()))
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
        Project* project = static_cast<Project*>(eventData[P_PROJECT].GetPtr());        
        
        if (GetExtension(projectPath) == ".atomic")
            projectPath = GetParentPath(projectPath);

        String projectName = project->GetProjectSettings()->GetName();

        solutionPath_ = AddTrailingSlash(projectPath) + "AtomicNET/Solution/" + projectName + ".sln";
        projectAssemblyPath_ = AddTrailingSlash(projectPath) + "Resources/" + projectName + ".dll";

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


#ifdef ATOMIC_PLATFORM_WINDOWS

        FileSystem* fileSystem = GetSubsystem<FileSystem>();

        // Query for Visual Studio 2015 path
        idePath_ = Poco::Environment::get("VS140COMNTOOLS").c_str();

        if (idePath_.Length())
        {
            idePath_.Replace("Tools\\", "IDE\\devenv.exe");

            if (!fileSystem->FileExists(idePath_))
                idePath_.Clear();
        }

#elif defined ATOMIC_PLATFORM_OSX

        FileSystem* fileSystem = GetSubsystem<FileSystem>();

        if (fileSystem->DirExists("/Applications/Xamarin Studio.app"))
        {
            idePath_ = "/Applications/Xamarin Studio.app/Contents/MacOS/XamarinStudio";
        }

#elif defined ATOMIC_PLATFORM_LINUX

        FileSystem* fileSystem = GetSubsystem<FileSystem>();

        if (fileSystem->FileExists("/usr/bin/monodevelop"))
        {
            idePath_ = "/usr/bin/monodevelop";
        }

#endif

    }


}
