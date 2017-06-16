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
#include <Atomic/Engine/Application.h>

#include <Atomic/IPC/IPC.h>
#include <Atomic/IPC/IPCEvents.h>
#include <Atomic/IPC/IPCBroker.h>

#include <Atomic/Core/CoreEvents.h>
#include <Atomic/Input/InputEvents.h>

#include <ToolCore/ToolEnvironment.h>
#include <ToolCore/ToolSystem.h>
#include <ToolCore/License/LicenseSystem.h>
#include <ToolCore/Project/Project.h>
#include <ToolCore/Project/ProjectSettings.h>

#include <ToolCore/NETTools/NETProjectSystem.h>
#include <ToolCore/NETTools/NETBuildSystem.h>

#include <AtomicJS/Javascript/JSIPCEvents.h>

#include <Atomic/UI/SystemUI/DebugHud.h>

#include <AtomicApp/Player/IPCPlayerAppEvents.h>

#include "AEEditorMode.h"

using namespace ToolCore;

namespace AtomicEditor
{

EditorMode::EditorMode(Context* context) :
    Object(context),
    playerEnabled_(false),
    debug_(false)
{

}

EditorMode::~EditorMode()
{
}

void EditorMode::HandleIPCWorkerStarted(StringHash eventType, VariantMap& eventData)
{
    VariantMap startupData;
    DebugHud* debugHud = GetSubsystem<DebugHud>();

    startupData["debugHudMode"] = debugHud ? debugHud->GetMode() : (unsigned) 0;
    startupData["debugHudProfilerMode"] = (unsigned) (debugHud ? debugHud->GetProfilerMode() : DEBUG_HUD_PROFILE_PERFORMANCE);

    SendEvent(E_EDITORPLAYREQUEST);

    playerBroker_->PostMessage(E_IPCINITIALIZE, startupData);

    SendEvent(E_EDITORPLAYERSTARTED);

    playerEnabled_ = true;

}

void EditorMode::HandleIPCWorkerExit(StringHash eventType, VariantMap& eventData)
{
    //SendEvent(E_EDITORPLAYSTOP);

    if (eventData[IPCWorkerExit::P_BROKER] == playerBroker_)
    {
        playerBroker_ = 0;
        playerEnabled_ = false;

        UnsubscribeFromEvent(E_IPCWORKERSTART);
        UnsubscribeFromEvent(E_IPCPLAYERPAUSERESUMEREQUEST);
        UnsubscribeFromEvent(E_IPCPLAYERUPDATESPAUSEDRESUMED);
        UnsubscribeFromEvent(E_IPCPLAYERPAUSESTEPREQUEST);
        UnsubscribeFromEvent(E_IPCPLAYEREXITREQUEST);

        SendEvent(E_EDITORPLAYERSTOPPED);
    }
    else
    {
        ATOMIC_LOGERROR("EditorMode::HandleIPCWorkerExit - Unknown Broker");
    }
}

void EditorMode::HandleIPCWorkerLog(StringHash eventType, VariantMap& eventData)
{
    using namespace IPCWorkerLog;

    // convert to a player log

    VariantMap playerLogData;

    playerLogData["message"]  = eventData[P_MESSAGE].GetString();
    playerLogData["level"]  = eventData[P_LEVEL].GetInt();

    SendEvent("EditorPlayerLog", playerLogData);

}

void EditorMode::HandleIPCJSError(StringHash eventType, VariantMap& eventData)
{

}

void EditorMode::HandleNETBuildResult(StringHash eventType, VariantMap& eventData)
{

    using namespace NETBuildResult;

    if (eventData[P_SUCCESS].GetBool())
    {
        NETProjectSystem* netProjectSystem = GetSubsystem<NETProjectSystem>();

        if (!netProjectSystem->GetSolutionAvailable() || netProjectSystem->GetProjectAssemblyDirty())
        {
            ATOMIC_LOGERROR("EditorMode::HandleNETBuildResult() - NETBuild was successful, however project still reported as dirty or missing");
        }

        PlayProjectInternal(additionalArgs_, debug_);
    }

    additionalArgs_.Clear();
    debug_ = false;

}

bool EditorMode::PlayProject(String addArgs, bool debug)
{
    additionalArgs_ = addArgs;
    debug_ = debug;

    NETProjectSystem* netProjectSystem = GetSubsystem<NETProjectSystem>();

    // If we're a net project, with a solution, and the project assembly is dirty build before playing
    if (netProjectSystem && netProjectSystem->GetSolutionAvailable() && netProjectSystem->GetProjectAssemblyDirty())
    {
        NETBuild* build = netProjectSystem->BuildAtomicProject();

        if (!build)
        {
            ATOMIC_LOGERROR("EditorMode::PlayProject() - Unable to instantiate C# build");
            return false;
        }

        SubscribeToEvent(build, E_NETBUILDRESULT, ATOMIC_HANDLER(EditorMode, HandleNETBuildResult));

        return true;
    }

    return PlayProjectInternal(addArgs, debug);
}

bool EditorMode::PlayProjectInternal(const String &addArgs, bool debug)
{
    if (playerBroker_.NotNull())
        return false;

    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();
    ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();

    Project* project = tsystem->GetProject();

    if (!project)
        return false;

    ToolEnvironment* env = GetSubsystem<ToolEnvironment>();

    String playerBinary = env->GetEditorBinary();

    ProjectSettings* settings = project->GetProjectSettings();

    String projectAssembly = settings->GetName() + ".dll";
    String projectExe = settings->GetName() + ".exe";

    // TODO: We need to configure project as managed
    bool managed = false;
    if (fileSystem->FileExists(project->GetResourcePath() + projectAssembly))
    {
        managed = true;

#ifdef ATOMIC_DEV_BUILD

#ifdef ATOMIC_DEBUG        
        playerBinary = project->GetProjectPath() + "AtomicNET/Debug/Bin/Desktop/" + projectExe;
#else
        playerBinary = project->GetProjectPath() + "AtomicNET/Release/Bin/Desktop/" + projectExe;
#endif

#else
        // TODO: We are using the release build of the managed project here, how and when to use debug?
        playerBinary = project->GetProjectPath() + "AtomicNET/Release/Bin/Desktop/" + projectExe;
#endif

        
        if (!fileSystem->FileExists(playerBinary))
        {
            ATOMIC_LOGERRORF("Managed player: %s does not exist", playerBinary.CString());
        }

    }


    Vector<String> paths;
    paths.Push(env->GetCoreDataDir());
    paths.Push(env->GetPlayerDataDir());
    paths.Push(project->GetResourcePath());

    // fixme: this is for loading from cache
    // https://github.com/AtomicGameEngine/AtomicGameEngine/issues/1037
    paths.Push(project->GetProjectPath());

    paths.Push(project->GetProjectPath() + "Cache");

    String resourcePaths;
    resourcePaths.Join(paths, "!");

    Vector<String> vargs;

    String args = ToString("--atomiceditor --player --project \"%s\"", AddTrailingSlash(project->GetProjectPath()).CString());

    vargs = args.Split(' ');

    if (managed)
    {
#ifdef ATOMIC_DEV_BUILD
        vargs.Insert(0, ToString("\"%s/Resources/\"", tenv->GetRootSourceDir().CString()));
#else

#ifdef ATOMIC_PLATFORM_OSX
        vargs.Insert(0, ToString("\"%s\"", (fileSystem->GetProgramDir() + "../Resources/").CString()));
#else
        vargs.Insert(0, ToString("\"%s\"", (fileSystem->GetProgramDir() + "Resources/").CString()));
#endif

#endif
        vargs.Insert(0, "--resourcePrefix");
    }

    if (debug)
        vargs.Insert(0, "--debug");

    // forward autometrics to player
    if (Application::GetAutoMetrics())
    {
        vargs.Insert(0, "--autometrics");
    }
    else
    {
        // enabled metrics at app start

        DebugHud* debugHud = GetSubsystem<DebugHud>();

        if ( debugHud && ( debugHud->GetMode() & Atomic::DEBUGHUD_SHOW_PROFILER) && (debugHud->GetProfilerMode() == DEBUG_HUD_PROFILE_METRICS))
        {
            vargs.Insert(0, "--playermetrics");
        }

    }

    if (addArgs.Length() > 0)
        vargs.Insert(0, addArgs.Split(' '));

#ifndef ATOMIC_PLATFORM_WINDOWS
    if (managed)
    {
        vargs.Insert(0, playerBinary);

#ifdef ATOMIC_PLATFORM_OSX
        playerBinary = tenv->GetMonoExecutableDir() + "mono64";
#else
        playerBinary = "mono";
#endif
    }

#endif

    String dump;
    dump.Join(vargs, " ");
    ATOMIC_LOGINFOF("Launching Broker %s %s", playerBinary.CString(), dump.CString());

    IPC* ipc = GetSubsystem<IPC>();
    playerBroker_ = ipc->SpawnWorker(playerBinary, vargs);

    if (playerBroker_)
    {
        SubscribeToEvent(playerBroker_, E_IPCWORKERSTART, ATOMIC_HANDLER(EditorMode, HandleIPCWorkerStarted));

        SubscribeToEvent(E_IPCPLAYERPAUSERESUMEREQUEST, ATOMIC_HANDLER(EditorMode, HandleIPCPlayerPauseResumeRequest));
        SubscribeToEvent(E_IPCPLAYERUPDATESPAUSEDRESUMED, ATOMIC_HANDLER(EditorMode, HandleIPCPlayerUpdatesPausedResumed));
        SubscribeToEvent(E_IPCPLAYERPAUSESTEPREQUEST, ATOMIC_HANDLER(EditorMode, HandleIPCPlayerPauseStepRequest));
        SubscribeToEvent(E_IPCPLAYEREXITREQUEST, ATOMIC_HANDLER(EditorMode, HandleIPCPlayerExitRequest));
    

        SubscribeToEvent(playerBroker_, E_IPCJSERROR, ATOMIC_HANDLER(EditorMode, HandleIPCJSError));
        SubscribeToEvent(playerBroker_, E_IPCWORKEREXIT, ATOMIC_HANDLER(EditorMode, HandleIPCWorkerExit));
        SubscribeToEvent(playerBroker_, E_IPCWORKERLOG, ATOMIC_HANDLER(EditorMode, HandleIPCWorkerLog));
    }

    return playerBroker_.NotNull();

}

void EditorMode::HandleIPCPlayerPauseResumeRequest(StringHash eventType, VariantMap& eventData)
{
    if (!playerBroker_) return;
    VariantMap noEventData;
    playerBroker_->PostMessage(E_PAUSERESUMEREQUESTED, noEventData);
}

void EditorMode::HandleIPCPlayerUpdatesPausedResumed(StringHash eventType, VariantMap& eventData)
{
    using namespace UpdatesPaused;

    bool paused = eventData[P_PAUSED].GetBool();
    if (paused)
        SendEvent(E_EDITORPLAYERPAUSED);
    else
        SendEvent(E_EDITORPLAYERRESUMED);
}

void EditorMode::HandleIPCPlayerPauseStepRequest(StringHash eventType, VariantMap& eventData)
{
    if (!playerBroker_) return;
    VariantMap noEventData;
    playerBroker_->PostMessage(E_PAUSESTEPREQUESTED, noEventData);
}

void EditorMode::HandleIPCPlayerExitRequest(StringHash eventType, VariantMap& eventData)
{
    if (!playerBroker_) return;
    VariantMap noEventData;
    playerBroker_->PostMessage(E_EXITREQUESTED, noEventData);
}

bool EditorMode::IsPlayerEnabled()
{
    return playerEnabled_;
}

}
