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

namespace embree
{
  /*! Instanced geometry */
  struct GeometryInstance : public Geometry
  {
    ALIGNED_STRUCT;
  public:
    GeometryInstance (Scene* parent, Geometry* geom); 
    virtual void build() {}
    virtual void enabling ();
    virtual void disabling();
    virtual void setMask (unsigned mask);
    virtual void setTransform(const AffineSpace3fa& local2world, size_t timeStep);
    
    void count(ssize_t f);
  public:
    AffineSpace3fa local2world; //!< transforms from local space to world space
    AffineSpace3fa world2local; //!< transforms from world space to local space
    Geometry* geom;             //!< pointer to instanced acceleration structure
  };
}
