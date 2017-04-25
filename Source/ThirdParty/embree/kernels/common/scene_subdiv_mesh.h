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

#include "geometry.h"
#include "buffer.h"
#include "../subdiv/half_edge.h"
#include "../subdiv/tessellation_cache.h"
#include "../subdiv/catmullclark_coefficients.h"
#include "../subdiv/patch.h"
#include "../../common/algorithms/parallel_map.h"
#include "../../common/algorithms/parallel_set.h"

namespace embree
{
  class SubdivMesh : public Geometry
  {
    ALIGNED_CLASS;
  public:

    typedef HalfEdge::Edge Edge;
    
    /*! type of this geometry */
    static const Geometry::Type geom_type = Geometry::SUBDIV_MESH;

    /*! structure used to sort half edges using radix sort by their key */
    struct KeyHalfEdge 
    {
      KeyHalfEdge() {}
      
      KeyHalfEdge (uint64_t key, HalfEdge* edge) 
      : key(key), edge(edge) {}
      
      __forceinline operator uint64_t() const { 
	return key; 
      }

      friend __forceinline bool operator<(const KeyHalfEdge& e0, const KeyHalfEdge& e1) {
        return e0.key < e1.key;
      }
      
    public:
      uint64_t key;
      HalfEdge* edge;
    };

  public:

    /*! subdiv mesh construction */
    SubdivMesh(Scene* parent, RTCGeometryFlags flags, size_t numFaces, size_t numEdges, size_t numVertices, 
               size_t numCreases, size_t numCorners, size_t numHoles, size_t numTimeSteps);

  public:
    void enabling();
    void disabling();
    void setMask (unsigned mask);
    void setSubdivisionMode (unsigned topologyID, RTCSubdivisionMode mode);
    void setIndexBuffer(RTCBufferType vertexBuffer, RTCBufferType indexBuffer);
    void setBuffer(RTCBufferType type, void* ptr, size_t offset, size_t stride, size_t size);
    void* map(RTCBufferType type);
    void unmap(RTCBufferType type);
    void update ();
    void updateBuffer (RTCBufferType type);
    void setTessellationRate(float N);
    void immutable ();
    bool verify ();
    void setDisplacementFunction (RTCDisplacementFunc func, RTCBounds* bounds);
    void setDisplacementFunction2 (RTCDisplacementFunc2 func, RTCBounds* bounds);
    void interpolate(unsigned primID, float u, float v, RTCBufferType buffer, float* P, float* dPdu, float* dPdv, float* ddPdudu, float* ddPdvdv, float* ddPdudv, size_t numFloats);
    void interpolateN(const void* valid_i, const unsigned* primIDs, const float* u, const float* v, size_t numUVs, 
                      RTCBufferType buffer, float* P, float* dPdu, float* dPdv, float* ddPdudu, float* ddPdvdv, float* ddPdudv, size_t numFloats);

  public:

    /*! return the number of faces */
    size_t size() const { 
      return faceVertices.size(); 
    }

    /*! return the number of faces */
    size_t numFaces() const { 
      return faceVertices.size(); 
    }

    /*! return the number of edges */
    size_t numEdges() const { 
      return topology[0].vertexIndices.size(); 
    }

    /*! return the number of vertices */
    size_t numVertices() const { 
      return vertices[0].size(); 
    }

    /*! calculates the bounds of the i'th subdivision patch at the j'th timestep */
    __forceinline BBox3fa bounds(size_t i, size_t j = 0) const {
      return topology[0].getHalfEdge(i)->bounds(vertices[j]);
    }

    /*! check if the i'th primitive is valid */
    __forceinline bool valid(size_t i) const {
      return topology[0].valid(i) && !invalidFace(i);
    }

    /*! check if the i'th primitive is valid for the j'th time range */
    __forceinline bool valid(size_t i, size_t j) const {
      return topology[0].valid(i) && !invalidFace(i,j);
    }

    /*! prints some statistics */
    void printStatistics();

    /*! initializes the half edge data structure */
    void initializeHalfEdgeStructures ();
 
  public:

    /*! returns the vertex buffer for some time step */
    __forceinline const BufferRefT<Vec3fa>& getVertexBuffer( const size_t t = 0 ) const {
      return vertices[t];
    }

    /* returns tessellation level of edge */
    __forceinline float getEdgeLevel(const size_t i) const
    {
      if (levels) return clamp(levels[i],1.0f,4096.0f); // FIXME: do we want to limit edge level?
      else return clamp(tessellationRate,1.0f,4096.0f); // FIXME: do we want to limit edge level?
    }

  public:
    RTCDisplacementFunc displFunc;    //!< displacement function
    RTCDisplacementFunc2 displFunc2;    //!< displacement function
    BBox3fa             displBounds;  //!< bounds for maximal displacement 

    /*! all buffers in this section are provided by the application */
  public:
    
    /*! the topology contains all data that may differ when
     *  interpolating different user data buffers */
    struct Topology
    {
    public:

      /*! Default topology construction */
      Topology () : halfEdges(nullptr) {}

      /*! Topology initialization */
      Topology (SubdivMesh* mesh, size_t numEdges);

      /*! make the class movable */
    public: 
      Topology (Topology&& other) // FIXME: this is only required to workaround compilation issues under Windows
        : mesh(std::move(other.mesh)), 
          vertexIndices(std::move(other.vertexIndices)),
          subdiv_mode(std::move(other.subdiv_mode)),
          halfEdges(std::move(other.halfEdges)),
          halfEdges0(std::move(other.halfEdges0)),
          halfEdges1(std::move(other.halfEdges1)) {}
      
      Topology& operator= (Topology&& other) // FIXME: this is only required to workaround compilation issues under Windows
      {
        mesh = std::move(other.mesh); 
        vertexIndices = std::move(other.vertexIndices);
        subdiv_mode = std::move(other.subdiv_mode);
        halfEdges = std::move(other.halfEdges);
        halfEdges0 = std::move(other.halfEdges0);
        halfEdges1 = std::move(other.halfEdges1);
        return *this;
      }

    public:
      /*! check if the i'th primitive is valid in this topology */
      __forceinline bool valid(size_t i) const 
      {
        if (unlikely(subdiv_mode == RTC_SUBDIV_NO_BOUNDARY)) {
          if (getHalfEdge(i)->faceHasBorder()) return false;
        }
        return true;
      }
      
      /*! updates the interpolation mode for the topology */
      void setSubdivisionMode (RTCSubdivisionMode mode);

      /*! marks all buffers as modified */
      void update ();

      /*! frees unused buffers */
      void immutable();

      /*! verifies index array */
      bool verify (size_t numVertices);

      /*! initializes the half edge data structure */
      void initializeHalfEdgeStructures ();

    private:
      
      /*! recalculates the half edges */
      void calculateHalfEdges();
      
      /*! updates half edges when recalculation is not necessary */
      void updateHalfEdges();
      
      /*! user input data */
    public:

      SubdivMesh* mesh;

      /*! indices of the vertices composing each face */
      APIBuffer<unsigned> vertexIndices;
      
      /*! subdiv interpolation mode */
      RTCSubdivisionMode subdiv_mode;

      /*! generated data */
    public:

      /*! returns the start half edge for face f */
      __forceinline const HalfEdge* getHalfEdge ( const size_t f ) const { 
        return &halfEdges[mesh->faceStartEdge[f]]; 
      }

      /*! Half edge structure, generated by initHalfEdgeStructures */
      mvector<HalfEdge> halfEdges;

      /*! the following data is only required during construction of the
       *  half edge structure and can be cleared for static scenes */
    private:
      
      /*! two arrays used to sort the half edges */
      std::vector<KeyHalfEdge> halfEdges0;
      std::vector<KeyHalfEdge> halfEdges1;
    };

    /*! returns the start half edge for topology t and face f */
    __forceinline const HalfEdge* getHalfEdge ( const size_t t , const size_t f ) const { 
      return topology[t].getHalfEdge(f);
    }

    /*! buffer containing the number of vertices for each face */
    APIBuffer<unsigned> faceVertices;

    /*! array of topologies */
    vector<Topology> topology;

    /*! vertex buffer (one buffer for each time step) */
    vector<APIBuffer<Vec3fa>> vertices;

    /*! user data buffers */
    vector<APIBuffer<char>> userbuffers;

    /*! edge crease buffer containing edges (pairs of vertices) that carry edge crease weights */
    APIBuffer<Edge> edge_creases;
    
    /*! edge crease weights for each edge of the edge_creases buffer */
    APIBuffer<float> edge_crease_weights;
    
    /*! vertex crease buffer containing all vertices that carry vertex crease weights */
    APIBuffer<unsigned> vertex_creases;
    
    /*! vertex crease weights for each vertex of the vertex_creases buffer */
    APIBuffer<float> vertex_crease_weights;

    /*! subdivision level for each half edge of the vertexIndices buffer */
    APIBuffer<float> levels;
    float tessellationRate;  // constant rate that is used when levels is not set

    /*! buffer that marks specific faces as holes */
    APIBuffer<unsigned> holes;

    /*! all data in this section is generated by initializeHalfEdgeStructures function */
  private:

    /*! number of half edges used by faces */
    size_t numHalfEdges; 

    /*! fast lookup table to find the first half edge for some face */
    mvector<uint32_t> faceStartEdge;

    /*! set with all holes */
    parallel_set<uint32_t> holeSet;

    /*! fast lookup table to detect invalid faces */
    mvector<char> invalid_face;

    /*! test if face i is invalid in timestep j */
    __forceinline       char& invalidFace(size_t i, size_t j = 0)       { return invalid_face[i*numTimeSteps+j]; }
    __forceinline const char& invalidFace(size_t i, size_t j = 0) const { return invalid_face[i*numTimeSteps+j]; }

    /*! interpolation cache */
  public:
    static __forceinline size_t numInterpolationSlots4(size_t stride) { return (stride+15)/16; }
    static __forceinline size_t numInterpolationSlots8(size_t stride) { return (stride+31)/32; }
    static __forceinline size_t interpolationSlot(size_t prim, size_t slot, size_t stride) {
#if defined (__AVX__)
      const size_t slots = numInterpolationSlots8(stride); 
#else
      const size_t slots = numInterpolationSlots4(stride); 
#endif
      assert(slot < slots); 
      return slots*prim+slot;
    }
    std::vector<std::vector<SharedLazyTessellationCache::CacheEntry>> vertex_buffer_tags;
    std::vector<std::vector<SharedLazyTessellationCache::CacheEntry>> user_buffer_tags;
    std::vector<Patch3fa::Ref> patch_eval_trees;
    
    /*! the following data is only required during construction of the
     *  half edge structure and can be cleared for static scenes */
  private:
    
    /*! map with all vertex creases */
    parallel_map<uint32_t,float> vertexCreaseMap;
    
    /*! map with all edge creases */
    parallel_map<uint64_t,float> edgeCreaseMap;
  };

  class SubdivMeshAVX : public SubdivMesh
  {
  public:
    //using SubdivMesh::SubdivMesh; // inherit all constructors // FIXME: compiler bug under VS2013
    SubdivMeshAVX (Scene* parent, RTCGeometryFlags flags, size_t numFaces, size_t numEdges, size_t numVertices, 
                  size_t numCreases, size_t numCorners, size_t numHoles, size_t numTimeSteps);

    void interpolate(unsigned primID, float u, float v, RTCBufferType buffer, float* P, float* dPdu, float* dPdv, float* ddPdudu, float* ddPdvdv, float* ddPdudv, size_t numFloats);
    void interpolateN(const void* valid_i, const unsigned* primIDs, const float* u, const float* v, size_t numUVs, 
                      RTCBufferType buffer, float* P, float* dPdu, float* dPdv, float* ddPdudu, float* ddPdvdv, float* ddPdudv, size_t numFloats);

    template<typename vbool, typename vint, typename vfloat>
      void interpolateHelper(const vbool& valid1, const vint& primID, const vfloat& uu, const vfloat& vv, size_t numUVs, 
                             RTCBufferType buffer, float* P, float* dPdu, float* dPdv, float* ddPdudu, float* ddPdvdv, float* ddPdudv, size_t numFloats);
  };
};
