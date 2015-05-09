// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Input/Input.h>
#include <Atomic/Graphics/Renderer.h>
#include <Atomic/Graphics/Graphics.h>
#include <Atomic/Engine/Engine.h>

#include "JSEvents.h"
#include "JSVM.h"
#include "JSGraphics.h"
#include "JSScene.h"
#include "JSAtomicGame.h"

#include <Atomic/Scene/Scene.h>
#include <Atomic/Environment/ProcSky.h>

namespace Atomic
{

extern void jsb_modules_init(JSVM* vm);

// TODO: add this properly
static int get_joystick_state(duk_context* ctx)
{
    JSVM* vm = JSVM::GetJSVM(ctx);
    Input* input = vm->GetSubsystem<Input>();

    JoystickState* state =  (JoystickState*) input->GetJoystickByIndex(duk_to_number(ctx, 0));

    duk_push_object(ctx);

    if (state->GetButtonDown(0))
        duk_push_true(ctx);
    else
        duk_push_false(ctx);

    duk_put_prop_string(ctx, -2, "button0");

    if (state->GetButtonDown(1))
        duk_push_true(ctx);
    else
        duk_push_false(ctx);

    duk_put_prop_string(ctx, -2, "button1");

    if (state->GetButtonDown(2))
        duk_push_true(ctx);
    else
        duk_push_false(ctx);

    duk_put_prop_string(ctx, -2, "button2");

    if (state->GetButtonDown(3))
        duk_push_true(ctx);
    else
        duk_push_false(ctx);

    duk_put_prop_string(ctx, -2, "button3");


    duk_push_number(ctx, state->GetAxisPosition(0));
    duk_put_prop_string(ctx, -2, "axis0");

    duk_push_number(ctx, state->GetAxisPosition(1));
    duk_put_prop_string(ctx, -2, "axis1");

    duk_push_number(ctx, state->GetAxisPosition(2));
    duk_put_prop_string(ctx, -2, "axis2");

    duk_push_number(ctx, state->GetAxisPosition(3));
    duk_put_prop_string(ctx, -2, "axis3");

    return 1;

}

void jsapi_init_atomicgame(JSVM* vm)
{
    duk_context* ctx = vm->GetJSContext();

    duk_get_global_string(ctx, "Atomic");

    duk_pop(ctx);

}

}
