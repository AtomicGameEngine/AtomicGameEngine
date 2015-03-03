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

#include "UIPlayer.h"
#include "UI/Modal/UIModalOps.h"

// TODO: Remove dependency
#include <Duktape/duktape.h>

namespace AtomicEditor
{

static int js_atomiceditor_SetView(duk_context* ctx)
{
    JSVM* vm = JSVM::GetJSVM(ctx);
    AEPlayer* player = vm->GetSubsystem<AEPlayer>();

    Scene* scene = js_to_class_instance<Scene>(ctx, 0, 0);
    Camera* camera = js_to_class_instance<Camera>(ctx, 1, 0);

    UIPlayer* uiPlayer = player->GetUIPlayer();
    Viewport* viewport = uiPlayer->SetView(scene, camera);

    IntVector2 size = uiPlayer->GetPlayerSize();
    viewport->SetRect(IntRect(0, 0, size.x_, size.y_));

    js_push_class_object_instance(ctx, viewport, "Viewport");

    return 1;
}


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

    if (errors_.Size())
        return;

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
    UIModalOps* ops = GetSubsystem<UIModalOps>();
    ops->Hide();
    context_->RemoveSubsystem<AEPlayer>();
    GetSubsystem<UI>()->GetRoot()->RemoveAllChildren();

// BEGIN LICENSE MANAGEMENT
    if (uiPlayer_.NotNull() && uiPlayer_->Show3DInfo())
        ops->ShowInfoModule3D();
// END LICENSE MANAGEMENT

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

    if (errors_.Size())
        return false;

    mode_ = mode;

    UIModalOps* ops = GetSubsystem<UIModalOps>();

    ops->ShowPlayer();

    duk_context* ctx = vm_->GetJSContext();

    duk_get_global_string(ctx, "Atomic");
    duk_get_prop_string(ctx, -1, "editor");
    duk_push_c_function(ctx, js_atomiceditor_SetView, 2);
    duk_put_prop_string(ctx, -2, "setView");
    duk_pop_2(ctx);

    bool ok = vm_->ExecuteMain();

    if (!ok)
    {
        SendEvent(E_PLAYERERROR);
    }

    return ok;
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
