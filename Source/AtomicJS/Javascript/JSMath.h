#pragma once

#include "JSVM.h"
#include <Atomic/Math/Ray.h>

namespace Atomic
{


bool duk_get_vector2(duk_context* ctx, duk_idx_t idx, Vector2& vec3);
bool duk_set_vector2(duk_context* ctx, duk_idx_t idx, const Vector2& vec3);
void duk_push_new_vector2(duk_context* ctx, const Vector2& vec3);

bool duk_get_vector3(duk_context* ctx, duk_idx_t idx, Vector3& vec3);
bool duk_set_vector3(duk_context* ctx, duk_idx_t idx, const Vector3& vec3);
void duk_push_new_vector3(duk_context* ctx, const Vector3& vec3);

bool duk_get_ray(duk_context* ctx, duk_idx_t idx, Ray& ray);
bool duk_set_ray(duk_context* ctx, duk_idx_t idx, const Ray& ray);
void duk_push_new_ray(duk_context* ctx, const Ray& ray);

}
