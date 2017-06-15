//
// Copyright (c) 2017, THUNDERBEAST GAMES LLC All rights reserved
// Copyright (c) 2015 Dmitry Sovetov
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include <Atomic/Container/RefCounted.h>
#include <Atomic/Container/ArrayPtr.h>

#include "LightRay.h"
#include "BakeLight.h"

using namespace Atomic;

namespace AtomicGlow
{

class SceneBaker;

const int PHOTON_TRI_MAX = 8;
const int PHOTON_TRI_INVALID = -1;

class PhotonMap : public RefCounted
{
    ATOMIC_REFCOUNTED(PhotonMap)

public:

    struct Photon
    {
      int tris_[PHOTON_TRI_MAX];
      Color gathered_[PHOTON_TRI_MAX];
      Color color_[PHOTON_TRI_MAX];
      int photons_[PHOTON_TRI_MAX];

      int MapTriIndex(int triIndex) const
      {
          for (int i = 0; i < PHOTON_TRI_MAX; i++)
          {
              if (tris_[i] == PHOTON_TRI_INVALID)
                  break;

              if (tris_[i] == triIndex)
                  return i;
          }

          return PHOTON_TRI_INVALID;

      }

      void Reset()
      {
          for (int i = 0; i < PHOTON_TRI_MAX; i++)
          {
              tris_[i] = PHOTON_TRI_INVALID;
              color_[i] = gathered_[i] = Color::BLACK;
              photons_[i] = 0;
          }
      }

    };

    PhotonMap(BakeMesh* bakeMesh, int width, int height);
    virtual ~PhotonMap();

    // Allocates a triangle, always check return, in readOnly mode won't allocate new tri
    bool GetPhoton(int triIndex, const Vector2& uv, Color& color, int& photons, Color& gathered, bool readOnly = false)
    {
        color = Color::BLACK;
        gathered = Color::BLACK;
        photons = 0;

        int x = static_cast<int>( uv.x_ * (width_  - 1) );
        int y = static_cast<int>( uv.y_ * (height_ - 1) );

        if (x < 0 || x >= width_)
            return false;

        if (y < 0 || y >= height_)
            return false;

        Photon* photon = &photons_[y * width_ + x];

        int idx = photon->MapTriIndex(triIndex);

        if (idx == PHOTON_TRI_INVALID)
        {
            if (readOnly)
            {
                return false;
            }

            for ( int i = 0; i < PHOTON_TRI_MAX; i++ )
            {
                if (photon->tris_[i] == PHOTON_TRI_INVALID)
                {
                    idx = photon->tris_[i] = triIndex;
                    break;
                }
            }

        }

        if (idx == PHOTON_TRI_INVALID)
        {
            // out of tri storage
            return false;
        }

        color = photon->color_[idx];
        gathered = photon->gathered_[idx];
        photons = photon->photons_[idx];

        return true;

    }

    // Adds a photon
    bool AddPhoton(int triIndex, const Vector2& uv, const Color& color)
    {

        if (color.r_ < 0.01f && color.g_ < 0.01f && color.b_ < 0.01f)
        {
            // filter out tiny adds
            return false;
        }

        int x = static_cast<int>( uv.x_ * (width_  - 1) );
        int y = static_cast<int>( uv.y_ * (height_ - 1) );

        if (x < 0 || x >= width_)
            return false;

        if (y < 0 || y >= height_)
            return false;

        Photon* photon = &photons_[y * width_ + x];

        int idx = photon->MapTriIndex(triIndex);

        if (idx == PHOTON_TRI_INVALID)
        {
            for ( int i = 0; i < PHOTON_TRI_MAX; i++ )
            {
                if (photon->tris_[i] == PHOTON_TRI_INVALID)
                {
                    idx = photon->tris_[i] = triIndex;
                    break;
                }
            }
        }

        if (idx == PHOTON_TRI_INVALID)
        {
            // out of tri storage
            return false;
        }

        photon->color_[idx] += color;
        photon->photons_[idx]++;

        return true;

    }

    void SavePng(const String& filename) const;

    void Gather( int radius );

    Color Gather( int triIndex, int x, int y, int radius ) const;

private:

    int width_;
    int height_;

    WeakPtr<BakeMesh> bakeMesh_;
    SharedArrayPtr<Photon> photons_;

};

class Photons : public RefCounted
{

    ATOMIC_REFCOUNTED(Photon)

public:

    // Constructs a Photons instance.
    // param scene Scene to be baked.
    // param passCount Amount of photon passes.
    // param maxDepth Maximum photon tracing depth (number of light bounces).
    // param energyThreshold The minimum energy that photon should have to continue tracing.
    // param maxDistance The reflected light maximum distance. All intersections above this value will be ignored.
    Photons( SceneBaker* sceneBaker, int passCount, int maxDepth, float energyThreshold, float maxDistance );

    // Emits photons from all scene lights, returning number of photons emitted
    virtual int Emit( const Vector<SharedPtr<BakeLight>>& bakeLights );

private:

    // Emits photons from a given light.
    void EmitPhotons( const BakeLight* light );

    // Traces a photon path for a given depth.
    /*
     Traces a photon path for a given depth. Each time the photon bounces the
     reflected light is stored to a photon map.

     \param attenuation Light attenuation model.
     \param position Photon's start position.
     \param direction Photon's direction.
     \param color Photon's color.
     \param energy Photon's energy.
     \param depth Current trace depth.
     */
    void Trace(const LightAttenuation* attenuation, const Vector3& position, const Vector3& direction, const Color& color, int depth, unsigned lastGeomID = RTC_INVALID_GEOMETRY_ID, unsigned lastPrimID = RTC_INVALID_GEOMETRY_ID );

    //Stores a photon bounce.
    void Store( PhotonMap* photonmap, int triIdx, const Color& color, const Vector2& uv );

private:

    // Parent scene.
    SharedPtr<SceneBaker> sceneBaker_;

    // Amount of photon passes, at each pass there will be emitted N photons.
    int passCount_;

    // Maximum tracing depth.
    int maxDepth_;

    // Photon energy threshold.
    float energyThreshold_;

    // Maximum intersection distance
    float maxDistance_;

    // Total amount of photons stored.
    int photonCount_;

    LightRay lightRay_;
};

}
