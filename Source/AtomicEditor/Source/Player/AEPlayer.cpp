// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include "../Subprocess/AESubprocessSystem.h"

#include <Atomic/Core/Context.h>
#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Input/Input.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/UI/UI.h>

#include <ToolCore/ToolEnvironment.h>

#include "AEPlayer.h"
#include "AEEvents.h"
#include "AEEditor.h"
#include "Project/AEProject.h"

#include "UIPlayer.h"
#include "UI/Modal/UIModalOps.h"

namespace AtomicEditor
{


AEPlayer::AEPlayer(Context* context) :
    Object(context),
    mode_(AE_PLAYERMODE_WIDGET)
{
    SubscribeToEvent(E_EDITORSHUTDOWN, HANDLER(AEPlayer, HandleEditorShutdown));
    SubscribeToEvent(E_IPCWORKERSTART, HANDLER(AEPlayer, HandleIPCWorkerStarted));

    assert(!context->GetSubsystem<AEPlayer>());
    context->RegisterSubsystem(this);

}

AEPlayer::~AEPlayer()
{
    LOGINFO("Player down");
}

void AEPlayer::Invalidate()
{
    UIModalOps* ops = GetSubsystem<UIModalOps>();
    ops->Hide();
    context_->RemoveSubsystem<AEPlayer>();
}

void AEPlayer::HandleJSError(StringHash eventType, VariantMap& eventData)
{

}

void AEPlayer::HandleIPCWorkerStarted(StringHash eventType, VariantMap& eventData)
{
    LOGINFOF("Yay");

    VariantMap weventData;
    weventData[HelloFromBroker::P_HELLO] = "Hello";
    weventData[HelloFromBroker::P_LIFETHEUNIVERSEANDEVERYTHING] = 42;
    broker_->PostMessage(E_IPCHELLOFROMBROKER, weventData);
}

bool AEPlayer::Play(AEPlayerMode mode, const IntRect &rect)
{
    ToolCore::ToolEnvironment* env = GetSubsystem<ToolCore::ToolEnvironment>();
    const String& editorBinary = env->GetEditorBinary();

    Vector<String> paths;
    paths.Push(env->GetCoreDataDir());
    paths.Push("/Users/josh/Dev/atomic/AtomicExamples/Basic2D/Resources");

    String resourcePaths;
    resourcePaths.Join(paths, "!");

    Vector<String> vargs;

    String args = ToString("--editor-resource-paths \"%s\"", resourcePaths.CString());

    vargs = args.Split(' ');
    vargs.Insert(0, "--player");

    IPC* ipc = GetSubsystem<IPC>();
    broker_ = ipc->SpawnWorker(editorBinary, vargs);

    return broker_.NotNull();
}

void AEPlayer::SetUIPlayer(UIPlayer* uiPlayer)
{
    uiPlayer_ = uiPlayer;
}

void AEPlayer::HandleEditorShutdown(StringHash eventType, VariantMap& eventData)
{
    context_->RemoveSubsystem(GetType());
}

}
