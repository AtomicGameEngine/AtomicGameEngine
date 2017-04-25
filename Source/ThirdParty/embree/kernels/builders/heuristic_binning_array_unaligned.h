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

#include "heuristic_binning.h"

namespace embree
{
  namespace isa
  { 
    /*! Performs standard object binning */
    template<typename PrimRef, size_t BINS>
      struct UnalignedHeuristicArrayBinningSAH
      {
        typedef BinSplit<BINS> Split;
        typedef BinInfoT<BINS,PrimRef,BBox3fa> Binner;
        typedef range<size_t> Set;

        __forceinline UnalignedHeuristicArrayBinningSAH () // FIXME: required?
          : scene(nullptr), prims(nullptr) {}
        
        /*! remember prim array */
        __forceinline UnalignedHeuristicArrayBinningSAH (Scene* scene, PrimRef* prims)
          : scene(scene), prims(prims) {}

        const LinearSpace3fa computeAlignedSpace(const range<size_t>& set)
        {
          /*! find first curve that defines valid direction */
          Vec3fa axis(0,0,1);
          for (size_t i=set.begin(); i<set.end(); i++)
          {
            NativeCurves* mesh = (NativeCurves*) scene->get(prims[i].geomID());
            const unsigned vtxID = mesh->curve(prims[i].primID());
            const Vec3fa v0 = mesh->vertex(vtxID+0);
            const Vec3fa v1 = mesh->vertex(vtxID+1);
            const Vec3fa v2 = mesh->vertex(vtxID+2);
            const Vec3fa v3 = mesh->vertex(vtxID+3);
            const Curve3fa curve(v0,v1,v2,v3);
            const Vec3fa p0 = curve.begin();
            const Vec3fa p3 = curve.end();
            const Vec3fa axis1 = normalize(p3 - p0);
            if (sqr_length(p3-p0) > 1E-18f) {
              axis = axis1;
              break;
            }
          }
          return frame(axis).transposed();
        }

        const PrimInfo computePrimInfo(const range<size_t>& set, const LinearSpace3fa& space)
        {
          auto computeBounds = [&](const range<size_t>& r) -> CentGeomBBox3fa
            {
              CentGeomBBox3fa bounds(empty);
              for (size_t i=r.begin(); i<r.end(); i++) {
                NativeCurves* mesh = (NativeCurves*) scene->get(prims[i].geomID());
                bounds.extend(mesh->bounds(space,prims[i].primID()));
              }
              return bounds;
            };
          
          const CentGeomBBox3fa bounds = parallel_reduce(set.begin(), set.end(), size_t(1024), size_t(4096), 
                                                         CentGeomBBox3fa(empty), computeBounds, CentGeomBBox3fa::merge2);

          return PrimInfo(set.begin(),set.end(),bounds.geomBounds,bounds.centBounds);
        }
        
        /*! finds the best split */
        __forceinline const Split find(const PrimInfoRange& pinfo, const size_t logBlockSize, const LinearSpace3fa& space)
        {
          if (likely(pinfo.size() < 10000))
            return find_template<false>(pinfo,logBlockSize,space);
          else
            return find_template<true>(pinfo,logBlockSize,space);
        }

        /*! finds the best split */
        template<bool parallel>
        const Split find_template(const PrimInfoRange& set, const size_t logBlockSize, const LinearSpace3fa& space)
        {
          Binner binner(empty);
          const BinMapping<BINS> mapping(set);
          bin_serial_or_parallel<parallel>(binner,prims,set.begin(),set.end(),size_t(4096),mapping,space,scene);
          return binner.best(mapping,logBlockSize);
        }
        
        /*! array partitioning */
        __forceinline void split(const Split& split, const LinearSpace3fa& space, const Set& set, PrimInfoRange& lset, PrimInfoRange& rset)
        {
          if (likely(set.size() < 10000))
            split_template<false>(split,space,set,lset,rset);
          else
            split_template<true>(split,space,set,lset,rset);
        }

        /*! array partitioning */
        template<bool parallel>
        __forceinline void split_template(const Split& split, const LinearSpace3fa& space, const Set& set, PrimInfoRange& lset, PrimInfoRange& rset)
        {
          if (!split.valid()) {
            deterministic_order(set);
            return splitFallback(set,lset,rset);
          }
          
          const size_t begin = set.begin();
          const size_t end   = set.end();
          CentGeomBBox3fa local_left(empty);
          CentGeomBBox3fa local_right(empty);
          const int splitPos = split.pos;
          const int splitDim = split.dim;

          size_t center = 0;
          if (likely(set.size() < 10000))
            center = serial_partitioning(prims,begin,end,local_left,local_right,
                                         [&] (const PrimRef& ref) { return split.mapping.bin_unsafe(ref,space,scene)[splitDim] < splitPos; },
                                         [] (CentGeomBBox3fa& pinfo,const PrimRef& ref) { pinfo.extend(ref.bounds()); });
          else
            center = parallel_partitioning(prims,begin,end,EmptyTy(),local_left,local_right,
                                           [&] (const PrimRef& ref) { return split.mapping.bin_unsafe(ref,space,scene)[splitDim] < splitPos; },
                                           [] (CentGeomBBox3fa& pinfo,const PrimRef& ref) { pinfo.extend(ref.bounds()); },
                                           [] (CentGeomBBox3fa& pinfo0,const CentGeomBBox3fa& pinfo1) { pinfo0.merge(pinfo1); },
                                           128);
          
          new (&lset) PrimInfoRange(begin,center,local_left.geomBounds,local_left.centBounds);
          new (&rset) PrimInfoRange(center,end,local_right.geomBounds,local_right.centBounds);
          assert(area(lset.geomBounds) >= 0.0f);
          assert(area(rset.geomBounds) >= 0.0f);
        }
        
        void deterministic_order(const range<size_t>& set) 
        {
          /* required as parallel partition destroys original primitive order */
          std::sort(&prims[set.begin()],&prims[set.end()]);
        }
        
        void splitFallback(const range<size_t>& set, PrimInfoRange& lset, PrimInfoRange& rset)
        {
          const size_t begin = set.begin();
          const size_t end   = set.end();
          const size_t center = (begin + end)/2;
          
          CentGeomBBox3fa left; left.reset();
          for (size_t i=begin; i<center; i++)
            left.extend(prims[i].bounds());
          new (&lset) PrimInfoRange(begin,center,left.geomBounds,left.centBounds);
          
          CentGeomBBox3fa right; right.reset();
          for (size_t i=center; i<end; i++)
            right.extend(prims[i].bounds());	
          new (&rset) PrimInfoRange(center,end,right.geomBounds,right.centBounds);
        }
        
      private:
        Scene* const scene;
        PrimRef* const prims;
      };
  }
}
