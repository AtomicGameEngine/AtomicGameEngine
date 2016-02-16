#include "JSVM.h"

#include <Atomic/Math/Ray.h>

namespace Atomic
{

bool duk_get_vector2(duk_context* ctx, duk_idx_t idx, Vector2& vec2)
{
    idx = duk_normalize_index(ctx, idx);

    // check that we are an array of length 3
    if (!duk_is_array(ctx, idx) || duk_get_length(ctx, idx) != 2)
        return false;

    // get the array values
    duk_get_prop_index(ctx, idx, 0);
    duk_get_prop_index(ctx, idx, 1);

    if (!duk_is_number(ctx, -2) || !duk_is_number(ctx, -1))
    {
        // pop off and return false
        duk_pop_2(ctx);
        return false;
    }

    vec2.x_ = (float) duk_to_number(ctx, -2);
    vec2.y_ = (float) duk_to_number(ctx, -1);

    duk_pop_2(ctx);

    return true;
}

bool duk_set_vector2(duk_context* ctx, duk_idx_t idx, const Vector2& vec2)
{
    idx = duk_normalize_index(ctx, idx);

    if (!duk_is_array(ctx, idx))
        return false;

    duk_push_number(ctx, vec2.x_);
    duk_put_prop_index(ctx, idx, 0);
    duk_push_number(ctx, vec2.y_);
    duk_put_prop_index(ctx, idx, 1);

    return true;
}


void duk_push_new_vector2(duk_context* ctx, const Vector2& vec2)
{
    duk_push_array(ctx);
    duk_set_vector2(ctx, -1, vec2);
}

bool duk_get_vector3(duk_context* ctx, duk_idx_t idx, Vector3& vec3)
{
    idx = duk_normalize_index(ctx, idx);

    // check that we are an array of length 3
    if (!duk_is_array(ctx, idx) || duk_get_length(ctx, idx) != 3)
        return false;

    // get the array values
    duk_get_prop_index(ctx, idx, 0);
    duk_get_prop_index(ctx, idx, 1);
    duk_get_prop_index(ctx, idx, 2);

    if (!duk_is_number(ctx, -3) || !duk_is_number(ctx, -2) || !duk_is_number(ctx, -1))
    {
        // pop off and return false
        duk_pop_3(ctx);
        return false;
    }

    vec3.x_ = (float) duk_to_number(ctx, -3);
    vec3.y_ = (float) duk_to_number(ctx, -2);
    vec3.z_ = (float) duk_to_number(ctx, -1);

    duk_pop_3(ctx);

    return true;
}

bool duk_set_vector3(duk_context* ctx, duk_idx_t idx, const Vector3& vec3)
{
    idx = duk_normalize_index(ctx, idx);

    if (!duk_is_array(ctx, idx))
        return false;

    duk_push_number(ctx, vec3.x_);
    duk_put_prop_index(ctx, idx, 0);
    duk_push_number(ctx, vec3.y_);
    duk_put_prop_index(ctx, idx, 1);
    duk_push_number(ctx, vec3.z_);
    duk_put_prop_index(ctx, idx, 2);

    return true;
}


void duk_push_new_vector3(duk_context* ctx, const Vector3& vec3)
{
    duk_push_array(ctx);
    duk_set_vector3(ctx, -1, vec3);
}

bool duk_get_ray(duk_context* ctx, duk_idx_t idx, Ray& ray)
{
    idx = duk_normalize_index(ctx, idx);

    // check that we are an object
    if (!duk_is_object(ctx, idx))
        return false;

    duk_get_prop_string(ctx, idx, "origin");
    duk_get_prop_string(ctx, idx, "direction");

    if (!duk_get_vector3(ctx, -2, ray.origin_) || !duk_get_vector3(ctx, -1, ray.direction_))
    {
        duk_pop_2(ctx);
        return false;
    }

    duk_pop_2(ctx);
    return true;
}

bool duk_set_ray(duk_context* ctx, duk_idx_t idx, const Ray& ray)
{
    idx = duk_normalize_index(ctx, idx);

    if (!duk_is_object(ctx, idx))
        return false;

    duk_push_new_vector3(ctx, ray.origin_);
    duk_put_prop_string(ctx, idx, "origin");

    duk_push_new_vector3(ctx, ray.direction_);
    duk_put_prop_string(ctx, idx, "direction");

    return true;
}


void duk_push_new_ray(duk_context* ctx, const Ray& ray)
{
    duk_push_object(ctx);
    duk_set_ray(ctx, -1, ray);
}

}
