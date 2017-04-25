
#pragma once

#include <Atomic/Math/Vector2.h>
#include <Atomic/Math/Vector3.h>

using namespace Atomic;

namespace AtomicGlow
{

/// A callback to sample the environment. Return false to terminate rasterization.
typedef bool (*RasterSamplingCallback)(void* param, int x, int y, const Vector3& barycentric,const Vector3& dx, const Vector3& dy, float coverage);

class Raster
{
public:

    // Process the given triangle. Returns false if rasterization was interrupted by the callback.
    static bool DrawTriangle(bool antialias, const Vector2& extents, bool enableScissors, const Vector2 v[3], RasterSamplingCallback cb, void* param);

    // Process the given quad. Returns false if rasterization was interrupted by the callback.
    static bool DrawQuad(bool antialias, const Vector2& extents, bool enableScissors, const Vector2 v[4], RasterSamplingCallback cb, void* param);

};


}
