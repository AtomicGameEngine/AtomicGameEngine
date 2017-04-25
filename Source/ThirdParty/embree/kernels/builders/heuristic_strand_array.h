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

#include "priminfo.h"
#include "../geometry/bezier1v.h"

#include "../../common/algorithms/parallel_reduce.h"
#include "../../common/algorithms/parallel_partition.h"

namespace embree
{
  namespace isa
  { 
    /*! Performs standard object binning */
    struct HeuristicStrandSplit
    {
      typedef range<size_t> Set;
  
      static const size_t PARALLEL_THRESHOLD = 10000;
      static const size_t PARALLEL_FIND_BLOCK_SIZE = 4096;
      static const size_t PARALLEL_PARTITION_BLOCK_SIZE = 64;

      /*! stores all information to perform some split */
      struct Split
      {    
	/*! construct an invalid split by default */
	__forceinline Split()
	  : sah(inf), axis0(zero), axis1(zero) {}
	
	/*! constructs specified split */
	__forceinline Split(const float sah, const Vec3fa& axis0, const Vec3fa& axis1)
	  : sah(sah), axis0(axis0), axis1(axis1) {}
	
	/*! calculates standard surface area heuristic for the split */
	__forceinline float splitSAH() const { return sah; }

        /*! test if this split is valid */
        __forceinline bool valid() const { return sah != float(inf); }
		
      public:
	float sah;             //!< SAH cost of the split
	Vec3fa axis0, axis1;   //!< axis the two strands are aligned into
      };

      __forceinline HeuristicStrandSplit () // FIXME: required?
        : scene(nullptr), prims(nullptr) {}
      
      /*! remember prim array */
      __forceinline HeuristicStrandSplit (Scene* scene, PrimRef* prims)
        : scene(scene), prims(prims) {}
      
      /*! finds the best split */
      const Split find(const range<size_t>& set)
      {
        if (likely(set.size() < PARALLEL_THRESHOLD)) return sequential_find(set);
        else                                         return   parallel_find(set);
      }
      
      __forceinline const Vec3fa direction(const PrimRef& prim)
      {
        const Curve3fa curve = scene->get<NativeCurves>(prim.geomID())->getCurve(prim.primID());
        return curve.end()-curve.begin();
      }
      
      __forceinline const BBox3fa bounds(const PrimRef& prim)
      {
        NativeCurves* curves = scene->get<NativeCurves>(prim.geomID());
        return curves->bounds(prim.primID());
      }

      __forceinline const BBox3fa bounds(const LinearSpace3fa& space, const PrimRef& prim)
      {
        NativeCurves* curves = scene->get<NativeCurves>(prim.geomID());
        return curves->bounds(space,prim.primID());
      }

      /*! finds the best split */
      const Split sequential_find(const range<size_t>& set)
      {
        /* first curve determines first axis */
        Vec3fa axis0 = normalize(direction(prims[set.begin()]));
      
        /* find 2nd axis that is most misaligned with first axis */
        float bestCos = 1.0f;
        Vec3fa axis1 = axis0;
        for (size_t i=set.begin(); i<set.end(); i++) 
        {
          Vec3fa axisi = direction(prims[i]);
          float leni = length(axisi);
          if (leni == 0.0f) continue;
          axisi /= leni;
          float cos = abs(dot(axisi,axis0));
          if (cos < bestCos) { bestCos = cos; axis1 = axisi; }
        }
      
        /* partition the two strands */
        size_t lnum = 0, rnum = 0;
        BBox3fa lbounds = empty, rbounds = empty;
        const LinearSpace3fa space0 = frame(axis0).transposed();
        const LinearSpace3fa space1 = frame(axis1).transposed();
        
        for (size_t i=set.begin(); i<set.end(); i++)
        {
          PrimRef& prim = prims[i];
          const Vec3fa axisi = normalize(direction(prim));
          const float cos0 = abs(dot(axisi,axis0));
          const float cos1 = abs(dot(axisi,axis1));
          
          if (cos0 > cos1) { lnum++; lbounds.extend(bounds(space0,prim)); }
          else             { rnum++; rbounds.extend(bounds(space1,prim)); }
        }
      
        /*! return an invalid split if we do not partition */
        if (lnum == 0 || rnum == 0) 
          return Split(inf,axis0,axis1);
      
        /*! calculate sah for the split */
        const float sah = madd(float(lnum),halfArea(lbounds),float(rnum)*halfArea(rbounds));
        return Split(sah,axis0,axis1);
      }

      /*! finds the best split */
      const Split parallel_find(const range<size_t>& set)
      {
        /* first curve determines first axis */
        const Vec3fa axis0 = normalize(direction(prims[set.begin()]));
      
        /* find 2nd axis that is most misaligned with first axis */
        struct __aligned(16) BestAxis
        {
          __forceinline BestAxis () 
            : cos(inf), axis(Vec3fa(1.0f,0.0f,0.0f)) {}

          __forceinline BestAxis (float otherCos, const Vec3fa& otherAxis) 
            : cos(otherCos), axis(otherAxis) {}

        public:
          float cos;
          Vec3fa axis;
        };
        
        const BestAxis best = parallel_reduce
          (set.begin(),set.end(),size_t(1024),BestAxis(inf,axis0),
           [&] (const range<size_t>& r) -> BestAxis
           {
             BestAxis best(inf,axis0);
             for (size_t i=r.begin(); i<r.end(); i++)
             {
               Vec3fa axisi = direction(prims[i]);
               float leni = length(axisi);
               if (leni == 0.0f) continue;
               axisi /= leni;
               float cos = abs(dot(axisi,axis0));
               if (cos < best.cos) { best.cos = cos; best.axis = axisi; }
             }
             return best;
           }, [] (const BestAxis& axis0, const BestAxis& axis1) -> BestAxis {
            if (axis0.cos < axis1.cos) return axis0; else return axis1;
          });
        const Vec3fa axis1 = best.axis;
        
        /* partition the two strands */
        struct __aligned(16) Info
        {
          __forceinline Info() 
            : lnum(0), rnum(0), lbounds(empty), rbounds(empty) {}

		  __forceinline static Info merge(const Info& a, const Info& b) 
          {
            Info c;
            c.lnum = a.lnum+b.lnum;
            c.rnum = a.rnum+b.rnum;
            c.lbounds = embree::merge(a.lbounds,b.lbounds);
            c.rbounds = embree::merge(a.rbounds,b.rbounds);
            return c;
          }
          size_t lnum, rnum;
          BBox3fa lbounds, rbounds;
        };
        const LinearSpace3fa space0 = frame(axis0).transposed();
        const LinearSpace3fa space1 = frame(axis1).transposed();
        
        Info info = parallel_reduce
          (set.begin(), set.end(), size_t(1024), Info(), 
           [&] (const range<size_t>& r) -> Info
           {
             Info info;
             for (size_t i=r.begin(); i<r.end(); i++)
             {
               PrimRef& prim = prims[i];
               const Vec3fa axisi = normalize(direction(prim));
               const float cos0 = abs(dot(axisi,axis0));
               const float cos1 = abs(dot(axisi,axis1));
               
               if (cos0 > cos1) { info.lnum++; info.lbounds.extend(bounds(space0,prim)); }
               else             { info.rnum++; info.rbounds.extend(bounds(space1,prim)); }
             }
             return info;
           },
           [] (const Info& info0, const Info& info1) {
             return Info::merge(info0,info1);
           });
      
        /*! return an invalid split if we do not partition */
        if (info.lnum == 0 || info.rnum == 0) 
          return Split(inf,axis0,axis1);
      
        /*! calculate sah for the split */
        const float sah = madd(float(info.lnum),halfArea(info.lbounds),float(info.rnum)*halfArea(info.rbounds));
        return Split(sah,axis0,axis1);
      }
      
      /*! array partitioning */
      void split(const Split& spliti, const PrimInfoRange& set, PrimInfoRange& lset, PrimInfoRange& rset) 
      {
        if (likely(set.size() < PARALLEL_THRESHOLD)) 
          sequential_split(spliti,set,lset,rset);
        else
          parallel_split(spliti,set,lset,rset);
      }
      
      /*! array partitioning */
      void sequential_split(const Split& split, const PrimInfoRange& set, PrimInfoRange& lset, PrimInfoRange& rset) 
      {
        if (!split.valid()) {
          deterministic_order(set);
          return splitFallback(set,lset,rset);
        }
        
        const size_t begin = set.begin();
        const size_t end   = set.end();
        CentGeomBBox3fa local_left(empty);
        CentGeomBBox3fa local_right(empty);

        auto primOnLeftSide = [&] (const PrimRef& prim) -> bool { 
          const Vec3fa axisi = normalize(direction(prim));
          const float cos0 = abs(dot(axisi,split.axis0));
          const float cos1 = abs(dot(axisi,split.axis1));
          return cos0 > cos1;
        };

        auto mergePrimBounds = [this] (CentGeomBBox3fa& pinfo,const PrimRef& ref) { 
          pinfo.extend(bounds(ref)); 
        };
        
        size_t center = serial_partitioning(prims,begin,end,local_left,local_right,primOnLeftSide,mergePrimBounds);
        
        new (&lset) PrimInfoRange(begin,center,local_left.geomBounds,local_left.centBounds);
        new (&rset) PrimInfoRange(center,end,local_right.geomBounds,local_right.centBounds);
        assert(area(lset.geomBounds) >= 0.0f);
        assert(area(rset.geomBounds) >= 0.0f);
      }

      /*! array partitioning */
      void parallel_split(const Split& split, const PrimInfoRange& set, PrimInfoRange& lset, PrimInfoRange& rset)
      {
        if (!split.valid()) {
          deterministic_order(set);
          return splitFallback(set,lset,rset);
        }
        
        const size_t begin = set.begin();
        const size_t end   = set.end();

        auto primOnLeftSide = [&] (const PrimRef& prim) -> bool { 
          const Vec3fa axisi = normalize(direction(prim));
          const float cos0 = abs(dot(axisi,split.axis0));
          const float cos1 = abs(dot(axisi,split.axis1));
          return cos0 > cos1;
        };

        PrimInfo linfo; linfo.reset(); // FIXME: use CentGeomBBox3fa
        PrimInfo rinfo; rinfo.reset();
        const size_t center = parallel_partitioning(
          prims,begin,end,EmptyTy(),linfo,rinfo,primOnLeftSide,
          [this] (PrimInfo &pinfo, const PrimRef& ref) { pinfo.add(bounds(ref)); },
          [] (PrimInfo &pinfo0,const PrimInfo& pinfo1) { pinfo0.merge(pinfo1); },
          PARALLEL_PARTITION_BLOCK_SIZE);
        
        new (&lset) PrimInfoRange(begin,center,linfo.geomBounds,linfo.centBounds);
        new (&rset) PrimInfoRange(center,end,rinfo.geomBounds,rinfo.centBounds);
      }
      
      void deterministic_order(const Set& set) 
      {
        /* required as parallel partition destroys original primitive order */
        std::sort(&prims[set.begin()],&prims[set.end()]);
      }
      
      void splitFallback(const Set& set, PrimInfoRange& lset, PrimInfoRange& rset)
      {
        const size_t begin = set.begin();
        const size_t end   = set.end();
        const size_t center = (begin + end)/2;
        
        CentGeomBBox3fa left; left.reset();
        for (size_t i=begin; i<center; i++)
          left.extend(bounds(prims[i]));
        new (&lset) PrimInfoRange(begin,center,left.geomBounds,left.centBounds);
        
        CentGeomBBox3fa right; right.reset();
        for (size_t i=center; i<end; i++)
          right.extend(bounds(prims[i]));	
        new (&rset) PrimInfoRange(center,end,right.geomBounds,right.centBounds);
      }
      
    private:
      Scene* const scene;
      PrimRef* const prims;
    };
  }
}
