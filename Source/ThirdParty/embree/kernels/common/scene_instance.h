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
  class InstanceFactory
  {
  public:
    InstanceFactory(int features);
    DEFINE_SYMBOL2(RTCBoundsFunc3,InstanceBoundsFunc);
    DEFINE_SYMBOL2(AccelSet::Intersector1,InstanceIntersector1);
    DEFINE_SYMBOL2(AccelSet::Intersector4,InstanceIntersector4);
    DEFINE_SYMBOL2(AccelSet::Intersector8,InstanceIntersector8);
    DEFINE_SYMBOL2(AccelSet::Intersector16,InstanceIntersector16);
    DEFINE_SYMBOL2(AccelSet::Intersector1M,InstanceIntersector1M);
  };

  /*! Instanced acceleration structure */
  struct Instance : public AccelSet
  {
    ALIGNED_STRUCT;
  public:
    static Instance* create (Scene* parent, Scene* object, size_t numTimeSteps) {
      return ::new (alignedMalloc(sizeof(Instance)+(numTimeSteps-1)*sizeof(AffineSpace3fa))) Instance(parent,object,numTimeSteps);
    }
  private:
    Instance (Scene* parent, Scene* object, size_t numTimeSteps); 
  public:
    virtual void setTransform(const AffineSpace3fa& local2world, size_t timeStep);
    virtual void setMask (unsigned mask);
    virtual void build() {}

  public:

    __forceinline AffineSpace3fa getWorld2Local() const {
      return world2local0;
    }

    __forceinline AffineSpace3fa getWorld2Local(float t) const 
    {
      float ftime;
      const size_t itime = getTimeSegment(t, fnumTimeSegments, ftime);
      return rcp(lerp(local2world[itime+0],local2world[itime+1],ftime));
    }

    template<int K>
      __forceinline AffineSpaceT<LinearSpace3<Vec3<vfloat<K>>>> getWorld2Local(const vbool<K>& valid, const vfloat<K>& t) const
    {
      typedef AffineSpaceT<LinearSpace3<Vec3<vfloat<K>>>> AffineSpace3vfK;
      
      vfloat<K> ftime;
      const vint<K> itime_k = getTimeSegment(t, vfloat<K>(fnumTimeSegments), ftime);
      assert(any(valid));
      const size_t index = __bsf(movemask(valid));
      const int itime = itime_k[index];
      const vfloat<K> t0 = vfloat<K>(1.0f)-ftime, t1 = ftime;
      if (likely(all(valid,itime_k == vint<K>(itime)))) {
        return rcp(t0*AffineSpace3vfK(local2world[itime+0])+t1*AffineSpace3vfK(local2world[itime+1]));
      } 
      else {
        AffineSpace3vfK space0,space1;
        foreach_unique(valid,itime_k,[&] (const vbool<K>& valid, int itime) {
            space0 = select(valid,AffineSpace3vfK(local2world[itime+0]),space0);
            space1 = select(valid,AffineSpace3vfK(local2world[itime+1]),space1);
          });
        return rcp(t0*space0+t1*space1);
      }
    }
    
  public:
    Scene* object;                 //!< pointer to instanced acceleration structure
    AffineSpace3fa world2local0;   //!< transformation from world space to local space for timestep 0
    AffineSpace3fa local2world[1]; //!< transformation from local space to world space for each timestep
  };
}
