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

#include "scene_subdiv_mesh.h"
#include "scene.h"
#include "../subdiv/patch_eval.h"
#include "../subdiv/patch_eval_simd.h"

#include "../../common/algorithms/parallel_sort.h"
#include "../../common/algorithms/parallel_prefix_sum.h"
#include "../../common/algorithms/parallel_for.h"

namespace embree
{
  SubdivMesh::SubdivMesh (Scene* parent, RTCGeometryFlags flags, size_t numFaces, size_t numEdges, size_t numVertices, 
			  size_t numEdgeCreases, size_t numVertexCreases, size_t numHoles, size_t numTimeSteps)
    : Geometry(parent,SUBDIV_MESH,numFaces,numTimeSteps,flags), 
      displFunc(nullptr),
      displFunc2(nullptr),
      displBounds(empty),
      tessellationRate(2.0f),
      numHalfEdges(0),
      faceStartEdge(parent->device),
      invalid_face(parent->device)
  {
    vertices.resize(numTimeSteps);
    vertex_buffer_tags.resize(numTimeSteps);
    for (size_t i=0; i<numTimeSteps; i++)
      vertices[i].init(parent->device,numVertices,sizeof(Vec3fa));

    faceVertices.init(parent->device,numFaces,sizeof(unsigned int));
    holes.init(parent->device,numHoles,sizeof(int));
    levels.init(parent->device,numEdges,sizeof(float));

    edge_creases.init(parent->device,numEdgeCreases,2*sizeof(unsigned int));
    edge_crease_weights.init(parent->device,numEdgeCreases,sizeof(float));
    vertex_creases.init(parent->device,numVertexCreases,sizeof(unsigned int));
    vertex_crease_weights.init(parent->device,numVertexCreases,sizeof(float));

    topology.resize(1);
    topology[0] = Topology(this,numEdges);
    enabling();
  }

  void SubdivMesh::enabling() 
  { 
    parent->numSubdivEnableDisableEvents++;
    if (numTimeSteps == 1) parent->world.numSubdivPatches += size();
    else                   parent->worldMB.numSubdivPatches += size();
  }
  
  void SubdivMesh::disabling() 
  { 
    parent->numSubdivEnableDisableEvents++;
    if (numTimeSteps == 1) parent->world.numSubdivPatches -= size();
    else                   parent->worldMB.numSubdivPatches -= size();
  }

  void SubdivMesh::setMask (unsigned mask) 
  {
    if (parent->isStatic() && parent->isBuild()) 
      throw_RTCError(RTC_INVALID_OPERATION,"static scenes cannot get modified");

    this->mask = mask; 
    Geometry::update();
  }

  void SubdivMesh::setSubdivisionMode (unsigned topologyID, RTCSubdivisionMode mode)
  {
    if (parent->isStatic() && parent->isBuild()) 
      throw_RTCError(RTC_INVALID_OPERATION,"static scenes cannot get modified");
    if (topologyID >= topology.size())
      throw_RTCError(RTC_INVALID_OPERATION,"invalid topology ID");
    topology[topologyID].setSubdivisionMode(mode);
  }

  void SubdivMesh::setIndexBuffer(RTCBufferType vertexBuffer, RTCBufferType indexBuffer)
  {
    if (vertexBuffer >= RTC_USER_VERTEX_BUFFER0 && vertexBuffer < RTC_USER_VERTEX_BUFFER0+(int)userbuffers.size()) {
      if (indexBuffer >= RTC_INDEX_BUFFER && indexBuffer < RTC_INDEX_BUFFER+(int)topology.size()) {
        unsigned vid = vertexBuffer & 0xFFFF;
        unsigned iid = indexBuffer & 0xFFFF;
        if ((unsigned)userbuffers[vid].userdata != iid) {
          userbuffers[vid].userdata = iid;
          parent->commitCounterSubdiv++; // triggers recalculation of cached interpolation data
        }
      } else {
        throw_RTCError(RTC_INVALID_OPERATION,"invalid index buffer specified");
      }
    } else {
      throw_RTCError(RTC_INVALID_OPERATION,"invalid vertex buffer specified");
    }
  }

  void SubdivMesh::setBuffer(RTCBufferType type, void* ptr, size_t offset, size_t stride, size_t size) 
  { 
    if (parent->isStatic() && parent->isBuild()) 
      throw_RTCError(RTC_INVALID_OPERATION,"static scenes cannot get modified");

    /* verify that all accesses are 4 bytes aligned */
    if (((size_t(ptr) + offset) & 0x3) || (stride & 0x3)) 
      throw_RTCError(RTC_INVALID_OPERATION,"data must be 4 bytes aligned");

    if (type != RTC_LEVEL_BUFFER)
      parent->commitCounterSubdiv++;

    unsigned bid = type & 0xFFFF;
    if (type >= RTC_VERTEX_BUFFER0 && type < RTC_VERTEX_BUFFER0+(int)numTimeSteps) 
    {
      vertices[bid].set(ptr,offset,stride,size); 
      vertices[bid].checkPadding16();
    }
    else if (type >= RTC_USER_VERTEX_BUFFER0 && type < RTC_USER_VERTEX_BUFFER0+RTC_MAX_USER_VERTEX_BUFFERS)
    {
      if (bid >= userbuffers.size()) {
        userbuffers.resize(bid+1);
        user_buffer_tags.resize(bid+1);
      }
      userbuffers[bid] = APIBuffer<char>(parent->device,numVertices(),stride); // FIXME: numVertices for compatibility
      userbuffers[bid].set(ptr,offset,stride,size);  
      userbuffers[bid].checkPadding16();
    }
    else if (type == RTC_FACE_BUFFER) 
    {
      if (size != (size_t)-1) disabling();
      faceVertices.set(ptr,offset,stride,size);
      setNumPrimitives(size);
      if (size != (size_t)-1) enabling();
    }

    else if (type >= RTC_INDEX_BUFFER && type < RTC_INDEX_BUFFER+RTC_MAX_INDEX_BUFFERS)
    {
      int begin = (int)topology.size();
      if (bid >= topology.size()) {
        topology.resize(bid+1);
        for (size_t i=begin; i<topology.size(); i++)
          topology[i] = Topology(this,numEdges());
      }
      topology[bid].vertexIndices.set(ptr,offset,stride,size);
    }
    else if (type == RTC_EDGE_CREASE_INDEX_BUFFER)
      edge_creases.set(ptr,offset,stride,size);

    else if (type == RTC_EDGE_CREASE_WEIGHT_BUFFER)
      edge_crease_weights.set(ptr,offset,stride,size);

    else if (type == RTC_VERTEX_CREASE_INDEX_BUFFER)
      vertex_creases.set(ptr,offset,stride,size);

    else if (type == RTC_VERTEX_CREASE_WEIGHT_BUFFER)
      vertex_crease_weights.set(ptr,offset,stride,size);

    else if (type == RTC_HOLE_BUFFER)
      holes.set(ptr,offset,stride,size);

    else if (type == RTC_LEVEL_BUFFER)
      levels.set(ptr,offset,stride,size);

    else
      throw_RTCError(RTC_INVALID_ARGUMENT,"unknown buffer type");
  }

  void* SubdivMesh::map(RTCBufferType type) 
  {
    if (parent->isStatic() && parent->isBuild())
      throw_RTCError(RTC_INVALID_OPERATION,"static scenes cannot get modified");

    unsigned bid = type & 0xFFFF;
    if (type >= RTC_VERTEX_BUFFER0 && type < RTC_VERTEX_BUFFER0+int(numTimeSteps)) 
      return vertices[bid].map(parent->numMappedBuffers);

    else if (type >= RTC_INDEX_BUFFER && type < RTC_INDEX_BUFFER+RTC_MAX_INDEX_BUFFERS)
      return topology[bid].vertexIndices.map(parent->numMappedBuffers);

    else if (type == RTC_FACE_BUFFER)
      return faceVertices.map(parent->numMappedBuffers);

    else if (type == RTC_EDGE_CREASE_INDEX_BUFFER)
      return edge_creases.map(parent->numMappedBuffers); 

    else if (type == RTC_EDGE_CREASE_WEIGHT_BUFFER)
      return edge_crease_weights.map(parent->numMappedBuffers); 

    else if (type == RTC_VERTEX_CREASE_INDEX_BUFFER)
      return vertex_creases.map(parent->numMappedBuffers); 
    
    else if (type == RTC_VERTEX_CREASE_WEIGHT_BUFFER)
      return vertex_crease_weights.map(parent->numMappedBuffers);

    else if (type == RTC_HOLE_BUFFER)
      return holes.map(parent->numMappedBuffers);

    else if (type == RTC_LEVEL_BUFFER)
      return levels.map(parent->numMappedBuffers); 

    else 
      throw_RTCError(RTC_INVALID_ARGUMENT,"unknown buffer type"); 

    return nullptr;
  }

  void SubdivMesh::unmap(RTCBufferType type) 
  {
    if (parent->isStatic() && parent->isBuild())
      throw_RTCError(RTC_INVALID_OPERATION,"static scenes cannot get modified");

    unsigned bid = type & 0xFFFF;
    if (type >= RTC_VERTEX_BUFFER0 && type < RTC_VERTEX_BUFFER0+(int)numTimeSteps) 
      vertices[bid].unmap(parent->numMappedBuffers);

    else if (type >= RTC_INDEX_BUFFER && type < RTC_INDEX_BUFFER+RTC_MAX_INDEX_BUFFERS)
      topology[bid].vertexIndices.unmap(parent->numMappedBuffers);

    else if (type == RTC_FACE_BUFFER)
      faceVertices.unmap(parent->numMappedBuffers);

    else if (type == RTC_EDGE_CREASE_INDEX_BUFFER)
      edge_creases.unmap(parent->numMappedBuffers); 

    else if (type == RTC_EDGE_CREASE_WEIGHT_BUFFER)
      edge_crease_weights.unmap(parent->numMappedBuffers); 

    else if (type == RTC_VERTEX_CREASE_INDEX_BUFFER)
      vertex_creases.unmap(parent->numMappedBuffers); 
    
    else if (type == RTC_VERTEX_CREASE_WEIGHT_BUFFER)
      vertex_crease_weights.unmap(parent->numMappedBuffers);

    else if (type == RTC_HOLE_BUFFER)
      holes.unmap(parent->numMappedBuffers);

    else if (type == RTC_LEVEL_BUFFER)
      levels.unmap(parent->numMappedBuffers); 

    else 
      throw_RTCError(RTC_INVALID_ARGUMENT,"unknown buffer type"); 
  }

  void SubdivMesh::update ()
  {
    faceVertices.setModified(true);
    holes.setModified(true);
    for (auto& buffer : vertices) buffer.setModified(true); 
    levels.setModified(true);
    edge_creases.setModified(true);
    edge_crease_weights.setModified(true);
    vertex_creases.setModified(true);
    vertex_crease_weights.setModified(true); 
    for (auto& t : topology) t.update();
    Geometry::update();
  }

  void SubdivMesh::updateBuffer (RTCBufferType type)
  {
    if (type != RTC_LEVEL_BUFFER)
      parent->commitCounterSubdiv++;

    unsigned bid = type & 0xFFFF;
    if (type >= RTC_VERTEX_BUFFER0 && type < RTC_VERTEX_BUFFER0+(int)numTimeSteps)
      vertices[bid].setModified(true);
    
    else if (type >= RTC_USER_VERTEX_BUFFER0 && type < RTC_USER_VERTEX_BUFFER0+2)
      userbuffers[bid].setModified(true);

    else if (type == RTC_FACE_BUFFER)
      faceVertices.setModified(true);

    else if (type >= RTC_INDEX_BUFFER && type < RTC_INDEX_BUFFER+RTC_MAX_INDEX_BUFFERS)
      topology[bid].vertexIndices.setModified(true);

    else if (type == RTC_EDGE_CREASE_INDEX_BUFFER)
      edge_creases.setModified(true);

    else if (type == RTC_EDGE_CREASE_WEIGHT_BUFFER)
      edge_crease_weights.setModified(true);

    else if (type == RTC_VERTEX_CREASE_INDEX_BUFFER)
      vertex_creases.setModified(true);

    else if (type == RTC_VERTEX_CREASE_WEIGHT_BUFFER)
      vertex_crease_weights.setModified(true);

    else if (type == RTC_HOLE_BUFFER)
      holes.setModified(true);

    else if (type == RTC_LEVEL_BUFFER)
      levels.setModified(true);

    else
      throw_RTCError(RTC_INVALID_ARGUMENT,"unknown buffer type");

    Geometry::update();
  }

  void SubdivMesh::setDisplacementFunction (RTCDisplacementFunc func, RTCBounds* bounds) 
  {
    if (parent->isStatic() && parent->isBuild())
      throw_RTCError(RTC_INVALID_OPERATION,"static scenes cannot get modified");

    this->displFunc   = func;
    if (bounds) this->displBounds = *(BBox3fa*)bounds; 
    else        this->displBounds = empty;
  }

  void SubdivMesh::setDisplacementFunction2 (RTCDisplacementFunc2 func, RTCBounds* bounds) 
  {
    if (parent->isStatic() && parent->isBuild())
      throw_RTCError(RTC_INVALID_OPERATION,"static scenes cannot get modified");

    this->displFunc2   = func;
    if (bounds) this->displBounds = *(BBox3fa*)bounds; 
    else        this->displBounds = empty;
  }

  void SubdivMesh::setTessellationRate(float N)
  {
    if (parent->isStatic() && parent->isBuild()) 
      throw_RTCError(RTC_INVALID_OPERATION,"static geometries cannot get modified");

    tessellationRate = N;
    levels.setModified(true);
  }

  void SubdivMesh::immutable () 
  {
    const bool freeVertices = !parent->needSubdivVertices;
    faceVertices.free();
    if (freeVertices )
      for (auto& buffer : vertices)
        buffer.free();
    levels.free();
    holes.free();
    edge_creases.free();
    edge_crease_weights.free();
    vertex_creases.free();
    vertex_crease_weights.free();
    for (auto& t : topology) 
      t.immutable();
  }

  __forceinline uint64_t pair64(unsigned int x, unsigned int y) 
  {
    if (x<y) std::swap(x,y);
    return (((uint64_t)x) << 32) | (uint64_t)y;
  }

  SubdivMesh::Topology::Topology(SubdivMesh* mesh, size_t numEdges)
    : mesh(mesh), subdiv_mode(RTC_SUBDIV_SMOOTH_BOUNDARY), halfEdges(mesh->parent->device)
  {
    vertexIndices.init(mesh->parent->device,numEdges,sizeof(unsigned int));
  }
  
  void SubdivMesh::Topology::setSubdivisionMode (RTCSubdivisionMode mode)
  {
    if (subdiv_mode == mode) return;
    subdiv_mode = mode;
    mesh->updateBuffer(RTC_VERTEX_CREASE_WEIGHT_BUFFER);
  }
  
  void SubdivMesh::Topology::update () {
    vertexIndices.setModified(true); 
  }

  void SubdivMesh::Topology::immutable () 
  {
    const bool freeIndices = !mesh->parent->needSubdivIndices;
    if (freeIndices) vertexIndices.free();
  }

  bool SubdivMesh::Topology::verify (size_t numVertices) 
  {
    size_t ofs = 0;
    for (size_t i=0; i<mesh->faceVertices.size(); i++) 
    {
      int valence = mesh->faceVertices[i];
      for (size_t j=ofs; j<ofs+valence; j++) 
      {
        if (j >= vertexIndices.size())
          return false;
          
        if (vertexIndices[j] >= numVertices)
          return false; 
      }
      ofs += valence;
    }
    return true;
  }

  void SubdivMesh::Topology::calculateHalfEdges()
  {
    const size_t blockSize = 4096;
    const size_t numEdges = mesh->numEdges();
    const size_t numFaces = mesh->numFaces();
    const size_t numHalfEdges = mesh->numHalfEdges;

    /* allocate temporary array */
    halfEdges0.resize(numEdges);
    halfEdges1.resize(numEdges);

    /* create all half edges */
    parallel_for( size_t(0), numFaces, blockSize, [&](const range<size_t>& r) 
    {
      for (size_t f=r.begin(); f<r.end(); f++) 
      {
	const unsigned N = mesh->faceVertices[f];
	const unsigned e = mesh->faceStartEdge[f];

	for (unsigned de=0; de<N; de++)
	{
	  HalfEdge* edge = &halfEdges[e+de];
          int nextOfs = (de == (N-1)) ? -int(N-1) : +1;
          int prevOfs = (de ==     0) ? +int(N-1) : -1;
	  
	  const unsigned int startVertex = vertexIndices[e+de];
          const unsigned int endVertex = vertexIndices[e+de+nextOfs]; 
	  const uint64_t key = SubdivMesh::Edge(startVertex,endVertex);

          /* we always have to use the geometry topology to lookup creases */
          const unsigned int startVertex0 = mesh->topology[0].vertexIndices[e+de];
          const unsigned int endVertex0 = mesh->topology[0].vertexIndices[e+de+nextOfs]; 
	  const uint64_t key0 = SubdivMesh::Edge(startVertex0,endVertex0);
	  
	  edge->vtx_index              = startVertex;
	  edge->next_half_edge_ofs     = nextOfs;
	  edge->prev_half_edge_ofs     = prevOfs;
	  edge->opposite_half_edge_ofs = 0;
	  edge->edge_crease_weight     = mesh->edgeCreaseMap.lookup(key0,0.0f);
	  edge->vertex_crease_weight   = mesh->vertexCreaseMap.lookup(startVertex0,0.0f);
	  edge->edge_level             = mesh->getEdgeLevel(e+de);
          edge->patch_type             = HalfEdge::COMPLEX_PATCH; // type gets updated below
          edge->vertex_type            = HalfEdge::REGULAR_VERTEX;

          if (unlikely(mesh->holeSet.lookup(unsigned(f)))) 
	    halfEdges1[e+de] = SubdivMesh::KeyHalfEdge(std::numeric_limits<uint64_t>::max(),edge);
	  else
	    halfEdges1[e+de] = SubdivMesh::KeyHalfEdge(key,edge);
	}
      }
    });

    /* sort half edges to find adjacent edges */
    radix_sort_u64(halfEdges1.data(),halfEdges0.data(),numHalfEdges);

    /* link all adjacent pairs of edges */
    parallel_for( size_t(0), numHalfEdges, blockSize, [&](const range<size_t>& r) 
    {
      /* skip if start of adjacent edges was not in our range */
      size_t e=r.begin();
      if (e != 0 && (halfEdges1[e].key == halfEdges1[e-1].key)) {
	const uint64_t key = halfEdges1[e].key;
	while (e<r.end() && halfEdges1[e].key == key) e++;
      }

      /* process all adjacent edges starting in our range */
      while (e<r.end())
      {
	const uint64_t key = halfEdges1[e].key;
	if (key == std::numeric_limits<uint64_t>::max()) break;
	size_t N=1; while (e+N<numHalfEdges && halfEdges1[e+N].key == key) N++;

        /* border edges are identified by not having an opposite edge set */
	if (N == 1) {
          halfEdges1[e].edge->edge_crease_weight = float(inf);
	}

        /* standard edge shared between two faces */
        else if (N == 2)
        {
          /* create edge crease if winding order mismatches between neighboring patches */
          if (halfEdges1[e+0].edge->next()->vtx_index != halfEdges1[e+1].edge->vtx_index)
          {
            halfEdges1[e+0].edge->edge_crease_weight = float(inf);
            halfEdges1[e+1].edge->edge_crease_weight = float(inf);
          }
          /* otherwise mark edges as opposites of each other */
          else {
            halfEdges1[e+0].edge->setOpposite(halfEdges1[e+1].edge);
            halfEdges1[e+1].edge->setOpposite(halfEdges1[e+0].edge);
          }
	}

        /* non-manifold geometry is handled by keeping vertices fixed during subdivision */
        else {
	  for (size_t i=0; i<N; i++) {
	    halfEdges1[e+i].edge->vertex_crease_weight = inf;
            halfEdges1[e+i].edge->vertex_type = HalfEdge::NON_MANIFOLD_EDGE_VERTEX;
            halfEdges1[e+i].edge->edge_crease_weight = inf;

	    halfEdges1[e+i].edge->next()->vertex_crease_weight = inf;
            halfEdges1[e+i].edge->next()->vertex_type = HalfEdge::NON_MANIFOLD_EDGE_VERTEX;
            halfEdges1[e+i].edge->next()->edge_crease_weight = inf;
	  }
	}
	e+=N;
      }
    });

    /* set subdivision mode and calculate patch types */
    parallel_for( size_t(0), numFaces, blockSize, [&](const range<size_t>& r) 
    {
      for (size_t f=r.begin(); f<r.end(); f++) 
      {
        HalfEdge* edge = &halfEdges[mesh->faceStartEdge[f]];

        /* for vertex topology we also test if vertices are valid */
        if (this == &mesh->topology[0])
        {
          /* calculate if face is valid */
          for (size_t t=0; t<mesh->numTimeSteps; t++)
            mesh->invalidFace(f,t) = !edge->valid(mesh->vertices[t]) || mesh->holeSet.lookup(unsigned(f));
        }

        /* pin some edges and vertices */
        for (size_t i=0; i<mesh->faceVertices[f]; i++) 
        {
          /* pin corner vertices when requested by user */
          if (subdiv_mode == RTC_SUBDIV_PIN_CORNERS && edge[i].isCorner())
            edge[i].vertex_crease_weight = float(inf);
          
          /* pin all border vertices when requested by user */
          else if (subdiv_mode == RTC_SUBDIV_PIN_BOUNDARY && edge[i].vertexHasBorder()) 
            edge[i].vertex_crease_weight = float(inf);

          /* pin all edges and vertices when requested by user */
          else if (subdiv_mode == RTC_SUBDIV_PIN_ALL) {
            edge[i].edge_crease_weight = float(inf);
            edge[i].vertex_crease_weight = float(inf);
          }
        }

        /* we have to calculate patch_type last! */
        HalfEdge::PatchType patch_type = edge->patchType();
        for (size_t i=0; i<mesh->faceVertices[f]; i++) 
          edge[i].patch_type = patch_type;
      }
    });
  }

  void SubdivMesh::Topology::updateHalfEdges()
  {
    /* we always use the geometry topology to lookup creases */
    mvector<HalfEdge>& halfEdgesGeom = mesh->topology[0].halfEdges;

    /* assume we do no longer recalculate in the future and clear these arrays */
    halfEdges0.clear();
    halfEdges1.clear();

    /* calculate which data to update */
    const bool updateEdgeCreases   = mesh->topology[0].vertexIndices.isModified() || mesh->edge_creases.isModified()   || mesh->edge_crease_weights.isModified();
    const bool updateVertexCreases = mesh->topology[0].vertexIndices.isModified() || mesh->vertex_creases.isModified() || mesh->vertex_crease_weights.isModified(); 
    const bool updateLevels = mesh->levels.isModified();

    /* parallel loop over all half edges */
    parallel_for( size_t(0), mesh->numHalfEdges, size_t(4096), [&](const range<size_t>& r) 
    {
      for (size_t i=r.begin(); i!=r.end(); i++)
      {
	HalfEdge& edge = halfEdges[i];

	if (updateLevels)
	  edge.edge_level = mesh->getEdgeLevel(i); 
        
	if (updateEdgeCreases) {
	  if (edge.hasOpposite()) // leave weight at inf for borders
            edge.edge_crease_weight = mesh->edgeCreaseMap.lookup((uint64_t)halfEdgesGeom[i].getEdge(),0.0f);
	}
        
        /* we only use user specified vertex_crease_weight if the vertex is manifold */
        if (updateVertexCreases && edge.vertex_type != HalfEdge::NON_MANIFOLD_EDGE_VERTEX) 
        {
	  edge.vertex_crease_weight = mesh->vertexCreaseMap.lookup(halfEdgesGeom[i].vtx_index,0.0f);

          /* pin corner vertices when requested by user */
          if (subdiv_mode == RTC_SUBDIV_PIN_CORNERS && edge.isCorner())
            edge.vertex_crease_weight = float(inf);
          
          /* pin all border vertices when requested by user */
          else if (subdiv_mode == RTC_SUBDIV_PIN_BOUNDARY && edge.vertexHasBorder()) 
            edge.vertex_crease_weight = float(inf);

          /* pin every vertex when requested by user */
          else if (subdiv_mode == RTC_SUBDIV_PIN_ALL) {
            edge.edge_crease_weight = float(inf);
            edge.vertex_crease_weight = float(inf);
          }
        }

        /* update patch type */
        if (updateEdgeCreases || updateVertexCreases) {
          edge.patch_type = edge.patchType();
        }
      }
    });
  }

  void SubdivMesh::Topology::initializeHalfEdgeStructures ()
  {
    /* if vertex indices not set we ignore this topology */
    if (!vertexIndices)
      return;

    /* allocate half edge array */
    halfEdges.resize(mesh->numEdges());

    /* check if we have to recalculate the half edges */
    bool recalculate = false;
    recalculate |= vertexIndices.isModified(); 
    recalculate |= mesh->faceVertices.isModified();
    recalculate |= mesh->holes.isModified();

    /* check if we can simply update the half edges */
    bool update = false;
    update |= mesh->topology[0].vertexIndices.isModified(); // we use this buffer to copy creases to interpolation topologies
    update |= mesh->edge_creases.isModified();
    update |= mesh->edge_crease_weights.isModified();
    update |= mesh->vertex_creases.isModified();
    update |= mesh->vertex_crease_weights.isModified(); 
    update |= mesh->levels.isModified();

    /* now either recalculate or update the half edges */
    if (recalculate) calculateHalfEdges();
    else if (update) updateHalfEdges();
   
    /* cleanup some state for static scenes */
    if (mesh->parent->isStatic()) 
    {
      halfEdges0.clear();
      halfEdges1.clear();
    }

    /* clear modified state of all buffers */
    vertexIndices.setModified(false); 
  }

  void SubdivMesh::printStatistics()
  {
    size_t numBilinearFaces = 0;
    size_t numRegularQuadFaces = 0;
    size_t numIrregularQuadFaces = 0;
    size_t numComplexFaces = 0;
    
    for (size_t e=0, f=0; f<numFaces(); e+=faceVertices[f++]) 
    {
      switch (topology[0].halfEdges[e].patch_type) {
      case HalfEdge::BILINEAR_PATCH      : numBilinearFaces++;   break;
      case HalfEdge::REGULAR_QUAD_PATCH  : numRegularQuadFaces++;   break;
      case HalfEdge::IRREGULAR_QUAD_PATCH: numIrregularQuadFaces++; break;
      case HalfEdge::COMPLEX_PATCH       : numComplexFaces++;   break;
      }
    }
    
    std::cout << "numFaces = " << numFaces() << ", " 
              << "numBilinearFaces = " << numBilinearFaces << " (" << 100.0f * numBilinearFaces / numFaces() << "%), " 
              << "numRegularQuadFaces = " << numRegularQuadFaces << " (" << 100.0f * numRegularQuadFaces / numFaces() << "%), " 
              << "numIrregularQuadFaces " << numIrregularQuadFaces << " (" << 100.0f * numIrregularQuadFaces / numFaces() << "%) " 
              << "numComplexFaces " << numComplexFaces << " (" << 100.0f * numComplexFaces / numFaces() << "%) " 
              << std::endl;
  }

  void SubdivMesh::initializeHalfEdgeStructures ()
  {
    double t0 = getSeconds();

    invalid_face.resize(numFaces()*numTimeSteps);
 
    /* calculate start edge of each face */
    faceStartEdge.resize(numFaces());
    if (faceVertices.isModified())
      numHalfEdges = parallel_prefix_sum(faceVertices,faceStartEdge,numFaces(),0,std::plus<unsigned>());

    /* create set with all vertex creases */
    if (vertex_creases.isModified() || vertex_crease_weights.isModified())
      vertexCreaseMap.init(vertex_creases,vertex_crease_weights);
    
    /* create map with all edge creases */
    if (edge_creases.isModified() || edge_crease_weights.isModified())
      edgeCreaseMap.init(edge_creases,edge_crease_weights);

    /* create set with all holes */
    if (holes.isModified())
      holeSet.init(holes);

    /* create topology */
    for (auto& t: topology)
      t.initializeHalfEdgeStructures();

    /* create interpolation cache mapping for interpolatable meshes */
    if (parent->isInterpolatable()) 
    {
#if defined (__TARGET_AVX__)
      auto numInterpolationSlots = [&] (size_t stride) {
        if (parent->device->hasISA(AVX)) return numInterpolationSlots8(stride); else return numInterpolationSlots4(stride);
      };
#else
      auto numInterpolationSlots = [] (size_t stride) {
        return numInterpolationSlots4(stride);
      };
#endif
      for (size_t i=0; i<vertex_buffer_tags.size(); i++)
        vertex_buffer_tags[i].resize(numFaces()*numInterpolationSlots(vertices[i].getStride()));
      for (size_t i=0; i<userbuffers.size(); i++)
        if (userbuffers[i]) user_buffer_tags[i].resize(numFaces()*numInterpolationSlots(userbuffers[i].getStride()));
    }

    /* cleanup some state for static scenes */
    if (parent->isStatic()) 
    {
      vertexCreaseMap.clear();
      edgeCreaseMap.clear();
    }

    /* clear modified state of all buffers */
    faceVertices.setModified(false);
    holes.setModified(false);
    for (auto& buffer : vertices) buffer.setModified(false); 
    levels.setModified(false);
    edge_creases.setModified(false);
    edge_crease_weights.setModified(false);
    vertex_creases.setModified(false);
    vertex_crease_weights.setModified(false); 

    double t1 = getSeconds();

    /* print statistics in verbose mode */
    if (parent->device->verbosity(2)) {
      std::cout << "half edge generation = " << 1000.0*(t1-t0) << "ms, " << 1E-6*double(numHalfEdges)/(t1-t0) << "M/s" << std::endl;
      printStatistics();
    }
  }

  bool SubdivMesh::verify () 
  {
    /*! verify consistent size of vertex arrays */
    if (vertices.size() == 0) return false;
    for (const auto& buffer : vertices)
      if (buffer.size() != numVertices())
        return false;

    /*! verify vertex indices */
    if (!topology[0].verify(numVertices()))
      return false;

    for (auto& b : userbuffers)
      if (!topology[b.userdata].verify(b.size()))
        return false;

    /*! verify vertices */
    for (const auto& buffer : vertices)
      for (size_t i=0; i<buffer.size(); i++)
	if (!isvalid(buffer[i])) 
	  return false;

    return true;
  }

  void SubdivMesh::interpolate(unsigned primID, float u, float v, RTCBufferType buffer, float* P, float* dPdu, float* dPdv, float* ddPdudu, float* ddPdvdv, float* ddPdudv, size_t numFloats) 
  {
    /* test if interpolation is enabled */
#if defined(DEBUG) 
    if ((parent->aflags & RTC_INTERPOLATE) == 0) 
      throw_RTCError(RTC_INVALID_OPERATION,"rtcInterpolate can only get called when RTC_INTERPOLATE is enabled for the scene");
#endif

    /* calculate base pointer and stride */
    assert((buffer >= RTC_VERTEX_BUFFER0 && buffer < RTCBufferType(RTC_VERTEX_BUFFER0 + RTC_MAX_TIME_STEPS)) ||
           (buffer >= RTC_USER_VERTEX_BUFFER0 && RTCBufferType(RTC_USER_VERTEX_BUFFER0 + RTC_MAX_USER_VERTEX_BUFFERS)));
    const char* src = nullptr; 
    size_t stride = 0;
    size_t bufID = buffer&0xFFFF;
    std::vector<SharedLazyTessellationCache::CacheEntry>* baseEntry = nullptr;
    Topology* topo = nullptr;
    if (buffer >= RTC_USER_VERTEX_BUFFER0) {
      assert(bufID < userbuffers.size());
      src    = userbuffers[bufID].getPtr();
      stride = userbuffers[bufID].getStride();
      baseEntry = &user_buffer_tags[bufID];
      int topologyID = userbuffers[bufID].userdata;
      topo = &topology[topologyID];
    } else {
      assert(bufID < numTimeSteps);
      src    = vertices[bufID].getPtr();
      stride = vertices[bufID].getStride();
      baseEntry = &vertex_buffer_tags[bufID];
      topo = &topology[0];
    }

    bool has_P = P;
    bool has_dP = dPdu;     assert(!has_dP  || dPdv);
    bool has_ddP = ddPdudu; assert(!has_ddP || (ddPdvdv && ddPdudu));

    for (size_t i=0; i<numFloats; i+=4)
    {
      vfloat4 Pt, dPdut, dPdvt, ddPdudut, ddPdvdvt, ddPdudvt;
      isa::PatchEval<vfloat4,vfloat4>(baseEntry->at(interpolationSlot(primID,i/4,stride)),parent->commitCounterSubdiv,
                                      topo->getHalfEdge(primID),src+i*sizeof(float),stride,u,v,
                                      has_P ? &Pt : nullptr, 
                                      has_dP ? &dPdut : nullptr, 
                                      has_dP ? &dPdvt : nullptr,
                                      has_ddP ? &ddPdudut : nullptr, 
                                      has_ddP ? &ddPdvdvt : nullptr, 
                                      has_ddP ? &ddPdudvt : nullptr);

      if (has_P) {
        for (size_t j=i; j<min(i+4,numFloats); j++) 
          P[j] = Pt[j-i];
      }
      if (has_dP) 
      {
        for (size_t j=i; j<min(i+4,numFloats); j++) {
          dPdu[j] = dPdut[j-i];
          dPdv[j] = dPdvt[j-i];
        }
      }
      if (has_ddP) 
      {
        for (size_t j=i; j<min(i+4,numFloats); j++) {
          ddPdudu[j] = ddPdudut[j-i];
          ddPdvdv[j] = ddPdvdvt[j-i];
          ddPdudv[j] = ddPdudvt[j-i];
        }
      }
    }
  }

  void SubdivMesh::interpolateN(const void* valid_i, const unsigned* primIDs, const float* u, const float* v, size_t numUVs, 
                                RTCBufferType buffer, float* P, float* dPdu, float* dPdv, float* ddPdudu, float* ddPdvdv, float* ddPdudv, size_t numFloats)
  {
    /* test if interpolation is enabled */
#if defined(DEBUG)
    if ((parent->aflags & RTC_INTERPOLATE) == 0) 
      throw_RTCError(RTC_INVALID_OPERATION,"rtcInterpolate can only get called when RTC_INTERPOLATE is enabled for the scene");
#endif

    /* calculate base pointer and stride */
    assert((buffer >= RTC_VERTEX_BUFFER0 && buffer < RTCBufferType(RTC_VERTEX_BUFFER0 + RTC_MAX_TIME_STEPS)) ||
           (buffer >= RTC_USER_VERTEX_BUFFER0 && RTCBufferType(RTC_USER_VERTEX_BUFFER0 + RTC_MAX_USER_VERTEX_BUFFERS)));
    const char* src = nullptr; 
    size_t stride = 0;
    size_t bufID = buffer&0xFFFF;
    std::vector<SharedLazyTessellationCache::CacheEntry>* baseEntry = nullptr;
    Topology* topo = nullptr;
    if (buffer >= RTC_USER_VERTEX_BUFFER0) {
      assert(bufID < userbuffers.size());
      src    = userbuffers[bufID].getPtr();
      stride = userbuffers[bufID].getStride();
      baseEntry = &user_buffer_tags[bufID];
      int topologyID = userbuffers[bufID].userdata;
      topo = &topology[topologyID];
    } else {
      assert(bufID < numTimeSteps);
      src    = vertices[bufID].getPtr();
      stride = vertices[bufID].getStride();
      baseEntry = &vertex_buffer_tags[bufID];
      topo = &topology[0];
    }

    const int* valid = (const int*) valid_i;
    
    for (size_t i=0; i<numUVs; i+=4) 
    {
      vbool4 valid1 = vint4(i)+vint4(step) < vint4(numUVs);
      if (valid) valid1 &= vint4::loadu(&valid[i]) == vint4(-1);
      if (none(valid1)) continue;
      
      const vint4 primID = vint4::loadu(&primIDs[i]);
      const vfloat4 uu = vfloat4::loadu(&u[i]);
      const vfloat4 vv = vfloat4::loadu(&v[i]);

      foreach_unique(valid1,primID,[&](const vbool4& valid1, const int primID)
      {
        for (size_t j=0; j<numFloats; j+=4) 
        {
          const size_t M = min(size_t(4),numFloats-j);
          isa::PatchEvalSimd<vbool4,vint4,vfloat4,vfloat4>(baseEntry->at(interpolationSlot(primID,j/4,stride)),parent->commitCounterSubdiv,
                                                           topo->getHalfEdge(primID),src+j*sizeof(float),stride,valid1,uu,vv,
                                                           P ? P+j*numUVs+i : nullptr,
                                                           dPdu ? dPdu+j*numUVs+i : nullptr,
                                                           dPdv ? dPdv+j*numUVs+i : nullptr,
                                                           ddPdudu ? ddPdudu+j*numUVs+i : nullptr,
                                                           ddPdvdv ? ddPdvdv+j*numUVs+i : nullptr,
                                                           ddPdudv ? ddPdudv+j*numUVs+i : nullptr,
                                                           numUVs,M);
        }
      });
    }
  }
}
