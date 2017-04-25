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

#include "accelset.h"

namespace embree
{
  /*! User geometry with user defined intersection functions */
  struct UserGeometry : public AccelSet
  {
  public:
    UserGeometry (Scene* parent, RTCGeometryFlags gflags, size_t items, size_t numTimeSteps); 
    virtual void setUserData (void* ptr);
    virtual void setMask (unsigned mask);
    virtual void setBoundsFunction (RTCBoundsFunc bounds);
    virtual void setBoundsFunction2 (RTCBoundsFunc2 bounds, void* userPtr);
    virtual void setBoundsFunction3 (RTCBoundsFunc3 bounds, void* userPtr);
    virtual void setIntersectFunction (RTCIntersectFunc intersect, bool ispc);
    virtual void setIntersectFunction4 (RTCIntersectFunc4 intersect4, bool ispc);
    virtual void setIntersectFunction8 (RTCIntersectFunc8 intersect8, bool ispc);
    virtual void setIntersectFunction16 (RTCIntersectFunc16 intersect16, bool ispc);
    virtual void setIntersectFunction1Mp (RTCIntersectFunc1Mp intersect);
    virtual void setIntersectFunctionN (RTCIntersectFuncN intersect);
    virtual void setOccludedFunction (RTCOccludedFunc occluded, bool ispc);
    virtual void setOccludedFunction4 (RTCOccludedFunc4 occluded4, bool ispc);
    virtual void setOccludedFunction8 (RTCOccludedFunc8 occluded8, bool ispc);
    virtual void setOccludedFunction16 (RTCOccludedFunc16 occluded16, bool ispc);
    virtual void setOccludedFunction1Mp (RTCOccludedFunc1Mp occluded);
    virtual void setOccludedFunctionN (RTCOccludedFuncN occluded);
    virtual void build() {}
  };
}
