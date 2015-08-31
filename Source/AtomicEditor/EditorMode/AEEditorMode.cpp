
#include <Atomic/IO/Log.h>

#include <Atomic/IPC/IPC.h>
#include <Atomic/IPC/IPCEvents.h>
#include <Atomic/IPC/IPCBroker.h>

#include <ToolCore/ToolEnvironment.h>
#include <ToolCore/ToolSystem.h>
#include <ToolCore/Project/Project.h>

#include <AtomicJS/Javascript/JSIPCEvents.h>

#include <Atomic/UI/SystemUI/DebugHud.h>

#include "AEEditorMode.h"

using namespace ToolCore;

namespace AtomicEditor
{

EditorMode::EditorMode(Context* context) :
    Object(context)
{    
    SubscribeToEvent(E_IPCWORKERSTART, HANDLER(EditorMode, HandleIPCWorkerStarted));
}

EditorMode::~EditorMode()
{

}

void EditorMode::HandleIPCWorkerStarted(StringHash eventType, VariantMap& eventData)
{
    VariantMap startupData;
    SystemUI::DebugHud* debugHud = GetSubsystem<SystemUI::DebugHud>();

    startupData["debugHudMode"] = debugHud ? debugHud->GetMode() : (unsigned) 0;

    playerBroker_->PostMessage(E_IPCINITIALIZE, startupData);

    SendEvent("EditorPlayerStarted");

}

void EditorMode::HandleIPCWorkerExit(StringHash eventType, VariantMap& eventData)
{
    //SendEvent(E_EDITORPLAYSTOP);
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

bool EditorMode::PlayProject()
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

}
