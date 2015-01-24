
#include "Precompiled.h"
#include "../IO/FileSystem.h"
#include "../IO/Log.h"
#include "../Resource/ResourceCache.h"
#include "../Input/Input.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/Graphics.h"

#include "../UI/UI.h"

#include "../Engine/Engine.h"

#include "../Javascript/JSEvents.h"
#include "../Javascript/JSVM.h"
#include "../Javascript/JSGraphics.h"
#include "../Javascript/JSScene.h"
#include "../Javascript/JSAtomicGame.h"

#include "../Scene/Scene.h"
#include "../Environment/ProcSky.h"

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
