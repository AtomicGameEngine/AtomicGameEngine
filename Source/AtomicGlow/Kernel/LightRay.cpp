// Copyright (c) 2014-2017, THUNDERBEAST GAMES LLC All rights reserved
// Copyright 2009-2017 Intel Corporation
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

#include <xmmintrin.h>
#include <pmmintrin.h>
#include <cmath>
#include <cfloat>

#include "LightRay.h"

namespace AtomicGlow
{

LightRay::LightRay()
{

}

LightRay::~LightRay()
{

}

void LightRay::ClearHit()
{
    rtcRay_.geomID = RTC_INVALID_GEOMETRY_ID;
    rtcRay_.primID = RTC_INVALID_GEOMETRY_ID;
    rtcRay_.instID = RTC_INVALID_GEOMETRY_ID;
    rtcRay_.u = 0.0f;
    rtcRay_.v = 0.0f;
    rtcRay_.Ng[0] = rtcRay_.Ng[1] = rtcRay_.Ng[2] = 0.0f;
}


void LightRay::SetupRay(const Vector3& origin, const Vector3& dir, float tNear, float tFar)
{
    rtcRay_.org[0] = origin.x_;
    rtcRay_.org[1] = origin.y_;
    rtcRay_.org[2] = origin.z_;

    rtcRay_.dir[0] = dir.x_;
    rtcRay_.dir[1] = dir.y_;
    rtcRay_.dir[2] = dir.z_;

    rtcRay_.tnear = tNear;
    rtcRay_.tfar = tFar;

    rtcRay_.mask = 0xFFFFFFFF;
    rtcRay_.time = 0.0f;

    ClearHit();
}

}
