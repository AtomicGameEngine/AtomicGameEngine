//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/IO/Log.h>

#include <Atomic/IPC/IPC.h>
#include <Atomic/IPC/IPCEvents.h>
#include <Atomic/IPC/IPCBroker.h>

#include <Atomic/Input/InputEvents.h>

#include <ToolCore/ToolEnvironment.h>
#include <ToolCore/ToolSystem.h>
#include <ToolCore/License/LicenseSystem.h>
#include <ToolCore/Project/Project.h>

#include <AtomicJS/Javascript/JSIPCEvents.h>

#include <Atomic/UI/SystemUI/DebugHud.h>

#include "../PlayerMode/AEPlayerEvents.h"

#include "AEEditorMode.h"

using namespace ToolCore;

namespace AtomicEditor
{

EditorMode::EditorMode(Context* context) :
    Object(context)
{
    SubscribeToEvent(E_IPCWORKERSTART, HANDLER(EditorMode, HandleIPCWorkerStarted));
    SubscribeToEvent(E_IPCPLAYEREXITREQUEST, HANDLER(EditorMode, HandleIPCPlayerExitRequest));
}

EditorMode::~EditorMode()
{

}

void EditorMode::HandleIPCWorkerStarted(StringHash eventType, VariantMap& eventData)
{
    LicenseSystem* licenseSystem = GetSubsystem<LicenseSystem>();

    VariantMap startupData;
    SystemUI::DebugHud* debugHud = GetSubsystem<SystemUI::DebugHud>();

// BEGIN LICENSE MANAGEMENT

    startupData["license3D"] = licenseSystem->GetLicenseModule3D();

// END LICENSE MANAGEMENT

    startupData["debugHudMode"] = debugHud ? debugHud->GetMode() : (unsigned) 0;

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
        SendEvent(E_EDITORPLAYERSTOPPED);
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

bool EditorMode::PlayProject(String addArgs, bool debug)
{
    ToolEnvironment* env = GetSubsystem<ToolEnvironment>();
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();

    const String& editorBinary = env->GetEditorBinary();

    Project* project = tsystem->GetProject();

    if (!project)
        return false;

    Vector<String> paths;
    paths.Push(env->GetCoreDataDir());
    paths.Push(env->GetPlayerDataDir());
    paths.Push(project->GetResourcePath());

    // fixme: this is for loading from cache
    paths.Push(project->GetProjectPath());
    paths.Push(project->GetProjectPath() + "Cache");

    String resourcePaths;
    resourcePaths.Join(paths, "!");

    Vector<String> vargs;

    String args = ToString("--player --project \"%s\"", AddTrailingSlash(project->GetProjectPath()).CString());

    vargs = args.Split(' ');

    if (debug)
        vargs.Insert(0, "--debug");

    if (addArgs.Length() > 0)
        vargs.Insert(0, addArgs.Split(' '));

    String dump;
    dump.Join(vargs, " ");
    LOGINFOF("Launching Broker %s %s", editorBinary.CString(), dump.CString());

    IPC* ipc = GetSubsystem<IPC>();
    playerBroker_ = ipc->SpawnWorker(editorBinary, vargs);

    if (playerBroker_)
    {
        SubscribeToEvent(playerBroker_, E_IPCJSERROR, HANDLER(EditorMode, HandleIPCJSError));
        SubscribeToEvent(playerBroker_, E_IPCWORKEREXIT, HANDLER(EditorMode, HandleIPCWorkerExit));
        SubscribeToEvent(playerBroker_, E_IPCWORKERLOG, HANDLER(EditorMode, HandleIPCWorkerLog));
    }

    return playerBroker_.NotNull();

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
