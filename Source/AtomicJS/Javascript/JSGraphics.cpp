// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "JSGraphics.h"
#include "JSVM.h"
#include <Atomic/Graphics/Light.h>

namespace Atomic
{

static int Light_SetShadowCascade(duk_context* ctx)
{
    //CascadeParameters(float split1, float split2, float split3, float split4, float fadeStart, float biasAutoAdjust = 1.0f) :
    int numargs = duk_get_top(ctx);

    float split1;
    float split2;
    float split3;
    float split4;
    float fadeStart;
    float biasAutoAdjust = 1.0f;

    split1 = (float) duk_to_number(ctx, 0);
    split2 = (float) duk_to_number(ctx, 1);
    split3 = (float) duk_to_number(ctx, 2);
    split4 = (float) duk_to_number(ctx, 3);
    fadeStart = (float) duk_to_number(ctx, 4);
    if (numargs == 6)
        biasAutoAdjust = (float) duk_to_number(ctx, 5);

    CascadeParameters cparms(split1, split2, split3, split4, fadeStart, biasAutoAdjust);

    duk_push_this(ctx);
    Light* light = js_to_class_instance<Light>(ctx, -1, 0);
    light->SetShadowCascade(cparms);
    return 0;
}

static int Light_SetShadowBias(duk_context* ctx)
{
    float constantBias = (float) duk_to_number(ctx, 0);
    float slopeScaledBias = (float) duk_to_number(ctx, 1);

    BiasParameters bparms(constantBias, slopeScaledBias);

    duk_push_this(ctx);
    Light* light = js_to_class_instance<Light>(ctx, -1, 0);
    light->SetShadowBias(bparms);
    return 0;
}


void jsapi_init_graphics(JSVM* vm)
{
    duk_context* ctx = vm->GetJSContext();

    js_class_get_prototype(ctx, "Atomic", "Light");
    duk_push_c_function(ctx, Light_SetShadowCascade, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "setShadowCascade");
    duk_push_c_function(ctx, Light_SetShadowBias, 2);
    duk_put_prop_string(ctx, -2, "setShadowBias");
    duk_pop(ctx);
}

}
