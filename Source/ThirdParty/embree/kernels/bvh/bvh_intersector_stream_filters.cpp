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

#include "bvh_intersector_stream_filters.h"
#include "bvh_intersector_stream.h"

namespace embree
{
  namespace isa
  {
    static const size_t MAX_RAYS_PER_OCTANT = 8*sizeof(size_t);

    static_assert(MAX_RAYS_PER_OCTANT <= MAX_INTERNAL_STREAM_SIZE,"maximal internal stream size exceeded");

    __forceinline void RayStream::filterAOS(Scene *scene, RTCRay* _rayN, const size_t N, const size_t stride, IntersectContext* context, const bool intersect)
    {
      Ray* __restrict__ rayN = (Ray*)_rayN;
      __aligned(64) Ray* octants[8][MAX_RAYS_PER_OCTANT];
      unsigned int rays_in_octant[8];

      for (size_t i=0;i<8;i++) rays_in_octant[i] = 0;
      size_t inputRayID = 0;

      while(1)
      {
        int cur_octant = -1;
        /* sort rays into octants */
        for (;inputRayID<N;)
        {
          Ray &ray = *(Ray*)((char*)rayN + inputRayID * stride);
          /* skip invalid rays */
          if (unlikely(ray.tnear > ray.tfar)) { inputRayID++; continue; }
          if (unlikely(!intersect && ray.geomID == 0)) { inputRayID++; continue; } // ignore already occluded rays

#if defined(EMBREE_IGNORE_INVALID_RAYS)
          if (unlikely(!ray.valid())) {  inputRayID++; continue; }
#endif

          const unsigned int octantID = movemask(vfloat4(ray.dir) < 0.0f) & 0x7;

          assert(octantID < 8);
          octants[octantID][rays_in_octant[octantID]++] = &ray;
          inputRayID++;
          if (unlikely(rays_in_octant[octantID] == MAX_RAYS_PER_OCTANT))
          {
            cur_octant = octantID;
            break;
          }
        }
        /* need to flush rays in octant ? */
        if (unlikely(cur_octant == -1))
          for (int i=0;i<8;i++)
            if (rays_in_octant[i])
            {
              cur_octant = i;
              break;
            }

        /* all rays traced ? */
        if (unlikely(cur_octant == -1))
          break;

        
        Ray** rays = &octants[cur_octant][0];
        const size_t numOctantRays = rays_in_octant[cur_octant];

        /* special codepath for very small number of rays per octant */
        if (numOctantRays == 1)
        {
          if (intersect) scene->intersect((RTCRay&)*rays[0],context);
          else           scene->occluded ((RTCRay&)*rays[0],context);
        }        
        /* codepath for large number of rays per octant */
        else
        {
          /* incoherent ray stream code path */
          if (intersect) scene->intersectN((RTCRay**)rays,numOctantRays,context);
          else           scene->occludedN ((RTCRay**)rays,numOctantRays,context);
        }
        rays_in_octant[cur_octant] = 0;

        }
    }

    __forceinline void RayStream::filterAOP(Scene *scene, RTCRay** _rayN, const size_t N,IntersectContext* context, const bool intersect)
    {
      Ray** __restrict__ rayN = (Ray**)_rayN;
      __aligned(64) Ray* octants[8][MAX_RAYS_PER_OCTANT];
      unsigned int rays_in_octant[8];

      for (size_t i=0;i<8;i++) rays_in_octant[i] = 0;
      size_t inputRayID = 0;

      while(1)
      {
        int cur_octant = -1;
        /* sort rays into octants */
        for (;inputRayID<N;)
        {
          Ray &ray = *rayN[inputRayID];
          /* skip invalid rays */
          if (unlikely(ray.tnear > ray.tfar)) { inputRayID++; continue; }
          if (unlikely(!intersect && ray.geomID == 0)) { inputRayID++; continue; } // ignore already occluded rays

#if defined(EMBREE_IGNORE_INVALID_RAYS)
          if (unlikely(!ray.valid())) {  inputRayID++; continue; }
#endif

          const unsigned int octantID = movemask(vfloat4(ray.dir) < 0.0f) & 0x7;

          assert(octantID < 8);
          octants[octantID][rays_in_octant[octantID]++] = &ray;
          inputRayID++;
          if (unlikely(rays_in_octant[octantID] == MAX_RAYS_PER_OCTANT))
          {
            cur_octant = octantID;
            break;
          }
        }
        /* need to flush rays in octant ? */
        if (unlikely(cur_octant == -1))
          for (int i=0;i<8;i++)
            if (rays_in_octant[i])
            {
              cur_octant = i;
              break;
            }

        /* all rays traced ? */
        if (unlikely(cur_octant == -1))
          break;

        
        Ray** rays = &octants[cur_octant][0];
        const size_t numOctantRays = rays_in_octant[cur_octant];

        /* special codepath for very small number of rays per octant */
        if (numOctantRays == 1)
        {
          if (intersect) scene->intersect((RTCRay&)*rays[0],context);
          else           scene->occluded ((RTCRay&)*rays[0],context);
        }        
        /* codepath for large number of rays per octant */
        else
        {
          /* incoherent ray stream code path */
          if (intersect) scene->intersectN((RTCRay**)rays,numOctantRays,context);
          else           scene->occludedN ((RTCRay**)rays,numOctantRays,context);
        }
        rays_in_octant[cur_octant] = 0;

        }
    }


    __forceinline void RayStream::filterSOA(Scene *scene, char* rayData, const size_t N, const size_t streams, const size_t stream_offset, IntersectContext* context, const bool intersect)
    {
      RayPacket rayN(rayData,N);

      /* can we use the fast path ? */
#if defined(__AVX__) && ENABLE_COHERENT_STREAM_PATH == 1 
      /* fast path for packet width == SIMD width && correct RayK alignment*/
      const size_t rayDataAlignment = (size_t)rayData        % (VSIZEX*sizeof(float));
      const size_t offsetAlignment  = (size_t)stream_offset  % (VSIZEX*sizeof(float));

      if (unlikely(isCoherent(context->user->flags) &&
                   N == VSIZEX                && 
                   !rayDataAlignment          && 
                   !offsetAlignment))
      {
        static const size_t MAX_COHERENT_RAY_PACKETS = MAX_RAYS_PER_OCTANT / VSIZEX;
        __aligned(64) RayK<VSIZEX> *rays_ptr[MAX_RAYS_PER_OCTANT / VSIZEX]; 

        /* check for common direction */
        vfloatx min_x(pos_inf), max_x(neg_inf);
        vfloatx min_y(pos_inf), max_y(neg_inf);
        vfloatx min_z(pos_inf), max_z(neg_inf);
        vboolx all_active(true);
        for (size_t s=0; s<streams; s++)
        {
          const size_t offset = s*stream_offset;
          RayK<VSIZEX> &ray = *(RayK<VSIZEX>*)(rayData + offset);
          min_x = min(min_x,ray.dir.x);
          min_y = min(min_y,ray.dir.y);
          min_z = min(min_z,ray.dir.z);
          max_x = max(max_x,ray.dir.x);
          max_y = max(max_y,ray.dir.y);
          max_z = max(max_z,ray.dir.z);          
          all_active &= ray.tnear <= ray.tfar;
#if defined(EMBREE_IGNORE_INVALID_RAYS)
          all_active &= ray.valid();
#endif
        }
        const bool commonDirection =                                    \
          (all(max_x < vfloatx(zero)) || all(min_x >= vfloatx(zero))) && 
          (all(max_y < vfloatx(zero)) || all(min_y >= vfloatx(zero))) && 
          (all(max_z < vfloatx(zero)) || all(min_z >= vfloatx(zero))); 

        /* fallback to chunk in case of non-common directions */
        if (unlikely(commonDirection == false 
                     || !all(all_active) 
                     || scene->isRobust()
                     || !scene->accels.validIsecN() ) ) /* all valid accels need to have a intersectN/occludedN */
        {
          for (size_t s=0; s<streams; s++)
          {
            const size_t offset = s*stream_offset;
            RayK<VSIZEX> &ray = *(RayK<VSIZEX>*)(rayData + offset);
            vboolx valid = ray.tnear <= ray.tfar;
            if (intersect) scene->intersect(valid,ray,context);
            else           scene->occluded (valid,ray,context);
          }
          return;
        }

        /* prevent SOA to AOS conversion by setting context flag */
        context->flags = IntersectContext::encodeSIMDWidth(VSIZEX);
        size_t numStreams = 0;

        for (size_t s=0; s<streams; s++)
        {
          const size_t offset = s*stream_offset;
          RayK<VSIZEX> &ray = *(RayK<VSIZEX>*)(rayData + offset);
          rays_ptr[numStreams++] = &ray;
          /* trace as stream */
          if (unlikely(numStreams == MAX_COHERENT_RAY_PACKETS))
          {                          
            const size_t size = numStreams*VSIZEX;
            if (intersect)
              scene->intersectN((RTCRay**)rays_ptr,size,context);
            else
              scene->occludedN((RTCRay**)rays_ptr,size,context);        
            numStreams = 0;
          }
        }
        /* flush remaining streams */
        if (unlikely(numStreams))
        {
          const size_t size = numStreams*VSIZEX;
          if (intersect)
            scene->intersectN((RTCRay**)rays_ptr,size,context);
          else
            scene->occludedN((RTCRay**)rays_ptr,size,context);        
        }
        return;
      }
#endif

      /* otherwise use stream intersector */
      __aligned(64) Ray rays[MAX_RAYS_PER_OCTANT];
      __aligned(64) Ray *rays_ptr[MAX_RAYS_PER_OCTANT];
      
      size_t octants[8][MAX_RAYS_PER_OCTANT];
      unsigned int rays_in_octant[8];

      for (size_t i=0;i<8;i++) rays_in_octant[i] = 0;

      size_t soffset = 0;

      for (size_t s=0;s<streams;s++,soffset+=stream_offset)
      {
        // todo: use SIMD width to compute octants
        for (size_t i=0;i<N;i++)
        {
          /* global + local offset */
          const size_t offset = soffset + sizeof(float) * i;

          if (unlikely(!rayN.isValid(offset))) continue;

#if defined(EMBREE_IGNORE_INVALID_RAYS)
          __aligned(64) Ray ray = rayN.gather(offset);
          if (unlikely(!ray.valid())) continue; 
#endif

          const size_t octantID = rayN.getOctant(offset);

          assert(octantID < 8);
          octants[octantID][rays_in_octant[octantID]++] = offset;
        
          if (unlikely(rays_in_octant[octantID] == MAX_RAYS_PER_OCTANT))
          {
            for (size_t j=0;j<MAX_RAYS_PER_OCTANT;j++)
            {
              rays_ptr[j] = &rays[j]; // rays_ptr might get reordered for occludedN
              rays[j] = rayN.gather(octants[octantID][j]);
            }

            if (intersect)
              scene->intersectN((RTCRay**)rays_ptr,MAX_RAYS_PER_OCTANT,context);
            else
              scene->occludedN((RTCRay**)rays_ptr,MAX_RAYS_PER_OCTANT,context);

            for (size_t j=0;j<MAX_RAYS_PER_OCTANT;j++)
              rayN.scatter(octants[octantID][j],rays[j],intersect);
            
            rays_in_octant[octantID] = 0;
          }
        }        
      }

      /* flush remaining rays per octant */
      for (size_t i=0;i<8;i++)
        if (rays_in_octant[i])
        {
          for (size_t j=0;j<rays_in_octant[i];j++)
          {
            rays_ptr[j] = &rays[j]; // rays_ptr might get reordered for occludedN
            rays[j] = rayN.gather(octants[i][j]);
          }

          if (intersect)
            scene->intersectN((RTCRay**)rays_ptr,rays_in_octant[i],context);
          else
            scene->occludedN((RTCRay**)rays_ptr,rays_in_octant[i],context);        

          for (size_t j=0;j<rays_in_octant[i];j++)
            rayN.scatter(octants[i][j],rays[j],intersect);
        }
    }

    void RayStream::filterSOP(Scene *scene, const RTCRayNp& _rayN, const size_t N, IntersectContext* context, const bool intersect)
    {
      RayPN& rayN = *(RayPN*)&_rayN;
      size_t rayStartIndex = 0;

      /* use packet intersector for coherent ray mode */
      if (unlikely(isCoherent(context->user->flags)))
      {
        size_t s = 0;
        size_t stream_offset = 0;
        const size_t numPackets = (N+VSIZEX-1) / VSIZEX;
        rayStartIndex += numPackets * VSIZEX;
        for (size_t i=0; i<numPackets * VSIZEX; i+=VSIZEX)
        {
          const vintx vi = vintx(int(i))+vintx(step);
          vboolx valid = vi < vintx(int(N));
          const size_t offset = s*stream_offset + sizeof(float) * i;
          RayK<VSIZEX> ray = rayN.gather<VSIZEX>(valid,offset);
          valid &= ray.tnear <= ray.tfar;
          if (intersect) scene->intersect(valid,ray,context);
          else           scene->occluded (valid,ray,context);
          rayN.scatter<VSIZEX>(valid,offset,ray,intersect);
        }
        return;
      }
      
      /* otherwise use stream intersector */
      __aligned(64) Ray rays[MAX_RAYS_PER_OCTANT];
      __aligned(64) Ray *rays_ptr[MAX_RAYS_PER_OCTANT];

      size_t octants[8][MAX_RAYS_PER_OCTANT];
      unsigned int rays_in_octant[8];

      for (size_t i=0;i<8;i++) rays_in_octant[i] = 0;

      {
        // todo: use SIMD width to compute octants
        for (size_t i=rayStartIndex;i<N;i++)
        {
          /* global + local offset */
          const size_t offset = sizeof(float) * i;

          if (unlikely(!rayN.isValidByOffset(offset))) continue;

#if defined(EMBREE_IGNORE_INVALID_RAYS)
          __aligned(64) Ray ray = rayN.gatherByOffset(offset);
          if (unlikely(!ray.valid())) continue; 
#endif

          const size_t octantID = rayN.getOctantByOffset(offset);

          assert(octantID < 8);
          octants[octantID][rays_in_octant[octantID]++] = offset;
        
          if (unlikely(rays_in_octant[octantID] == MAX_RAYS_PER_OCTANT))
          {
            for (size_t j=0;j<MAX_RAYS_PER_OCTANT;j++)
            {
              rays_ptr[j] = &rays[j]; // rays_ptr might get reordered for occludedN
              rays[j] = rayN.gatherByOffset(octants[octantID][j]);
            }

            if (intersect)
              scene->intersectN((RTCRay**)rays_ptr,MAX_RAYS_PER_OCTANT,context);
            else
              scene->occludedN((RTCRay**)rays_ptr,MAX_RAYS_PER_OCTANT,context);

            for (size_t j=0;j<MAX_RAYS_PER_OCTANT;j++)
              rayN.scatterByOffset(octants[octantID][j],rays[j],intersect);
            
            rays_in_octant[octantID] = 0;
          }
        }        
      }

      /* flush remaining rays per octant */
      for (size_t i=0;i<8;i++)
        if (rays_in_octant[i])
        {
          for (size_t j=0;j<rays_in_octant[i];j++)
          {
            rays_ptr[j] = &rays[j]; // rays_ptr might get reordered for occludedN
            rays[j] = rayN.gatherByOffset(octants[i][j]);
          }

          if (intersect)
            scene->intersectN((RTCRay**)rays_ptr,rays_in_octant[i],context);
          else
            scene->occludedN((RTCRay**)rays_ptr,rays_in_octant[i],context);        

          for (size_t j=0;j<rays_in_octant[i];j++)
            rayN.scatterByOffset(octants[i][j],rays[j],intersect);
        }
    }

    RayStreamFilterFuncs rayStreamFilterFuncs() {
      return RayStreamFilterFuncs(RayStream::filterAOS,RayStream::filterAOP,RayStream::filterSOA,RayStream::filterSOP);
    }
  };
};
