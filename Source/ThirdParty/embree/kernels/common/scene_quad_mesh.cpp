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

#include "scene_quad_mesh.h"
#include "scene.h"

namespace embree
{
  QuadMesh::QuadMesh (Scene* parent, RTCGeometryFlags flags, size_t numQuads, size_t numVertices, size_t numTimeSteps)
    : Geometry(parent,QUAD_MESH,numQuads,numTimeSteps,flags)
  {
    quads.init(parent->device,numQuads,sizeof(Quad));
    vertices.resize(numTimeSteps);
    for (size_t i=0; i<numTimeSteps; i++) {
      vertices[i].init(parent->device,numVertices,sizeof(Vec3fa));
    }
    enabling();
  }

  void QuadMesh::enabling() 
  { 
    if (numTimeSteps == 1) parent->world.numQuads += quads.size();
    else                   parent->worldMB.numQuads += quads.size();
  }
  
  void QuadMesh::disabling() 
  { 
    if (numTimeSteps == 1) parent->world.numQuads -= quads.size();
    else                   parent->worldMB.numQuads -= quads.size();
  }

  void QuadMesh::setMask (unsigned mask) 
  {
    if (parent->isStatic() && parent->isBuild())
      throw_RTCError(RTC_INVALID_OPERATION,"static scenes cannot get modified");

    this->mask = mask; 
    Geometry::update();
  }

  void QuadMesh::setBuffer(RTCBufferType type, void* ptr, size_t offset, size_t stride, size_t size) 
  { 
    if (parent->isStatic() && parent->isBuild()) 
      throw_RTCError(RTC_INVALID_OPERATION,"static scenes cannot get modified");

    /* verify that all accesses are 4 bytes aligned */
    if (((size_t(ptr) + offset) & 0x3) || (stride & 0x3)) 
      throw_RTCError(RTC_INVALID_OPERATION,"data must be 4 bytes aligned");

    unsigned bid = type & 0xFFFF;
    if (type >= RTC_VERTEX_BUFFER0 && type < RTCBufferType(RTC_VERTEX_BUFFER0 + numTimeSteps)) 
    {
      size_t t = type - RTC_VERTEX_BUFFER0;
      vertices[t].set(ptr,offset,stride,size); 
      vertices[t].checkPadding16();
      vertices0 = vertices[0];
    } 
    else if (type >= RTC_USER_VERTEX_BUFFER0 && type < RTC_USER_VERTEX_BUFFER0+RTC_MAX_USER_VERTEX_BUFFERS)
    {
      if (bid >= userbuffers.size()) userbuffers.resize(bid+1);
      userbuffers[bid] = APIBuffer<char>(parent->device,numVertices(),stride);
      userbuffers[bid].set(ptr,offset,stride,size);  
      userbuffers[bid].checkPadding16();
    }
    else if (type == RTC_INDEX_BUFFER) 
    {
      if (size != (size_t)-1) disabling();
      quads.set(ptr,offset,stride,size);
      setNumPrimitives(size);
      if (size != (size_t)-1) enabling();
    }
    else
      throw_RTCError(RTC_INVALID_ARGUMENT,"unknown buffer type");
  }

  void* QuadMesh::map(RTCBufferType type) 
  {
    if (parent->isStatic() && parent->isBuild())
      throw_RTCError(RTC_INVALID_OPERATION,"static scenes cannot get modified");

	if (type == RTC_INDEX_BUFFER) {
      return quads.map(parent->numMappedBuffers);
    }
    else if (type >= RTC_VERTEX_BUFFER0 && type < RTCBufferType(RTC_VERTEX_BUFFER0 + numTimeSteps)) {
      return vertices[type - RTC_VERTEX_BUFFER0].map(parent->numMappedBuffers);
    }
    else {
      throw_RTCError(RTC_INVALID_ARGUMENT,"unknown buffer type"); 
      return nullptr;
    }
  }

  void QuadMesh::unmap(RTCBufferType type) 
  {
    if (parent->isStatic() && parent->isBuild())
      throw_RTCError(RTC_INVALID_OPERATION,"static scenes cannot get modified");

    if (type == RTC_INDEX_BUFFER) {
      quads.unmap(parent->numMappedBuffers);
    }
    else if (type >= RTC_VERTEX_BUFFER0 && type < RTCBufferType(RTC_VERTEX_BUFFER0 + numTimeSteps)) {
      vertices[type - RTC_VERTEX_BUFFER0].unmap(parent->numMappedBuffers);
      vertices0 = vertices[0];
    }
    else {
      throw_RTCError(RTC_INVALID_ARGUMENT,"unknown buffer type"); 
    }
  }

  void QuadMesh::immutable () 
  {
    const bool freeQuads = !parent->needQuadIndices;
    const bool freeVertices  = !parent->needQuadVertices;
    if (freeQuads) quads.free(); 
    if (freeVertices )
      for (auto& buffer : vertices)
        buffer.free();
  }

  bool QuadMesh::verify () 
  {
    /*! verify consistent size of vertex arrays */
    if (vertices.size() == 0) return false;
    for (const auto& buffer : vertices)
      if (buffer.size() != numVertices())
        return false;

    /*! verify quad indices */
    for (size_t i=0; i<quads.size(); i++) {     
      if (quads[i].v[0] >= numVertices()) return false; 
      if (quads[i].v[1] >= numVertices()) return false; 
      if (quads[i].v[2] >= numVertices()) return false; 
      if (quads[i].v[3] >= numVertices()) return false; 
    }

    /*! verify vertices */
    for (const auto& buffer : vertices)
      for (size_t i=0; i<buffer.size(); i++)
	if (!isvalid(buffer[i])) 
	  return false;

    return true;
  }

  void QuadMesh::interpolate(unsigned primID, float u, float v, RTCBufferType buffer, float* P, float* dPdu, float* dPdv, float* ddPdudu, float* ddPdvdv, float* ddPdudv, size_t numFloats)
  {
    /* test if interpolation is enabled */
#if defined(DEBUG)
    if ((parent->aflags & RTC_INTERPOLATE) == 0) 
      throw_RTCError(RTC_INVALID_OPERATION,"rtcInterpolate can only get called when RTC_INTERPOLATE is enabled for the scene");
#endif

    /* calculate base pointer and stride */
    assert((buffer >= RTC_VERTEX_BUFFER0 && buffer < RTCBufferType(RTC_VERTEX_BUFFER0 + numTimeSteps)) ||
           (buffer >= RTC_USER_VERTEX_BUFFER0 && buffer <= RTC_USER_VERTEX_BUFFER1));
    const char* src = nullptr; 
    size_t stride = 0;
    if (buffer >= RTC_USER_VERTEX_BUFFER0) {
      src    = userbuffers[buffer&0xFFFF].getPtr();
      stride = userbuffers[buffer&0xFFFF].getStride();
    } else {
      src    = vertices[buffer&0xFFFF].getPtr();
      stride = vertices[buffer&0xFFFF].getStride();
    }

    for (size_t i=0; i<numFloats; i+=VSIZEX)
    {
      const vboolx valid = vintx((int)i)+vintx(step) < vintx(numFloats);
      const size_t ofs = i*sizeof(float);
      const Quad& tri = quad(primID);
      const vfloatx p0 = vfloatx::loadu(valid,(float*)&src[tri.v[0]*stride+ofs]);
      const vfloatx p1 = vfloatx::loadu(valid,(float*)&src[tri.v[1]*stride+ofs]);
      const vfloatx p2 = vfloatx::loadu(valid,(float*)&src[tri.v[2]*stride+ofs]);
      const vfloatx p3 = vfloatx::loadu(valid,(float*)&src[tri.v[3]*stride+ofs]);      
      const vboolx left = u+v <= 1.0f;
      const vfloatx Q0 = select(left,p0,p2);
      const vfloatx Q1 = select(left,p1,p3);
      const vfloatx Q2 = select(left,p3,p1);
      const vfloatx U  = select(left,u,vfloatx(1.0f)-u);
      const vfloatx V  = select(left,v,vfloatx(1.0f)-v);
      const vfloatx W  = 1.0f-U-V;
      if (P) {
        vfloatx::storeu(valid,P+i,madd(W,Q0,madd(U,Q1,V*Q2)));
      }
      if (dPdu) { 
        assert(dPdu); vfloatx::storeu(valid,dPdu+i,select(left,Q1-Q0,Q0-Q1));
        assert(dPdv); vfloatx::storeu(valid,dPdv+i,select(left,Q2-Q0,Q0-Q2));
      }
      if (ddPdudu) { 
        assert(ddPdudu); vfloatx::storeu(valid,ddPdudu+i,vfloatx(zero));
        assert(ddPdvdv); vfloatx::storeu(valid,ddPdvdv+i,vfloatx(zero));
        assert(ddPdudv); vfloatx::storeu(valid,ddPdudv+i,vfloatx(zero));
      }
    }
  }
}
