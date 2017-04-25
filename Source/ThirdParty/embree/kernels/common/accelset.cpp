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

#include "accelset.h"
#include "scene.h"

namespace embree
{
  AccelSet::AccelSet (Scene* parent, RTCGeometryFlags gflags, size_t numItems, size_t numTimeSteps) 
    : Geometry(parent,Geometry::USER_GEOMETRY,numItems,numTimeSteps,gflags), boundsFunc(nullptr), boundsFunc2(nullptr), boundsFunc3(nullptr), boundsFuncUserPtr(nullptr)
  {
    intersectors.ptr = nullptr; 
    enabling();
  }

  void AccelSet::enabling () {
    if (numTimeSteps == 1) parent->world.numUserGeometries += numPrimitives;
    else                   parent->worldMB.numUserGeometries += numPrimitives;
  }
  
  void AccelSet::disabling() { 
    if (numTimeSteps == 1) parent->world.numUserGeometries -= numPrimitives;
    else                   parent->worldMB.numUserGeometries -= numPrimitives;
  }

  /* This is a workaround for some Clang 3.8.0-2ubuntu4 optimization
   * that creates issues with the ISA selection. This code essentially
   * is compiled with non-SSE2 ISA, but has to run on SSE2
   * machines. Clang 3.8.0-2ubuntu4 issues AVX-128 instuctions to
   * update intersect and occluded pointers at once. Only providing
   * these functions for SSE2 ISA fixes this issue. */
  
  AccelSet::Intersector1::Intersector1 (ErrorFunc error) 
    : intersect((IntersectFunc)error), occluded((OccludedFunc)error), name(nullptr) {}
  
  AccelSet::Intersector1::Intersector1 (IntersectFunc intersect, OccludedFunc occluded, const char* name)
    : intersect(intersect), occluded(occluded), name(name) {}
  
  AccelSet::Intersector4::Intersector4 (ErrorFunc error) 
    : intersect((void*)error), occluded((void*)error), name(nullptr), ispc(false) {}
  
  AccelSet::Intersector4::Intersector4 (void* intersect, void* occluded, const char* name, bool ispc)
    : intersect(intersect), occluded(occluded), name(name), ispc(ispc) {}
  
  AccelSet::Intersector8::Intersector8 (ErrorFunc error) 
    : intersect((void*)error), occluded((void*)error), name(nullptr), ispc(false) {}
  
  AccelSet::Intersector8::Intersector8 (void* intersect, void* occluded, const char* name, bool ispc)
    : intersect(intersect), occluded(occluded), name(name), ispc(ispc) {}
  
  AccelSet::Intersector16::Intersector16 (ErrorFunc error) 
    : intersect((void*)error), occluded((void*)error), name(nullptr), ispc(false) {}
  
  AccelSet::Intersector16::Intersector16 (void* intersect, void* occluded, const char* name, bool ispc)
    : intersect(intersect), occluded(occluded), name(name), ispc(ispc) {}

  AccelSet::Intersector1M::Intersector1M (ErrorFunc error) 
    : intersect((IntersectFunc1M)error), occluded((OccludedFunc1M)error), name(nullptr) {}
  
  AccelSet::Intersector1M::Intersector1M (IntersectFunc1M intersect, OccludedFunc1M occluded, const char* name)
    : intersect(intersect), occluded(occluded), name(name) {}
  
  AccelSet::IntersectorN::IntersectorN (ErrorFunc error) 
    : intersect((IntersectFuncN)error), occluded((OccludedFuncN)error), name(nullptr) {}
  
  AccelSet::IntersectorN::IntersectorN (IntersectFuncN intersect, OccludedFuncN occluded, const char* name)
    : intersect(intersect), occluded(occluded), name(name) {}
}
