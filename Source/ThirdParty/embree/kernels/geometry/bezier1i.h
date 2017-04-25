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

#include "primitive.h"
#include "bezier1v.h"

namespace embree
{
  struct Bezier1i
  {
    struct Type : public PrimitiveType {
      Type ();
      size_t size(const char* This) const;
    };
    static Type type;

  public:

    /* Returns maximal number of stored primitives */
    static __forceinline size_t max_size() { return 1; }

    /* Returns required number of primitive blocks for N primitives */
    static __forceinline size_t blocks(size_t N) { return N; }

  public:

    /*! Default constructor. */
    __forceinline Bezier1i () {}

    /*! Construction from vertices and IDs. */
    __forceinline Bezier1i (const unsigned vertexID, const unsigned geomID, const unsigned primID)
      : vertexID(vertexID), geom(geomID), prim(primID) {}

    /*! returns geometry ID */
    __forceinline unsigned geomID() const { return geom; }

    /*! returns primitive ID */
    __forceinline unsigned primID() const { return prim; }

    /*! fill curve from curve list */
    __forceinline void fill(const PrimRef* prims, size_t& i, size_t end, Scene* scene)
    {
      const PrimRef& prim = prims[i];
      i++;
      const unsigned geomID = prim.geomID();
      const unsigned primID = prim.primID();
      const NativeCurves* curves = scene->get<NativeCurves>(geomID);
      const unsigned vertexID = curves->curve(primID);
      new (this) Bezier1i(vertexID,geomID,primID);
    }

    /*! fill curve from curve list */
    __forceinline void fill(const BezierPrim* prims, size_t& i, size_t end, Scene* scene)
    {
      const BezierPrim& curve = prims[i]; i++;
      const unsigned geomID = curve.geomID();
      const unsigned primID = curve.primID();
      const NativeCurves* curves = scene->get<NativeCurves>(geomID);
      const unsigned vertexID = curves->curve(primID);
      new (this) Bezier1i(vertexID,geomID,primID);
    }

  public:
    unsigned vertexID; //!< index of start vertex
    unsigned geom;     //!< geometry ID
    unsigned prim;     //!< primitive ID
  };
}
