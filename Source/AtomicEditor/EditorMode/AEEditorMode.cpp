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

#include <Atomic/IPC/IPC.h>
#include <Atomic/IPC/IPCEvents.h>
#include <Atomic/IPC/IPCBroker.h>

#include <Atomic/Core/CoreEvents.h>
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
    SubscribeToEvent(E_IPCPLAYERPAUSERESUMEREQUEST, HANDLER(EditorMode, HandleIPCPlayerPauseResumeRequest));
    SubscribeToEvent(E_IPCPLAYERUPDATESPAUSEDRESUMED, HANDLER(EditorMode, HandleIPCPlayerUpdatesPausedResumed));
    SubscribeToEvent(E_IPCPLAYERPAUSESTEPREQUEST, HANDLER(EditorMode, HandleIPCPlayerPauseStepRequest));
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
