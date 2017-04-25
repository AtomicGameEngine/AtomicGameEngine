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

#include "../common/default.h"
#include "../common/scene.h"
#include "../common/simd/simd.h"
#include "../common/primref.h"

namespace embree
{
  struct PrimitiveType
  {
    /*! constructs the primitive type */
    PrimitiveType (const std::string& name, size_t bytes, size_t blockSize) 
    : name(name), bytes(bytes), blockSize(blockSize) {} 

    /*! Returns the number of stored primitives in a block. */
    virtual size_t size(const char* This) const = 0;

  public:
    std::string name;       //!< name of this primitive type
    size_t bytes;           //!< number of bytes of the triangle data
    size_t blockSize;       //!< block size
  };

  class RayPrecalculations
  {
  public:
    __forceinline RayPrecalculations() {}
    __forceinline RayPrecalculations(const Ray& ray, const void* ptr, unsigned numTimeSteps) {}

    __forceinline int itime() const { return 0; }
    __forceinline float ftime() const { return 0.0f; }

    __forceinline unsigned numTimeSteps() const { return 1; }
  };

  class RayPrecalculationsMB
  {
  public:
    __forceinline RayPrecalculationsMB() {}

    __forceinline RayPrecalculationsMB(const Ray& ray, const void* ptr, unsigned numTimeSteps)
    {
      itime_ = getTimeSegment(ray.time, float(int(numTimeSteps-1)), ftime_);
      numTimeSteps_ = numTimeSteps;
    }

    __forceinline int itime() const { return itime_; }
    __forceinline float ftime() const { return ftime_; }

    __forceinline unsigned numTimeSteps() const { return numTimeSteps_; }

  private:
    /* used for msmblur implementation */
    int itime_;
    float ftime_;
    int numTimeSteps_;
  };

  template<int K>
  class RayKPrecalculations
  {
  public:
    __forceinline RayKPrecalculations() {}
    __forceinline RayKPrecalculations(const vbool<K>& valid, const RayK<K>& ray, unsigned numTimeSteps) {}

    __forceinline vint<K> itime() const { return zero; }
    __forceinline vfloat<K> ftime() const { return zero; }

    __forceinline int itime(size_t k) const { return 0; }
    __forceinline float ftime(size_t k) const { return 0.0f; }

    __forceinline unsigned numTimeSteps() const { return 1; }
  };

  template<int K>
  class RayKPrecalculationsMB
  {
  public:
    __forceinline RayKPrecalculationsMB() {}
    __forceinline RayKPrecalculationsMB(const vbool<K>& valid, const RayK<K>& ray, unsigned numTimeSteps)
    {
      itime_ = getTimeSegment(ray.time, vfloat<K>(float(int(numTimeSteps-1))), ftime_);
      numTimeSteps_ = numTimeSteps;
    }

    __forceinline vint<K> itime() const { return itime_; }
    __forceinline vfloat<K> ftime() const { return ftime_; }

    __forceinline int itime(size_t k) const { return itime_[k]; }
    __forceinline float ftime(size_t k) const { return ftime_[k]; }

    __forceinline unsigned numTimeSteps() const { return numTimeSteps_; }

  private:
    /* used for msmblur implementation */
    vint<K> itime_;
    vfloat<K> ftime_;
    unsigned numTimeSteps_;
  };

  template<typename Precalculations>
  struct Intersector1Precalculations : public RayPrecalculations, public Precalculations
  {
    __forceinline Intersector1Precalculations() {}

    __forceinline Intersector1Precalculations(const Ray& ray, const void* ptr, unsigned numTimeSteps)
      : RayPrecalculations(ray, ptr, numTimeSteps), Precalculations(ray, ptr) {}
  };

  template<typename Precalculations>
  struct Intersector1PrecalculationsMB : public RayPrecalculationsMB, public Precalculations
  {
    __forceinline Intersector1PrecalculationsMB() {}

    __forceinline Intersector1PrecalculationsMB(const Ray& ray, const void* ptr, unsigned numTimeSteps)
      : RayPrecalculationsMB(ray, ptr, numTimeSteps), Precalculations(ray, ptr) {}
  };

  template<int K, typename Precalculations>
  struct IntersectorKPrecalculations : public RayKPrecalculations<K>, public Precalculations
  {
    __forceinline IntersectorKPrecalculations() {}

    __forceinline IntersectorKPrecalculations(const vbool<K>& valid, const RayK<K>& ray, unsigned numTimeSteps)
      : RayKPrecalculations<K>(valid, ray, numTimeSteps), Precalculations(valid, ray) {}
  };

  template<int K, typename Precalculations>
  struct IntersectorKPrecalculationsMB : public RayKPrecalculationsMB<K>, public Precalculations
  {
    __forceinline IntersectorKPrecalculationsMB() {}

    __forceinline IntersectorKPrecalculationsMB(const vbool<K>& valid, const RayK<K>& ray, unsigned numTimeSteps)
      : RayKPrecalculationsMB<K>(valid, ray, numTimeSteps), Precalculations(valid, ray) {}
  };
}
