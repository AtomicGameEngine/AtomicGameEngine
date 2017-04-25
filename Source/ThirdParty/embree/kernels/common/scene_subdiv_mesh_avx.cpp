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

namespace embree
{
  SubdivMeshAVX::SubdivMeshAVX(Scene* parent, RTCGeometryFlags flags, size_t numFaces, size_t numEdges, size_t numVertices, 
                               size_t numCreases, size_t numCorners, size_t numHoles, size_t numTimeSteps)
                               : SubdivMesh(parent,flags,numFaces,numEdges,numVertices,numCreases,numCorners,numHoles,numTimeSteps) {}
    
  void SubdivMeshAVX::interpolate(unsigned primID, float u, float v, RTCBufferType buffer, float* P, float* dPdu, float* dPdv, float* ddPdudu, float* ddPdvdv, float* ddPdudv, size_t numFloats) 
  {
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

    for (size_t i=0,slot=0; i<numFloats; slot++)
    {
      if (i+4 >= numFloats)
      {
        vfloat4 Pt, dPdut, dPdvt, ddPdudut, ddPdvdvt, ddPdudvt;; 
        isa::PatchEval<vfloat4>(baseEntry->at(interpolationSlot(primID,slot,stride)),parent->commitCounterSubdiv,
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
        i+=4;
      }
      else
      {
        vfloat8 Pt, dPdut, dPdvt, ddPdudut, ddPdvdvt, ddPdudvt; 
        isa::PatchEval<vfloat8>(baseEntry->at(interpolationSlot(primID,slot,stride)),parent->commitCounterSubdiv,
                                topo->getHalfEdge(primID),src+i*sizeof(float),stride,u,v,
                                has_P ? &Pt : nullptr, 
                                has_dP ? &dPdut : nullptr, 
                                has_dP ? &dPdvt : nullptr,
                                has_ddP ? &ddPdudut : nullptr, 
                                has_ddP ? &ddPdvdvt : nullptr, 
                                has_ddP ? &ddPdudvt : nullptr);
                                    
        if (has_P) {
          for (size_t j=i; j<i+8; j++) 
            P[j] = Pt[j-i];
        }
        if (has_dP) 
        {
          for (size_t j=i; j<i+8; j++) {
            dPdu[j] = dPdut[j-i];
            dPdv[j] = dPdvt[j-i];
          }
        }
        if (has_ddP) 
        {
          for (size_t j=i; j<i+8; j++) {
            ddPdudu[j] = ddPdudut[j-i];
            ddPdvdv[j] = ddPdvdvt[j-i];
            ddPdudv[j] = ddPdudvt[j-i];
          }
        }
        i+=8;
      }
    }
    AVX_ZERO_UPPER();
  }
  
  template<typename vbool, typename vint, typename vfloat>
  void SubdivMeshAVX::interpolateHelper(const vbool& valid1, const vint& primID, const vfloat& uu, const vfloat& vv, size_t numUVs, 
                                        RTCBufferType buffer, float* P, float* dPdu, float* dPdv, float* ddPdudu, float* ddPdvdv, float* ddPdudv, size_t numFloats)
  {
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

    foreach_unique(valid1,primID,[&](const vbool& valid1, const int primID) 
    {
      for (size_t j=0,slot=0; j<numFloats; slot++)
      {
        if (j+4 >= numFloats)
        {
          const size_t M = min(size_t(4),numFloats-j);
          isa::PatchEvalSimd<vbool,vint,vfloat,vfloat4>(baseEntry->at(interpolationSlot(primID,slot,stride)),parent->commitCounterSubdiv,
                                                        topo->getHalfEdge(primID),src+j*sizeof(float),stride,valid1,uu,vv,
                                                        P ? P+j*numUVs : nullptr,
                                                        dPdu ? dPdu+j*numUVs : nullptr,
                                                        dPdv ? dPdv+j*numUVs : nullptr,
                                                        ddPdudu ? ddPdudu+j*numUVs : nullptr,
                                                        ddPdvdv ? ddPdvdv+j*numUVs : nullptr,
                                                        ddPdudv ? ddPdudv+j*numUVs : nullptr,
                                                        numUVs,M);
          j+=4;
        }
        else
        {
          const size_t M = min(size_t(8),numFloats-j);
          isa::PatchEvalSimd<vbool,vint,vfloat,vfloat8>(baseEntry->at(interpolationSlot(primID,slot,stride)),parent->commitCounterSubdiv,
                                                        topo->getHalfEdge(primID),src+j*sizeof(float),stride,valid1,uu,vv,
                                                        P ? P+j*numUVs : nullptr,
                                                        dPdu ? dPdu+j*numUVs : nullptr,
                                                        dPdv ? dPdv+j*numUVs : nullptr,
                                                        ddPdudu ? ddPdudu+j*numUVs : nullptr,
                                                        ddPdvdv ? ddPdvdv+j*numUVs : nullptr,
                                                        ddPdudv ? ddPdudv+j*numUVs : nullptr,
                                                        numUVs,M);
          j+=8;
        }
      }
    });
  }

  void SubdivMeshAVX::interpolateN(const void* valid_i, const unsigned* primIDs, const float* u, const float* v, size_t numUVs, 
                                   RTCBufferType buffer, float* P, float* dPdu, float* dPdv, float* ddPdudu, float* ddPdvdv, float* ddPdudv, size_t numFloats)
  {
#if defined(DEBUG)
    if ((parent->aflags & RTC_INTERPOLATE) == 0) 
      throw_RTCError(RTC_INVALID_OPERATION,"rtcInterpolate can only get called when RTC_INTERPOLATE is enabled for the scene");
#endif

    const int* valid = (const int*) valid_i;
    
    for (size_t i=0; i<numUVs;) 
    {
      if (i+4 >= numUVs)
      {
        vbool4 valid1 = vint4(int(i))+vint4(step) < vint4(numUVs);
        if (valid) valid1 &= vint4::loadu(&valid[i]) == vint4(-1);
        if (none(valid1)) { i+=4; continue; }
        interpolateHelper(valid1,vint4::loadu(&primIDs[i]),vfloat4::loadu(&u[i]),vfloat4::loadu(&v[i]),numUVs,buffer, 
                          P ? P+i : nullptr, 
                          dPdu ? dPdu+i : nullptr, 
                          dPdv ? dPdv+i : nullptr,
                          ddPdudu ? ddPdudu+i : nullptr,
                          ddPdvdv ? ddPdvdv+i : nullptr,
                          ddPdudv ? ddPdudv+i : nullptr,
                          numFloats);
        i+=4;
      }
      else
      {
        vbool8 valid1 = vint8(int(i))+vint8(step) < vint8(int(numUVs));
        if (valid) valid1 &= vint8::loadu(&valid[i]) == vint8(-1);
        if (none(valid1)) { i+=8; continue; }
        interpolateHelper(valid1,vint8::loadu(&primIDs[i]),vfloat8::loadu(&u[i]),vfloat8::loadu(&v[i]),numUVs,buffer, 
                          P ? P+i : nullptr, 
                          dPdu ? dPdu+i : nullptr, 
                          dPdv ? dPdv+i : nullptr,
                          ddPdudu ? ddPdudu+i : nullptr,
                          ddPdvdv ? ddPdvdv+i : nullptr,
                          ddPdudv ? ddPdudv+i : nullptr,
                          numFloats);
        i+=8;
      }
    }
    AVX_ZERO_UPPER();
  }
}
