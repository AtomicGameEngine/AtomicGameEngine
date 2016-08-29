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

#include <Atomic/Core/Context.h>
#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>

#include <Atomic/IPC/IPC.h>
#include <Atomic/IPC/IPCEvents.h>
#include <Atomic/IPC/IPCBroker.h>

#include "../ToolSystem.h"
#include "../ToolEnvironment.h"
#include "../Project/Project.h"
#include "../Project/ProjectSettings.h"

#include "../Build/BuildSystem.h"

#include "NETCmd.h"
#include "../NETTools/AtomicNETService.h"
#include "../NETTools/NETBuildSystem.h"
#include "../NETTools/NETProjectGen.h"

namespace ToolCore
{

NETCmd::NETCmd(Context* context) : Command(context),
	requiresProjectLoad_(false)
{

}

NETCmd::~NETCmd()
{

}

bool NETCmd::Parse(const Vector<String>& arguments, unsigned startIndex, String& errorMsg)
{
    String argument = arguments[startIndex].ToLower();
    command_ = startIndex + 1 < arguments.Size() ? arguments[startIndex + 1].ToLower() : String::EMPTY;

    if (argument != "net")
    {
        errorMsg = "Unable to parse net command";
        return false;
    }

    if (command_ == "compile")
    {
        solutionPath_ = startIndex + 2 < arguments.Size() ? arguments[startIndex + 2] : String::EMPTY;
        platform_ = startIndex + 3 < arguments.Size() ? arguments[startIndex + 3] : String::EMPTY;
        configuration_ = startIndex + 4 < arguments.Size() ? arguments[startIndex + 4] : "Release";

        bool exists = false;

        if (solutionPath_.Length())
        {
            FileSystem* fs = GetSubsystem<FileSystem>();
            exists = fs->FileExists(solutionPath_);
        }

        if (!exists)
        {
            errorMsg = ToString("Solution file not found: %s", solutionPath_.CString());
            return false;
        }

        if (!platform_.Length())
        {
            errorMsg = "Platform not specified";
            return false;
        }

        return true;
    }
	else if (command_ == "genresources")
	{
		projectPath_ = startIndex + 2 < arguments.Size() ? arguments[startIndex + 2] : String::EMPTY;
		platform_ = startIndex + 3 < arguments.Size() ? arguments[startIndex + 3] : String::EMPTY;
		requiresProjectLoad_ = true;
	}
    else
    {
        errorMsg = "Unknown net command";
        return false;
    }


    return true;
}

void NETCmd::HandleNETBuildResult(StringHash eventType, VariantMap& eventData)
{
    using namespace NETBuildResult;

    if (eventData[P_SUCCESS].GetBool())
    {
        ATOMIC_LOGINFOF("NETBuild Success for solution: %s", solutionPath_.CString());
        Finished();
    }
    else
    {
        const String& errorText = eventData[P_ERRORTEXT].GetString();

        ATOMIC_LOGERRORF("\n%s\n", errorText.CString());
        Error(ToString("NETBuild Error for solution: %s", solutionPath_.CString()));
        Finished();
    }
    
}

void NETCmd::Run()
{
    if (command_ == "compile")
    {
		
		FileSystem* fileSystem = GetSubsystem<FileSystem>();

        NETBuildSystem* buildSystem = new NETBuildSystem(context_);
        context_->RegisterSubsystem(buildSystem);

		NETBuild* build = 0;

		String solutionPath;
		String fileName;
		String ext;

		// detect project        
		SplitPath(solutionPath_, solutionPath, fileName, ext);

		if (ext == ".atomic")
		{
			SharedPtr<NETProjectGen> gen(new NETProjectGen(context_));

			if (!gen->LoadAtomicProject(solutionPath_))
			{
				Error(ToString("NETProjectGen: Error loading project (%s)", solutionPath.CString()));
				Finished();
				return;
			}

			if (!gen->Generate())
			{
				Error(ToString("NETProjectGen: Error generating project (%s)", solutionPath.CString()));
				Finished();
				return;
			}

			solutionPath_ = solutionPath + "/AtomicNET/Solution/" + gen->GetProjectSettings()->GetName() + ".sln";

		}

		// json project file
		build = buildSystem->Build(solutionPath_, platform_, configuration_);

        if (!build)
        {
            Error("Unable to start build");
            Finished();
            return;
        }

        build->SubscribeToEvent(E_NETBUILDRESULT, ATOMIC_HANDLER(NETCmd, HandleNETBuildResult));

    }
	else if (command_ == "genresources")
	{
		BuildSystem* buildSystem = GetSubsystem<BuildSystem>();
		ToolSystem* toolSystem = GetSubsystem<ToolSystem>();
		Project* project = toolSystem->GetProject();

		if (!project)
		{
			Error("Unable to get project");
			Finished();
			return;
		}

		buildSystem->SetBuildPath(project->GetProjectPath() + "AtomicNET/Resources/");

		Platform* platform = toolSystem->GetPlatformByName(platform_);

		if (!platform)
		{
			Error(ToString("Unknown platform %s", platform_.CString()));
			Finished();
			return;
		}

		BuildBase* buildBase = platform->NewBuild(project);
		buildBase->SetResourcesOnly(true);
		buildBase->SetVerbose(true);
		buildSystem->QueueBuild(buildBase);
		buildSystem->StartNextBuild();

		Finished();
		return;
	}

}

}
