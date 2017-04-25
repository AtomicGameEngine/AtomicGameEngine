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

#include "default.h"

#define MAX_INTERNAL_STREAM_SIZE 64

namespace embree
{
  /* Ray structure for K rays */
  template<int K>
  struct RayK
  {
    /* Default construction does nothing */
    __forceinline RayK() {}

    /* Constructs a ray from origin, direction, and ray segment. Near
     * has to be smaller than far */
    __forceinline RayK(const Vec3<vfloat<K>>& org, const Vec3<vfloat<K>>& dir,
                       const vfloat<K>& tnear = zero, const vfloat<K>& tfar = inf,
                       const vfloat<K>& time = zero, const vint<K>& mask = -1)
      : org(org), dir(dir), tnear(tnear), tfar(tfar), time(time), mask(mask), geomID(-1), primID(-1), instID(-1) {}

    /* Returns the size of the ray */
    static __forceinline size_t size() { return K; }

    /* Tests if we hit something */
    __forceinline operator vbool<K>() const { return geomID != vint<K>(-1); }

    /* Calculates if this is a valid ray that does not cause issues during traversal */
    __forceinline vbool<K> valid() const
    {
      const vbool<K> vx = (abs(org.x) <= vfloat<K>(FLT_LARGE)) & (abs(dir.x) <= vfloat<K>(FLT_LARGE));
      const vbool<K> vy = (abs(org.y) <= vfloat<K>(FLT_LARGE)) & (abs(dir.y) <= vfloat<K>(FLT_LARGE));
      const vbool<K> vz = (abs(org.z) <= vfloat<K>(FLT_LARGE)) & (abs(dir.z) <= vfloat<K>(FLT_LARGE));
      const vbool<K> vn = abs(tnear) <= vfloat<K>(inf);
      const vbool<K> vf = abs(tfar) <= vfloat<K>(inf);
      return vx & vy & vz & vn & vf;
    }

    __forceinline void get(RayK<1>* ray) const;
    __forceinline void get(const size_t i, RayK<1>& ray) const;
    __forceinline void set(const RayK<1>* ray);
    __forceinline void set(const size_t i, const RayK<1>& ray);

    __forceinline void copy(const size_t dest, const size_t source);

    __forceinline void update(const vbool<K>& m_mask,
                              const vfloat<K>& new_t,
                              const vfloat<K>& new_u,
                              const vfloat<K>& new_v,
                              const vfloat<K>& new_gnormalx,
                              const vfloat<K>& new_gnormaly,
                              const vfloat<K>& new_gnormalz,
                              const vint<K>& new_geomID,
                              const vint<K>& new_primID)
    {
      vfloat<K>::store(m_mask,(float*)&tfar,new_t);
      vfloat<K>::store(m_mask,(float*)&u,new_u);
      vfloat<K>::store(m_mask,(float*)&v,new_v);
      vfloat<K>::store(m_mask,(float*)&Ng.x,new_gnormalx);
      vfloat<K>::store(m_mask,(float*)&Ng.y,new_gnormaly);
      vfloat<K>::store(m_mask,(float*)&Ng.z,new_gnormalz);
      vint<K>::store(m_mask,(int*)&geomID,new_geomID);
      vint<K>::store(m_mask,(int*)&primID,new_primID);
    }

    template<int M>
    __forceinline void updateK(const size_t i,
                               const size_t rayIndex,
                               const vfloat<M>& new_t,
                               const vfloat<M>& new_u,
                               const vfloat<M>& new_v,
                               const vfloat<M>& new_gnormalx,
                               const vfloat<M>& new_gnormaly,
                               const vfloat<M>& new_gnormalz,
                               const int new_geomID,
                               const vint<M> &new_primID)
    {
      u[rayIndex] = new_u[i];
      v[rayIndex] = new_v[i];
      tfar[rayIndex] = new_t[i];
      Ng.x[rayIndex] = new_gnormalx[i];
      Ng.y[rayIndex] = new_gnormaly[i];
      Ng.z[rayIndex] = new_gnormalz[i];
      geomID[rayIndex] = new_geomID;
      primID[rayIndex] = new_primID[i];
    }


    /* Ray data */
    Vec3<vfloat<K>> org; // ray origin
    Vec3<vfloat<K>> dir; // ray direction
    vfloat<K> tnear;     // start of ray segment
    vfloat<K> tfar;      // end of ray segment
    vfloat<K> time;      // time of this ray for motion blur.
    vint<K> mask;        // used to mask out objects during traversal

    /* Hit data */
    Vec3<vfloat<K>> Ng;  // geometry normal
    vfloat<K> u;         // barycentric u coordinate of hit
    vfloat<K> v;         // barycentric v coordinate of hit
    vint<K> geomID;      // geometry ID
    vint<K> primID;      // primitive ID
    vint<K> instID;      // instance ID
  };

#if defined(__AVX512F__)
  template<> template<>
    __forceinline void RayK<16>::updateK<16>(const size_t i,
                                             const size_t rayIndex,
                                             const vfloat16& new_t,
                                             const vfloat16& new_u,
                                             const vfloat16& new_v,
                                             const vfloat16& new_gnormalx,
                                             const vfloat16& new_gnormaly,
                                             const vfloat16& new_gnormalz,
                                             const int new_geomID,
                                             const vint16 &new_primID)
    {
      const vbool16 m_mask((unsigned int)1 << i);
      vfloat16::storeu_compact_single(m_mask,&tfar[rayIndex],new_t);
      vfloat16::storeu_compact_single(m_mask,&Ng.x[rayIndex],new_gnormalx);
      vfloat16::storeu_compact_single(m_mask,&Ng.y[rayIndex],new_gnormaly);
      vfloat16::storeu_compact_single(m_mask,&Ng.z[rayIndex],new_gnormalz);
      vfloat16::storeu_compact_single(m_mask,&u[rayIndex],new_u);
      vfloat16::storeu_compact_single(m_mask,&v[rayIndex],new_v);
      vint16::storeu_compact_single(m_mask,&primID[rayIndex],new_primID);
      geomID[rayIndex] = new_geomID;
    }
#endif


  /* Specialization for a single ray */
  template<>
  struct RayK<1>
  {
    /* Default construction does nothing */
    __forceinline RayK() {}

    /* Constructs a ray from origin, direction, and ray segment. Near
     *  has to be smaller than far */
    __forceinline RayK(const Vec3fa& org, const Vec3fa& dir, float tnear = zero, float tfar = inf, float time = zero, int mask = -1)
      : org(org), dir(dir), tnear(tnear), tfar(tfar), time(time), mask(mask), geomID(-1), primID(-1), instID(-1) {}

    /* Tests if we hit something */
    __forceinline operator bool() const { return geomID != RTC_INVALID_GEOMETRY_ID; }

    /* Calculates if this is a valid ray that does not cause issues during traversal */
    __forceinline bool valid() const {
      return all(le_mask(abs(org),Vec3fa(FLT_LARGE)) & le_mask(abs(dir),Vec3fa(FLT_LARGE))) && fabs(tnear) <= float(inf) && fabs(tfar) <= float(inf);
    }

    /* filter out all occluded rays from a stream of rays */
    __forceinline static void filterOutOccluded(RayK<1>** ray, size_t& N)
    {
      size_t l=0, r=N;
      while (l<r) {
        if (ray[l]->geomID != 0) l++;
        else std::swap(ray[l],ray[--r]); 
      }
      N = l;
    }
    
    /* Ray data */
    Vec3fa org;  // ray origin
    Vec3fa dir;  // ray direction
    float tnear; // start of ray segment
    float tfar;  // end of ray segment
    float time;  // time of this ray for motion blur.
    int mask;    // used to mask out objects during traversal

    /* Hit data */
    Vec3fa Ng;   // not normalized geometry normal
    float u;     // barycentric u coordinate of hit
    float v;     // barycentric v coordinate of hit
    unsigned geomID;  // geometry ID
    unsigned primID;  // primitive ID
    unsigned instID;  // instance ID

#if defined(__AVX512F__)
    __forceinline void update(const vbool16& m_mask,
                              const vfloat16& new_t,
                              const vfloat16& new_u,
                              const vfloat16& new_v,
                              const vfloat16& new_gnormalx,
                              const vfloat16& new_gnormaly,
                              const vfloat16& new_gnormalz,
			      const int new_geomID,
			      const int new_primID)
    {
      geomID = new_geomID;
      primID = new_primID;

      vfloat16::storeu_compact_single(m_mask,&tfar,new_t);
      vfloat16::storeu_compact_single(m_mask,&u,new_u); 
      vfloat16::storeu_compact_single(m_mask,&v,new_v); 
      vfloat16::storeu_compact_single(m_mask,&Ng.x,new_gnormalx); 
      vfloat16::storeu_compact_single(m_mask,&Ng.y,new_gnormaly); 
      vfloat16::storeu_compact_single(m_mask,&Ng.z,new_gnormalz);       
    }

    __forceinline void update(const vbool16& m_mask,
                              const vfloat16& new_t,
                              const vfloat16& new_u,
                              const vfloat16& new_v,
                              const vfloat16& new_gnormalx,
                              const vfloat16& new_gnormaly,
                              const vfloat16& new_gnormalz,
			      const vint16 &new_geomID,
			      const vint16 &new_primID)
    {
      vint16::storeu_compact_single(m_mask,&geomID,new_geomID);
      vint16::storeu_compact_single(m_mask,&primID,new_primID);
      vfloat16::storeu_compact_single(m_mask,&tfar,new_t);
      vfloat16::storeu_compact_single(m_mask,&u,new_u); 
      vfloat16::storeu_compact_single(m_mask,&v,new_v); 
      vfloat16::storeu_compact_single(m_mask,&Ng.x,new_gnormalx); 
      vfloat16::storeu_compact_single(m_mask,&Ng.y,new_gnormaly); 
      vfloat16::storeu_compact_single(m_mask,&Ng.z,new_gnormalz);       
    }

#endif
  };

  /* Converts ray packet to single rays */
  template<int K>
  __forceinline void RayK<K>::get(RayK<1>* ray) const
  {
    for (size_t i=0; i<K; i++) // FIXME: use SIMD transpose
    {
      ray[i].org.x = org.x[i]; ray[i].org.y = org.y[i]; ray[i].org.z = org.z[i];
      ray[i].dir.x = dir.x[i]; ray[i].dir.y = dir.y[i]; ray[i].dir.z = dir.z[i];
      ray[i].tnear = tnear[i]; ray[i].tfar  = tfar [i]; ray[i].time  = time[i]; ray[i].mask = mask[i];
      ray[i].Ng.x = Ng.x[i]; ray[i].Ng.y = Ng.y[i]; ray[i].Ng.z = Ng.z[i];
      ray[i].u = u[i]; ray[i].v = v[i];
      ray[i].geomID = geomID[i]; ray[i].primID = primID[i]; ray[i].instID = instID[i];
    }
  }

  /* Extracts a single ray out of a ray packet*/
  template<int K>
    __forceinline void RayK<K>::get(const size_t i, RayK<1>& ray) const
  {
    ray.org.x = org.x[i]; ray.org.y = org.y[i]; ray.org.z = org.z[i];
    ray.dir.x = dir.x[i]; ray.dir.y = dir.y[i]; ray.dir.z = dir.z[i];
    ray.tnear = tnear[i]; ray.tfar  = tfar [i]; ray.time  = time[i]; ray.mask = mask[i];
    ray.Ng.x = Ng.x[i]; ray.Ng.y = Ng.y[i]; ray.Ng.z = Ng.z[i];
    ray.u = u[i]; ray.v = v[i];
    ray.geomID = geomID[i]; ray.primID = primID[i]; ray.instID = instID[i];
  }

  /* Converts single rays to ray packet */
  template<int K>
  __forceinline void RayK<K>::set(const RayK<1>* ray)
  {
    for (size_t i=0; i<K; i++)
    {
      org.x[i] = ray[i].org.x; org.y[i] = ray[i].org.y; org.z[i] = ray[i].org.z;
      dir.x[i] = ray[i].dir.x; dir.y[i] = ray[i].dir.y; dir.z[i] = ray[i].dir.z;
      tnear[i] = ray[i].tnear; tfar [i] = ray[i].tfar;  time[i] = ray[i].time; mask[i] = ray[i].mask;
      Ng.x[i] = ray[i].Ng.x; Ng.y[i] = ray[i].Ng.y; Ng.z[i] = ray[i].Ng.z;
      u[i] = ray[i].u; v[i] = ray[i].v;
      geomID[i] = ray[i].geomID; primID[i] = ray[i].primID; instID[i] = ray[i].instID;
    }
  }

  /* inserts a single ray into a ray packet element */
  template<int K>
    __forceinline void RayK<K>::set(const size_t i, const RayK<1>& ray)
  {
    org.x[i] = ray.org.x; org.y[i] = ray.org.y; org.z[i] = ray.org.z;
    dir.x[i] = ray.dir.x; dir.y[i] = ray.dir.y; dir.z[i] = ray.dir.z;
    tnear[i] = ray.tnear; tfar [i] = ray.tfar;  time[i] = ray.time; mask[i] = ray.mask;
    Ng.x[i] = ray.Ng.x; Ng.y[i] = ray.Ng.y; Ng.z[i] = ray.Ng.z;
    u[i] = ray.u; v[i] = ray.v;
    geomID[i] = ray.geomID; primID[i] = ray.primID; instID[i] = ray.instID;
  }

  /* copies a ray packet element into another element*/
  template<int K>
    __forceinline void RayK<K>::copy(const size_t dest, const size_t source)
  {
    org.x[dest] = org.x[source]; org.y[dest] = org.y[source]; org.z[dest] = org.z[source];
    dir.x[dest] = dir.x[source]; dir.y[dest] = dir.y[source]; dir.z[dest] = dir.z[source];
    tnear[dest] = tnear[source]; tfar [dest] = tfar[source];  time[dest] = time[source]; mask[dest] = mask[source];
    Ng.x[dest] = Ng.x[source]; Ng.y[dest] = Ng.y[source]; Ng.z[dest] = Ng.z[source];
    u[dest] = u[source]; v[dest] = v[source];
    geomID[dest] = geomID[source]; primID[dest] = primID[source]; instID[dest] = instID[source];
  }

  /* Shortcuts */
  typedef RayK<1>  Ray;
  typedef RayK<4>  Ray4;
  typedef RayK<8>  Ray8;
  typedef RayK<16> Ray16;

  /* Outputs ray to stream */
  template<int K>
  inline std::ostream& operator<<(std::ostream& cout, const RayK<K>& ray)
  {
    return cout << "{ " << std::endl
                << "  org = " << ray.org << std::endl
                << "  dir = " << ray.dir << std::endl
                << "  near = " << ray.tnear << std::endl
                << "  far = " << ray.tfar << std::endl
                << "  time = " << ray.time << std::endl
                << "  mask = " << ray.mask << std::endl
                << "  instID = " << ray.instID << std::endl
                << "  geomID = " << ray.geomID << std::endl
                << "  primID = " << ray.primID <<  std::endl
                << "  u = " << ray.u <<  std::endl
                << "  v = " << ray.v << std::endl
                << "  Ng = " << ray.Ng
                << "}";
  }

  struct RayPacket
  {
  public:
    __forceinline RayPacket(void* ptr, size_t K)
      : ptr((char*)ptr), K(K) {}

  public:
    /* ray data access functions */
    __forceinline float* orgx(size_t offset) { return (float*) &ptr[0*4*K+offset]; }  //!< x coordinate of ray origin
    __forceinline float* orgy(size_t offset) { return (float*) &ptr[1*4*K+offset]; }  //!< y coordinate of ray origin
    __forceinline float* orgz(size_t offset) { return (float*) &ptr[2*4*K+offset]; };  //!< z coordinate of ray origin

    __forceinline float* dirx(size_t offset) { return (float*) &ptr[3*4*K+offset]; };  //!< x coordinate of ray direction
    __forceinline float* diry(size_t offset) { return (float*) &ptr[4*4*K+offset]; };  //!< y coordinate of ray direction
    __forceinline float* dirz(size_t offset) { return (float*) &ptr[5*4*K+offset]; };  //!< z coordinate of ray direction

    __forceinline float* tnear(size_t offset) { return (float*) &ptr[6*4*K+offset]; }; //!< Start of ray segment 
    __forceinline float* tfar (size_t offset) { return (float*) &ptr[7*4*K+offset]; }; //!< End of ray segment (set to hit distance)

    __forceinline float* time(size_t offset) { return (float*) &ptr[8*4*K+offset]; };  //!< Time of this ray for motion blur 
    __forceinline int*   mask(size_t offset) { return (int*)   &ptr[9*4*K+offset]; };  //!< Used to mask out objects during traversal (optional)

    /* hit data access functions */
    __forceinline float* Ngx(size_t offset) { return (float*) &ptr[10*4*K+offset]; };   //!< x coordinate of geometry normal
    __forceinline float* Ngy(size_t offset) { return (float*) &ptr[11*4*K+offset]; };   //!< y coordinate of geometry normal
    __forceinline float* Ngz(size_t offset) { return (float*) &ptr[12*4*K+offset]; };   //!< z coordinate of geometry normal

    __forceinline float* u(size_t offset) { return (float*) &ptr[13*4*K+offset]; };     //!< Barycentric u coordinate of hit
    __forceinline float* v(size_t offset) { return (float*) &ptr[14*4*K+offset]; };     //!< Barycentric v coordinate of hit
 
    __forceinline int* geomID(size_t offset) { return (int*) &ptr[15*4*K+offset]; };  //!< geometry ID
    __forceinline int* primID(size_t offset) { return (int*) &ptr[16*4*K+offset]; };  //!< primitive ID
    __forceinline int* instID(size_t offset) { return (int*) &ptr[17*4*K+offset]; };  //!< instance ID

  public:

    __forceinline void writeRay(const size_t i, int* valid, Ray& ray)
    {
      const size_t offset = 4*i;
      valid[i] = -1;
      orgx(offset)[0] = ray.org.x;
      orgy(offset)[0] = ray.org.y;
      orgz(offset)[0] = ray.org.z;
      dirx(offset)[0] = ray.dir.x;
      diry(offset)[0] = ray.dir.y;
      dirz(offset)[0] = ray.dir.z;
      tnear(offset)[0] = ray.tnear;
      tfar(offset)[0] = ray.tfar;
      time(offset)[0] = ray.time;
      mask(offset)[0] = ray.mask;
      instID(offset)[0] = ray.instID;
      geomID(offset)[0] = RTC_INVALID_GEOMETRY_ID;
    }

    __forceinline void readHit(const size_t i, Ray& ray)
    {
      const size_t offset = 4*i;
      const unsigned int geometryID = geomID(offset)[0];
      if (geometryID != RTC_INVALID_GEOMETRY_ID)
      {
        ray.tfar = tfar(offset)[0];
        ray.u = u(offset)[0];
        ray.v = v(offset)[0];
        ray.Ng.x = Ngx(offset)[0];
        ray.Ng.y = Ngy(offset)[0];
        ray.Ng.z = Ngz(offset)[0];
        ray.instID = instID(offset)[0];
        ray.geomID = geometryID;
        ray.primID = primID(offset)[0];
      }
    }

    __forceinline void readOcclusion(const size_t i, Ray& ray) {
      ray.geomID = geomID(4*i)[0];
    }

    __forceinline Ray gather(const size_t offset)
    {
      Ray ray;
      ray.org.x = orgx(offset)[0];
      ray.org.y = orgy(offset)[0];
      ray.org.z = orgz(offset)[0];
      ray.dir.x = dirx(offset)[0];
      ray.dir.y = diry(offset)[0];
      ray.dir.z = dirz(offset)[0];
      ray.tnear = tnear(offset)[0];
      ray.tfar  = tfar(offset)[0];
      ray.time  = time(offset)[0];
      ray.mask  = mask(offset)[0];
      ray.instID = instID(offset)[0];
      ray.geomID = RTC_INVALID_GEOMETRY_ID;
      return ray;
    }

    template<int K>
    __forceinline RayK<K> gather(const size_t offset)
    {
      RayK<K> ray;
      ray.org.x = vfloat<K>::loadu(orgx(offset));
      ray.org.y = vfloat<K>::loadu(orgy(offset));
      ray.org.z = vfloat<K>::loadu(orgz(offset));
      ray.dir.x = vfloat<K>::loadu(dirx(offset));
      ray.dir.y = vfloat<K>::loadu(diry(offset));
      ray.dir.z = vfloat<K>::loadu(dirz(offset));
      ray.tnear = vfloat<K>::loadu(tnear(offset));
      ray.tfar  = vfloat<K>::loadu(tfar(offset));
      ray.time  = vfloat<K>::loadu(time(offset));
      ray.mask  = vint<K>  ::loadu(mask(offset));
      ray.instID= vint<K>  ::loadu(instID(offset));
      ray.geomID = RTC_INVALID_GEOMETRY_ID;
      return ray;
    }

    __forceinline void scatter(const size_t offset, const Ray& ray, const bool all)
    {
      geomID(offset)[0] = ray.geomID;
      if (all && ray.geomID !=  RTC_INVALID_GEOMETRY_ID)
      {
        tfar(offset)[0] = ray.tfar;
        u(offset)[0] = ray.u;
        v(offset)[0] = ray.v;
        primID(offset)[0] = ray.primID;
        Ngx(offset)[0] = ray.Ng.x;
        Ngy(offset)[0] = ray.Ng.y;
        Ngz(offset)[0] = ray.Ng.z;
        instID(offset)[0] = ray.instID;
      }
    }

    template<int K>
    __forceinline void scatter(const vbool<K>& valid_i, const size_t offset, const RayK<K>& ray, const bool all)
    {
      vbool<K> valid = valid_i;
      vint<K>::storeu(valid,geomID(offset),ray.geomID);
      if (!all) return;

      valid &= ray.geomID !=  RTC_INVALID_GEOMETRY_ID;
      if (none(valid)) return;

      vfloat<K>::storeu(valid,tfar(offset),ray.tfar);
      vfloat<K>::storeu(valid,u(offset),ray.u);
      vfloat<K>::storeu(valid,v(offset),ray.v);
      vint<K>  ::storeu(valid,primID(offset),ray.primID);
      vfloat<K>::storeu(valid,Ngx(offset),ray.Ng.x);
      vfloat<K>::storeu(valid,Ngy(offset),ray.Ng.y);
      vfloat<K>::storeu(valid,Ngz(offset),ray.Ng.z);
      vint<K>  ::storeu(valid,instID(offset),ray.instID);
    }

    __forceinline size_t getOctant(const size_t offset)
    {
      const float dx = dirx(offset)[0];
      const float dy = diry(offset)[0];
      const float dz = dirz(offset)[0];
      const size_t octantID = (dx < 0.0f ? 1 : 0) + (dy < 0.0f ? 2 : 0) + (dz < 0.0f ? 4 : 0);
      return octantID;
    }

    __forceinline bool isValid(const size_t offset)
    {
      const float nnear = tnear(offset)[0];
      const float ffar  = tfar(offset)[0];
      return nnear <= ffar;
    }

  public:
    char* __restrict__ ptr;
    size_t K;
  };

  template<size_t MAX_K>
    struct StackRayPacket : public RayPacket
  {
  public:
    __forceinline StackRayPacket(size_t K)
      : RayPacket(data,K) { assert(K<=MAX_K); }
  public:
    char data[MAX_K/4*sizeof(Ray4)];
  };
  
  struct RayPN
  {
    /* ray data */
  public:

    float* __restrict__ orgx;  //!< x coordinate of ray origin
    float* __restrict__ orgy;  //!< y coordinate of ray origin
    float* __restrict__ orgz;  //!< z coordinate of ray origin

    float* __restrict__ dirx;  //!< x coordinate of ray direction
    float* __restrict__ diry;  //!< y coordinate of ray direction
    float* __restrict__ dirz;  //!< z coordinate of ray direction

    float* __restrict__ tnear; //!< Start of ray segment (optional)
    float* __restrict__ tfar;  //!< End of ray segment (set to hit distance)

 
    float* __restrict__ time;     //!< Time of this ray for motion blur (optional)
    unsigned* __restrict__ mask;  //!< Used to mask out objects during traversal (optional)

    /* hit data */

    float* __restrict__ Ngx;   //!< x coordinate of geometry normal (optional)
    float* __restrict__ Ngy;   //!<y coordinate of geometry normal (optional)
    float* __restrict__ Ngz;   //!< z coordinate of geometry normal (optional)

 

    float* __restrict__ u;     //!< Barycentric u coordinate of hit
    float* __restrict__ v;     //!< Barycentric v coordinate of hit

 
    unsigned* __restrict__ geomID;  //!< geometry ID
    unsigned* __restrict__ primID;  //!< primitive ID
    unsigned* __restrict__ instID;  //!< instance ID (optional)

    template<class T>
    __forceinline void init(T &t)
    {
      orgx   = (float*)&t.org.x;
      orgy   = (float*)&t.org.y;
      orgz   = (float*)&t.org.z;
      dirx   = (float*)&t.dir.x;
      diry   = (float*)&t.dir.y;
      dirz   = (float*)&t.dir.z;
      tnear  = (float*)&t.tnear;
      tfar   = (float*)&t.tfar;
      time   = (float*)&t.time;
      mask   = (unsigned *)&t.mask;
      Ngx    = (float*)&t.Ng.x;
      Ngy    = (float*)&t.Ng.y;
      Ngz    = (float*)&t.Ng.z;
      u      = (float*)&t.u;
      v      = (float*)&t.v;
      geomID = (unsigned *)&t.geomID;
      primID = (unsigned *)&t.primID;
      instID = (unsigned *)&t.instID;
    }

    __forceinline Ray gatherByOffset(const size_t offset)
    {
      Ray ray;
      ray.org.x = *(float* __restrict__ )((char*)orgx + offset);
      ray.org.y = *(float* __restrict__ )((char*)orgy + offset);
      ray.org.z = *(float* __restrict__ )((char*)orgz + offset);
      ray.dir.x = *(float* __restrict__ )((char*)dirx + offset);
      ray.dir.y = *(float* __restrict__ )((char*)diry + offset);
      ray.dir.z = *(float* __restrict__ )((char*)dirz + offset);
      ray.tfar  = *(float* __restrict__ )((char*)tfar + offset);
      ray.tnear = tnear ? *(float* __restrict__ )((char*)tnear + offset) : 0.0f;
      ray.time  = time  ? *(float* __restrict__ )((char*)time  + offset) : 0.0f;
      ray.mask  = mask  ? *(unsigned * __restrict__ )((char*)mask  + offset) : -1;
      ray.instID  = instID  ? *(unsigned * __restrict__ )((char*)instID  + offset) : -1;
      ray.geomID = RTC_INVALID_GEOMETRY_ID;
      return ray;
    }

    template<int K>
    __forceinline RayK<K> gather(const vbool<K>& valid, const size_t offset)
    {
      RayK<K> ray;
      ray.org.x = vfloat<K>::loadu(valid,(float* __restrict__ )((char*)orgx + offset));
      ray.org.y = vfloat<K>::loadu(valid,(float* __restrict__ )((char*)orgy + offset));
      ray.org.z = vfloat<K>::loadu(valid,(float* __restrict__ )((char*)orgz + offset));
      ray.dir.x = vfloat<K>::loadu(valid,(float* __restrict__ )((char*)dirx + offset));
      ray.dir.y = vfloat<K>::loadu(valid,(float* __restrict__ )((char*)diry + offset));
      ray.dir.z = vfloat<K>::loadu(valid,(float* __restrict__ )((char*)dirz + offset));
      ray.tfar  = vfloat<K>::loadu(valid,(float* __restrict__ )((char*)tfar + offset));
      ray.tnear = tnear ? vfloat<K>::loadu(valid,(float* __restrict__ )((char*)tnear + offset)) : 0.0f;
      ray.time  = time  ? vfloat<K>::loadu(valid,(float* __restrict__ )((char*)time  + offset)) : 0.0f;
      ray.mask  = mask  ? vint<K>::loadu(valid,(const void * __restrict__ )((char*)mask  + offset)) : -1;
      ray.instID = instID  ? vint<K>::loadu(valid,(const void * __restrict__ )((char*)instID  + offset)) : -1;
      ray.geomID = RTC_INVALID_GEOMETRY_ID;
      return ray;
    }

    __forceinline void scatterByOffset(const size_t offset, const Ray& ray, const bool all=true)
    {
      *(unsigned * __restrict__ )((char*)geomID + offset) = ray.geomID;
      if (all)
        if (ray.geomID !=  RTC_INVALID_GEOMETRY_ID)
        {
          *(float* __restrict__ )((char*)tfar + offset) = ray.tfar;
          *(float* __restrict__ )((char*)u + offset) = ray.u;
          *(float* __restrict__ )((char*)v + offset) = ray.v;
          *(unsigned * __restrict__ )((char*)primID + offset) = ray.primID;
          if (likely(Ngx)) *(float* __restrict__ )((char*)Ngx + offset) = ray.Ng.x;
          if (likely(Ngy)) *(float* __restrict__ )((char*)Ngy + offset) = ray.Ng.y;
          if (likely(Ngz)) *(float* __restrict__ )((char*)Ngz + offset) = ray.Ng.z;
          if (likely(instID)) *(unsigned * __restrict__ )((char*)instID + offset) = ray.instID;
        }
    }

    template<int K>
    __forceinline void scatter(const vbool<K>& valid_i, const size_t offset, const RayK<K>& ray, const bool all=true)
    {
      vbool<K> valid = valid_i;
      vint<K>::storeu(valid,(int * __restrict__ )((char*)geomID + offset), ray.geomID);
      if (!all) return;

      valid &= ray.geomID !=  RTC_INVALID_GEOMETRY_ID;
      if (none(valid)) return;
      
      vfloat<K>::storeu(valid,(float* __restrict__ )((char*)tfar + offset), ray.tfar);
      vfloat<K>::storeu(valid,(float* __restrict__ )((char*)u + offset), ray.u);
      vfloat<K>::storeu(valid,(float* __restrict__ )((char*)v + offset), ray.v);
      vint<K>::storeu(valid,(int * __restrict__ )((char*)primID + offset), ray.primID);
      if (likely(Ngx)) vfloat<K>::storeu(valid,(float* __restrict__ )((char*)Ngx + offset), ray.Ng.x);
      if (likely(Ngy)) vfloat<K>::storeu(valid,(float* __restrict__ )((char*)Ngy + offset), ray.Ng.y);
      if (likely(Ngz)) vfloat<K>::storeu(valid,(float* __restrict__ )((char*)Ngz + offset), ray.Ng.z);
      if (likely(instID)) vint<K>::storeu(valid,(int * __restrict__ )((char*)instID + offset), ray.instID);
    }

    __forceinline size_t getOctantByOffset(const size_t offset)
    {
      const float dx = *(float* __restrict__ )((char*)dirx + offset);
      const float dy = *(float* __restrict__ )((char*)diry + offset);
      const float dz = *(float* __restrict__ )((char*)dirz + offset);
      const size_t octantID = (dx < 0.0f ? 1 : 0) + (dy < 0.0f ? 2 : 0) + (dz < 0.0f ? 4 : 0);
      return octantID;
    }

    __forceinline bool isValidByOffset(const size_t offset)
    {
      const float nnear = tnear ? *(float* __restrict__ )((char*)tnear + offset) : 0.0f;
      const float ffar  = *(float* __restrict__ )((char*)tfar  + offset);
      return nnear <= ffar;
    }

  };
}
