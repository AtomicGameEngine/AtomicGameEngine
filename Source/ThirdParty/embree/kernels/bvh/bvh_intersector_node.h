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

#include "bvh.h"

namespace embree
{
  namespace isa
  {
    template<int N, int Nx>
      struct TravRay 
    {
      __forceinline TravRay () {}

      __forceinline TravRay(const Vec3fa& ray_org, const Vec3fa& ray_dir) 
        : org_xyz(ray_org), dir_xyz(ray_dir) 
      {
        const Vec3fa ray_rdir = rcp_safe(ray_dir);
        org = Vec3<vfloat<N>>(ray_org.x,ray_org.y,ray_org.z);
        dir = Vec3<vfloat<N>>(ray_dir.x,ray_dir.y,ray_dir.z);
        rdir = Vec3<vfloat<N>>(ray_rdir.x,ray_rdir.y,ray_rdir.z);
#if defined(__AVX2__)
        const Vec3fa ray_org_rdir = ray_org*ray_rdir;
        org_rdir = Vec3<vfloat<N>>(ray_org_rdir.x,ray_org_rdir.y,ray_org_rdir.z);
#endif
        nearX = ray_rdir.x >= 0.0f ? 0*sizeof(vfloat<N>) : 1*sizeof(vfloat<N>);
        nearY = ray_rdir.y >= 0.0f ? 2*sizeof(vfloat<N>) : 3*sizeof(vfloat<N>);
        nearZ = ray_rdir.z >= 0.0f ? 4*sizeof(vfloat<N>) : 5*sizeof(vfloat<N>);
        farX  = nearX ^ sizeof(vfloat<N>);
        farY  = nearY ^ sizeof(vfloat<N>);
        farZ  = nearZ ^ sizeof(vfloat<N>);
      }

      template<int K>
      __forceinline TravRay (size_t k, const Vec3<vfloat<K>> &ray_org, const Vec3<vfloat<K>> &ray_dir, 
                             const Vec3<vfloat<K>> &ray_rdir, const Vec3<vint<K>>& nearXYZ, const size_t flip = sizeof(vfloat<N>))
      {
        org = Vec3<vfloat<N>>(ray_org.x[k], ray_org.y[k], ray_org.z[k]);
	dir = Vec3<vfloat<N>>(ray_dir.x[k], ray_dir.y[k], ray_dir.z[k]);
	rdir = Vec3<vfloat<N>>(ray_rdir.x[k], ray_rdir.y[k], ray_rdir.z[k]);
#if defined(__AVX2__)
	org_rdir = org*rdir;
#endif
	nearX = nearXYZ.x[k];
	nearY = nearXYZ.y[k];
	nearZ = nearXYZ.z[k];
        farX  = nearX ^ flip;
        farY  = nearY ^ flip;
        farZ  = nearZ ^ flip;
      }

      __forceinline TravRay (const TravRay& ray)
      {
        org_xyz = ray.org_xyz;
        dir_xyz = ray.dir_xyz;
        org = ray.org;
        dir = ray.dir;
        rdir = ray.rdir;
#if defined(__AVX2__)
        org_rdir = ray.org_rdir;
#endif
        nearX = ray.nearX;
        nearY = ray.nearY;
        nearZ = ray.nearZ;
        farX = ray.farX;
        farY = ray.farY;
        farZ = ray.farZ;
      }

      Vec3fa org_xyz, dir_xyz;
      Vec3<vfloat<Nx>> org, dir, rdir;
#if defined(__AVX2__)
      Vec3<vfloat<Nx>> org_rdir;
#endif
      size_t nearX, nearY, nearZ;
      size_t farX, farY, farZ;
    };

    //////////////////////////////////////////////////////////////////////////////////////
    // fast ray/BVHN::AlignedNode intersection
    //////////////////////////////////////////////////////////////////////////////////////

    template<int N, int Nx>
      __forceinline size_t intersectNode(const typename BVHN<N>::AlignedNode* node, const TravRay<N,Nx>& ray, 
                                         const vfloat<Nx>& tnear, const vfloat<Nx>& tfar, vfloat<Nx>& dist);

    template<>
      __forceinline size_t intersectNode<4,4>(const typename BVH4::AlignedNode* node, const TravRay<4,4>& ray, 
                                              const vfloat4& tnear, const vfloat4& tfar, vfloat4& dist)
    {
#if defined(__AVX2__)
      const vfloat4 tNearX = msub(vfloat4::load((float*)((const char*)&node->lower_x+ray.nearX)), ray.rdir.x, ray.org_rdir.x);
      const vfloat4 tNearY = msub(vfloat4::load((float*)((const char*)&node->lower_x+ray.nearY)), ray.rdir.y, ray.org_rdir.y);
      const vfloat4 tNearZ = msub(vfloat4::load((float*)((const char*)&node->lower_x+ray.nearZ)), ray.rdir.z, ray.org_rdir.z);
      const vfloat4 tFarX  = msub(vfloat4::load((float*)((const char*)&node->lower_x+ray.farX )), ray.rdir.x, ray.org_rdir.x);
      const vfloat4 tFarY  = msub(vfloat4::load((float*)((const char*)&node->lower_x+ray.farY )), ray.rdir.y, ray.org_rdir.y);
      const vfloat4 tFarZ  = msub(vfloat4::load((float*)((const char*)&node->lower_x+ray.farZ )), ray.rdir.z, ray.org_rdir.z);
#else
      const vfloat4 tNearX = (vfloat4::load((float*)((const char*)&node->lower_x+ray.nearX)) - ray.org.x) * ray.rdir.x;
      const vfloat4 tNearY = (vfloat4::load((float*)((const char*)&node->lower_x+ray.nearY)) - ray.org.y) * ray.rdir.y;
      const vfloat4 tNearZ = (vfloat4::load((float*)((const char*)&node->lower_x+ray.nearZ)) - ray.org.z) * ray.rdir.z;
      const vfloat4 tFarX  = (vfloat4::load((float*)((const char*)&node->lower_x+ray.farX )) - ray.org.x) * ray.rdir.x;
      const vfloat4 tFarY  = (vfloat4::load((float*)((const char*)&node->lower_x+ray.farY )) - ray.org.y) * ray.rdir.y;
      const vfloat4 tFarZ  = (vfloat4::load((float*)((const char*)&node->lower_x+ray.farZ )) - ray.org.z) * ray.rdir.z;
#endif
      
#if defined(__SSE4_1__) && !defined(__AVX512F__) // up to HSW
      const vfloat4 tNear = maxi(tNearX,tNearY,tNearZ,tnear);
      const vfloat4 tFar  = mini(tFarX ,tFarY ,tFarZ ,tfar);
      const vbool4 vmask = asInt(tNear) > asInt(tFar);
      const size_t mask = movemask(vmask) ^ ((1<<4)-1);
#elif defined(__AVX512F__) && !defined(__AVX512ER__) // SKX
      const vfloat4 tNear = maxi(tNearX,tNearY,tNearZ,tnear);
      const vfloat4 tFar  = mini(tFarX ,tFarY ,tFarZ ,tfar);
      const vbool4 vmask = asInt(tNear) <= asInt(tFar);
      const size_t mask = movemask(vmask);
#else
      const vfloat4 tNear = max(tNearX,tNearY,tNearZ,tnear);
      const vfloat4 tFar  = min(tFarX ,tFarY ,tFarZ ,tfar);
      const vbool4 vmask = tNear <= tFar;
      const size_t mask = movemask(vmask);
#endif
      dist = tNear;
      return mask;
    }

#if defined(__AVX__)

    template<>
      __forceinline size_t intersectNode<8,8>(const typename BVH8::AlignedNode* node, const TravRay<8,8>& ray, 
                                              const vfloat8& tnear, const vfloat8& tfar, vfloat8& dist)
    {
#if defined(__AVX2__)
      const vfloat8 tNearX = msub(vfloat8::load((float*)((const char*)&node->lower_x+ray.nearX)), ray.rdir.x, ray.org_rdir.x);
      const vfloat8 tNearY = msub(vfloat8::load((float*)((const char*)&node->lower_x+ray.nearY)), ray.rdir.y, ray.org_rdir.y);
      const vfloat8 tNearZ = msub(vfloat8::load((float*)((const char*)&node->lower_x+ray.nearZ)), ray.rdir.z, ray.org_rdir.z);
      const vfloat8 tFarX  = msub(vfloat8::load((float*)((const char*)&node->lower_x+ray.farX )), ray.rdir.x, ray.org_rdir.x);
      const vfloat8 tFarY  = msub(vfloat8::load((float*)((const char*)&node->lower_x+ray.farY )), ray.rdir.y, ray.org_rdir.y);
      const vfloat8 tFarZ  = msub(vfloat8::load((float*)((const char*)&node->lower_x+ray.farZ )), ray.rdir.z, ray.org_rdir.z);
#else
      const vfloat8 tNearX = (vfloat8::load((float*)((const char*)&node->lower_x+ray.nearX)) - ray.org.x) * ray.rdir.x;
      const vfloat8 tNearY = (vfloat8::load((float*)((const char*)&node->lower_x+ray.nearY)) - ray.org.y) * ray.rdir.y;
      const vfloat8 tNearZ = (vfloat8::load((float*)((const char*)&node->lower_x+ray.nearZ)) - ray.org.z) * ray.rdir.z;
      const vfloat8 tFarX  = (vfloat8::load((float*)((const char*)&node->lower_x+ray.farX )) - ray.org.x) * ray.rdir.x;
      const vfloat8 tFarY  = (vfloat8::load((float*)((const char*)&node->lower_x+ray.farY )) - ray.org.y) * ray.rdir.y;
      const vfloat8 tFarZ  = (vfloat8::load((float*)((const char*)&node->lower_x+ray.farZ )) - ray.org.z) * ray.rdir.z;
#endif
      
#if defined(__AVX2__) && !defined(__AVX512F__) // HSW
      const vfloat8 tNear = maxi(tNearX,tNearY,tNearZ,tnear);
      const vfloat8 tFar  = mini(tFarX ,tFarY ,tFarZ ,tfar);
      const vbool8 vmask = asInt(tNear) > asInt(tFar);
      const size_t mask = movemask(vmask) ^ ((1<<8)-1);
#elif defined(__AVX512F__) && !defined(__AVX512ER__) // SKX
      const vfloat8 tNear = maxi(tNearX,tNearY,tNearZ,tnear);
      const vfloat8 tFar  = mini(tFarX ,tFarY ,tFarZ ,tfar);
      const vbool8 vmask = asInt(tNear) <= asInt(tFar);
      const size_t mask = movemask(vmask);
#else
      const vfloat8 tNear = max(tNearX,tNearY,tNearZ,tnear);
      const vfloat8 tFar  = min(tFarX ,tFarY ,tFarZ ,tfar);
      const vbool8 vmask = tNear <= tFar;
      const size_t mask = movemask(vmask);
#endif
      dist = tNear;
      return mask;
    }

#endif

#if defined(__AVX512F__)

    template<>
      __forceinline size_t intersectNode<4,16>(const typename BVH4::AlignedNode* node, const TravRay<4,16>& ray, const vfloat16& tnear, const vfloat16& tfar, vfloat16& dist)
    {
      const vfloat16 tNearX = msub(vfloat16(*(vfloat4*)((const char*)&node->lower_x+ray.nearX)), ray.rdir.x, ray.org_rdir.x);
      const vfloat16 tNearY = msub(vfloat16(*(vfloat4*)((const char*)&node->lower_x+ray.nearY)), ray.rdir.y, ray.org_rdir.y);
      const vfloat16 tNearZ = msub(vfloat16(*(vfloat4*)((const char*)&node->lower_x+ray.nearZ)), ray.rdir.z, ray.org_rdir.z);
      const vfloat16 tFarX  = msub(vfloat16(*(vfloat4*)((const char*)&node->lower_x+ray.farX )), ray.rdir.x, ray.org_rdir.x);
      const vfloat16 tFarY  = msub(vfloat16(*(vfloat4*)((const char*)&node->lower_x+ray.farY )), ray.rdir.y, ray.org_rdir.y);
      const vfloat16 tFarZ  = msub(vfloat16(*(vfloat4*)((const char*)&node->lower_x+ray.farZ )), ray.rdir.z, ray.org_rdir.z);      
      const vfloat16 tNear  = max(tNearX,tNearY,tNearZ,tnear);
      const vfloat16 tFar   = min(tFarX ,tFarY ,tFarZ ,tfar);
      const vbool16 vmask   = le(vbool16(0xf),tNear,tFar);
      const size_t mask     = movemask(vmask);
      dist = tNear;
      return mask;
    }

    template<>
      __forceinline size_t intersectNode<8,16>(const typename BVH8::AlignedNode* node, const TravRay<8,16>& ray, const vfloat16& tnear, const vfloat16& tfar, vfloat16& dist)
    {
      const vfloat16 tNearX = msub(vfloat16(*(vfloat8*)((const char*)&node->lower_x+ray.nearX)), ray.rdir.x, ray.org_rdir.x);
      const vfloat16 tNearY = msub(vfloat16(*(vfloat8*)((const char*)&node->lower_x+ray.nearY)), ray.rdir.y, ray.org_rdir.y);
      const vfloat16 tNearZ = msub(vfloat16(*(vfloat8*)((const char*)&node->lower_x+ray.nearZ)), ray.rdir.z, ray.org_rdir.z);
      const vfloat16 tFarX  = msub(vfloat16(*(vfloat8*)((const char*)&node->lower_x+ray.farX )), ray.rdir.x, ray.org_rdir.x);
      const vfloat16 tFarY  = msub(vfloat16(*(vfloat8*)((const char*)&node->lower_x+ray.farY )), ray.rdir.y, ray.org_rdir.y);
      const vfloat16 tFarZ  = msub(vfloat16(*(vfloat8*)((const char*)&node->lower_x+ray.farZ )), ray.rdir.z, ray.org_rdir.z);      
      const vfloat16 tNear  = max(tNearX,tNearY,tNearZ,tnear);
      const vfloat16 tFar   = min(tFarX ,tFarY ,tFarZ ,tfar);
      const vbool16 vmask   = le(vbool16(0xff),tNear,tFar);
      const size_t mask     = movemask(vmask);
      dist = tNear;
      return mask;
    }
    
#endif

    template<int N, int K>
      __forceinline vbool<K> intersectNode(const typename BVHN<N>::AlignedNode* node, size_t i, 
                                           const Vec3<vfloat<K>>& org, const Vec3<vfloat<K>>& dir, const Vec3<vfloat<K>>& rdir, const Vec3<vfloat<K>>& org_rdir,
                                           const vfloat<K>& tnear, const vfloat<K>& tfar, vfloat<K>& dist)
 
    {
#if defined(__AVX2__)
      const vfloat<K> lclipMinX = msub(node->lower_x[i],rdir.x,org_rdir.x);
      const vfloat<K> lclipMinY = msub(node->lower_y[i],rdir.y,org_rdir.y);
      const vfloat<K> lclipMinZ = msub(node->lower_z[i],rdir.z,org_rdir.z);
      const vfloat<K> lclipMaxX = msub(node->upper_x[i],rdir.x,org_rdir.x);
      const vfloat<K> lclipMaxY = msub(node->upper_y[i],rdir.y,org_rdir.y);
      const vfloat<K> lclipMaxZ = msub(node->upper_z[i],rdir.z,org_rdir.z);
#else
      const vfloat<K> lclipMinX = (node->lower_x[i] - org.x) * rdir.x;
      const vfloat<K> lclipMinY = (node->lower_y[i] - org.y) * rdir.y;
      const vfloat<K> lclipMinZ = (node->lower_z[i] - org.z) * rdir.z;
      const vfloat<K> lclipMaxX = (node->upper_x[i] - org.x) * rdir.x;
      const vfloat<K> lclipMaxY = (node->upper_y[i] - org.y) * rdir.y;
      const vfloat<K> lclipMaxZ = (node->upper_z[i] - org.z) * rdir.z;
#endif  

#if defined(__AVX512F__) && !defined(__AVX512ER__) // SKX
      if (K == 16)
      {
        /* use mixed float/int min/max */
        const vfloat<K> lnearP = maxi(min(lclipMinX, lclipMaxX), min(lclipMinY, lclipMaxY), min(lclipMinZ, lclipMaxZ));
        const vfloat<K> lfarP  = mini(max(lclipMinX, lclipMaxX), max(lclipMinY, lclipMaxY), max(lclipMinZ, lclipMaxZ));
        const vbool<K> lhit    = asInt(maxi(lnearP, tnear)) <= asInt(mini(lfarP, tfar));
        dist = lnearP;
        return lhit;
      }
      else
#endif
      {
        const vfloat<K> lnearP = maxi(mini(lclipMinX, lclipMaxX), mini(lclipMinY, lclipMaxY), mini(lclipMinZ, lclipMaxZ));
        const vfloat<K> lfarP  = mini(maxi(lclipMinX, lclipMaxX), maxi(lclipMinY, lclipMaxY), maxi(lclipMinZ, lclipMaxZ));
#if defined(__AVX512F__) && !defined(__AVX512ER__) // SKX
        const vbool<K> lhit    = asInt(maxi(lnearP, tnear)) <= asInt(mini(lfarP, tfar));
#else
        const vbool<K> lhit    = maxi(lnearP, tnear) <= mini(lfarP, tfar);
#endif
        dist = lnearP;
        return lhit;
      }
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // robust ray/BVHN::AlignedNode intersection
    //////////////////////////////////////////////////////////////////////////////////////

    template<int N,int Nx>
      __forceinline size_t intersectNodeRobust(const typename BVHN<N>::AlignedNode* node, const TravRay<N,Nx>& ray, 
                                               const vfloat<Nx>& tnear, const vfloat<Nx>& tfar, vfloat<Nx>& dist) 
    {      
      const vfloat<N> tNearX = (vfloat<N>::load((float*)((const char*)&node->lower_x+ray.nearX)) - ray.org.x) * ray.rdir.x;
      const vfloat<N> tNearY = (vfloat<N>::load((float*)((const char*)&node->lower_x+ray.nearY)) - ray.org.y) * ray.rdir.y;
      const vfloat<N> tNearZ = (vfloat<N>::load((float*)((const char*)&node->lower_x+ray.nearZ)) - ray.org.z) * ray.rdir.z;
      const vfloat<N> tFarX  = (vfloat<N>::load((float*)((const char*)&node->lower_x+ray.farX )) - ray.org.x) * ray.rdir.x;
      const vfloat<N> tFarY  = (vfloat<N>::load((float*)((const char*)&node->lower_x+ray.farY )) - ray.org.y) * ray.rdir.y;
      const vfloat<N> tFarZ  = (vfloat<N>::load((float*)((const char*)&node->lower_x+ray.farZ )) - ray.org.z) * ray.rdir.z;
      const float round_down = 1.0f-2.0f*float(ulp); // FIXME: use per instruction rounding for AVX512
      const float round_up   = 1.0f+2.0f*float(ulp);
      const vfloat<N> tNear = max(tNearX,tNearY,tNearZ,tnear);
      const vfloat<N> tFar  = min(tFarX ,tFarY ,tFarZ ,tfar);
      const vbool<N> vmask = (round_down*tNear <= round_up*tFar);
      const size_t mask = movemask(vmask);
      dist = tNear;
      return mask;
    }

#if defined(__AVX512F__)

    template<>
      __forceinline size_t intersectNodeRobust<4,16>(const typename BVHN<4>::AlignedNode* node, const TravRay<4,16>& ray, 
                                                     const vfloat<16>& tnear, const vfloat<16>& tfar, vfloat<16>& dist) 
    {      
      const vfloat16 tNearX = (vfloat16(*(vfloat<4>*)((const char*)&node->lower_x+ray.nearX)) - ray.org.x) * ray.rdir.x;
      const vfloat16 tNearY = (vfloat16(*(vfloat<4>*)((const char*)&node->lower_x+ray.nearY)) - ray.org.y) * ray.rdir.y;
      const vfloat16 tNearZ = (vfloat16(*(vfloat<4>*)((const char*)&node->lower_x+ray.nearZ)) - ray.org.z) * ray.rdir.z;
      const vfloat16 tFarX  = (vfloat16(*(vfloat<4>*)((const char*)&node->lower_x+ray.farX )) - ray.org.x) * ray.rdir.x;
      const vfloat16 tFarY  = (vfloat16(*(vfloat<4>*)((const char*)&node->lower_x+ray.farY )) - ray.org.y) * ray.rdir.y;
      const vfloat16 tFarZ  = (vfloat16(*(vfloat<4>*)((const char*)&node->lower_x+ray.farZ )) - ray.org.z) * ray.rdir.z;
      const float round_down = 1.0f-2.0f*float(ulp); // FIXME: use per instruction rounding for AVX512
      const float round_up   = 1.0f+2.0f*float(ulp);
      const vfloat16 tNear = max(tNearX,tNearY,tNearZ,tnear);
      const vfloat16 tFar  = min(tFarX ,tFarY ,tFarZ ,tfar);
      const vbool16 vmask = le((1 << 4)-1,round_down*tNear,round_up*tFar);
      const size_t mask = movemask(vmask);
      dist = tNear;
      return mask;
    }

    template<>
      __forceinline size_t intersectNodeRobust<8,16>(const typename BVHN<8>::AlignedNode* node, const TravRay<8,16>& ray, 
                                                     const vfloat<16>& tnear, const vfloat<16>& tfar, vfloat<16>& dist) 
    {      
      const vfloat16 tNearX = (vfloat16(*(vfloat<8>*)((const char*)&node->lower_x+ray.nearX)) - ray.org.x) * ray.rdir.x;
      const vfloat16 tNearY = (vfloat16(*(vfloat<8>*)((const char*)&node->lower_x+ray.nearY)) - ray.org.y) * ray.rdir.y;
      const vfloat16 tNearZ = (vfloat16(*(vfloat<8>*)((const char*)&node->lower_x+ray.nearZ)) - ray.org.z) * ray.rdir.z;
      const vfloat16 tFarX  = (vfloat16(*(vfloat<8>*)((const char*)&node->lower_x+ray.farX )) - ray.org.x) * ray.rdir.x;
      const vfloat16 tFarY  = (vfloat16(*(vfloat<8>*)((const char*)&node->lower_x+ray.farY )) - ray.org.y) * ray.rdir.y;
      const vfloat16 tFarZ  = (vfloat16(*(vfloat<8>*)((const char*)&node->lower_x+ray.farZ )) - ray.org.z) * ray.rdir.z;
      const float round_down = 1.0f-2.0f*float(ulp); // FIXME: use per instruction rounding for AVX512
      const float round_up   = 1.0f+2.0f*float(ulp);
      const vfloat16 tNear = max(tNearX,tNearY,tNearZ,tnear);
      const vfloat16 tFar  = min(tFarX ,tFarY ,tFarZ ,tfar);
      const vbool16 vmask = le((1 << 8)-1,round_down*tNear,round_up*tFar);
      const size_t mask = movemask(vmask);
      dist = tNear;
      return mask;
    }

#endif

    template<int N, int K>
      __forceinline vbool<K> intersectNodeRobust(const typename BVHN<N>::AlignedNode* node, size_t i, 
                                                 const Vec3<vfloat<K>>& org, const Vec3<vfloat<K>>& dir, const Vec3<vfloat<K>>& rdir, const Vec3<vfloat<K>>& org_rdir,
                                                 const vfloat<K>& tnear, const vfloat<K>& tfar, vfloat<K>& dist)
    {
      // FIXME: use per instruction rounding for AVX512
      const vfloat<K> lclipMinX = (node->lower_x[i] - org.x) * rdir.x;
      const vfloat<K> lclipMinY = (node->lower_y[i] - org.y) * rdir.y;
      const vfloat<K> lclipMinZ = (node->lower_z[i] - org.z) * rdir.z;
      const vfloat<K> lclipMaxX = (node->upper_x[i] - org.x) * rdir.x;
      const vfloat<K> lclipMaxY = (node->upper_y[i] - org.y) * rdir.y;
      const vfloat<K> lclipMaxZ = (node->upper_z[i] - org.z) * rdir.z;
      const float round_down = 1.0f-2.0f*float(ulp);
      const float round_up   = 1.0f+2.0f*float(ulp);
      const vfloat<K> lnearP = max(max(min(lclipMinX, lclipMaxX), min(lclipMinY, lclipMaxY)), min(lclipMinZ, lclipMaxZ));
      const vfloat<K> lfarP  = min(min(max(lclipMinX, lclipMaxX), max(lclipMinY, lclipMaxY)), max(lclipMinZ, lclipMaxZ));
      const vbool<K> lhit   = round_down*max(lnearP,tnear) <= round_up*min(lfarP,tfar);
      dist = lnearP;
      return lhit;
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // fast ray/BVHN::AlignedNodeMB intersection
    //////////////////////////////////////////////////////////////////////////////////////

    template<int N>
      __forceinline size_t intersectNode(const typename BVHN<N>::AlignedNodeMB* node, const TravRay<N,N>& ray, const vfloat<N>& tnear, const vfloat<N>& tfar, const float time, vfloat<N>& dist)
    {
      const vfloat<N>* pNearX = (const vfloat<N>*)((const char*)&node->lower_x+ray.nearX);
      const vfloat<N>* pNearY = (const vfloat<N>*)((const char*)&node->lower_x+ray.nearY);
      const vfloat<N>* pNearZ = (const vfloat<N>*)((const char*)&node->lower_x+ray.nearZ);
      const vfloat<N>* pFarX  = (const vfloat<N>*)((const char*)&node->lower_x+ray.farX);
      const vfloat<N>* pFarY  = (const vfloat<N>*)((const char*)&node->lower_x+ray.farY);
      const vfloat<N>* pFarZ  = (const vfloat<N>*)((const char*)&node->lower_x+ray.farZ);
#if defined(__AVX2__)
      const vfloat<N> tNearX = msub(madd(time,pNearX[6],vfloat<N>(pNearX[0])), ray.rdir.x, ray.org_rdir.x);
      const vfloat<N> tNearY = msub(madd(time,pNearY[6],vfloat<N>(pNearY[0])), ray.rdir.y, ray.org_rdir.y);
      const vfloat<N> tNearZ = msub(madd(time,pNearZ[6],vfloat<N>(pNearZ[0])), ray.rdir.z, ray.org_rdir.z);
      const vfloat<N> tFarX  = msub(madd(time,pFarX [6],vfloat<N>(pFarX [0])), ray.rdir.x, ray.org_rdir.x);
      const vfloat<N> tFarY  = msub(madd(time,pFarY [6],vfloat<N>(pFarY [0])), ray.rdir.y, ray.org_rdir.y);
      const vfloat<N> tFarZ  = msub(madd(time,pFarZ [6],vfloat<N>(pFarZ [0])), ray.rdir.z, ray.org_rdir.z);
#else
      const vfloat<N> tNearX = (madd(time,pNearX[6],vfloat<N>(pNearX[0])) - ray.org.x) * ray.rdir.x;
      const vfloat<N> tNearY = (madd(time,pNearY[6],vfloat<N>(pNearY[0])) - ray.org.y) * ray.rdir.y;
      const vfloat<N> tNearZ = (madd(time,pNearZ[6],vfloat<N>(pNearZ[0])) - ray.org.z) * ray.rdir.z;
      const vfloat<N> tFarX  = (madd(time,pFarX [6],vfloat<N>(pFarX [0])) - ray.org.x) * ray.rdir.x;
      const vfloat<N> tFarY  = (madd(time,pFarY [6],vfloat<N>(pFarY [0])) - ray.org.y) * ray.rdir.y;
      const vfloat<N> tFarZ  = (madd(time,pFarZ [6],vfloat<N>(pFarZ [0])) - ray.org.z) * ray.rdir.z;
#endif
#if defined(__AVX2__) && !defined(__AVX512F__) // HSW
      const vfloat<N> tNear = maxi(tNearX,tNearY,tNearZ,tnear);
      const vfloat<N> tFar  = mini(tFarX ,tFarY ,tFarZ ,tfar);
      const vbool<N> vmask = asInt(tNear) > asInt(tFar);
      const size_t mask = movemask(vmask) ^ ((1<<N)-1);
#elif defined(__AVX512F__) && !defined(__AVX512ER__) // SKX
      const vfloat<N> tNear = maxi(tNearX,tNearY,tNearZ,tnear);
      const vfloat<N> tFar  = mini(tFarX ,tFarY ,tFarZ ,tfar);
      const vbool<N> vmask = asInt(tNear) <= asInt(tFar);
      const size_t mask = movemask(vmask);
#else
      const vfloat<N> tNear = max(tnear,tNearX,tNearY,tNearZ);
      const vfloat<N> tFar  = min(tfar, tFarX ,tFarY ,tFarZ );
      const vbool<N> vmask = tNear <= tFar;
      const size_t mask = movemask(vmask);
#endif
      dist = tNear;
      return mask;
    }

    template<int N, int K>
    __forceinline vbool<K> intersectNode(const typename BVHN<N>::AlignedNodeMB* node, const size_t i, 
                                         const Vec3<vfloat<K>>& org, const Vec3<vfloat<K>>& dir, const Vec3<vfloat<K>>& rdir, const Vec3<vfloat<K>>& org_rdir,
                                         const vfloat<K>& tnear, const vfloat<K>& tfar, const vfloat<K>& time, vfloat<K>& dist)
    {
      const vfloat<K> vlower_x = madd(time,vfloat<K>(node->lower_dx[i]),vfloat<K>(node->lower_x[i]));
      const vfloat<K> vlower_y = madd(time,vfloat<K>(node->lower_dy[i]),vfloat<K>(node->lower_y[i]));
      const vfloat<K> vlower_z = madd(time,vfloat<K>(node->lower_dz[i]),vfloat<K>(node->lower_z[i]));
      const vfloat<K> vupper_x = madd(time,vfloat<K>(node->upper_dx[i]),vfloat<K>(node->upper_x[i]));
      const vfloat<K> vupper_y = madd(time,vfloat<K>(node->upper_dy[i]),vfloat<K>(node->upper_y[i]));
      const vfloat<K> vupper_z = madd(time,vfloat<K>(node->upper_dz[i]),vfloat<K>(node->upper_z[i]));

#if defined(__AVX2__)
      const vfloat<K> lclipMinX = msub(vlower_x,rdir.x,org_rdir.x);
      const vfloat<K> lclipMinY = msub(vlower_y,rdir.y,org_rdir.y);
      const vfloat<K> lclipMinZ = msub(vlower_z,rdir.z,org_rdir.z);
      const vfloat<K> lclipMaxX = msub(vupper_x,rdir.x,org_rdir.x);
      const vfloat<K> lclipMaxY = msub(vupper_y,rdir.y,org_rdir.y);
      const vfloat<K> lclipMaxZ = msub(vupper_z,rdir.z,org_rdir.z);
#else
      const vfloat<K> lclipMinX = (vlower_x - org.x) * rdir.x;
      const vfloat<K> lclipMinY = (vlower_y - org.y) * rdir.y;
      const vfloat<K> lclipMinZ = (vlower_z - org.z) * rdir.z;
      const vfloat<K> lclipMaxX = (vupper_x - org.x) * rdir.x;
      const vfloat<K> lclipMaxY = (vupper_y - org.y) * rdir.y;
      const vfloat<K> lclipMaxZ = (vupper_z - org.z) * rdir.z;
#endif

#if defined(__AVX512F__) && !defined(__AVX512ER__) // SKX
      if (K == 16)
      {
        /* use mixed float/int min/max */
        const vfloat<K> lnearP = maxi(min(lclipMinX, lclipMaxX), min(lclipMinY, lclipMaxY), min(lclipMinZ, lclipMaxZ));
        const vfloat<K> lfarP  = mini(max(lclipMinX, lclipMaxX), max(lclipMinY, lclipMaxY), max(lclipMinZ, lclipMaxZ));
        const vbool<K> lhit    = asInt(maxi(lnearP, tnear)) <= asInt(mini(lfarP, tfar));
        dist = lnearP;
        return lhit;
      }
      else
#endif
      {
        const vfloat<K> lnearP = maxi(mini(lclipMinX, lclipMaxX), mini(lclipMinY, lclipMaxY), mini(lclipMinZ, lclipMaxZ));
        const vfloat<K> lfarP  = mini(maxi(lclipMinX, lclipMaxX), maxi(lclipMinY, lclipMaxY), maxi(lclipMinZ, lclipMaxZ));
#if defined(__AVX512F__) && !defined(__AVX512ER__) // SKX
        const vbool<K> lhit    = asInt(maxi(lnearP, tnear)) <= asInt(mini(lfarP, tfar));
#else
        const vbool<K> lhit    = maxi(lnearP, tnear) <= mini(lfarP, tfar);
#endif
        dist = lnearP;
        return lhit;
      }
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // robust ray/BVHN::AlignedNodeMB intersection
    //////////////////////////////////////////////////////////////////////////////////////

    template<int N>
      __forceinline size_t intersectNodeRobust(const typename BVHN<N>::AlignedNodeMB* node, const TravRay<N,N>& ray, const vfloat<N>& tnear, const vfloat<N>& tfar, const float time, vfloat<N>& dist)
    {
      const vfloat<N>* pNearX = (const vfloat<N>*)((const char*)&node->lower_x+ray.nearX);
      const vfloat<N>* pNearY = (const vfloat<N>*)((const char*)&node->lower_x+ray.nearY);
      const vfloat<N>* pNearZ = (const vfloat<N>*)((const char*)&node->lower_x+ray.nearZ);
      const vfloat<N> tNearX = (madd(time,pNearX[6],vfloat<N>(pNearX[0])) - ray.org.x) * ray.rdir.x;
      const vfloat<N> tNearY = (madd(time,pNearY[6],vfloat<N>(pNearY[0])) - ray.org.y) * ray.rdir.y;
      const vfloat<N> tNearZ = (madd(time,pNearZ[6],vfloat<N>(pNearZ[0])) - ray.org.z) * ray.rdir.z;
      const vfloat<N> tNear = max(tnear,tNearX,tNearY,tNearZ);
      const vfloat<N>* pFarX = (const vfloat<N>*)((const char*)&node->lower_x+ray.farX);
      const vfloat<N>* pFarY = (const vfloat<N>*)((const char*)&node->lower_x+ray.farY);
      const vfloat<N>* pFarZ = (const vfloat<N>*)((const char*)&node->lower_x+ray.farZ);
      const vfloat<N> tFarX = (madd(time,pFarX[6],vfloat<N>(pFarX[0])) - ray.org.x) * ray.rdir.x;
      const vfloat<N> tFarY = (madd(time,pFarY[6],vfloat<N>(pFarY[0])) - ray.org.y) * ray.rdir.y;
      const vfloat<N> tFarZ = (madd(time,pFarZ[6],vfloat<N>(pFarZ[0])) - ray.org.z) * ray.rdir.z;
      const vfloat<N> tFar = min(tfar,tFarX,tFarY,tFarZ);
      const float round_down = 1.0f-2.0f*float(ulp); // FIXME: use per instruction rounding for AVX512
      const float round_up   = 1.0f+2.0f*float(ulp);
      const size_t mask = movemask(round_down*tNear <= round_up*tFar);
      dist = tNear;
      return mask;
    }

    template<int N, int K>
    __forceinline vbool<K> intersectNodeRobust(const typename BVHN<N>::AlignedNodeMB* node, const size_t i, 
                                               const Vec3<vfloat<K>>& org, const Vec3<vfloat<K>>& dir, const Vec3<vfloat<K>>& rdir, const Vec3<vfloat<K>>& org_rdir,
                                               const vfloat<K>& tnear, const vfloat<K>& tfar, const vfloat<K>& time, vfloat<K>& dist)
    {
      const vfloat<K> vlower_x = madd(time,vfloat<K>(node->lower_dx[i]),vfloat<K>(node->lower_x[i]));
      const vfloat<K> vlower_y = madd(time,vfloat<K>(node->lower_dy[i]),vfloat<K>(node->lower_y[i]));
      const vfloat<K> vlower_z = madd(time,vfloat<K>(node->lower_dz[i]),vfloat<K>(node->lower_z[i]));
      const vfloat<K> vupper_x = madd(time,vfloat<K>(node->upper_dx[i]),vfloat<K>(node->upper_x[i]));
      const vfloat<K> vupper_y = madd(time,vfloat<K>(node->upper_dy[i]),vfloat<K>(node->upper_y[i]));
      const vfloat<K> vupper_z = madd(time,vfloat<K>(node->upper_dz[i]),vfloat<K>(node->upper_z[i]));

      const vfloat<K> lclipMinX = (vlower_x - org.x) * rdir.x;
      const vfloat<K> lclipMinY = (vlower_y - org.y) * rdir.y;
      const vfloat<K> lclipMinZ = (vlower_z - org.z) * rdir.z;
      const vfloat<K> lclipMaxX = (vupper_x - org.x) * rdir.x;
      const vfloat<K> lclipMaxY = (vupper_y - org.y) * rdir.y;
      const vfloat<K> lclipMaxZ = (vupper_z - org.z) * rdir.z;

      const float round_down = 1.0f-2.0f*float(ulp);
      const float round_up   = 1.0f+2.0f*float(ulp);

#if defined(__AVX512F__) && !defined(__AVX512ER__) // SKX
      if (K == 16)
      {
        const vfloat<K> lnearP = maxi(min(lclipMinX, lclipMaxX), min(lclipMinY, lclipMaxY), min(lclipMinZ, lclipMaxZ));
        const vfloat<K> lfarP  = mini(max(lclipMinX, lclipMaxX), max(lclipMinY, lclipMaxY), max(lclipMinZ, lclipMaxZ));
        const vbool<K>  lhit   = round_down*maxi(lnearP, tnear) <= round_up*mini(lfarP, tfar);
        dist = lnearP;
        return lhit;
      }
      else
#endif
      {
        const vfloat<K> lnearP = maxi(mini(lclipMinX, lclipMaxX), mini(lclipMinY, lclipMaxY), mini(lclipMinZ, lclipMaxZ));
        const vfloat<K> lfarP  = mini(maxi(lclipMinX, lclipMaxX), maxi(lclipMinY, lclipMaxY), maxi(lclipMinZ, lclipMaxZ));
        const vbool<K>  lhit   = round_down*maxi(lnearP, tnear) <= round_up*mini(lfarP, tfar);
        dist = lnearP;
        return lhit;
      }
    }
    
    //////////////////////////////////////////////////////////////////////////////////////
    // fast ray/BVHN::QuantizedNode intersection
    //////////////////////////////////////////////////////////////////////////////////////

    template<int N, int Nx>
      __forceinline size_t intersectNode(const typename BVHN<N>::QuantizedNode* node, const TravRay<N,Nx>& ray, const vfloat<Nx>& tnear, const vfloat<Nx>& tfar, vfloat<Nx>& dist);

    template<>
      __forceinline size_t intersectNode<4,4>(const typename BVH4::QuantizedNode* node, const TravRay<4,4>& ray, const vfloat4& tnear, const vfloat4& tfar, vfloat4& dist)
    {
      const vfloat4 start_x(node->start.x);
      const vfloat4 scale_x(node->scale.x);
      const vfloat4 lower_x = madd(node->dequantize<4>(ray.nearX >> 2),scale_x,start_x);
      const vfloat4 upper_x = madd(node->dequantize<4>(ray.farX  >> 2),scale_x,start_x);
      const vfloat4 start_y(node->start.y);
      const vfloat4 scale_y(node->scale.y);
      const vfloat4 lower_y = madd(node->dequantize<4>(ray.nearY >> 2),scale_y,start_y);
      const vfloat4 upper_y = madd(node->dequantize<4>(ray.farY  >> 2),scale_y,start_y);
      const vfloat4 start_z(node->start.z);
      const vfloat4 scale_z(node->scale.z);
      const vfloat4 lower_z = madd(node->dequantize<4>(ray.nearZ >> 2),scale_z,start_z);
      const vfloat4 upper_z = madd(node->dequantize<4>(ray.farZ  >> 2),scale_z,start_z);

#if defined(__AVX2__)
      const vfloat4 tNearX = msub(lower_x, ray.rdir.x, ray.org_rdir.x);
      const vfloat4 tNearY = msub(lower_y, ray.rdir.y, ray.org_rdir.y);
      const vfloat4 tNearZ = msub(lower_z, ray.rdir.z, ray.org_rdir.z);
      const vfloat4 tFarX  = msub(upper_x, ray.rdir.x, ray.org_rdir.x);
      const vfloat4 tFarY  = msub(upper_y, ray.rdir.y, ray.org_rdir.y);
      const vfloat4 tFarZ  = msub(upper_z, ray.rdir.z, ray.org_rdir.z);
#else
      const vfloat4 tNearX = (lower_x - ray.org.x) * ray.rdir.x;
      const vfloat4 tNearY = (lower_y - ray.org.y) * ray.rdir.y;
      const vfloat4 tNearZ = (lower_z - ray.org.z) * ray.rdir.z;
      const vfloat4 tFarX  = (upper_x - ray.org.x) * ray.rdir.x;
      const vfloat4 tFarY  = (upper_y - ray.org.y) * ray.rdir.y;
      const vfloat4 tFarZ  = (upper_z - ray.org.z) * ray.rdir.z;
#endif
      
#if defined(__SSE4_1__) && !defined(__AVX512F__) // up to HSW
      const vfloat4 tNear = maxi(tNearX,tNearY,tNearZ,tnear);
      const vfloat4 tFar  = mini(tFarX ,tFarY ,tFarZ ,tfar);
      const vbool4 vmask = asInt(tNear) > asInt(tFar);
      const size_t mask = movemask(vmask) ^ ((1<<4)-1);
#elif defined(__AVX512F__) && !defined(__AVX512ER__) // SKX
      const vfloat4 tNear = maxi(tNearX,tNearY,tNearZ,tnear);
      const vfloat4 tFar  = mini(tFarX ,tFarY ,tFarZ ,tfar);
      const vbool4 vmask = asInt(tNear) <= asInt(tFar);
      const size_t mask = movemask(vmask);
#else
      const vfloat4 tNear = max(tNearX,tNearY,tNearZ,tnear);
      const vfloat4 tFar  = min(tFarX ,tFarY ,tFarZ ,tfar);
      const vbool4 vmask = tNear <= tFar;
      const size_t mask = movemask(vmask);
#endif
      dist = tNear;
      return mask;
    }

#if defined(__AVX__)

    template<>
      __forceinline size_t intersectNode<8,8>(const typename BVH8::QuantizedNode* node, const TravRay<8,8>& ray, const vfloat8& tnear, const vfloat8& tfar, vfloat8& dist)
    {
      const vfloat8 start_x(node->start.x);
      const vfloat8 scale_x(node->scale.x);
      const vfloat8 lower_x = madd(node->dequantize<8>(ray.nearX >> 2),scale_x,start_x);
      const vfloat8 upper_x = madd(node->dequantize<8>(ray.farX  >> 2),scale_x,start_x);
      const vfloat8 start_y(node->start.y);
      const vfloat8 scale_y(node->scale.y);
      const vfloat8 lower_y = madd(node->dequantize<8>(ray.nearY >> 2),scale_y,start_y);
      const vfloat8 upper_y = madd(node->dequantize<8>(ray.farY  >> 2),scale_y,start_y);
      const vfloat8 start_z(node->start.z);
      const vfloat8 scale_z(node->scale.z);
      const vfloat8 lower_z = madd(node->dequantize<8>(ray.nearZ >> 2),scale_z,start_z);
      const vfloat8 upper_z = madd(node->dequantize<8>(ray.farZ  >> 2),scale_z,start_z);

#if defined(__AVX2__)
      const vfloat8 tNearX = msub(lower_x, ray.rdir.x, ray.org_rdir.x);
      const vfloat8 tNearY = msub(lower_y, ray.rdir.y, ray.org_rdir.y);
      const vfloat8 tNearZ = msub(lower_z, ray.rdir.z, ray.org_rdir.z);
      const vfloat8 tFarX  = msub(upper_x, ray.rdir.x, ray.org_rdir.x);
      const vfloat8 tFarY  = msub(upper_y, ray.rdir.y, ray.org_rdir.y);
      const vfloat8 tFarZ  = msub(upper_z, ray.rdir.z, ray.org_rdir.z);
#else
      const vfloat8 tNearX = (lower_x - ray.org.x) * ray.rdir.x;
      const vfloat8 tNearY = (lower_y - ray.org.y) * ray.rdir.y;
      const vfloat8 tNearZ = (lower_z - ray.org.z) * ray.rdir.z;
      const vfloat8 tFarX  = (upper_x - ray.org.x) * ray.rdir.x;
      const vfloat8 tFarY  = (upper_y - ray.org.y) * ray.rdir.y;
      const vfloat8 tFarZ  = (upper_z - ray.org.z) * ray.rdir.z;
#endif
      
#if defined(__AVX2__) && !defined(__AVX512F__) // HSW
      const vfloat8 tNear = maxi(tNearX,tNearY,tNearZ,tnear);
      const vfloat8 tFar  = mini(tFarX ,tFarY ,tFarZ ,tfar);
      const vbool8 vmask = asInt(tNear) > asInt(tFar);
      const size_t mask = movemask(vmask) ^ ((1<<8)-1);
#elif defined(__AVX512F__) && !defined(__AVX512ER__) // SKX
      const vfloat8 tNear = maxi(tNearX,tNearY,tNearZ,tnear);
      const vfloat8 tFar  = mini(tFarX ,tFarY ,tFarZ ,tfar);
      const vbool8 vmask = asInt(tNear) <= asInt(tFar);
      const size_t mask = movemask(vmask);
#else
      const vfloat8 tNear = max(tNearX,tNearY,tNearZ,tnear);
      const vfloat8 tFar  = min(tFarX ,tFarY ,tFarZ ,tfar);
      const vbool8 vmask = tNear <= tFar;
      const size_t mask = movemask(vmask);
#endif
      dist = tNear;
      return mask;
    }

#endif

#if defined(__AVX512F__)

    template<>
      __forceinline size_t intersectNode<4,16>(const typename BVH4::QuantizedNode* node, const TravRay<4,16>& ray, const vfloat16& tnear, const vfloat16& tfar, vfloat16& dist)
    {
      const vfloat16 start_x(node->start.x);
      const vfloat16 scale_x(node->scale.x);
      const vfloat16 lower_x = madd(vfloat16(node->dequantize<16>(ray.nearX >> 2)),scale_x,start_x);
      const vfloat16 upper_x = madd(vfloat16(node->dequantize<16>(ray.farX  >> 2)),scale_x,start_x);
      const vfloat16 start_y(node->start.y);
      const vfloat16 scale_y(node->scale.y);
      const vfloat16 lower_y = madd(vfloat16(node->dequantize<16>(ray.nearY >> 2)),scale_y,start_y);
      const vfloat16 upper_y = madd(vfloat16(node->dequantize<16>(ray.farY  >> 2)),scale_y,start_y);
      const vfloat16 start_z(node->start.z);
      const vfloat16 scale_z(node->scale.z);
      const vfloat16 lower_z = madd(vfloat16(node->dequantize<16>(ray.nearZ >> 2)),scale_z,start_z);
      const vfloat16 upper_z = madd(vfloat16(node->dequantize<16>(ray.farZ  >> 2)),scale_z,start_z);

      const vfloat16 tNearX = msub(lower_x, ray.rdir.x, ray.org_rdir.x);
      const vfloat16 tNearY = msub(lower_y, ray.rdir.y, ray.org_rdir.y);
      const vfloat16 tNearZ = msub(lower_z, ray.rdir.z, ray.org_rdir.z);
      const vfloat16 tFarX  = msub(upper_x, ray.rdir.x, ray.org_rdir.x);
      const vfloat16 tFarY  = msub(upper_y, ray.rdir.y, ray.org_rdir.y);
      const vfloat16 tFarZ  = msub(upper_z, ray.rdir.z, ray.org_rdir.z);      
      const vfloat16 tNear  = max(tNearX,tNearY,tNearZ,tnear);
      const vfloat16 tFar   = min(tFarX ,tFarY ,tFarZ ,tfar);
      const vbool16 vmask   = le(vbool16(0xf),tNear,tFar);
      const size_t mask     = movemask(vmask);
      dist = tNear;
      return mask;
    }

    template<>
      __forceinline size_t intersectNode<8,16>(const typename BVH8::QuantizedNode* node, const TravRay<8,16>& ray, const vfloat16& tnear, const vfloat16& tfar, vfloat16& dist)
    {
      const vfloat16 start_x(node->start.x);
      const vfloat16 scale_x(node->scale.x);
      const vfloat16 lower_x = madd(vfloat16(node->dequantize<16>(ray.nearX >> 2)),scale_x,start_x);
      const vfloat16 upper_x = madd(vfloat16(node->dequantize<16>(ray.farX  >> 2)),scale_x,start_x);
      const vfloat16 start_y(node->start.y);
      const vfloat16 scale_y(node->scale.y);
      const vfloat16 lower_y = madd(vfloat16(node->dequantize<16>(ray.nearY >> 2)),scale_y,start_y);
      const vfloat16 upper_y = madd(vfloat16(node->dequantize<16>(ray.farY  >> 2)),scale_y,start_y);
      const vfloat16 start_z(node->start.z);
      const vfloat16 scale_z(node->scale.z);
      const vfloat16 lower_z = madd(vfloat16(node->dequantize<16>(ray.nearZ >> 2)),scale_z,start_z);
      const vfloat16 upper_z = madd(vfloat16(node->dequantize<16>(ray.farZ  >> 2)),scale_z,start_z);

      const vfloat16 tNearX = msub(lower_x, ray.rdir.x, ray.org_rdir.x);
      const vfloat16 tNearY = msub(lower_y, ray.rdir.y, ray.org_rdir.y);
      const vfloat16 tNearZ = msub(lower_z, ray.rdir.z, ray.org_rdir.z);
      const vfloat16 tFarX  = msub(upper_x, ray.rdir.x, ray.org_rdir.x);
      const vfloat16 tFarY  = msub(upper_y, ray.rdir.y, ray.org_rdir.y);
      const vfloat16 tFarZ  = msub(upper_z, ray.rdir.z, ray.org_rdir.z);      
      
      const vfloat16 tNear  = max(tNearX,tNearY,tNearZ,tnear);
      const vfloat16 tFar   = min(tFarX ,tFarY ,tFarZ ,tfar);
      const vbool16 vmask   = le(vbool16(0xff),tNear,tFar);
      const size_t mask     = movemask(vmask);
      dist = tNear;
      return mask;
    }
    
#endif

    //////////////////////////////////////////////////////////////////////////////////////
    // fast ray/BVHN::UnalignedNode intersection
    //////////////////////////////////////////////////////////////////////////////////////

    template<int N>
      __forceinline size_t intersectNode(const typename BVHN<N>::UnalignedNode* node, const TravRay<N,N>& ray, 
                                         const vfloat<N>& tnear, const vfloat<N>& tfar, vfloat<N>& dist)
    {
      const Vec3<vfloat<N>> dir = xfmVector(node->naabb,ray.dir);
      //const Vec3<vfloat<N>> nrdir = Vec3<vfloat<N>>(vfloat<N>(-1.0f))/dir;
      const Vec3<vfloat<N>> nrdir = Vec3<vfloat<N>>(vfloat<N>(-1.0f))*rcp_safe(dir);
      const Vec3<vfloat<N>> org = xfmPoint(node->naabb,ray.org);
      const Vec3<vfloat<N>> tLowerXYZ = org * nrdir;       // (Vec3fa(zero) - org) * rdir;
      const Vec3<vfloat<N>> tUpperXYZ = tLowerXYZ - nrdir; // (Vec3fa(one ) - org) * rdir;

      const vfloat<N> tNearX = mini(tLowerXYZ.x,tUpperXYZ.x);
      const vfloat<N> tNearY = mini(tLowerXYZ.y,tUpperXYZ.y);
      const vfloat<N> tNearZ = mini(tLowerXYZ.z,tUpperXYZ.z);
      const vfloat<N> tFarX  = maxi(tLowerXYZ.x,tUpperXYZ.x);
      const vfloat<N> tFarY  = maxi(tLowerXYZ.y,tUpperXYZ.y);
      const vfloat<N> tFarZ  = maxi(tLowerXYZ.z,tUpperXYZ.z);
      const vfloat<N> tNear  = max(tnear, tNearX,tNearY,tNearZ);
      const vfloat<N> tFar   = min(tfar,  tFarX ,tFarY ,tFarZ );
      const vbool<N> vmask = tNear <= tFar;
      dist = tNear;
      return movemask(vmask);
    }

    template<int N, int K>
      __forceinline vbool<K> intersectNode(const typename BVHN<N>::UnalignedNode* node, const size_t i, 
                                           const Vec3<vfloat<K>>& org_i, const Vec3<vfloat<K>>& dir_i, const Vec3<vfloat<K>>& rdir_i, const Vec3<vfloat<K>>& org_rdir_i,
                                           const vfloat<K>& tnear, const vfloat<K>& tfar, vfloat<K>& dist)
    {
      typedef Vec3<vfloat<K>> Vec3vfK;
      typedef AffineSpaceT<LinearSpace3<Vec3vfK>> AffineSpace3vfK;
      
      const AffineSpace3vfK naabb(Vec3f(node->naabb.l.vx.x[i],node->naabb.l.vx.y[i],node->naabb.l.vx.z[i]),
                                  Vec3f(node->naabb.l.vy.x[i],node->naabb.l.vy.y[i],node->naabb.l.vy.z[i]),
                                  Vec3f(node->naabb.l.vz.x[i],node->naabb.l.vz.y[i],node->naabb.l.vz.z[i]),
                                  Vec3f(node->naabb.p   .x[i],node->naabb.p   .y[i],node->naabb.p   .z[i]));

      const Vec3<vfloat<K>> dir = xfmVector(naabb,dir_i);
      const Vec3<vfloat<K>> nrdir = Vec3<vfloat<K>>(vfloat<K>(-1.0f))* rcp_safe(dir); // FIXME: negate instead of mul with -1?
      const Vec3<vfloat<K>> org = xfmPoint(naabb,org_i);

      const vfloat<K> lclipMinX = org.x * nrdir.x; // (Vec3fa(zero) - org) * rdir;
      const vfloat<K> lclipMinY = org.y * nrdir.y;
      const vfloat<K> lclipMinZ = org.z * nrdir.z;
      const vfloat<K> lclipMaxX  = lclipMinX - nrdir.x; // (Vec3fa(one ) - org) * rdir;
      const vfloat<K> lclipMaxY  = lclipMinY - nrdir.y;
      const vfloat<K> lclipMaxZ  = lclipMinZ - nrdir.z;
      
      const vfloat<K> lnearP = maxi(mini(lclipMinX, lclipMaxX), mini(lclipMinY, lclipMaxY), mini(lclipMinZ, lclipMaxZ));
      const vfloat<K> lfarP  = mini(maxi(lclipMinX, lclipMaxX), maxi(lclipMinY, lclipMaxY), maxi(lclipMinZ, lclipMaxZ));
      const vbool<K> lhit    = maxi(lnearP, tnear) <= mini(lfarP, tfar);
      dist = lnearP;
      return lhit;
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // fast ray/BVHN::UnalignedNodeMB intersection
    //////////////////////////////////////////////////////////////////////////////////////

    template<int N>
      __forceinline size_t intersectNode(const typename BVHN<N>::UnalignedNodeMB* node, const TravRay<N,N>& ray, 
                                         const vfloat<N>& tnear, const vfloat<N>& tfar, const float time, vfloat<N>& dist)
    {
      const AffineSpaceT<LinearSpace3<Vec3<vfloat<N>>>> xfm = node->space0;
      const Vec3<vfloat<N>> b0_lower = zero;
      const Vec3<vfloat<N>> b0_upper = one;
      const Vec3<vfloat<N>> lower = lerp(b0_lower,node->b1.lower,vfloat<N>(time));
      const Vec3<vfloat<N>> upper = lerp(b0_upper,node->b1.upper,vfloat<N>(time));

      const BBox<Vec3<vfloat<N>>> bounds(lower,upper);
      const Vec3<vfloat<N>> dir = xfmVector(xfm,ray.dir);
      const Vec3<vfloat<N>> rdir = rcp_safe(dir);
      const Vec3<vfloat<N>> org = xfmPoint(xfm,ray.org);

      const Vec3<vfloat<N>> tLowerXYZ = (bounds.lower - org) * rdir;
      const Vec3<vfloat<N>> tUpperXYZ = (bounds.upper - org) * rdir;

      const vfloat<N> tNearX = mini(tLowerXYZ.x,tUpperXYZ.x);
      const vfloat<N> tNearY = mini(tLowerXYZ.y,tUpperXYZ.y);
      const vfloat<N> tNearZ = mini(tLowerXYZ.z,tUpperXYZ.z);
      const vfloat<N> tFarX  = maxi(tLowerXYZ.x,tUpperXYZ.x);
      const vfloat<N> tFarY  = maxi(tLowerXYZ.y,tUpperXYZ.y);
      const vfloat<N> tFarZ  = maxi(tLowerXYZ.z,tUpperXYZ.z);
      const vfloat<N> tNear  = max(tnear, tNearX,tNearY,tNearZ);
      const vfloat<N> tFar   = min(tfar,  tFarX ,tFarY ,tFarZ );
      const vbool<N> vmask = tNear <= tFar;
      dist = tNear;
      return movemask(vmask);
    }

    template<int N, int K>
      __forceinline vbool<K> intersectNode(const typename BVHN<N>::UnalignedNodeMB* node, const size_t i, 
                                           const Vec3<vfloat<K>>& org_i, const Vec3<vfloat<K>>& dir_i, const Vec3<vfloat<K>>& rdir_i, const Vec3<vfloat<K>>& org_rdir_i,
                                           const vfloat<K>& tnear, const vfloat<K>& tfar, const vfloat<K>& time, vfloat<K>& dist)
    {
      typedef Vec3<vfloat<K>> Vec3vfK;
      typedef AffineSpaceT<LinearSpace3<Vec3vfK>> AffineSpace3vfK;
      
      const AffineSpace3vfK xfm(Vec3f(node->space0.l.vx.x[i],node->space0.l.vx.y[i],node->space0.l.vx.z[i]),
                                Vec3f(node->space0.l.vy.x[i],node->space0.l.vy.y[i],node->space0.l.vy.z[i]),
                                Vec3f(node->space0.l.vz.x[i],node->space0.l.vz.y[i],node->space0.l.vz.z[i]),
                                Vec3f(node->space0.p   .x[i],node->space0.p   .y[i],node->space0.p   .z[i]));

      const Vec3<vfloat<K>> b0_lower = zero;
      const Vec3<vfloat<K>> b0_upper = one;
      const Vec3<vfloat<K>> b1_lower(node->b1.lower.x[i],node->b1.lower.y[i],node->b1.lower.z[i]);
      const Vec3<vfloat<K>> b1_upper(node->b1.upper.x[i],node->b1.upper.y[i],node->b1.upper.z[i]);
      const Vec3<vfloat<K>> lower = lerp(b0_lower,b1_lower,time);
      const Vec3<vfloat<K>> upper = lerp(b0_upper,b1_upper,time);

      const Vec3<vfloat<K>> dir = xfmVector(xfm,dir_i);
      const Vec3<vfloat<K>> rdir = rcp_safe(dir);
      const Vec3<vfloat<K>> org = xfmPoint(xfm,org_i);

      const vfloat<K> lclipMinX = (lower.x - org.x) * rdir.x;
      const vfloat<K> lclipMinY = (lower.y - org.y) * rdir.y;
      const vfloat<K> lclipMinZ = (lower.z - org.z) * rdir.z;
      const vfloat<K> lclipMaxX  = (upper.x - org.x) * rdir.x;
      const vfloat<K> lclipMaxY  = (upper.y - org.y) * rdir.y;
      const vfloat<K> lclipMaxZ  = (upper.z - org.z) * rdir.z;
      
      const vfloat<K> lnearP = maxi(mini(lclipMinX, lclipMaxX), mini(lclipMinY, lclipMaxY), mini(lclipMinZ, lclipMaxZ));
      const vfloat<K> lfarP  = mini(maxi(lclipMinX, lclipMaxX), maxi(lclipMinY, lclipMaxY), maxi(lclipMinZ, lclipMaxZ));
      const vbool<K> lhit    = maxi(lnearP, tnear) <= mini(lfarP, tfar);
      dist = lnearP;
      return lhit;
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // Node intersectors used in ray traversal
    //////////////////////////////////////////////////////////////////////////////////////

    /*! Intersects N nodes with 1 ray */
    template<int N, int Nx, int types, bool robust>
    struct BVHNNodeIntersector1;

    template<int N, int Nx>
      struct BVHNNodeIntersector1<N,Nx,BVH_AN1,false>
    {
      static __forceinline bool intersect(const typename BVHN<N>::NodeRef& node, const TravRay<N,Nx>& ray, const vfloat<Nx>& tnear, const vfloat<Nx>& tfar, const float time, vfloat<Nx>& dist, size_t& mask)
      {
        if (unlikely(node.isLeaf())) return false;
        mask = intersectNode<N,Nx>(node.alignedNode(),ray,tnear,tfar,dist);
        return true;
      }
    };

    template<int N, int Nx>
      struct BVHNNodeIntersector1<N,Nx,BVH_AN1,true>
    {
      static __forceinline bool intersect(const typename BVHN<N>::NodeRef& node, const TravRay<N,Nx>& ray, const vfloat<Nx>& tnear, const vfloat<Nx>& tfar, const float time, vfloat<Nx>& dist, size_t& mask)
      {
        if (unlikely(node.isLeaf())) return false;
        mask = intersectNodeRobust<N,Nx>(node.alignedNode(),ray,tnear,tfar,dist);
        return true;
      }
    };

    template<int N, int Nx>
      struct BVHNNodeIntersector1<N,Nx,BVH_AN2,false>
    {
      static __forceinline bool intersect(const typename BVHN<N>::NodeRef& node, const TravRay<N,Nx>& ray, const vfloat<N>& tnear, const vfloat<N>& tfar, const float time, vfloat<N>& dist, size_t& mask)
      {
        if (unlikely(node.isLeaf())) return false;
        mask = intersectNode<N>(node.alignedNodeMB(),ray,tnear,tfar,time,dist);
        return true;
      }
    };

    template<int N, int Nx>
      struct BVHNNodeIntersector1<N,Nx,BVH_AN2,true>
    {
      static __forceinline bool intersect(const typename BVHN<N>::NodeRef& node, const TravRay<N,Nx>& ray, const vfloat<N>& tnear, const vfloat<N>& tfar, const float time, vfloat<N>& dist, size_t& mask)
      {
        if (unlikely(node.isLeaf())) return false;
        mask = intersectNodeRobust<N>(node.alignedNodeMB(),ray,tnear,tfar,time,dist);
        return true;
      }
    };

    template<int N, int Nx>
      struct BVHNNodeIntersector1<N,Nx,BVH_AN1_UN1,false>
    {
      static __forceinline bool intersect(const typename BVHN<N>::NodeRef& node, const TravRay<N,Nx>& ray, const vfloat<N>& tnear, const vfloat<N>& tfar, const float time, vfloat<N>& dist, size_t& mask)
      {
        if (likely(node.isAlignedNode()))          mask = intersectNode<N,N>(node.alignedNode(),ray,tnear,tfar,dist);
        else if (unlikely(node.isUnalignedNode())) mask = intersectNode<N>(node.unalignedNode(),ray,tnear,tfar,dist);
        else return false;
        return true;
      }
    };

    template<int N, int Nx>
      struct BVHNNodeIntersector1<N,Nx,BVH_AN2_UN2,false>
    {
      static __forceinline bool intersect(const typename BVHN<N>::NodeRef& node, const TravRay<N,Nx>& ray, const vfloat<N>& tnear, const vfloat<N>& tfar, const float time, vfloat<N>& dist, size_t& mask)
      {
        if (likely(node.isAlignedNodeMB()))           mask = intersectNode<N>(node.alignedNodeMB(),ray,tnear,tfar,time,dist);
        else if (unlikely(node.isUnalignedNodeMB()))  mask = intersectNode<N>(node.unalignedNodeMB(),ray,tnear,tfar,time,dist);
        else return false;
        return true;
      }
    };

    template<int N, int Nx>
      struct BVHNNodeIntersector1<N,Nx,BVH_TN_AN1,false>
    {
      static __forceinline bool intersect(const typename BVHN<N>::NodeRef& node, const TravRay<N,Nx>& ray, const vfloat<N>& tnear, const vfloat<N>& tfar, const float time, vfloat<N>& dist, size_t& mask)
      {
        if (likely(node.isAlignedNode())) mask = intersectNode<N,N>(node.alignedNode(),ray,tnear,tfar,dist);
        else return false;
        return true;
      }
    };

    template<int N, int Nx>
      struct BVHNNodeIntersector1<N,Nx,BVH_TN_AN1_AN2,false>
    {
      static __forceinline bool intersect(const typename BVHN<N>::NodeRef& node, const TravRay<N,Nx>& ray, const vfloat<N>& tnear, const vfloat<N>& tfar, const float time, vfloat<N>& dist, size_t& mask)
      {
        if (likely(node.isAlignedNode()))        mask = intersectNode<N,N>(node.alignedNode(),ray,tnear,tfar,dist);
        else if (likely(node.isAlignedNodeMB())) mask = intersectNode<N>(node.alignedNodeMB(),ray,tnear,tfar,time,dist);
        else return false;
        return true;
      }
    };

    template<int N, int Nx>
      struct BVHNNodeIntersector1<N,Nx,BVH_QN1,false>
    {
      static __forceinline bool intersect(const typename BVHN<N>::NodeRef& node, const TravRay<N,Nx>& ray, const vfloat<Nx>& tnear, const vfloat<Nx>& tfar, const float time, vfloat<Nx>& dist, size_t& mask)
      {
        if (unlikely(node.isLeaf())) return false;
        mask = intersectNode<N,Nx>((const typename BVHN<N>::QuantizedNode*)node.quantizedNode(),ray,tnear,tfar,dist);
        return true;
      }
    };

    template<int N, int Nx>
      struct BVHNNodeIntersector1<N,Nx,BVH_QN1,true>
    {
      static __forceinline bool intersect(const typename BVHN<N>::NodeRef& node, const TravRay<N,Nx>& ray, const vfloat<Nx>& tnear, const vfloat<Nx>& tfar, const float time, vfloat<Nx>& dist, size_t& mask)
      {
        if (unlikely(node.isLeaf())) return false;
        mask = intersectNodeRobust<N,Nx>((const typename BVHN<N>::QuantizedNode*)node.quantizedNode(),ray,tnear,tfar,dist);
        return true;
      }
    };

    /*! Intersects N nodes with K rays */
    template<int N, int K, int types, bool robust>
    struct BVHNNodeIntersectorK;

    template<int N, int K>
    struct BVHNNodeIntersectorK<N,K,BVH_AN1,false>
    {
      static __forceinline bool intersect(const typename BVHN<N>::NodeRef& node, const size_t i, const Vec3<vfloat<K>>& org, const Vec3<vfloat<K>>& dir, const Vec3<vfloat<K>>& rdir, const Vec3<vfloat<K>>& org_rdir,
                                          const vfloat<K>& tnear, const vfloat<K>& tfar, const vfloat<K>& time, vfloat<K>& dist, vbool<K>& vmask)
      {
        vmask = intersectNode<N,K>(node.alignedNode(),i,org,dir,rdir,org_rdir,tnear,tfar,dist);
        return true;
      }
    };

    template<int N, int K>
      struct BVHNNodeIntersectorK<N,K,BVH_AN1,true>
    {
      static __forceinline bool intersect(const typename BVHN<N>::NodeRef& node, const size_t i, const Vec3<vfloat<K>>& org, const Vec3<vfloat<K>>& dir, const Vec3<vfloat<K>>& rdir, const Vec3<vfloat<K>>& org_rdir,
                                          const vfloat<K>& tnear, const vfloat<K>& tfar, const vfloat<K>& time, vfloat<K>& dist, vbool<K>& vmask)
      {
        vmask = intersectNodeRobust<N,K>(node.alignedNode(),i,org,dir,rdir,org_rdir,tnear,tfar,dist);
        return true;
      }
    };

    template<int N, int K>
    struct BVHNNodeIntersectorK<N,K,BVH_AN2,false>
    {
      static __forceinline bool intersect(const typename BVHN<N>::NodeRef& node, const size_t i, const Vec3<vfloat<K>>& org, const Vec3<vfloat<K>>& dir, const Vec3<vfloat<K>>& rdir, const Vec3<vfloat<K>>& org_rdir,
                                          const vfloat<K>& tnear, const vfloat<K>& tfar, const vfloat<K>& time, vfloat<K>& dist, vbool<K>& vmask)
      {
        vmask = intersectNode<N,K>(node.alignedNodeMB(),i,org,dir,rdir,org_rdir,tnear,tfar,time,dist);
        return true;
      }
    };

    template<int N, int K>
    struct BVHNNodeIntersectorK<N,K,BVH_AN2,true>
    {
      static __forceinline bool intersect(const typename BVHN<N>::NodeRef& node, const size_t i, const Vec3<vfloat<K>>& org, const Vec3<vfloat<K>>& dir, const Vec3<vfloat<K>>& rdir, const Vec3<vfloat<K>>& org_rdir,
                                          const vfloat<K>& tnear, const vfloat<K>& tfar, const vfloat<K>& time, vfloat<K>& dist, vbool<K>& vmask)
      {
        vmask = intersectNodeRobust<N,K>(node.alignedNodeMB(),i,org,dir,rdir,org_rdir,tnear,tfar,time,dist);
        return true;
      }
    };

    template<int N, int K>
      struct BVHNNodeIntersectorK<N,K,BVH_AN1_UN1,false>
    {
      static __forceinline bool intersect(const typename BVHN<N>::NodeRef& node, const size_t i, 
                                          const Vec3<vfloat<K>>& org, const Vec3<vfloat<K>>& dir, const Vec3<vfloat<K>>& rdir, const Vec3<vfloat<K>>& org_rdir,
                                          const vfloat<K>& tnear, const vfloat<K>& tfar, const vfloat<K>& time, vfloat<K>& dist, vbool<K>& vmask)
      {
        if (likely(node.isAlignedNode()))              vmask = intersectNode<N,K>(node.alignedNode(),i,org,dir,rdir,org_rdir,tnear,tfar,dist);
        else /*if (unlikely(node.isUnalignedNode()))*/ vmask = intersectNode<N,K>(node.unalignedNode(),i,org,dir,rdir,org_rdir,tnear,tfar,dist);
        return true;
      }
    };

    template<int N, int K>
      struct BVHNNodeIntersectorK<N,K,BVH_AN2_UN2,false>
    {
      static __forceinline bool intersect(const typename BVHN<N>::NodeRef& node, const size_t i, 
                                          const Vec3<vfloat<K>>& org, const Vec3<vfloat<K>>& dir, const Vec3<vfloat<K>>& rdir, const Vec3<vfloat<K>>& org_rdir,
                                          const vfloat<K>& tnear, const vfloat<K>& tfar, const vfloat<K>& time, vfloat<K>& dist, vbool<K>& vmask)
      {
        if (likely(node.isAlignedNodeMB()))              vmask = intersectNode<N,K>(node.alignedNodeMB(),i,org,dir,rdir,org_rdir,tnear,tfar,time,dist);
        else /*if (unlikely(node.isUnalignedNodeMB()))*/ vmask = intersectNode<N,K>(node.unalignedNodeMB(),i,org,dir,rdir,org_rdir,tnear,tfar,time,dist);
        return true;
      }
    };
  }
}

