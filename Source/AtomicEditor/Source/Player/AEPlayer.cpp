// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <Atomic/Core/Context.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Input/Input.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Javascript/Javascript.h>
#include <Atomic/Javascript/JSVM.h>
#include <Atomic/Javascript/JSEvents.h>
#include <Atomic/UI/TBUI.h>
#include <Atomic/UI/UI.h>

#include "AEPlayer.h"
#include "AEEvents.h"
#include "AEEditor.h"
#include "Project/AEProject.h"

// TODO: Remove dependency
#include <Duktape/duktape.h>

namespace AtomicEditor
{

AEPlayer::AEPlayer(Context* context) :
    Object(context),
    mode_(AE_PLAYERMODE_WIDGET)
{
    SubscribeToEvent(E_EDITORSHUTDOWN, HANDLER(AEPlayer, HandleEditorShutdown));

    assert(!context->GetSubsystem<AEPlayer>());
    context->RegisterSubsystem(this);

    Javascript* javascript = context->GetSubsystem<Javascript>();
    vm_ = javascript->InstantiateVM("AEPlayerVM");

    // only subscribe to errors on our VM
    SubscribeToEvent(vm_, E_JSERROR, HANDLER(AEPlayer, HandleJSError));

    vm_->InitJSContext();
    vm_->SetModuleSearchPath("Modules");

    duk_eval_string_noresult(vm_->GetJSContext(), "require(\"AtomicGame\"); require (\"AtomicEditor\");");


}

AEPlayer::~AEPlayer()
{
    UnsubscribeFromEvent(E_JSERROR);
    Javascript* javascript = context_->GetSubsystem<Javascript>();
    // this can be NULL when exiting during play mode
    if (javascript)
        javascript->ShutdownVM("AEPlayerVM");
    vm_ = NULL;

    GetSubsystem<Input>()->SetTouchEmulation(false);

}

void AEPlayer::Invalidate()
{
    context_->RemoveSubsystem<AEPlayer>();
    GetSubsystem<UI>()->GetRoot()->RemoveAllChildren();
}

void AEPlayer::HandleJSError(StringHash eventType, VariantMap& eventData)
{
    SendEvent(E_PLAYERERROR);

    AEPlayerError err;

    using namespace JSError;
    err.name_ = eventData[P_ERRORNAME].GetString();
    err.message_ = eventData[P_ERRORMESSAGE].GetString();
    err.filename_ = eventData[P_ERRORFILENAME].GetString();
    err.stack_ = eventData[P_ERRORSTACK].GetString();
    err.lineNumber_ = eventData[P_ERRORLINENUMBER].GetInt();

    errors_.Push(err);

}

bool AEPlayer::Play(AEPlayerMode mode, const IntRect &rect)
{
    mode_ = mode;

    duk_context* ctx = vm_->GetJSContext();

    String init;
    init.AppendWithFormat("Atomic.editor.setViewport(%i, %i, %i, %i);",
                          rect.left_, rect.top_, rect.right_, rect.bottom_);

    duk_eval_string_noresult(vm_->GetJSContext(),init.CString());

    Editor* editor = GetSubsystem<Editor>();
    Project* project = editor->GetProject();

    String mainPath = project->GetResourcePath();

    bool ok = vm_->ExecuteMain();

    if (!ok)
    {
        SendEvent(E_PLAYERERROR);
    }

    return ok;
}

void AEPlayer::HandleEditorShutdown(StringHash eventType, VariantMap& eventData)
{
    context_->RemoveSubsystem(GetType());
}

}
