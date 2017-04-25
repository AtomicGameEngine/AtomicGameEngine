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

#include "bvh.h"

#if defined(EMBREE_GEOMETRY_SUBDIV)

#include "bvh_refit.h"
#include "bvh_builder.h"

#include "../builders/primrefgen.h"
#include "../builders/bvh_builder_sah.h"

#include "../../common/algorithms/parallel_for_for.h"
#include "../../common/algorithms/parallel_for_for_prefix_sum.h"

#include "../subdiv/bezier_curve.h"
#include "../subdiv/bspline_curve.h"

#include "../geometry/subdivpatch1cached.h"
#include "../geometry/grid_soa.h"

namespace embree
{
  namespace isa
  {
    typedef FastAllocator::ThreadLocal2 Allocator;

    template<int N>
    struct BVHNSubdivPatch1EagerBuilderBinnedSAHClass : public Builder
    {
      ALIGNED_STRUCT;

      typedef BVHN<N> BVH;
      typedef typename BVH::NodeRef NodeRef;

      BVH* bvh;
      Scene* scene;
      mvector<PrimRef> prims;
      ParallelForForPrefixSumState<PrimInfo> pstate;
      
      BVHNSubdivPatch1EagerBuilderBinnedSAHClass (BVH* bvh, Scene* scene)
        : bvh(bvh), scene(scene), prims(scene->device) {}

#define SUBGRID 9

      static unsigned getNumEagerLeaves(unsigned width, unsigned height) {
        const unsigned w = (width+SUBGRID-1)/SUBGRID;
        const unsigned h = (height+SUBGRID-1)/SUBGRID;
        return w*h;
      }

      __forceinline static unsigned createEager(SubdivPatch1Base& patch, Scene* scene, SubdivMesh* mesh, unsigned primID, FastAllocator::ThreadLocal& alloc, PrimRef* prims)
      {
        unsigned NN = 0;
        const unsigned x0 = 0, x1 = patch.grid_u_res-1;
        const unsigned y0 = 0, y1 = patch.grid_v_res-1;
        
        for (unsigned y=y0; y<y1; y+=SUBGRID-1)
        {
          for (unsigned x=x0; x<x1; x+=SUBGRID-1) 
          {
            const unsigned lx0 = x, lx1 = min(lx0+SUBGRID-1,x1);
            const unsigned ly0 = y, ly1 = min(ly0+SUBGRID-1,y1);
            BBox3fa bounds;
            GridSOA* leaf = GridSOA::create(&patch,1,1,lx0,lx1,ly0,ly1,scene,alloc,&bounds);
            *prims = PrimRef(bounds,BVH4::encodeTypedLeaf(leaf,1)); prims++;
            NN++;
          }
        }
        return NN;
      }

      void build() 
      {
        /* initialize all half edge structures */
        const size_t numPrimitives = scene->getNumPrimitives<SubdivMesh,false>();
        if (numPrimitives > 0 || scene->isInterpolatable()) {
          Scene::Iterator<SubdivMesh> iter(scene,scene->isInterpolatable());
          parallel_for(size_t(0),iter.size(),[&](const range<size_t>& range) {
              for (size_t i=range.begin(); i<range.end(); i++)
                if (iter[i]) iter[i]->initializeHalfEdgeStructures();
            });
        }

        /* skip build for empty scene */
        if (numPrimitives == 0) {
          prims.resize(numPrimitives);
          bvh->set(BVH::emptyNode,empty,0);
          return;
        }
        //bvh->alloc.reset();
        bvh->alloc.init_estimate(numPrimitives*sizeof(PrimRef));


        double t0 = bvh->preBuild(TOSTRING(isa) "::BVH" + toString(N) + "SubdivPatch1EagerBuilderBinnedSAH");

        auto progress = [&] (size_t dn) { bvh->scene->progressMonitor(double(dn)); };
        auto virtualprogress = BuildProgressMonitorFromClosure(progress);

        /* initialize allocator and parallel_for_for_prefix_sum */
        Scene::Iterator<SubdivMesh> iter(scene);
        pstate.init(iter,size_t(1024));

        PrimInfo pinfo1 = parallel_for_for_prefix_sum( pstate, iter, PrimInfo(empty), [&](SubdivMesh* mesh, const range<size_t>& r, size_t k, const PrimInfo& base) -> PrimInfo
        { 
          size_t p = 0;
          size_t g = 0;
          for (size_t f=r.begin(); f!=r.end(); ++f) {          
            if (!mesh->valid(f)) continue;
            patch_eval_subdivision(mesh->getHalfEdge(0,f),[&](const Vec2f uv[4], const int subdiv[4], const float edge_level[4], int subPatch)
            {
              float level[4]; SubdivPatch1Base::computeEdgeLevels(edge_level,subdiv,level);
              Vec2i grid = SubdivPatch1Base::computeGridSize(level);
              size_t num = getNumEagerLeaves(grid.x,grid.y);
              g+=num;
              p++;
            });
          }
          return PrimInfo(p,g,empty,empty);
        }, [](const PrimInfo& a, const PrimInfo& b) -> PrimInfo { return PrimInfo(a.begin+b.begin,a.end+b.end,empty,empty); });
        size_t numSubPatches = pinfo1.begin;
        if (numSubPatches == 0) {
          bvh->set(BVH::emptyNode,empty,0);
          return;
        }

        prims.resize(pinfo1.end);
        if (pinfo1.end == 0) {
          bvh->set(BVH::emptyNode,empty,0);
          return;
        }

        PrimInfo pinfo3 = parallel_for_for_prefix_sum( pstate, iter, PrimInfo(empty), [&](SubdivMesh* mesh, const range<size_t>& r, size_t k, const PrimInfo& base) -> PrimInfo
        {
          FastAllocator::ThreadLocal& alloc = *bvh->alloc.threadLocal();
          
          PrimInfo s(empty);
          for (size_t f=r.begin(); f!=r.end(); ++f) {
            if (!mesh->valid(f)) continue;
            
            patch_eval_subdivision(mesh->getHalfEdge(0,f),[&](const Vec2f uv[4], const int subdiv[4], const float edge_level[4], int subPatch)
            {
              SubdivPatch1Base patch(mesh->id,unsigned(f),subPatch,mesh,0,uv,edge_level,subdiv,VSIZEX);
              size_t num = createEager(patch,scene,mesh,unsigned(f),alloc,&prims[base.end+s.end]);
              assert(num == getNumEagerLeaves(patch.grid_u_res,patch.grid_v_res));
              for (size_t i=0; i<num; i++)
                s.add(prims[base.end+s.end].bounds());
              s.begin++;
            });
          }
          return s;
        }, [](const PrimInfo& a, const PrimInfo& b) -> PrimInfo { return PrimInfo::merge(a, b); });

        PrimInfo pinfo(pinfo3.end,pinfo3.geomBounds,pinfo3.centBounds);
        
        auto createLeaf = [&] (const BVHBuilderBinnedSAH::BuildRecord& current, Allocator* alloc) -> NodeRef {
          assert(current.prims.size() == 1);
          size_t leaf = (size_t) prims[current.prims.begin()].ID();
          return NodeRef(leaf);
        };

        /* settings for BVH build */
        GeneralBVHBuilder::Settings settings;
        settings.logBlockSize = __bsr(N);
        settings.minLeafSize = 1;
        settings.maxLeafSize = 1;
        settings.travCost = 1.0f;
        settings.intCost = 1.0f;
        settings.singleThreadThreshold = DEFAULT_SINGLE_THREAD_THRESHOLD;

        NodeRef root = BVHNBuilderVirtual<N>::build(&bvh->alloc,createLeaf,virtualprogress,prims.data(),pinfo,settings);
        bvh->set(root,LBBox3fa(pinfo.geomBounds),pinfo.size());
        bvh->layoutLargeNodes(size_t(pinfo.size()*0.005f));
        
	/* clear temporary data for static geometry */
	if (scene->isStatic()) {
          prims.clear();
          bvh->shrink();
        }
        bvh->cleanup();
        bvh->postBuild(t0);
      }

      void clear() {
        prims.clear();
      }
    };

    // =======================================================================================================
    // =======================================================================================================
    // =======================================================================================================


    template<int N, bool mblur>
    struct BVHNSubdivPatch1CachedBuilderBinnedSAHClass : public Builder, public BVHNRefitter<N>::LeafBoundsInterface
    {
      ALIGNED_STRUCT;

      typedef BVHN<N> BVH;
      typedef typename BVH::NodeRef NodeRef;
      typedef typename BVHN<N>::Allocator BVH_Allocator;

      BVH* bvh;
      std::unique_ptr<BVHNRefitter<N>> refitter;
      Scene* scene;
      size_t numTimeSteps;
      mvector<PrimRef> prims; 
      mvector<BBox3fa> bounds; 
      ParallelForForPrefixSumState<PrimInfo> pstate;
      size_t numSubdivEnableDisableEvents;
      bool cached;

      BVHNSubdivPatch1CachedBuilderBinnedSAHClass (BVH* bvh, Scene* scene, bool cached)
        : bvh(bvh), refitter(nullptr), scene(scene), numTimeSteps(0), prims(scene->device), bounds(scene->device), numSubdivEnableDisableEvents(0), cached(cached) {}
      
      virtual const BBox3fa leafBounds (NodeRef& ref) const
      {
        if (ref == BVH::emptyNode) return BBox3fa(empty);
        size_t num; SubdivPatch1Cached *sptr = (SubdivPatch1Cached*)ref.leaf(num);
        const size_t index = ((size_t)sptr - (size_t)bvh->subdiv_patches.data()) / sizeof(SubdivPatch1Cached);
        return prims[index].bounds(); 
      }

      bool initializeHalfEdges(size_t& numPrimitives)
      {
        /* initialize all half edge structures */
        bool fastUpdateMode = true;
        numPrimitives = scene->getNumPrimitives<SubdivMesh,mblur>();
        if (numPrimitives > 0 || scene->isInterpolatable()) 
        {
          Scene::Iterator<SubdivMesh,mblur> iter(scene,scene->isInterpolatable());
          fastUpdateMode = parallel_reduce(size_t(0),iter.size(),true,[&](const range<size_t>& range)
          {
            bool fastUpdate = true;
            for (size_t i=range.begin(); i<range.end(); i++)
            {
              if (!iter[i]) continue;
              fastUpdate &= !iter[i]->faceVertices.isModified();
              fastUpdate &= !iter[i]->holes.isModified();
              fastUpdate &= iter[i]->levels.isModified();
              fastUpdate &= !iter[i]->topology[0].vertexIndices.isModified(); 
              fastUpdate &= !iter[i]->edge_creases.isModified();
              fastUpdate &= !iter[i]->edge_crease_weights.isModified();
              fastUpdate &= !iter[i]->vertex_creases.isModified();
              fastUpdate &= !iter[i]->vertex_crease_weights.isModified();               
              iter[i]->initializeHalfEdgeStructures();
              //iter[i]->patch_eval_trees.resize(iter[i]->size()*numTimeSteps);
            }
            return fastUpdate;
          }, [](const bool a, const bool b) { return a && b; });
        }

        /* only enable fast mode if no subdiv mesh got enabled or disabled since last run */
        fastUpdateMode &= numSubdivEnableDisableEvents == scene->numSubdivEnableDisableEvents;
        numSubdivEnableDisableEvents = scene->numSubdivEnableDisableEvents;
        return fastUpdateMode;
      }

      void countSubPatches(size_t& numSubPatches, size_t& numSubPatchesMB)
      {
        Scene::Iterator<SubdivMesh,mblur> iter(scene);
        pstate.init(iter,size_t(1024));

        PrimInfo pinfo = parallel_for_for_prefix_sum( pstate, iter, PrimInfo(empty), [&](SubdivMesh* mesh, const range<size_t>& r, size_t k, const PrimInfo& base) -> PrimInfo
        { 
          size_t s = 0;
          size_t sMB = 0;
          for (size_t f=r.begin(); f!=r.end(); ++f) 
          {          
            if (!mesh->valid(f)) continue;
            size_t count = patch_eval_subdivision_count(mesh->getHalfEdge(0,f));
            s += count;
            sMB += count * mesh->numTimeSteps;
          }
          return PrimInfo(s,sMB,empty,empty);
        }, [](const PrimInfo& a, const PrimInfo& b) -> PrimInfo { return PrimInfo(a.begin+b.begin,a.end+b.end,empty,empty); });

        numSubPatches = pinfo.begin;
        numSubPatchesMB = pinfo.end;
      }

      PrimInfo updatePrimRefArray(size_t t)
      {
        SubdivPatch1Cached* const subdiv_patches = (SubdivPatch1Cached*) bvh->subdiv_patches.data();

        const PrimInfo pinfo = parallel_reduce(size_t(0), prims.size(), PrimInfo(empty), [&] (const range<size_t>& r) -> PrimInfo
        {
          PrimInfo pinfo(empty);
          for (size_t i = r.begin(); i < r.end(); i++) {
            size_t patchIndexMB = prims[i].ID();

            BBox3fa bound;
            if (mblur) {
              size_t patchNumTimeSteps = scene->get<SubdivMesh>(subdiv_patches[patchIndexMB].geom)->numTimeSteps;
              Geometry::buildBounds([&] (size_t itime, BBox3fa& bbox) -> bool
                                    {
                                      bbox = bounds[patchIndexMB+itime];
                                      return true;
                                    },
                                    t, numTimeSteps, patchNumTimeSteps, bound);
            } else {
              bound = bounds[patchIndexMB];
            }

            prims[i] = PrimRef(bound,patchIndexMB);
            pinfo.add(bound);
          }
          return pinfo;
        }, [] (const PrimInfo& a, const PrimInfo& b) { return PrimInfo::merge(a,b); });

        return pinfo;
      }

      void rebuild(size_t numPrimitives)
      {
        SubdivPatch1Cached* const subdiv_patches = (SubdivPatch1Cached*) bvh->subdiv_patches.data();
        //bvh->alloc.reset();
        bvh->alloc.init_estimate(numPrimitives*sizeof(PrimRef)*numTimeSteps);

        Scene::Iterator<SubdivMesh,mblur> iter(scene);
        parallel_for_for_prefix_sum( pstate, iter, PrimInfo(empty), [&](SubdivMesh* mesh, const range<size_t>& r, size_t k, const PrimInfo& base) -> PrimInfo
        {
          size_t s = 0;
          size_t sMB = 0;
          for (size_t f=r.begin(); f!=r.end(); ++f) 
          {
            if (!mesh->valid(f)) continue;
            
            BVH_Allocator alloc(bvh);
            //for (size_t t=0; t<numTimeSteps; t++)
            //mesh->patch_eval_trees[f*numTimeSteps+t] = Patch3fa::create(alloc, mesh->getHalfEdge(0,f), mesh->getVertexBuffer(t).getPtr(), mesh->getVertexBuffer(t).getStride());

            patch_eval_subdivision(mesh->getHalfEdge(0,f),[&](const Vec2f uv[4], const int subdiv[4], const float edge_level[4], int subPatch)
            {
              const size_t patchIndex = base.begin+s;
              const size_t patchIndexMB = base.end+sMB;
              assert(patchIndex < numPrimitives);

              for (size_t t=0; t<mesh->numTimeSteps; t++)
              {
                SubdivPatch1Base& patch = subdiv_patches[patchIndexMB+t];
                new (&patch) SubdivPatch1Cached(mesh->id,unsigned(f),subPatch,mesh,t,uv,edge_level,subdiv,VSIZEX);
              }

              if (cached)
              {
                for (size_t t=0; t<mesh->numTimeSteps; t++)
                {
                  SubdivPatch1Base& patch = subdiv_patches[patchIndexMB+t];
                  BBox3fa bound = evalGridBounds(patch,0,patch.grid_u_res-1,0,patch.grid_v_res-1,patch.grid_u_res,patch.grid_v_res,mesh);
                  bounds[patchIndexMB+t] = bound;
                }
              }
              else
              {
                SubdivPatch1Base& patch0 = subdiv_patches[patchIndexMB];
                patch0.root_ref.set((int64_t) GridSOA::create(&patch0,(unsigned)mesh->numTimeSteps,(unsigned)numTimeSteps,scene,alloc,&bounds[patchIndexMB]));
              }

              prims[patchIndex] = PrimRef(empty,patchIndexMB);
              s++;
              sMB += mesh->numTimeSteps;
            });
          }
          return PrimInfo(s,sMB,empty,empty);
        }, [](const PrimInfo& a, const PrimInfo& b) -> PrimInfo { return PrimInfo(a.begin+b.begin,a.end+b.end,empty,empty); });

        auto virtualprogress = BuildProgressMonitorFromClosure([&] (size_t dn) { 
            //bvh->scene->progressMonitor(double(dn)); // FIXME: triggers GCC compiler bug
          });
        
        /* build normal BVH over patches */
        if (!mblur)
        {
          auto createLeaf = [&] (const BVHBuilderBinnedSAH::BuildRecord& current, Allocator* alloc) -> NodeRef {
            assert(current.prims.size() == 1);
            const size_t patchIndex = prims[current.prims.begin()].ID();
            return bvh->encodeLeaf((char*)&subdiv_patches[patchIndex],1);
          };

          /* create primrefs */
          const PrimInfo pinfo = updatePrimRefArray(0);

          /* settings for BVH build */
          GeneralBVHBuilder::Settings settings;
          settings.logBlockSize = __bsr(N);
          settings.minLeafSize = 1;
          settings.maxLeafSize = 1;
          settings.travCost = 1.0f;
          settings.intCost = 1.0f;
          settings.singleThreadThreshold = DEFAULT_SINGLE_THREAD_THRESHOLD;

          /* call BVH builder */
          NodeRef root = BVHNBuilderVirtual<N>::build(&bvh->alloc,createLeaf,virtualprogress,prims.data(),pinfo,settings);
          bvh->set(root,LBBox3fa(pinfo.geomBounds),pinfo.size());
          bvh->layoutLargeNodes(size_t(pinfo.size()*0.005f));
        }

        /* build MBlur BVH over patches */
        else
        {
          /* allocate buffers */
          const size_t numTimeSegments = bvh->numTimeSteps-1; assert(bvh->numTimeSteps > 1);
          NodeRef* roots = (NodeRef*) bvh->alloc.threadLocal()->malloc(sizeof(NodeRef)*numTimeSegments,BVH::byteNodeAlignment);
          
          /* build BVH for each timestep */
          avector<BBox3fa> boxes(bvh->numTimeSteps);
          size_t num_bvh_primitives = 0;
          for (size_t t=0; t<numTimeSegments; t++)
          {
            auto createLeaf = [&] (const BVHBuilderBinnedSAH::BuildRecord& current, Allocator* alloc) -> std::pair<NodeRef,LBBox3fa> {
              assert(current.prims.size() == 1);
              const size_t patchIndexMB = prims[current.prims.begin()].ID();
              SubdivPatch1Base& patch = subdiv_patches[patchIndexMB+0];
              NodeRef ref = bvh->encodeLeaf((char*)&patch,1);
              size_t patchNumTimeSteps = scene->get<SubdivMesh>(patch.geom)->numTimeSteps;
              LBBox3fa lbounds = Geometry::linearBounds([&] (size_t itime) { return bounds[patchIndexMB+itime]; },
                                                        t, numTimeSteps, patchNumTimeSteps);
              return std::make_pair(ref,lbounds);
            };

            /* create primrefs */
            const PrimInfo pinfo = updatePrimRefArray(t);

            /* settings for BVH build */
            GeneralBVHBuilder::Settings settings;
            settings.logBlockSize = __bsr(N);
            settings.minLeafSize = 1;
            settings.maxLeafSize = 1;
            settings.travCost = 1.0f;
            settings.intCost = 1.0f;
            settings.singleThreadThreshold = DEFAULT_SINGLE_THREAD_THRESHOLD;

            /* call BVH builder */
            NodeRef root; LBBox3fa tbounds;
            std::tie(root, tbounds) = BVHNBuilderMblurVirtual<N>::build(&bvh->alloc,createLeaf,bvh->scene->progressInterface,prims.data(),pinfo,settings);
            roots[t] = root;
            boxes[t+0] = tbounds.bounds0;
            boxes[t+1] = tbounds.bounds1;
            num_bvh_primitives = max(num_bvh_primitives,pinfo.size());
          }
          bvh->set(NodeRef((size_t)roots),LBBox3fa(boxes),num_bvh_primitives);
          bvh->msmblur = true;
        }
      }

      void cachedUpdate(size_t numPrimitives)
      {
        SubdivPatch1Cached* const subdiv_patches = (SubdivPatch1Cached*) bvh->subdiv_patches.data();

        Scene::Iterator<SubdivMesh,mblur> iter(scene);
        parallel_for_for_prefix_sum( pstate, iter, PrimInfo(empty), [&](SubdivMesh* mesh, const range<size_t>& r, size_t k, const PrimInfo& base) -> PrimInfo
        {
          size_t s = 0;
          for (size_t f=r.begin(); f!=r.end(); ++f) 
          {
            if (!mesh->valid(f)) continue;
            
            patch_eval_subdivision(mesh->getHalfEdge(0,f),[&](const Vec2f uv[4], const int subdiv[4], const float edge_level[4], int subPatch)
            {
              const size_t patchIndex = base.begin+s;
              assert(patchIndex < numPrimitives);
              SubdivPatch1Base& patch = subdiv_patches[patchIndex];
              BBox3fa bound = empty;
              
              bool grid_changed = patch.updateEdgeLevels(edge_level,subdiv,mesh,VSIZEX);
              if (grid_changed) {
                patch.resetRootRef();
                bound = evalGridBounds(patch,0,patch.grid_u_res-1,0,patch.grid_v_res-1,patch.grid_u_res,patch.grid_v_res,mesh);
              }
              else {
                bound = bounds[patchIndex];
              }

              bounds[patchIndex] = bound;
              prims[patchIndex] = PrimRef(bound,patchIndex);
              s++;
            });
          }
          return PrimInfo(s,s,empty,empty);
        }, [](const PrimInfo& a, const PrimInfo& b) -> PrimInfo { return PrimInfo(a.begin+b.begin,a.end+b.end,empty,empty); });

        /* refit BVH over patches */
        if (!refitter)
          refitter.reset(new BVHNRefitter<N>(bvh,*(typename BVHNRefitter<N>::LeafBoundsInterface*)this));
        
        refitter->refit();
      }

      void build() 
      {
        bvh->numTimeSteps = scene->getNumTimeSteps<SubdivMesh,mblur>();
        numTimeSteps = bvh->numTimeSteps;

        /* initialize all half edge structures */
        size_t numPatches;
        bool fastUpdateMode = initializeHalfEdges(numPatches);
        //static size_t counter = 0; if ((++counter) % 16 == 0) fastUpdateMode = false;

        /* skip build for empty scene */
        if (numPatches == 0) {
          prims.resize(numPatches);
          bounds.resize(numPatches);
          bvh->set(BVH::emptyNode,empty,0);
          return;
        }

        double t0 = bvh->preBuild(TOSTRING(isa) "::BVH" + toString(N) + "SubdivPatch1" + (mblur ? "MBlur" : "") + (cached ? "Cached" : "") + "BuilderBinnedSAH");
        
        /* calculate number of primitives (some patches need initial subdivision) */
        size_t numSubPatches, numSubPatchesMB;
        countSubPatches(numSubPatches, numSubPatchesMB);
        prims.resize(numSubPatches);
        bounds.resize(numSubPatchesMB);
        
        /* exit if there are no primitives to process */
        if (numSubPatches == 0) {
          bvh->set(BVH::emptyNode,empty,0);
          bvh->postBuild(t0);
          return;
        }
        
        /* Allocate memory for gregory and b-spline patches */
        bvh->subdiv_patches.resize(sizeof(SubdivPatch1Cached) * numSubPatchesMB);

        /* switch between fast and slow mode */
        if (mblur) rebuild(numSubPatches);
        else if (cached && fastUpdateMode) cachedUpdate(numSubPatches);
        else rebuild(numSubPatches);
        
	/* clear temporary data for static geometry */
	if (scene->isStatic()) {
          prims.clear();
          bvh->shrink();
        }
        bvh->cleanup();
        bvh->postBuild(t0);        
      }
      
      void clear() {
        prims.clear();
      }
    };
    
    /* entry functions for the scene builder */
    Builder* BVH4SubdivPatch1EagerBuilderBinnedSAH(void* bvh, Scene* scene, size_t mode) { return new BVHNSubdivPatch1EagerBuilderBinnedSAHClass<4>((BVH4*)bvh,scene); }
    Builder* BVH4SubdivPatch1CachedBuilderBinnedSAH(void* bvh, Scene* scene, size_t mode) { return new BVHNSubdivPatch1CachedBuilderBinnedSAHClass<4,false>((BVH4*)bvh,scene,mode); }
    Builder* BVH4SubdivPatch1MBlurCachedBuilderBinnedSAH(void* bvh, Scene* scene, size_t mode) { return new BVHNSubdivPatch1CachedBuilderBinnedSAHClass<4,true>((BVH4*)bvh,scene,mode); }
  }
}
#endif
