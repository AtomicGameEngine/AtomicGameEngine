// ======================================================================== //
// Copyright 2009-2017 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include "../common/sys/platform.h"
#include "../common/sys/sysinfo.h"
#include "../common/sys/thread.h"
#include "../common/sys/alloc.h"
#include "../common/sys/ref.h"
#include "../common/sys/intrinsics.h"
#include "../common/sys/atomic.h"
#include "../common/sys/mutex.h"
#include "../common/sys/vector.h"
#include "../common/sys/array.h"
#include "../common/sys/string.h"
#include "../common/sys/regression.h"

#include "../common/math/math.h"
#include "../common/simd/simd.h"
#include "../common/math/vec2.h"
#include "../common/math/vec3.h"
#include "../common/math/vec4.h"
#include "../common/math/bbox.h"
#include "../common/math/obbox.h"
#include "../common/math/lbbox.h"
#include "../common/math/linearspace2.h"
#include "../common/math/linearspace3.h"
#include "../common/math/affinespace.h"
#include "../common/math/range.h"
#include "../common/lexers/tokenstream.h"

#include "../common/tasking/taskscheduler.h"

#define COMMA ,

#include "config.h"
#include "isa.h"
#include "stat.h"
#include "profile.h"
#include "rtcore.h"
#include "vector.h"
#include "state.h"

#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <utility>

namespace embree
{
  ////////////////////////////////////////////////////////////////////////////////
  /// Vec2 shortcuts
  ////////////////////////////////////////////////////////////////////////////////

  typedef Vec2<vfloat4>  Vec2vf4;
  typedef Vec2<vdouble4> Vec2vd4;
  typedef Vec2<vreal4>   Vec2vr4;
  typedef Vec2<vint4>    Vec2vi4;
  typedef Vec2<vllong4>  Vec2vl4;
  typedef Vec2<vbool4>   Vec2vb4;
  typedef Vec2<vboolf4>  Vec2vbf4;
  typedef Vec2<vboold4>  Vec2vbd4;

  typedef Vec2<vfloat8>  Vec2vf8;
  typedef Vec2<vdouble8> Vec2vd8;
  typedef Vec2<vreal8>   Vec2vr8;
  typedef Vec2<vint8>    Vec2vi8;
  typedef Vec2<vllong8>  Vec2vl8;
  typedef Vec2<vbool8>   Vec2vb8;
  typedef Vec2<vboolf8>  Vec2vbf8;
  typedef Vec2<vboold8>  Vec2vbd8;

  typedef Vec2<vfloat16>  Vec2vf16;
  typedef Vec2<vdouble16> Vec2vd16;
  typedef Vec2<vreal16>   Vec2vr16;
  typedef Vec2<vint16>    Vec2vi16;
  typedef Vec2<vllong16>  Vec2vl16;
  typedef Vec2<vbool16>   Vec2vb16;
  typedef Vec2<vboolf16>  Vec2vbf16;
  typedef Vec2<vboold16>  Vec2vbd16;

  typedef Vec2<vfloatx>  Vec2vfx;
  typedef Vec2<vdoublex> Vec2vdx;
  typedef Vec2<vrealx>   Vec2vrx;
  typedef Vec2<vintx>    Vec2vix;
  typedef Vec2<vllongx>  Vec2vlx;
  typedef Vec2<vboolx>   Vec2vbx;
  typedef Vec2<vboolfx>  Vec2vbfx;
  typedef Vec2<vbooldx>  Vec2vbdx;

  ////////////////////////////////////////////////////////////////////////////////
  /// Vec3 shortcuts
  ////////////////////////////////////////////////////////////////////////////////

  typedef Vec3<vfloat4>  Vec3vf4;
  typedef Vec3<vdouble4> Vec3vd4;
  typedef Vec3<vreal4>   Vec3vr4;
  typedef Vec3<vint4>    Vec3vi4;
  typedef Vec3<vllong4>  Vec3vl4;
  typedef Vec3<vbool4>   Vec3vb4;
  typedef Vec3<vboolf4>  Vec3vbf4;
  typedef Vec3<vboold4>  Vec3vbd4;

  typedef Vec3<vfloat8>  Vec3vf8;
  typedef Vec3<vdouble8> Vec3vd8;
  typedef Vec3<vreal8>   Vec3vr8;
  typedef Vec3<vint8>    Vec3vi8;
  typedef Vec3<vllong8>  Vec3vl8;
  typedef Vec3<vbool8>   Vec3vb8;
  typedef Vec3<vboolf8>  Vec3vbf8;
  typedef Vec3<vboold8>  Vec3vbd8;

  typedef Vec3<vfloat16>  Vec3vf16;
  typedef Vec3<vdouble16> Vec3vd16;
  typedef Vec3<vreal16>   Vec3vr16;
  typedef Vec3<vint16>    Vec3vi16;
  typedef Vec3<vllong16>  Vec3vl16;
  typedef Vec3<vbool16>   Vec3vb16;
  typedef Vec3<vboolf16>  Vec3vbf16;
  typedef Vec3<vboold16>  Vec3vbd16;

  typedef Vec3<vfloatx>  Vec3vfx;
  typedef Vec3<vdoublex> Vec3vdx;
  typedef Vec3<vrealx>   Vec3vrx;
  typedef Vec3<vintx>    Vec3vix;
  typedef Vec3<vllongx>  Vec3vlx;
  typedef Vec3<vboolx>   Vec3vbx;
  typedef Vec3<vboolfx>  Vec3vbfx;
  typedef Vec3<vbooldx>  Vec3vbdx;

  ////////////////////////////////////////////////////////////////////////////////
  /// Vec4 shortcuts
  ////////////////////////////////////////////////////////////////////////////////

  typedef Vec4<vfloat4>  Vec4vf4;
  typedef Vec4<vdouble4> Vec4vd4;
  typedef Vec4<vreal4>   Vec4vr4;
  typedef Vec4<vint4>    Vec4vi4;
  typedef Vec4<vllong4>  Vec4vl4;
  typedef Vec4<vbool4>   Vec4vb4;
  typedef Vec4<vboolf4>  Vec4vbf4;
  typedef Vec4<vboold4>  Vec4vbd4;

  typedef Vec4<vfloat8>  Vec4vf8;
  typedef Vec4<vdouble8> Vec4vd8;
  typedef Vec4<vreal8>   Vec4vr8;
  typedef Vec4<vint8>    Vec4vi8;
  typedef Vec4<vllong8>  Vec4vl8;
  typedef Vec4<vbool8>   Vec4vb8;
  typedef Vec4<vboolf8>  Vec4vbf8;
  typedef Vec4<vboold8>  Vec4vbd8;

  typedef Vec4<vfloat16>  Vec4vf16;
  typedef Vec4<vdouble16> Vec4vd16;
  typedef Vec4<vreal16>   Vec4vr16;
  typedef Vec4<vint16>    Vec4vi16;
  typedef Vec4<vllong16>  Vec4vl16;
  typedef Vec4<vbool16>   Vec4vb16;
  typedef Vec4<vboolf16>  Vec4vbf16;
  typedef Vec4<vboold16>  Vec4vbd16;

  typedef Vec4<vfloatx>  Vec4vfx;
  typedef Vec4<vdoublex> Vec4vdx;
  typedef Vec4<vrealx>   Vec4vrx;
  typedef Vec4<vintx>    Vec4vix;
  typedef Vec4<vllongx>  Vec4vlx;
  typedef Vec4<vboolx>   Vec4vbx;
  typedef Vec4<vboolfx>  Vec4vbfx;
  typedef Vec4<vbooldx>  Vec4vbdx;

  ////////////////////////////////////////////////////////////////////////////////
  /// Other shortcuts
  ////////////////////////////////////////////////////////////////////////////////

  typedef LinearSpace3<Vec3vf4>  LinearSpace3vf4;
  typedef LinearSpace3<Vec3vf8>  LinearSpace3vf8;
  typedef LinearSpace3<Vec3vf16> LinearSpace3vf16;

  typedef AffineSpaceT<LinearSpace3<Vec3vf4>>  AffineSpace3vf4;
  typedef AffineSpaceT<LinearSpace3<Vec3vf8>>  AffineSpace3vf8;
  typedef AffineSpaceT<LinearSpace3<Vec3vf16>> AffineSpace3vf16;

  typedef BBox<Vec3vf4>  BBox3vf4;
  typedef BBox<Vec3vf8>  BBox3vf8;
  typedef BBox<Vec3vf16> BBox3vf16;
}
