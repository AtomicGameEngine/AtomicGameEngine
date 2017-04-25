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

#include <xmmintrin.h>
#include <pmmintrin.h>
#include <cmath>
#include <cfloat>

#include "Embree.h"

#include <Atomic/IO/Log.h>

#include "BakeMesh.h"
#include "EmbreeScene.h"

// TODO: Configurable for fast/final bakes?

/*
enum RTCSceneFlags
{
  // dynamic type flags
  RTC_SCENE_STATIC     = (0 << 0),    //!< specifies static scene
  RTC_SCENE_DYNAMIC    = (1 << 0),    //!< specifies dynamic scene

  // acceleration structure flags
  RTC_SCENE_COMPACT    = (1 << 8),    //!< use memory conservative data structures
  RTC_SCENE_COHERENT   = (1 << 9),    //!< optimize data structures for coherent rays
  RTC_SCENE_INCOHERENT = (1 << 10),    //!< optimize data structures for in-coherent rays (enabled by default)
  RTC_SCENE_HIGH_QUALITY = (1 << 11),  //!< create higher quality data structures

  // traversal algorithm flags
  RTC_SCENE_ROBUST     = (1 << 16)     //!< use more robust traversal algorithms
};

*/

namespace AtomicGlow
{

static void RTCErrorCallback(const RTCError code, const char* str)
{
    ATOMIC_LOGERRORF("RTC Error %d: %s", code, str);
}


EmbreeScene::EmbreeScene(Context* context) : Object(context),
    rtcDevice_(0),
    rtcScene_(0)
{

    // Intel says to do this, so we're doing it.
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

    // Create the embree device and scene.
    rtcDevice_ = rtcNewDevice(NULL);

    rtcDeviceSetErrorFunction(rtcDevice_, RTCErrorCallback);

    rtcScene_ = rtcDeviceNewScene(rtcDevice_, RTC_SCENE_STATIC, RTC_INTERSECT1);
}

EmbreeScene::~EmbreeScene()
{

}

void EmbreeScene::Commit()
{
    rtcCommit(rtcScene_);
}

}
