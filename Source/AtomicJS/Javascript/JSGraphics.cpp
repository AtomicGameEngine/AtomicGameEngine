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

#include "JSMath.h"
#include "JSGraphics.h"

#include <Atomic/Graphics/Material.h>
#include <Atomic/Graphics/Light.h>
#include <Atomic/Graphics/Octree.h>
#include <Atomic/Graphics/Camera.h>
#include <Atomic/Scene/Node.h>

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

static int Camera_GetScreenRay(duk_context* ctx)
{
    float x = (float) duk_to_number(ctx, 0);
    float y = (float) duk_to_number(ctx, 1);

    duk_push_this(ctx);
    Camera* camera = js_to_class_instance<Camera>(ctx, -1, 0);

    Ray ray = camera->GetScreenRay(x, y);
    duk_push_new_ray(ctx, ray);

    return 1;
}

// Octree Queries

static duk_int_t DUK_MAGIC_RAYCAST = 1;
static duk_int_t DUK_MAGIC_RAYCAST_SINGLE = 2;

static void duk_push_rayqueryresult(duk_context* ctx, const RayQueryResult& result)
{
    duk_push_object(ctx);

    duk_push_new_vector3(ctx, result.position_);
    duk_put_prop_string(ctx, -2, "position");

    duk_push_new_vector3(ctx, result.normal_);
    duk_put_prop_string(ctx, -2, "normal");

    duk_push_new_vector2(ctx, result.textureUV_);
    duk_put_prop_string(ctx, -2, "textureUV");

    duk_push_number(ctx, result.distance_);
    duk_put_prop_string(ctx, -2, "distance");

    js_push_class_object_instance(ctx, result.drawable_, "Drawable");
    duk_put_prop_string(ctx, -2, "drawable");

    js_push_class_object_instance(ctx, result.node_, "Node");
    duk_put_prop_string(ctx, -2, "node");

    duk_push_number(ctx, (duk_double_t) result.subObject_);
    duk_put_prop_string(ctx, -2, "subObject");

}

static int Octree_Raycast(duk_context* ctx)
{
    bool single = duk_get_current_magic(ctx) == DUK_MAGIC_RAYCAST_SINGLE;

    duk_idx_t nargs = duk_get_top(ctx);

    // require at least the ray
    if (nargs < 1)
    {
        duk_push_undefined(ctx);
        return 1;
    }

    Ray ray;
    if (!duk_get_ray(ctx, 0, ray))
    {
        duk_push_undefined(ctx);
        return 1;
    }

    RayQueryLevel level = RAY_TRIANGLE;
    if (nargs > 1)
    {
        unsigned _level = (unsigned) duk_to_number(ctx, 1);
        if (_level > (unsigned) RAY_TRIANGLE_UV)
        {
            duk_push_undefined(ctx);
            return 1;
        }

        level = (RayQueryLevel) _level;
    }

    float maxDistance = M_INFINITY;
    if (nargs > 2)
    {
        maxDistance = (float) duk_to_number(ctx, 2);
    }

    unsigned char drawableFlags = DRAWABLE_ANY;
    if (nargs > 3)
    {
        drawableFlags = (unsigned char) duk_to_number(ctx, 3);
    }

    unsigned viewMask = DEFAULT_VIEWMASK;
    if (nargs > 4)
    {
        viewMask = (unsigned) duk_to_number(ctx, 4);
    }

    duk_push_this(ctx);
    Octree* octree = js_to_class_instance<Octree>(ctx, -1, 0);

    PODVector<RayQueryResult> result;
    RayOctreeQuery query(result, ray, level, maxDistance, drawableFlags, viewMask);

    single ? octree->RaycastSingle(query) : octree->Raycast(query);

    // handle case of nothing hit
    if (!result.Size())
    {
        if (single)
            duk_push_null(ctx);
        else
            duk_push_array(ctx);

        return 1;
    }
    else
    {
        if (single)
        {
            duk_push_rayqueryresult(ctx, result[0]);
        }
        else
        {
            duk_push_array(ctx);

            for (unsigned i = 0; i < result.Size(); i++)
            {
                duk_push_rayqueryresult(ctx, result[i]);
                duk_put_prop_index(ctx, -2, i);
            }
        }
    }

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

    js_class_get_prototype(ctx, "Atomic", "Octree");
    duk_push_c_function(ctx, Octree_Raycast, DUK_VARARGS);
    duk_set_magic(ctx, -1, (unsigned) DUK_MAGIC_RAYCAST);
    duk_put_prop_string(ctx, -2, "rayCast");
    duk_push_c_function(ctx, Octree_Raycast, DUK_VARARGS);
    duk_set_magic(ctx, -1, (unsigned) DUK_MAGIC_RAYCAST_SINGLE);
    duk_put_prop_string(ctx, -2, "rayCastSingle");
    duk_pop(ctx);

    js_class_get_prototype(ctx, "Atomic", "Camera");
    duk_push_c_function(ctx, Camera_GetScreenRay, 2);
    duk_put_prop_string(ctx, -2, "getScreenRay");
    duk_pop(ctx);

    // static methods
    js_class_get_constructor(ctx, "Atomic", "Material");
    duk_push_c_function(ctx, Material_GetTextureUnitName, 1);
    duk_put_prop_string(ctx, -2, "getTextureUnitName");
    duk_pop(ctx);


}

}
