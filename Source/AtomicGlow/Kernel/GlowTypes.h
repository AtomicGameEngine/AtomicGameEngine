// Copyright (c) 2014-2017, THUNDERBEAST GAMES LLC All rights reserved
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

#pragma once

#include <Atomic/Math/Vector3.h>

using namespace Atomic;

namespace AtomicGlow
{

const unsigned GLOW_MAX_BOUNCE_SAMPLE_TRIANGLES = 8;

enum GlowLightMode
{
    GLOW_LIGHTMODE_UNDEFINED,
    GLOW_LIGHTMODE_AMBIENT,
    GLOW_LIGHTMODE_DIRECT,
    GLOW_LIGHTMODE_INDIRECT,
    GLOW_LIGHTMODE_COMPLETE
};

struct BounceSample
{
    int triIndex_[GLOW_MAX_BOUNCE_SAMPLE_TRIANGLES];
    Vector3 position_;
    Vector3 radiance_;
    Vector3 srcColor_;
    int hits_;

    BounceSample()
    {
        Reset();
    }

    void Reset()
    {
        for (int i = 0; i < GLOW_MAX_BOUNCE_SAMPLE_TRIANGLES; i++)
        {
            triIndex_[i] = -1;
        }

        Vector3 v(-1, -1, -1);
        position_ = radiance_ = v;
        srcColor_ = Vector3::ZERO;
        hits_ = 0;
    }
};

}
