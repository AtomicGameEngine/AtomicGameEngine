// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "JSGraphics.h"
#include "JSVM.h"

#include <Atomic/Graphics/Material.h>
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

// Material

static int Material_GetShaderParameters(duk_context* ctx)
{
    duk_push_this(ctx);
    Material* material = js_to_class_instance<Material>(ctx, -1, 0);

    const HashMap<StringHash, MaterialShaderParameter>& params =  material->GetShaderParameters();

    duk_push_array(ctx);

    unsigned j = 0;
    for (HashMap<StringHash, MaterialShaderParameter>::ConstIterator i = params.Begin(); i != params.End(); ++i)
    {
        duk_push_object(ctx);

        duk_push_string(ctx, i->second_.name_.CString());
        duk_put_prop_string(ctx, -2, "name");

        js_push_variant(ctx, i->second_.value_);
        duk_put_prop_string(ctx, -2, "value");

        duk_push_string(ctx, i->second_.value_.ToString().CString());
        duk_put_prop_string(ctx, -2, "valueString");

        duk_push_string(ctx, i->second_.value_.GetTypeName().CString());
        duk_put_prop_string(ctx, -2, "typeName");

        duk_push_number(ctx, (double) i->second_.value_.GetType());
        duk_put_prop_string(ctx, -2, "type");

        duk_put_prop_index(ctx, -2, j++);
    }

    return 1;
}

static int Material_GetTextureUnitName(duk_context* ctx)
{
    duk_push_string(ctx, Material::GetTextureUnitName((TextureUnit) duk_get_number(ctx, 0)).CString());
    return 1;
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

    js_class_get_prototype(ctx, "Atomic", "Material");
    duk_push_c_function(ctx, Material_GetShaderParameters, 0);
    duk_put_prop_string(ctx, -2, "getShaderParameters");
    duk_pop(ctx);

    // static methods
    js_class_get_constructor(ctx, "Atomic", "Material");
    duk_push_c_function(ctx, Material_GetTextureUnitName, 1);
    duk_put_prop_string(ctx, -2, "getTextureUnitName");
    duk_pop(ctx);

}

}
