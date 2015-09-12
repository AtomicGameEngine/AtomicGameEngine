//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
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

static int Light_SetShadowCascadeParameter(duk_context* ctx)
{
    int index = (int) duk_require_number(ctx, 0);
    float value = (float) duk_require_number(ctx, 1);

    duk_push_this(ctx);
    Light* light = js_to_class_instance<Light>(ctx, -1, 0);

    CascadeParameters parms = light->GetShadowCascade();

    switch (index)
    {
    case 0:
        parms.splits_[0] = value;
        break;
    case 1:
        parms.splits_[1] = value;
        break;
    case 2:
        parms.splits_[2] = value;
        break;
    case 3:
        parms.splits_[3] = value;
        break;
    case 4:
        parms.fadeStart_ = value;
        break;
    case 5:
        parms.biasAutoAdjust_ = value;
        break;
    }

    light->SetShadowCascade(parms);

    return 0;
}


static int Light_GetShadowCascade(duk_context* ctx)
{
    duk_push_this(ctx);
    Light* light = js_to_class_instance<Light>(ctx, -1, 0);

    const CascadeParameters& parms = light->GetShadowCascade();

    duk_push_array(ctx);
    duk_push_number(ctx, parms.splits_[0]);
    duk_put_prop_index(ctx, -2, 0);
    duk_push_number(ctx, parms.splits_[1]);
    duk_put_prop_index(ctx, -2, 1);
    duk_push_number(ctx, parms.splits_[2]);
    duk_put_prop_index(ctx, -2, 2);
    duk_push_number(ctx, parms.splits_[3]);
    duk_put_prop_index(ctx, -2, 3);
    duk_push_number(ctx, parms.fadeStart_);
    duk_put_prop_index(ctx, -2, 4);
    duk_push_number(ctx, parms.biasAutoAdjust_);
    duk_put_prop_index(ctx, -2, 5);

    return 1;
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

static int Material_SetShaderParameter(duk_context* ctx)
{
    duk_push_this(ctx);
    Material* material = js_to_class_instance<Material>(ctx, -1, 0);

    const char* name = duk_require_string(ctx, 0);
    String value = duk_require_string(ctx, 1);

    const Variant& v = material->GetShaderParameter(name);

    if (v == Variant::EMPTY)
        return 0;

    Variant vset;
    vset.FromString(v.GetType(), value);

    material->SetShaderParameter(name, vset);

    return 0;

}

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
    duk_push_string(ctx, Material::GetTextureUnitName((TextureUnit) ((int) duk_get_number(ctx, 0))).CString());
    return 1;
}


void jsapi_init_graphics(JSVM* vm)
{
    duk_context* ctx = vm->GetJSContext();

    js_class_get_prototype(ctx, "Atomic", "Light");
    duk_push_c_function(ctx, Light_SetShadowCascade, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "setShadowCascade");
    duk_push_c_function(ctx, Light_SetShadowCascadeParameter, 2);
    duk_put_prop_string(ctx, -2, "setShadowCascadeParameter");
    duk_push_c_function(ctx, Light_GetShadowCascade, 0);
    duk_put_prop_string(ctx, -2, "getShadowCascade");
    duk_push_c_function(ctx, Light_SetShadowBias, 2);
    duk_put_prop_string(ctx, -2, "setShadowBias");
    duk_pop(ctx);

    js_class_get_prototype(ctx, "Atomic", "Material");
    duk_push_c_function(ctx, Material_GetShaderParameters, 0);
    duk_put_prop_string(ctx, -2, "getShaderParameters");
    duk_push_c_function(ctx, Material_SetShaderParameter, 2);
    duk_put_prop_string(ctx, -2, "setShaderParameter");
    duk_pop(ctx);

    // static methods
    js_class_get_constructor(ctx, "Atomic", "Material");
    duk_push_c_function(ctx, Material_GetTextureUnitName, 1);
    duk_put_prop_string(ctx, -2, "getTextureUnitName");
    duk_pop(ctx);

}

}
