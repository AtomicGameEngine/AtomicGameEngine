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

#include "subdivpatch1cached.h"
#include "grid_soa_intersector1.h"
#include "grid_soa_intersector.h"
#include "../common/ray.h"

namespace embree
{
  namespace isa
  {
    template<typename T, bool cached>
      class SubdivPatch1CachedPrecalculations : public T
    { 
    public:
      __forceinline SubdivPatch1CachedPrecalculations (const Ray& ray, const void* ptr, unsigned numTimeSteps)
        : T(ray,ptr,numTimeSteps) {}
      
      __forceinline ~SubdivPatch1CachedPrecalculations() {
        if (cached && this->grid) SharedLazyTessellationCache::sharedLazyTessellationCache.unlock();
      }
    };

    template<int K, typename T, bool cached>
      class SubdivPatch1CachedPrecalculationsK : public T
    { 
    public:
      __forceinline SubdivPatch1CachedPrecalculationsK (const vbool<K>& valid, RayK<K>& ray, unsigned numTimeSteps)
        : T(valid,ray,numTimeSteps) {}
      
      __forceinline ~SubdivPatch1CachedPrecalculationsK() {
        if (cached && this->grid) SharedLazyTessellationCache::sharedLazyTessellationCache.unlock();
      }
    };

    template<bool cached>
      class SubdivPatch1CachedIntersector1
    {
    public:
      typedef SubdivPatch1Cached Primitive;
      typedef SubdivPatch1CachedPrecalculations<GridSOAIntersector1::Precalculations,cached> Precalculations;

      static __forceinline bool processLazyNode(Precalculations& pre, IntersectContext* context, const Primitive* prim_i, size_t& lazy_node)
      {
        Primitive* prim = (Primitive*) prim_i;
        GridSOA* grid = nullptr;
        if (cached) 
        {          
          Scene* scene = context->scene;
          if (pre.grid) SharedLazyTessellationCache::sharedLazyTessellationCache.unlock();
          grid = (GridSOA*) SharedLazyTessellationCache::lookup(prim->entry(),scene->commitCounterSubdiv,[&] () {
              auto alloc = [] (const size_t bytes) { return SharedLazyTessellationCache::sharedLazyTessellationCache.malloc(bytes); };
              return GridSOA::create((SubdivPatch1Base*)prim,1,1,scene,alloc);
            });
        }
        else {
          grid = (GridSOA*) prim->root_ref.get();
        }
        lazy_node = grid->root(0);
        pre.grid = grid;
        return false;
      }

      /*! Intersect a ray with the primitive. */
      static __forceinline void intersect(Precalculations& pre, Ray& ray, IntersectContext* context, const Primitive* prim, size_t ty, size_t& lazy_node) 
      {
        if (likely(ty == 0)) GridSOAIntersector1::intersect(pre,ray,context,prim,ty,lazy_node);
        else                 processLazyNode(pre,context,prim,lazy_node);
      }
      static __forceinline void intersect(Precalculations& pre, Ray& ray, IntersectContext* context, size_t ty0, const Primitive* prim, size_t ty, size_t& lazy_node) {
        intersect(pre,ray,context,prim,ty,lazy_node);
      }
      
      /*! Test if the ray is occluded by the primitive */
      static __forceinline bool occluded(Precalculations& pre, Ray& ray, IntersectContext* context, const Primitive* prim, size_t ty, size_t& lazy_node) 
      {
        if (likely(ty == 0)) return GridSOAIntersector1::occluded(pre,ray,context,prim,ty,lazy_node);
        else                 return processLazyNode(pre,context,prim,lazy_node);
      }
      static __forceinline bool occluded(Precalculations& pre, Ray& ray, IntersectContext* context, size_t ty0, const Primitive* prim, size_t ty, size_t& lazy_node) {
        return occluded(pre,ray,context,prim,ty,lazy_node);
      }
    };

    template<bool cached>
      class SubdivPatch1MBlurCachedIntersector1
    {
    public:
      typedef SubdivPatch1Cached Primitive;
      typedef SubdivPatch1CachedPrecalculations<GridSOAMBlurIntersector1::Precalculations,cached> Precalculations;
      
      static __forceinline bool processLazyNode(Precalculations& pre, Ray& ray, IntersectContext* context, const Primitive* prim_i, size_t& lazy_node)
      {
        Primitive* prim = (Primitive*) prim_i;
        GridSOA* grid = nullptr;
        if (cached) 
        {
          Scene* scene = context->scene;
          if (pre.grid) SharedLazyTessellationCache::sharedLazyTessellationCache.unlock();
          grid = (GridSOA*) SharedLazyTessellationCache::lookup(prim->entry(),scene->commitCounterSubdiv,[&] () {
              auto alloc = [] (const size_t bytes) { return SharedLazyTessellationCache::sharedLazyTessellationCache.malloc(bytes); };
              return GridSOA::create((SubdivPatch1Base*)prim,(unsigned)scene->get<SubdivMesh>(prim->geom)->numTimeSteps,pre.numTimeSteps(),scene,alloc);
            });
        }
        else {
          grid = (GridSOA*) prim->root_ref.get();
        }
        lazy_node = grid->root(pre.itime());
        pre.grid = grid;
        return false;
      }

      /*! Intersect a ray with the primitive. */
      static __forceinline void intersect(Precalculations& pre, Ray& ray, IntersectContext* context, const Primitive* prim, size_t ty, size_t& lazy_node) 
      {
        if (likely(ty == 0)) GridSOAMBlurIntersector1::intersect(pre,ray,context,prim,ty,lazy_node);
        else                 processLazyNode(pre,ray,context,prim,lazy_node);
      }
      static __forceinline void intersect(Precalculations& pre, Ray& ray, IntersectContext* context, size_t ty0, const Primitive* prim, size_t ty, size_t& lazy_node) {
        intersect(pre,ray,context,prim,ty,lazy_node);
      }
      
      /*! Test if the ray is occluded by the primitive */
      static __forceinline bool occluded(Precalculations& pre, Ray& ray, IntersectContext* context, const Primitive* prim, size_t ty, size_t& lazy_node) 
      {
        if (likely(ty == 0)) return GridSOAMBlurIntersector1::occluded(pre,ray,context,prim,ty,lazy_node);
        else                 return processLazyNode(pre,ray,context,prim,lazy_node);
      }
      static __forceinline bool occluded(Precalculations& pre, Ray& ray, IntersectContext* context, size_t ty0, const Primitive* prim, size_t ty, size_t& lazy_node) {
        return occluded(pre,ray,context,prim,ty,lazy_node);
      }
    };

    template <int K, bool cached>
      struct SubdivPatch1CachedIntersectorK
    {
      typedef SubdivPatch1Cached Primitive;
      typedef SubdivPatch1CachedPrecalculationsK<K,typename GridSOAIntersectorK<K>::Precalculations,cached> Precalculations;
      
      static __forceinline bool processLazyNode(Precalculations& pre, IntersectContext* context, const Primitive* prim_i, size_t& lazy_node)
      {
        Primitive* prim = (Primitive*) prim_i;
        GridSOA* grid = nullptr;
        if (cached)
        {
          Scene* scene = context->scene;
          if (pre.grid) SharedLazyTessellationCache::sharedLazyTessellationCache.unlock();
          grid = (GridSOA*) SharedLazyTessellationCache::lookup(prim->entry(),scene->commitCounterSubdiv,[&] () {
              auto alloc = [] (const size_t bytes) { return SharedLazyTessellationCache::sharedLazyTessellationCache.malloc(bytes); };
              return GridSOA::create((SubdivPatch1Base*)prim,1,1,scene,alloc);
            });
        }
        else {
          grid = (GridSOA*) prim->root_ref.get();
        }
        lazy_node = grid->root(0);
        pre.grid = grid;
        return false;
      }
      
      static __forceinline void intersect(const vbool<K>& valid, Precalculations& pre, RayK<K>& ray, IntersectContext* context, const Primitive* prim, size_t ty, size_t& lazy_node)
      {
        if (likely(ty == 0)) GridSOAIntersectorK<K>::intersect(valid,pre,ray,context,prim,ty,lazy_node);
        else                 processLazyNode(pre,context,prim,lazy_node);
      }
      
      static __forceinline vbool<K> occluded(const vbool<K>& valid, Precalculations& pre, RayK<K>& ray, IntersectContext* context, const Primitive* prim, size_t ty, size_t& lazy_node)
      {
        if (likely(ty == 0)) return GridSOAIntersectorK<K>::occluded(valid,pre,ray,context,prim,ty,lazy_node);
        else                 return processLazyNode(pre,context,prim,lazy_node);
      }
      
      static __forceinline void intersect(Precalculations& pre, RayK<K>& ray, size_t k, IntersectContext* context, const Primitive* prim, size_t ty, size_t& lazy_node)
      {
        if (likely(ty == 0)) GridSOAIntersectorK<K>::intersect(pre,ray,k,context,prim,ty,lazy_node);
        else                 processLazyNode(pre,context,prim,lazy_node);
      }
      
      static __forceinline bool occluded(Precalculations& pre, RayK<K>& ray, size_t k, IntersectContext* context, const Primitive* prim, size_t ty, size_t& lazy_node)
      {
        if (likely(ty == 0)) return GridSOAIntersectorK<K>::occluded(pre,ray,k,context,prim,ty,lazy_node);
        else                 return processLazyNode(pre,context,prim,lazy_node);
      }
    };

    typedef SubdivPatch1CachedIntersectorK<4,false>  SubdivPatch1Intersector4;
    typedef SubdivPatch1CachedIntersectorK<8,false>  SubdivPatch1Intersector8;
    typedef SubdivPatch1CachedIntersectorK<16,false> SubdivPatch1Intersector16;

    typedef SubdivPatch1CachedIntersectorK<4,true>  SubdivPatch1CachedIntersector4;
    typedef SubdivPatch1CachedIntersectorK<8,true>  SubdivPatch1CachedIntersector8;
    typedef SubdivPatch1CachedIntersectorK<16,true> SubdivPatch1CachedIntersector16;

    template <int K, bool cached>
      struct SubdivPatch1MBlurCachedIntersectorK
    {
      typedef SubdivPatch1Cached Primitive;
      typedef SubdivPatch1CachedPrecalculationsK<K,typename GridSOAMBlurIntersectorK<K>::Precalculations,cached> Precalculations;
      
      static __forceinline bool processLazyNode(Precalculations& pre, RayK<K>& ray, size_t k, IntersectContext* context, const Primitive* prim_i, size_t& lazy_node)
      {
        Primitive* prim = (Primitive*) prim_i;
        GridSOA* grid = nullptr;
        if (cached)
        {
          Scene* scene = context->scene;
          if (pre.grid) SharedLazyTessellationCache::sharedLazyTessellationCache.unlock();
          grid = (GridSOA*) SharedLazyTessellationCache::lookup(prim->entry(),scene->commitCounterSubdiv,[&] () {
              auto alloc = [] (const size_t bytes) { return SharedLazyTessellationCache::sharedLazyTessellationCache.malloc(bytes); };
              return GridSOA::create((SubdivPatch1Base*)prim,(unsigned)scene->get<SubdivMesh>(prim->geom)->numTimeSteps,pre.numTimeSteps(),scene,alloc);
            });
        }
        else {
          grid = (GridSOA*) prim->root_ref.get();
        }
        lazy_node = grid->root(pre.itime(k));
        pre.grid = grid;
        return false;
      }
      
      static __forceinline void intersect(Precalculations& pre, RayK<K>& ray, size_t k, IntersectContext* context, const Primitive* prim, size_t ty, size_t& lazy_node)
      {
        if (likely(ty == 0)) GridSOAMBlurIntersectorK<K>::intersect(pre,ray,k,context,prim,ty,lazy_node);
        else                 processLazyNode(pre,ray,k,context,prim,lazy_node);
      }
      
      static __forceinline bool occluded(Precalculations& pre, RayK<K>& ray, size_t k, IntersectContext* context, const Primitive* prim, size_t ty, size_t& lazy_node)
      {
        if (likely(ty == 0)) return GridSOAMBlurIntersectorK<K>::occluded(pre,ray,k,context,prim,ty,lazy_node);
        else                 return processLazyNode(pre,ray,k,context,prim,lazy_node);
      }
    };

    typedef SubdivPatch1MBlurCachedIntersectorK<4,false>  SubdivPatch1MBlurIntersector4;
    typedef SubdivPatch1MBlurCachedIntersectorK<8,false>  SubdivPatch1MBlurIntersector8;
    typedef SubdivPatch1MBlurCachedIntersectorK<16,false> SubdivPatch1MBlurIntersector16;

    typedef SubdivPatch1MBlurCachedIntersectorK<4,true>  SubdivPatch1MBlurCachedIntersector4;
    typedef SubdivPatch1MBlurCachedIntersectorK<8,true>  SubdivPatch1MBlurCachedIntersector8;
    typedef SubdivPatch1MBlurCachedIntersectorK<16,true> SubdivPatch1MBlurCachedIntersector16;
  }
}
