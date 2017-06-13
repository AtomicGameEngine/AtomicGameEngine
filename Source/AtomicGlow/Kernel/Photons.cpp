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

#include "EmbreeScene.h"
#include "SceneBaker.h"
#include "Photons.h"
#include "BakeMesh.h"

namespace AtomicGlow
{


PhotonMap::PhotonMap(int width, int height) :
    width_(width),
    height_(height)
{

    if (width_ > 0 && height_ > 0)
    {
        photons_ = new Photon[width_ * height_];

        for (int i = 0; i < width_ * height_; i++)
        {
            photons_[i].Reset();
        }
    }

}

PhotonMap::~PhotonMap()
{

}

void PhotonMap::Gather( int radius )
{
    for( int y = 0; y < height_; y++ )
    {
        for( int x = 0; x < width_; x++ )
        {
            photons_[y * width_ + x].gathered_ = Gather(x, y, radius);
        }
    }
}

Color PhotonMap::Gather( int x, int y, int radius ) const
{
    Color color;
    int photons = 0;

    for( int j = y - radius; j <= y + radius; j++ )
    {
        for( int i = x - radius; i <= x + radius; i++ )
        {
            if( i < 0 || j < 0 || i >= width_ || j >= height_ )
            {
                continue;
            }

            const Photon& photon = photons_[j * width_ + i];

            float distance = sqrtf( static_cast<float>( (x - i) * (x - i) + (y - j) * (y - j) ) );

            if( distance > radius ) {
                continue;
            }

            if (photon.photons_)
            {
                color += photon.color_;
                photons += photon.photons_;
            }
        }
    }

    if( photons == 0 )
    {
        return Color::BLACK;
    }

    float divisor = 1.0f / static_cast<float>( photons );

    color.r_ *= divisor;
    color.g_ *= divisor;
    color.b_ *= divisor;
    color.a_ *= divisor;

    return color;
}


Photons::Photons( SceneBaker* sceneBaker, int passCount, int maxDepth, float energyThreshold, float maxDistance )
    : sceneBaker_( SharedPtr<SceneBaker>(sceneBaker) ),
      passCount_( passCount ),
      maxDepth_( maxDepth ),
      energyThreshold_( energyThreshold ),
      maxDistance_( maxDistance ),
      photonCount_( 0 )
{

}

int Photons::Emit( const Vector<SharedPtr<BakeLight>>& bakeLights )
{
    // TODO: depending on performance, photon emission may need to be threaded

    for( int j = 0; j < passCount_; j++ )
    {
        Vector<SharedPtr<BakeLight>>::ConstIterator itr = bakeLights.Begin();

        while (itr != bakeLights.End())
        {
            BakeLight* bakeLight = *itr;

            if( !bakeLight->GetPhotonEmitter() )
            {
                itr++;
                continue;
            }

            EmitPhotons( bakeLight );

            itr++;
        }
    }

    return photonCount_;
}

void Photons::EmitPhotons( const BakeLight* light )
{
    PhotonEmitter* emitter = light->GetPhotonEmitter();
    Vector3 direction;
    Vector3 position;

    for( int i = 0, n = emitter->GetPhotonCount(); i < n; i++ )
    {
        // ** Emit photon
        emitter->Emit( sceneBaker_, position, direction );

        // ** Calculate light cutoff
        float cut = 1.0f;

        if( const LightCutoff* cutoff = light->GetCutoffModel() )
        {
            cut = cutoff->GetCutoffForDirection( direction );
        }

        if( cut <= 0.0f )
        {
            continue;
        }

        Trace( light->GetAttenuationModel(), position, direction, light->GetColor() * light->GetIntensity() * cut, 0 );
    }

}

void Photons::Trace(const LightAttenuation* attenuation, const Vector3& position, const Vector3& direction, const Color& color, int depth , unsigned lastGeomID, unsigned lastPrimID)
{
    // ** Maximum depth or energy threshold exceeded
    if( depth > maxDepth_ || color.Luma() < energyThreshold_ )
    {
        return;
    }

    // clean this mess up
    RTCScene scene = sceneBaker_->GetEmbreeScene()->GetRTCScene();

    lightRay_.SetupRay(position, direction, .001f, maxDistance_);

    RTCRay& ray = lightRay_.rtcRay_;

    rtcIntersect(scene, ray);

    // The photon didn't hit anything
    if (ray.geomID == RTC_INVALID_GEOMETRY_ID)
    {
        return;
    }

    // self hit
    if (ray.geomID == lastGeomID && ray.primID == lastPrimID)
    {
        return;
    }

    Vector3 hitPosition = position + (direction * ray.tfar);

    Vector3 hitNormal(ray.Ng[0], ray.Ng[1], ray.Ng[2]);
    hitNormal.Normalize();

    // ** Energy attenuation after a photon has passed the traced segment
    float att = 1.0f;
    if( attenuation ) {
        /*att =*/ attenuation->Calculate( (position - hitPosition).Length() );
    }

    // ** Energy after reflection
    float influence = LightInfluence::GetLambert( direction, hitNormal ) * att;

    if (influence <= 0.0f)
    {
        return;
    }

    BakeMesh* bakeMesh = sceneBaker_->GetEmbreeScene()->GetBakeMesh(ray.geomID);

    Color hitColor;
    Vector3 bary(ray.u, ray.v, 1.0f-ray.u-ray.v);

    // TODO: alpha mask support?
    if (!bakeMesh || !bakeMesh->GetUV0Color(ray.primID, bary, hitColor))
    {
        return;
    }

    // ** Final photon color
    hitColor.r_ *= (color.r_ * influence);
    hitColor.g_ *= (color.g_ * influence);
    hitColor.b_ *= (color.b_ * influence);

    Vector2 st;
    bakeMesh->GetST(ray.primID, 1, bary, st);

    // ** Store photon energy
    Store( bakeMesh->GetPhotonMap(), hitColor, st );

    // ** Keep tracing
    Vector3 dir;
    Vector3::GetRandomHemisphereDirection(dir, hitNormal);
    Trace( attenuation,hitPosition, dir, hitColor, depth + 1, ray.geomID, ray.primID );
}

void Photons::Store( PhotonMap* photonmap, const Color& color, const Vector2& uv )
{
    if( !photonmap ) {
        return;
    }

    if (color.r_ < 0.01f && color.g_ < 0.01f && color.b_ < 0.01f)
    {
        // filter out tiny adds
        return;
    }

    PhotonMap::Photon* photon = photonmap->GetPhoton( uv );

    if( !photon ) {
        return;
    }

    photon->color_ += color;
    photon->photons_++;

    photonCount_++;
}

}
