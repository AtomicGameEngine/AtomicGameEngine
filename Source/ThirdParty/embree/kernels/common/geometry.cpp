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

#include "geometry.h"
#include "scene.h"

namespace embree
{
  Geometry::Geometry (Scene* parent, Type type, size_t numPrimitives, size_t numTimeSteps, RTCGeometryFlags flags) 
    : parent(parent), id(0), type(type), 
      numPrimitives(numPrimitives), numPrimitivesChanged(false),
      numTimeSteps(unsigned(numTimeSteps)), fnumTimeSegments(float(numTimeSteps-1)), flags(flags),
      enabled(true), modified(true), userPtr(nullptr), mask(-1), used(1),
      intersectionFilter1(nullptr), occlusionFilter1(nullptr),
      intersectionFilter4(nullptr), occlusionFilter4(nullptr),
      intersectionFilter8(nullptr), occlusionFilter8(nullptr),
      intersectionFilter16(nullptr), occlusionFilter16(nullptr),
      intersectionFilterN(nullptr), occlusionFilterN(nullptr),
      hasIntersectionFilterMask(0), hasOcclusionFilterMask(0), ispcIntersectionFilterMask(0), ispcOcclusionFilterMask(0)
  {
    parent->setModified();
  }

  Geometry::~Geometry() {
  }

  void Geometry::preCommit() {
  }

  void Geometry::postCommit()
  {
    /* make static geometry immutable */
    if (parent->isStatic()) 
      immutable();

    /* clear modified flag */
    if (isEnabled()) 
      clearModified();
  }

  void Geometry::updateIntersectionFilters(bool enable)
  {
    const size_t num1  = (intersectionFilter1  != nullptr) + (occlusionFilter1  != nullptr);
    const size_t num4  = (intersectionFilter4  != nullptr) + (occlusionFilter4  != nullptr);
    const size_t num8  = (intersectionFilter8  != nullptr) + (occlusionFilter8  != nullptr);
    const size_t num16 = (intersectionFilter16 != nullptr) + (occlusionFilter16 != nullptr);
    const size_t numN  = (intersectionFilterN  != nullptr) + (occlusionFilterN  != nullptr);

    if (enable) {
      parent->numIntersectionFilters1 += num1;
      parent->numIntersectionFilters4 += num4;
      parent->numIntersectionFilters8 += num8;
      parent->numIntersectionFilters16 += num16;
      parent->numIntersectionFiltersN += numN;
    } else {
      parent->numIntersectionFilters1 -= num1;
      parent->numIntersectionFilters4 -= num4;
      parent->numIntersectionFilters8 -= num8;
      parent->numIntersectionFilters16 -= num16;
      parent->numIntersectionFiltersN -= numN;
    }
  }

  void Geometry::enable () 
  {
    if (parent->isStatic() && parent->isBuild()) 
      throw_RTCError(RTC_INVALID_OPERATION,"static scenes cannot get modified");

    if (isEnabled()) 
      return;

    updateIntersectionFilters(true);
    parent->setModified();
    used++;
    enabled = true;
    enabling();
  }

  void Geometry::update() 
  {
    if (parent->isStatic() && parent->isBuild()) 
      throw_RTCError(RTC_INVALID_OPERATION,"static scenes cannot get modified");

    parent->setModified();
    modified = true;
  }

  void Geometry::disable () 
  {
    if (parent->isStatic() && parent->isBuild()) 
      throw_RTCError(RTC_INVALID_OPERATION,"static scenes cannot get modified");

    if (isDisabled()) 
      return;

    updateIntersectionFilters(false);
    parent->setModified();
    used--;
    enabled = false;
    disabling();
  }

  void Geometry::setUserData (void* ptr)
  {
    if (parent->isStatic() && parent->isBuild())
      throw_RTCError(RTC_INVALID_OPERATION,"static scenes cannot get modified");

    userPtr = ptr;
  }
  
  void Geometry::setIntersectionFilterFunction (RTCFilterFunc filter, bool ispc) 
  {
    if (parent->isStreamMode())
      throw_RTCError(RTC_INVALID_OPERATION,"you have to use rtcSetIntersectionFilterFunctionN in stream mode");

    if (parent->isStatic() && parent->isBuild())
      throw_RTCError(RTC_INVALID_OPERATION,"static scenes cannot get modified");

    if (type != TRIANGLE_MESH && type != QUAD_MESH && type != LINE_SEGMENTS && type != BEZIER_CURVES && type != SUBDIV_MESH)
      throw_RTCError(RTC_INVALID_OPERATION,"filter functions not supported for this geometry"); 
    
    parent->numIntersectionFilters1 -= intersectionFilter1 != nullptr;
    parent->numIntersectionFilters1 += filter != nullptr;
    intersectionFilter1 = filter;
    if (filter) hasIntersectionFilterMask  |= HAS_FILTER1; else hasIntersectionFilterMask  &= ~HAS_FILTER1;
  }
    
  void Geometry::setIntersectionFilterFunction4 (RTCFilterFunc4 filter, bool ispc) 
  { 
    if (parent->isStreamMode())
      throw_RTCError(RTC_INVALID_OPERATION,"you have to use rtcSetIntersectionFilterFunctionN in stream mode");

    if (parent->isStatic() && parent->isBuild())
      throw_RTCError(RTC_INVALID_OPERATION,"static scenes cannot get modified");

    if (type != TRIANGLE_MESH && type != QUAD_MESH && type != LINE_SEGMENTS && type != BEZIER_CURVES && type != SUBDIV_MESH)
      throw_RTCError(RTC_INVALID_OPERATION,"filter functions not supported for this geometry"); 

    parent->numIntersectionFilters4 -= intersectionFilter4 != nullptr;
    parent->numIntersectionFilters4 += filter != nullptr;
    intersectionFilter4 = filter;
    if (filter) hasIntersectionFilterMask  |= HAS_FILTER4; else hasIntersectionFilterMask  &= ~HAS_FILTER4;
    if (ispc  ) ispcIntersectionFilterMask |= HAS_FILTER4; else ispcIntersectionFilterMask &= ~HAS_FILTER4;
  }
    
  void Geometry::setIntersectionFilterFunction8 (RTCFilterFunc8 filter, bool ispc) 
  { 
    if (parent->isStreamMode())
      throw_RTCError(RTC_INVALID_OPERATION,"you have to use rtcSetIntersectionFilterFunctionN in stream mode");

    if (parent->isStatic() && parent->isBuild())
      throw_RTCError(RTC_INVALID_OPERATION,"static scenes cannot get modified");
    
    if (type != TRIANGLE_MESH && type != QUAD_MESH && type != LINE_SEGMENTS && type != BEZIER_CURVES && type != SUBDIV_MESH)
      throw_RTCError(RTC_INVALID_OPERATION,"filter functions not supported for this geometry"); 

    parent->numIntersectionFilters8 -= intersectionFilter8 != nullptr;
    parent->numIntersectionFilters8 += filter != nullptr;
    intersectionFilter8 = filter;
    if (filter) hasIntersectionFilterMask  |= HAS_FILTER8; else hasIntersectionFilterMask  &= ~HAS_FILTER8;
    if (ispc  ) ispcIntersectionFilterMask |= HAS_FILTER8; else ispcIntersectionFilterMask &= ~HAS_FILTER8;
  }
  
  void Geometry::setIntersectionFilterFunction16 (RTCFilterFunc16 filter, bool ispc) 
  { 
    if (parent->isStreamMode())
      throw_RTCError(RTC_INVALID_OPERATION,"you have to use rtcSetIntersectionFilterFunctionN in stream mode");

    if (parent->isStatic() && parent->isBuild())
      throw_RTCError(RTC_INVALID_OPERATION,"static scenes cannot get modified");

    if (type != TRIANGLE_MESH && type != QUAD_MESH && type != LINE_SEGMENTS && type != BEZIER_CURVES && type != SUBDIV_MESH)
      throw_RTCError(RTC_INVALID_OPERATION,"filter functions not supported for this geometry"); 

    parent->numIntersectionFilters16 -= intersectionFilter16 != nullptr;
    parent->numIntersectionFilters16 += filter != nullptr;
    intersectionFilter16 = filter;
    if (filter) hasIntersectionFilterMask  |= HAS_FILTER16; else hasIntersectionFilterMask  &= ~HAS_FILTER16;
    if (ispc  ) ispcIntersectionFilterMask |= HAS_FILTER16; else ispcIntersectionFilterMask &= ~HAS_FILTER16;
  }
  
  void Geometry::setIntersectionFilterFunctionN (RTCFilterFuncN filter) 
  { 
    if (!parent->isStreamMode())
      throw_RTCError(RTC_INVALID_OPERATION,"you can use rtcSetIntersectionFilterFunctionN only in stream mode");

    if (parent->isStatic() && parent->isBuild())
      throw_RTCError(RTC_INVALID_OPERATION,"static scenes cannot get modified");

    if (type != TRIANGLE_MESH && type != QUAD_MESH && type != LINE_SEGMENTS && type != BEZIER_CURVES && type != SUBDIV_MESH)
      throw_RTCError(RTC_INVALID_OPERATION,"filter functions not supported for this geometry"); 

    parent->numIntersectionFiltersN -= intersectionFilterN != nullptr;
    parent->numIntersectionFiltersN += filter != nullptr;
    intersectionFilterN = filter;
    if (filter) hasIntersectionFilterMask  |= HAS_FILTERN; else hasIntersectionFilterMask  &= ~HAS_FILTERN;
  }

  void Geometry::setOcclusionFilterFunction (RTCFilterFunc filter, bool ispc) 
  {
    if (parent->isStreamMode())
      throw_RTCError(RTC_INVALID_OPERATION,"you have to use rtcSetOcclusionFilterFunctionN in stream mode");

    if (parent->isStatic() && parent->isBuild())
      throw_RTCError(RTC_INVALID_OPERATION,"static scenes cannot get modified");

    if (type != TRIANGLE_MESH && type != QUAD_MESH && type != LINE_SEGMENTS && type != BEZIER_CURVES && type != SUBDIV_MESH)
      throw_RTCError(RTC_INVALID_OPERATION,"filter functions not supported for this geometry"); 

    parent->numIntersectionFilters1 -= occlusionFilter1 != nullptr;
    parent->numIntersectionFilters1 += filter != nullptr;
    occlusionFilter1 = filter;
    if (filter) hasOcclusionFilterMask  |= HAS_FILTER1; else hasOcclusionFilterMask  &= ~HAS_FILTER1;
  }
    
  void Geometry::setOcclusionFilterFunction4 (RTCFilterFunc4 filter, bool ispc) 
  { 
    if (parent->isStreamMode())
      throw_RTCError(RTC_INVALID_OPERATION,"you have to use rtcSetOcclusionFilterFunctionN in stream mode");

    if (parent->isStatic() && parent->isBuild())
      throw_RTCError(RTC_INVALID_OPERATION,"static scenes cannot get modified");

    if (type != TRIANGLE_MESH && type != QUAD_MESH && type != LINE_SEGMENTS && type != BEZIER_CURVES && type != SUBDIV_MESH)
      throw_RTCError(RTC_INVALID_OPERATION,"filter functions not supported for this geometry"); 

    parent->numIntersectionFilters4 -= occlusionFilter4 != nullptr;
    parent->numIntersectionFilters4 += filter != nullptr;
    occlusionFilter4 = filter;
    if (filter) hasOcclusionFilterMask  |= HAS_FILTER4; else hasOcclusionFilterMask  &= ~HAS_FILTER4;
    if (ispc  ) ispcOcclusionFilterMask |= HAS_FILTER4; else ispcOcclusionFilterMask &= ~HAS_FILTER4;
  }
    
  void Geometry::setOcclusionFilterFunction8 (RTCFilterFunc8 filter, bool ispc) 
  { 
    if (parent->isStreamMode())
      throw_RTCError(RTC_INVALID_OPERATION,"you have to use rtcSetOcclusionFilterFunctionN in stream mode");

    if (parent->isStatic() && parent->isBuild())
      throw_RTCError(RTC_INVALID_OPERATION,"static scenes cannot get modified");

    if (type != TRIANGLE_MESH && type != QUAD_MESH && type != LINE_SEGMENTS && type != BEZIER_CURVES && type != SUBDIV_MESH)
      throw_RTCError(RTC_INVALID_OPERATION,"filter functions not supported for this geometry"); 

    parent->numIntersectionFilters8 -= occlusionFilter8 != nullptr;
    parent->numIntersectionFilters8 += filter != nullptr;
    occlusionFilter8 = filter;
    if (filter) hasOcclusionFilterMask  |= HAS_FILTER8; else hasOcclusionFilterMask  &= ~HAS_FILTER8;
    if (ispc  ) ispcOcclusionFilterMask |= HAS_FILTER8; else ispcOcclusionFilterMask &= ~HAS_FILTER8;
  }
  
  void Geometry::setOcclusionFilterFunction16 (RTCFilterFunc16 filter, bool ispc) 
  { 
    if (parent->isStreamMode())
      throw_RTCError(RTC_INVALID_OPERATION,"you have to use rtcSetOcclusionFilterFunctionN in stream mode");

    if (parent->isStatic() && parent->isBuild())
      throw_RTCError(RTC_INVALID_OPERATION,"static scenes cannot get modified");

    if (type != TRIANGLE_MESH && type != QUAD_MESH && type != LINE_SEGMENTS && type != BEZIER_CURVES && type != SUBDIV_MESH) 
      throw_RTCError(RTC_INVALID_OPERATION,"filter functions not supported for this geometry"); 

    parent->numIntersectionFilters16 -= occlusionFilter16 != nullptr;
    parent->numIntersectionFilters16 += filter != nullptr;
    occlusionFilter16 = filter;
    if (filter) hasOcclusionFilterMask  |= HAS_FILTER16; else hasOcclusionFilterMask  &= ~HAS_FILTER16;
    if (ispc  ) ispcOcclusionFilterMask |= HAS_FILTER16; else ispcOcclusionFilterMask &= ~HAS_FILTER16;
  }

  void Geometry::setOcclusionFilterFunctionN (RTCFilterFuncN filter) 
  { 
    if (!parent->isStreamMode())
      throw_RTCError(RTC_INVALID_OPERATION,"you can use rtcSetOcclusionFilterFunctionN only in stream mode");

    if (parent->isStatic() && parent->isBuild())
      throw_RTCError(RTC_INVALID_OPERATION,"static scenes cannot get modified");

    if (type != TRIANGLE_MESH && type != QUAD_MESH && type != LINE_SEGMENTS && type != BEZIER_CURVES && type != SUBDIV_MESH) 
      throw_RTCError(RTC_INVALID_OPERATION,"filter functions not supported for this geometry"); 

    parent->numIntersectionFiltersN -= occlusionFilterN != nullptr;
    parent->numIntersectionFiltersN += filter != nullptr;
    occlusionFilterN = filter;
    if (filter) hasOcclusionFilterMask  |= HAS_FILTERN; else hasOcclusionFilterMask  &= ~HAS_FILTERN;
  }

  void Geometry::interpolateN(const void* valid_i, const unsigned* primIDs, const float* u, const float* v, size_t numUVs, 
                              RTCBufferType buffer, float* P, float* dPdu, float* dPdv, float* ddPdudu, float* ddPdvdv, float* ddPdudv, size_t numFloats)
  {
    if (numFloats > 256) throw_RTCError(RTC_INVALID_OPERATION,"maximally 256 floating point values can be interpolated per vertex");
    const int* valid = (const int*) valid_i;

    __aligned(64) float P_tmp[256];
    __aligned(64) float dPdu_tmp[256];
    __aligned(64) float dPdv_tmp[256];
    __aligned(64) float ddPdudu_tmp[256];
    __aligned(64) float ddPdvdv_tmp[256];
    __aligned(64) float ddPdudv_tmp[256];

    float* Pt = P ? P_tmp : nullptr;
    float* dPdut = nullptr, *dPdvt = nullptr;
    if (dPdu) { dPdut = dPdu_tmp; dPdvt = dPdv_tmp; }
    float* ddPdudut = nullptr, *ddPdvdvt = nullptr, *ddPdudvt = nullptr;
    if (ddPdudu) { ddPdudut = ddPdudu_tmp; ddPdvdvt = ddPdvdv_tmp; ddPdudvt = ddPdudv_tmp; }
    
    for (size_t i=0; i<numUVs; i++)
    {
      if (valid && !valid[i]) continue;
      interpolate(primIDs[i],u[i],v[i],buffer,Pt,dPdut,dPdvt,ddPdudut,ddPdvdvt,ddPdudvt,numFloats);
      
      if (likely(P)) {
        for (size_t j=0; j<numFloats; j++) 
          P[j*numUVs+i] = Pt[j];
      }
      if (likely(dPdu)) 
      {
        for (size_t j=0; j<numFloats; j++) {
          dPdu[j*numUVs+i] = dPdut[j];
          dPdv[j*numUVs+i] = dPdvt[j];
        }
      }
      if (likely(ddPdudu)) 
      {
        for (size_t j=0; j<numFloats; j++) {
          ddPdudu[j*numUVs+i] = ddPdudut[j];
          ddPdvdv[j*numUVs+i] = ddPdvdvt[j];
          ddPdudv[j*numUVs+i] = ddPdudvt[j];
        }
      }
    }
  }
}
