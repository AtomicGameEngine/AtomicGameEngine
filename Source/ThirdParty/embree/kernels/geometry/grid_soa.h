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

#include "../common/ray.h"
#include "../common/scene_subdiv_mesh.h"
#include "filter.h"
#include "../bvh/bvh.h"
#include "../subdiv/tessellation.h"
#include "../subdiv/tessellation_cache.h"
#include "subdivpatch1cached.h"

namespace embree
{
  namespace isa
  {
    class GridSOA
    {
    public:

      /*! GridSOA constructor */
      GridSOA(const SubdivPatch1Base* patches, const unsigned time_steps, const unsigned time_steps_global,
              const unsigned x0, const unsigned x1, const unsigned y0, const unsigned y1, const unsigned swidth, const unsigned sheight,
              const SubdivMesh* const geom, const size_t bvhBytes, const size_t gridBytes, BBox3fa* bounds_o = nullptr);

      /*! Subgrid creation */
      template<typename Allocator>
        static GridSOA* create(const SubdivPatch1Base* patches, const unsigned time_steps, const unsigned time_steps_global,
                               unsigned x0, unsigned x1, unsigned y0, unsigned y1, 
                               const Scene* scene, Allocator& alloc, BBox3fa* bounds_o = nullptr)
      {
        const unsigned width = x1-x0+1;  
        const unsigned height = y1-y0+1; 
        const GridRange range(0,width-1,0,height-1);
        const size_t nodeBytes = time_steps_global == 1 ? sizeof(BVH4::AlignedNode) : sizeof(BVH4::AlignedNodeMB);
        const size_t bvhBytes  = getBVHBytes(range,nodeBytes,0);
        const size_t gridBytes = 4*size_t(width)*size_t(height)*sizeof(float);  
        size_t rootBytes = time_steps_global*sizeof(BVH4::NodeRef);
#if !defined(__X86_64__)
        rootBytes += 4; // We read 2 elements behind the grid. As we store at least 8 root bytes after the grid we are fine in 64 bit mode. But in 32 bit mode we have to do additional padding.
#endif
        void* data = alloc(offsetof(GridSOA,data)+max(1u,time_steps_global-1)*bvhBytes+time_steps*gridBytes+rootBytes);
        assert(data);
        return new (data) GridSOA(patches,time_steps,time_steps_global,x0,x1,y0,y1,patches->grid_u_res,patches->grid_v_res,scene->get<SubdivMesh>(patches->geom),bvhBytes,gridBytes,bounds_o);
      }

      /*! Grid creation */
      template<typename Allocator>
        static GridSOA* create(const SubdivPatch1Base* const patches, const unsigned time_steps, const unsigned time_steps_global,
                               const Scene* scene, const Allocator& alloc, BBox3fa* bounds_o = nullptr) 
      {
        return create(patches,time_steps,time_steps_global,0,patches->grid_u_res-1,0,patches->grid_v_res-1,scene,alloc,bounds_o);
      }

       /*! returns reference to root */
      __forceinline       BVH4::NodeRef& root(size_t t = 0)       { return (BVH4::NodeRef&)data[rootOffset + t*sizeof(BVH4::NodeRef)]; }
      __forceinline const BVH4::NodeRef& root(size_t t = 0) const { return (BVH4::NodeRef&)data[rootOffset + t*sizeof(BVH4::NodeRef)]; }

      /*! returns pointer to BVH array */
      __forceinline       char* bvhData(size_t t = 0)       { return &data[t*bvhBytes]; }
      __forceinline const char* bvhData(size_t t = 0) const { return &data[t*bvhBytes]; }

      /*! returns pointer to Grid array */
      __forceinline       float* gridData(size_t t = 0)       { return (float*) &data[gridOffset + t*gridBytes]; }
      __forceinline const float* gridData(size_t t = 0) const { return (float*) &data[gridOffset + t*gridBytes]; }
      
      __forceinline void* encodeLeaf(size_t u, size_t v) {
        return (void*) (16*(v * width + u + 1)); // +1 to not create empty leaf
      }
      __forceinline float* decodeLeaf(size_t t, const void* ptr) {
        return gridData(t) + (((size_t) (ptr) >> 4) - 1);
      }

      /*! returns the size of the BVH over the grid in bytes */
      static size_t getBVHBytes(const GridRange& range, const size_t nodeBytes, const size_t leafBytes);

      /*! calculates bounding box of grid range */
      __forceinline BBox3fa calculateBounds(size_t time, const GridRange& range) const
      {
        const float* const grid_array = gridData(time);
        const float* const grid_x_array = grid_array + 0 * dim_offset;
        const float* const grid_y_array = grid_array + 1 * dim_offset;
        const float* const grid_z_array = grid_array + 2 * dim_offset;
        
        /* compute the bounds just for the range! */
        BBox3fa bounds( empty );
        for (unsigned v = range.v_start; v<=range.v_end; v++) 
        {
          for (unsigned u = range.u_start; u<=range.u_end; u++)
          {
            const float x = grid_x_array[ v * width + u];
            const float y = grid_y_array[ v * width + u];
            const float z = grid_z_array[ v * width + u];
            bounds.extend( Vec3fa(x,y,z) );
          }
        }
        assert(is_finite(bounds));
        return bounds;
      }

      /*! Evaluates grid over patch and builds BVH4 tree over the grid. */
      BVH4::NodeRef buildBVH(size_t time, BBox3fa* bounds_o);
      
      /*! Create BVH4 tree over grid. */
      BBox3fa buildBVH(BVH4::NodeRef& curNode, size_t time, const GridRange& range, size_t& allocator);

      /*! Evaluates grid over patch and builds MBlur BVH4 tree over the grid. */
      BVH4::NodeRef buildMBlurBVH(size_t time, LBBox3fa* bounds_o);
      
      /*! Create MBlur BVH4 tree over grid. */
      LBBox3fa buildMBlurBVH(BVH4::NodeRef& curNode, size_t time, const GridRange& range, size_t& allocator);

      template<typename Loader>
        struct MapUV
      {
        typedef typename Loader::vfloat vfloat;
        const float* const grid_uv;
        size_t line_offset;
        size_t lines;

        __forceinline MapUV(const float* const grid_uv, size_t line_offset, const size_t lines)
          : grid_uv(grid_uv), line_offset(line_offset), lines(lines) {}

        __forceinline void operator() (vfloat& u, vfloat& v) const {
          const Vec3<vfloat> tri_v012_uv = Loader::gather(grid_uv,line_offset,lines);	
          const Vec2<vfloat> uv0 = GridSOA::decodeUV(tri_v012_uv[0]);
          const Vec2<vfloat> uv1 = GridSOA::decodeUV(tri_v012_uv[1]);
          const Vec2<vfloat> uv2 = GridSOA::decodeUV(tri_v012_uv[2]);        
          const Vec2<vfloat> uv = u * uv1 + v * uv2 + (1.0f-u-v) * uv0;        
          u = uv[0];v = uv[1]; 
        }
      };

      struct Gather2x3
      {
        enum { M = 4 };
        typedef vbool4 vbool;
        typedef vint4 vint;
        typedef vfloat4 vfloat;
        
        static __forceinline const Vec3<vfloat4> gather(const float* const grid, const size_t line_offset, const size_t lines)
        {
          vfloat4 r0 = vfloat4::loadu(grid + 0*line_offset);
          vfloat4 r1 = vfloat4::loadu(grid + 1*line_offset); // this accesses 2 elements too much in case of 2x2 grid, but this is ok as we ensure enough padding after the grid
          if (unlikely(line_offset == 2))
          {
            r0 = shuffle<0,1,1,1>(r0);
            r1 = shuffle<0,1,1,1>(r1);
          }
          return Vec3<vfloat4>(unpacklo(r0,r1),       // r00, r10, r01, r11
                               shuffle<1,1,2,2>(r0),  // r01, r01, r02, r02
                               shuffle<0,1,1,2>(r1)); // r10, r11, r11, r12
        }

        static __forceinline void gather(const float* const grid_x, 
                                         const float* const grid_y, 
                                         const float* const grid_z, 
                                         const size_t line_offset,
                                         const size_t lines,
                                         Vec3<vfloat4>& v0_o,
                                         Vec3<vfloat4>& v1_o,
                                         Vec3<vfloat4>& v2_o)
        {
          const Vec3<vfloat4> tri_v012_x = gather(grid_x,line_offset,lines);
          const Vec3<vfloat4> tri_v012_y = gather(grid_y,line_offset,lines);
          const Vec3<vfloat4> tri_v012_z = gather(grid_z,line_offset,lines);
          v0_o = Vec3<vfloat4>(tri_v012_x[0],tri_v012_y[0],tri_v012_z[0]);
          v1_o = Vec3<vfloat4>(tri_v012_x[1],tri_v012_y[1],tri_v012_z[1]);
          v2_o = Vec3<vfloat4>(tri_v012_x[2],tri_v012_y[2],tri_v012_z[2]);
        }
      };
      
#if defined (__AVX__)
      struct Gather3x3
      {
        enum { M = 8 };
        typedef vbool8 vbool;
        typedef vint8 vint;
        typedef vfloat8 vfloat;
        
        static __forceinline const Vec3<vfloat8> gather(const float* const grid, const size_t line_offset, const size_t lines)
        {
          vfloat4 ra = vfloat4::loadu(grid + 0*line_offset);
          vfloat4 rb = vfloat4::loadu(grid + 1*line_offset); // this accesses 2 elements too much in case of 2x2 grid, but this is ok as we ensure enough padding after the grid
          vfloat4 rc;
          if (likely(lines > 2)) 
            rc = vfloat4::loadu(grid + 2*line_offset);
          else                   
            rc = rb;

          if (unlikely(line_offset == 2))
          {
            ra = shuffle<0,1,1,1>(ra);
            rb = shuffle<0,1,1,1>(rb);
            rc = shuffle<0,1,1,1>(rc);
          }
          
          const vfloat8 r0 = vfloat8(ra,rb);
          const vfloat8 r1 = vfloat8(rb,rc);
          return Vec3<vfloat8>(unpacklo(r0,r1),         // r00, r10, r01, r11, r10, r20, r11, r21
                               shuffle<1,1,2,2>(r0),    // r01, r01, r02, r02, r11, r11, r12, r12
                               shuffle<0,1,1,2>(r1));   // r10, r11, r11, r12, r20, r21, r21, r22
        }

        static __forceinline void gather(const float* const grid_x, 
                                         const float* const grid_y, 
                                         const float* const grid_z, 
                                         const size_t line_offset,
                                         const size_t lines,
                                         Vec3<vfloat8>& v0_o,
                                         Vec3<vfloat8>& v1_o,
                                         Vec3<vfloat8>& v2_o)
        {
          const Vec3<vfloat8> tri_v012_x = gather(grid_x,line_offset,lines);
          const Vec3<vfloat8> tri_v012_y = gather(grid_y,line_offset,lines);
          const Vec3<vfloat8> tri_v012_z = gather(grid_z,line_offset,lines);
          v0_o = Vec3<vfloat8>(tri_v012_x[0],tri_v012_y[0],tri_v012_z[0]);
          v1_o = Vec3<vfloat8>(tri_v012_x[1],tri_v012_y[1],tri_v012_z[1]);
          v2_o = Vec3<vfloat8>(tri_v012_x[2],tri_v012_y[2],tri_v012_z[2]);
        }
      };
#endif

      template<typename vfloat>
      static __forceinline Vec2<vfloat> decodeUV(const vfloat& uv)
      {
        typedef typename vfloat::Int vint;
        const vint iu  = asInt(uv) & 0xffff;
        const vint iv  = srl(asInt(uv),16);
	const vfloat u = (vfloat)iu * vfloat(1.0f/0xFFFF);
	const vfloat v = (vfloat)iv * vfloat(1.0f/0xFFFF);
	return Vec2<vfloat>(u,v);
      }

    public:
      unsigned align0;
      unsigned time_steps_global;
      unsigned time_steps;
      unsigned width;

      unsigned height;
      unsigned dim_offset;
      unsigned geomID;
      unsigned primID;

      unsigned bvhBytes;
      unsigned gridOffset;
      unsigned gridBytes;
      unsigned rootOffset;

      char data[1];        //!< after the struct we first store the BVH, then the grid, and finally the roots
    };
  }
}
