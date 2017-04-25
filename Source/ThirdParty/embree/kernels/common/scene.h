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
#include "device.h"
#include "scene_triangle_mesh.h"
#include "scene_quad_mesh.h"
#include "scene_user_geometry.h"
#include "scene_instance.h"
#include "scene_geometry_instance.h"
#include "scene_bezier_curves.h"
#include "scene_line_segments.h"
#include "scene_subdiv_mesh.h"

#include "../subdiv/tessellation_cache.h"

#include "acceln.h"
#include "geometry.h"

namespace embree
{
  /*! Base class all scenes are derived from */
  class Scene : public Accel
  {
    ALIGNED_CLASS;

  public:
    template<typename Ty, bool mblur = false>
      class Iterator
      {
      public:
      Iterator ()  {}
      
      Iterator (Scene* scene, bool all = false) 
      : scene(scene), all(all) {}
      
      __forceinline Ty* at(const size_t i)
      {
        Geometry* geom = scene->geometries[i];
        if (geom == nullptr) return nullptr;
        if (!all && !geom->isEnabled()) return nullptr;
        if (geom->getType() != Ty::geom_type) return nullptr;
        if ((geom->numTimeSteps != 1) != mblur) return nullptr;
        return (Ty*) geom;
      }

      __forceinline Ty* operator[] (const size_t i) {
        return at(i);
      }

      __forceinline size_t size() const {
        return scene->size();
      }
      
      __forceinline size_t numPrimitives() const {
        return scene->getNumPrimitives<Ty,mblur>();
      }

      __forceinline size_t maxPrimitivesPerGeometry() 
      {
        size_t ret = 0;
        for (size_t i=0; i<scene->size(); i++) {
          Ty* mesh = at(i);
          if (mesh == nullptr) continue;
          ret = max(ret,mesh->size());
        }
        return ret;
      }

      __forceinline unsigned maxTimeStepsPerGeometry()
      {
        unsigned ret = 0;
        for (size_t i=0; i<scene->size(); i++) {
          Ty* mesh = at(i);
          if (mesh == nullptr) continue;
          ret = max(ret,mesh->numTimeSteps);
        }
        return ret;
      }
      
    private:
      Scene* scene;
      bool all;
      };

  public:
    
    /*! Scene construction */
    Scene (Device* device, RTCSceneFlags flags, RTCAlgorithmFlags aflags);

  private:
    /*! class is non-copyable */
    Scene (const Scene& other) DELETED; // do not implement
    Scene& operator= (const Scene& other) DELETED; // do not implement

  public:
    void createTriangleAccel();
    void createQuadAccel();
    void createTriangleMBAccel();
    void createQuadMBAccel();
    void createHairAccel();
    void createHairMBAccel();
    void createLineAccel();
    void createLineMBAccel();
    void createSubdivAccel();
    void createSubdivMBAccel();
    void createUserGeometryAccel();
    void createUserGeometryMBAccel();

    /*! Scene destruction */
    ~Scene ();
    
    /*! clears the scene */
    void clear();

    /*! Creates new user geometry. */
    unsigned int newUserGeometry (RTCGeometryFlags gflags, size_t items, size_t numTimeSteps);

    /*! Creates a new scene instance. */
    unsigned int newInstance (Scene* scene, size_t numTimeSteps);

    /*! Creates a new geometry instance. */
    unsigned int newGeometryInstance (Geometry* geom);

    /*! Creates a new triangle mesh. */
    unsigned int newTriangleMesh (RTCGeometryFlags flags, size_t maxTriangles, size_t maxVertices, size_t numTimeSteps);

    /*! Creates a new quad mesh. */
    unsigned int newQuadMesh (RTCGeometryFlags flags, size_t maxQuads, size_t maxVertices, size_t numTimeSteps);

    /*! Creates a new collection of quadratic bezier curves. */
    unsigned int newCurves (NativeCurves::SubType subtype, NativeCurves::Basis basis, RTCGeometryFlags flags, size_t maxCurves, size_t maxVertices, size_t numTimeSteps);

    /*! Creates a new collection of line segments. */
    unsigned int newLineSegments (RTCGeometryFlags flags, size_t maxSegments, size_t maxVertices, size_t numTimeSteps);

    /*! Creates a new subdivision mesh. */
    unsigned int newSubdivisionMesh (RTCGeometryFlags flags, size_t numFaces, size_t numEdges, size_t numVertices, size_t numEdgeCreases, size_t numVertexCreases, size_t numHoles, size_t numTimeSteps);

    /*! deletes some geometry */
    void deleteGeometry(size_t geomID);

    /*! Builds acceleration structure for the scene. */
    void commit (size_t threadIndex, size_t threadCount, bool useThreadPool);
    void commit_task ();
    void build () {}

    void updateInterface();

    /* return number of geometries */
    __forceinline size_t size() const { return geometries.size(); }
    
    /* add user geometry to scene */
    unsigned int add (Geometry* geometry);
    
    /* determines of the scene is ready to get build */
    bool ready() { return numMappedBuffers == 0; }

    /* determines if scene is modified */
    __forceinline bool isModified() const { return modified; }

    /* sets modified flag */
    __forceinline void setModified(bool f = true) { 
      modified = f; 
    }

    /* get mesh by ID */
    __forceinline       Geometry* get(size_t i)       { assert(i < geometries.size()); return geometries[i]; }
    __forceinline const Geometry* get(size_t i) const { assert(i < geometries.size()); return geometries[i]; }

    template<typename Mesh>
      __forceinline       Mesh* get(size_t i)       { 
      assert(i < geometries.size()); 
      assert(geometries[i]->getType() == Mesh::geom_type);
      return (Mesh*)geometries[i]; 
    }
    template<typename Mesh>
      __forceinline const Mesh* get(size_t i) const { 
      assert(i < geometries.size()); 
      assert(geometries[i]->getType() == Mesh::geom_type);
      return (Mesh*)geometries[i]; 
    }

    template<typename Mesh>
    __forceinline Mesh* getSafe(size_t i) {
      assert(i < geometries.size());
      if (geometries[i] == nullptr) return nullptr;
      if (geometries[i]->getType() != Mesh::geom_type) return nullptr;
      else return (Mesh*) geometries[i];
    }

    __forceinline Geometry* get_locked(size_t i)  {
      Lock<SpinLock> lock(geometriesMutex);
      Geometry *g = geometries[i]; 
      assert(i < geometries.size()); 
      return g; 
    }

    /* test if this is a static scene */
    __forceinline bool isStatic() const { return embree::isStatic(flags); }

    /* test if this is a dynamic scene */
    __forceinline bool isDynamic() const { return embree::isDynamic(flags); }

    __forceinline bool isCompact() const { return embree::isCompact(flags); }
    __forceinline bool isCoherent() const { return embree::isCoherent(flags); }
    __forceinline bool isRobust() const { return embree::isRobust(flags); }
    __forceinline bool isHighQuality() const { return embree::isHighQuality(flags); }
    __forceinline bool isInterpolatable() const { return embree::isInterpolatable(aflags); }
    __forceinline bool isStreamMode() const { return embree::isStreamMode(aflags); }

    __forceinline bool isExclusiveIntersect1Mode() const { 
      if (!embree::isIntersect1Mode(aflags)) return false;
      if (embree::isIntersect4Mode(aflags))  return false;
      if (embree::isIntersect8Mode(aflags))  return false;
      if (embree::isIntersect16Mode(aflags)) return false;
      return true;
    }

    /* test if scene got already build */
    __forceinline bool isBuild() const { return is_build; }

  public:
    IDPool<unsigned> id_pool;
    std::vector<Geometry*> geometries; //!< list of all user geometries
    
  public:
    Device* device;
    AccelN accels;
    std::atomic<size_t> commitCounterSubdiv;
    std::atomic<size_t> numMappedBuffers;         //!< number of mapped buffers
    RTCSceneFlags flags;
    RTCAlgorithmFlags aflags;
    bool needTriangleIndices; 
    bool needTriangleVertices; 
    bool needQuadIndices; 
    bool needQuadVertices; 
    bool needBezierIndices;
    bool needBezierVertices;
    bool needLineIndices;
    bool needLineVertices;
    bool needSubdivIndices;
    bool needSubdivVertices;
    MutexSys buildMutex;
    SpinLock geometriesMutex;
    bool is_build;
    bool modified;                   //!< true if scene got modified
    
    /*! global lock step task scheduler */
#if defined(TASKING_INTERNAL) 
    MutexSys schedulerMutex;
    Ref<TaskScheduler> scheduler;
#elif defined(TASKING_TBB)
    tbb::task_group* group;
    BarrierActiveAutoReset group_barrier;
#elif defined(TASKING_PPL)
    concurrency::task_group* group;
    BarrierActiveAutoReset group_barrier;
#endif
    
  public:
    struct BuildProgressMonitorInterface : public BuildProgressMonitor {
      BuildProgressMonitorInterface(Scene* scene) 
      : scene(scene) {}
      void operator() (size_t dn) const { scene->progressMonitor(double(dn)); }
    private:
      Scene* scene;
    };
    BuildProgressMonitorInterface progressInterface;
    RTCProgressMonitorFunc progress_monitor_function;
    void* progress_monitor_ptr;
    std::atomic<size_t> progress_monitor_counter;
    void progressMonitor(double nprims);
    void setProgressMonitorFunction(RTCProgressMonitorFunc func, void* ptr);

  public:
    struct GeometryCounts 
    {
      __forceinline GeometryCounts()
        : numTriangles(0), numQuads(0), numBezierCurves(0), numLineSegments(0), numSubdivPatches(0), numUserGeometries(0) {}

      __forceinline size_t size() const {
        return numTriangles + numQuads + numBezierCurves + numLineSegments + numSubdivPatches + numUserGeometries;
      }

      std::atomic<size_t> numTriangles;             //!< number of enabled triangles
      std::atomic<size_t> numQuads;                 //!< number of enabled quads
      std::atomic<size_t> numBezierCurves;          //!< number of enabled curves
      std::atomic<size_t> numLineSegments;          //!< number of enabled line segments
      std::atomic<size_t> numSubdivPatches;         //!< number of enabled subdivision patches
      std::atomic<size_t> numUserGeometries;        //!< number of enabled user geometries
    };
    
    GeometryCounts world;               //!< counts for non-motion blurred geometry
    GeometryCounts worldMB;             //!< counts for motion blurred geometry
    GeometryCounts instanced;           //!< instance counts for non-motion blurred geometry
    GeometryCounts instancedMB;         //!< instance counts for motion blurred geometry

    std::atomic<size_t> numSubdivEnableDisableEvents; //!< number of enable/disable calls for any subdiv geometry

    __forceinline size_t numPrimitives() const {
      return world.size() + worldMB.size();
    }

    template<typename Mesh, bool mblur> __forceinline size_t getNumPrimitives() const;

    template<typename Mesh, bool mblur>
    __forceinline unsigned getNumTimeSteps()
    {
      if (!mblur)
        return 1;

      Scene::Iterator<Mesh,mblur> iter(this);
      return iter.maxTimeStepsPerGeometry();
    }
   
    std::atomic<size_t> numIntersectionFilters1;   //!< number of enabled intersection/occlusion filters for single rays
    std::atomic<size_t> numIntersectionFilters4;   //!< number of enabled intersection/occlusion filters for 4-wide ray packets
    std::atomic<size_t> numIntersectionFilters8;   //!< number of enabled intersection/occlusion filters for 8-wide ray packets
    std::atomic<size_t> numIntersectionFilters16;  //!< number of enabled intersection/occlusion filters for 16-wide ray packets
    std::atomic<size_t> numIntersectionFiltersN;   //!< number of enabled intersection/occlusion filters for N-wide ray packets
  };

  template<> __forceinline size_t Scene::getNumPrimitives<TriangleMesh,false>() const { return world.numTriangles; }
  template<> __forceinline size_t Scene::getNumPrimitives<TriangleMesh,true>() const { return worldMB.numTriangles; }
  template<> __forceinline size_t Scene::getNumPrimitives<QuadMesh,false>() const { return world.numQuads; }
  template<> __forceinline size_t Scene::getNumPrimitives<QuadMesh,true>() const { return worldMB.numQuads; }
  template<> __forceinline size_t Scene::getNumPrimitives<NativeCurves,false>() const { return world.numBezierCurves; }
  template<> __forceinline size_t Scene::getNumPrimitives<NativeCurves,true>() const { return worldMB.numBezierCurves; }
  template<> __forceinline size_t Scene::getNumPrimitives<LineSegments,false>() const { return world.numLineSegments; }
  template<> __forceinline size_t Scene::getNumPrimitives<LineSegments,true>() const { return worldMB.numLineSegments; }
  template<> __forceinline size_t Scene::getNumPrimitives<SubdivMesh,false>() const { return world.numSubdivPatches; }
  template<> __forceinline size_t Scene::getNumPrimitives<SubdivMesh,true>() const { return worldMB.numSubdivPatches; }
  template<> __forceinline size_t Scene::getNumPrimitives<AccelSet,false>() const { return world.numUserGeometries; }
  template<> __forceinline size_t Scene::getNumPrimitives<AccelSet,true>() const { return worldMB.numUserGeometries; }
}
