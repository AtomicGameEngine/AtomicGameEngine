// Copyright (c) 2014-2017, THUNDERBEAST GAMES LLC All rights reserved
// Copyright 2009-2017 Intel Corporation
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

#include <Atomic/IO/Log.h>
#include <Atomic/Graphics/Zone.h>

#include <AtomicGlow/Common/GlowSettings.h>

#include "EmbreeScene.h"

#include "LightRay.h"
#include "BakeLight.h"
#include "BakeMesh.h"
#include "SceneBaker.h"

namespace AtomicGlow
{

const float LIGHT_ANGLE_EPSILON = 0.001f;

// http://www.altdevblogaday.com/2012/05/03/generating-uniformly-distributed-points-on-sphere/
static inline void GetRandomDirection(Vector3& result)
{
    float z = 2.0f * rand() / RAND_MAX - 1.0f;
    float t = 2.0f * rand() / RAND_MAX * 3.14f;
    float r = sqrt(1.0f - z * z);
    result.x_ = r * (float) cos(t);
    result.y_ = r * (float) sin(t);
    result.z_ = z;
}

BakeLight::BakeLight(Context* context, SceneBaker* sceneBaker) : BakeNode(context, sceneBaker),
    range_(0.0f)
{
}

BakeLight::~BakeLight()
{

}

// Zone Lights

ZoneBakeLight::ZoneBakeLight(Context* context, SceneBaker* sceneBaker) : BakeLight(context, sceneBaker)
{
}

ZoneBakeLight::~ZoneBakeLight()
{

}

void ZoneBakeLight::Light(LightRay* lightRay)
{
    LightRay::SamplePoint& source = lightRay->samplePoint_;

    if (source.normal == Vector3::ZERO)
        return;

    RTCScene scene = sceneBaker_->GetEmbreeScene()->GetRTCScene();

    const Color& color = zone_->GetAmbientColor();

    Vector3 rad(color.r_, color.g_, color.b_);

    if (!GlobalGlowSettings.aoEnabled_)
    {
        source.bakeMesh->ContributeRadiance(lightRay, rad, GLOW_LIGHTMODE_AMBIENT);
        return;
    }


    // TODO: AO using ray packets/streams

    RTCRay& ray = lightRay->rtcRay_;

    unsigned nsamples = GlobalGlowSettings.nsamples_;

    // this needs to be based on model/scale likely?
    float aoDepth = GlobalGlowSettings.aoDepth_;

    // smallest percent of ao value to use
    float aoMin = GlobalGlowSettings.aoMin_;

    // brightness control
    float multiply = GlobalGlowSettings.aoMultiply_;

    // Shoot rays through the differential hemisphere.
    int nhits = 0;
    float avgDepth = 0.0f;
    for (unsigned nsamp = 0; nsamp < nsamples; nsamp++)
    {
        Vector3 rayDir;
        GetRandomDirection(rayDir);

        float dotp = source.normal.x_ * rayDir.x_ +
                     source.normal.y_ * rayDir.y_ +
                     source.normal.z_ * rayDir.z_;

        if (dotp < 0.1f)
        {
            continue;
        }

        float variance = 0.0f;//nsamples <= 32 ? 0.0f : aoDepth * ((float) rand() / (float) RAND_MAX) * 0.25f;

        float depth = aoDepth + variance;

        lightRay->SetupRay(source.position, rayDir, .001f, depth);

        rtcOccluded(scene, ray);

        if (ray.geomID != RTC_INVALID_GEOMETRY_ID)
        {
            avgDepth += Min<float>(ray.tfar, aoDepth);
            nhits++;
        }
    }

    if (nhits)// && (nsamples <= 32 ? true : nhits > 4))
    {
        avgDepth /= float(nhits);
        avgDepth /= aoDepth;

        avgDepth = Clamp<float>(avgDepth, 0.1f, 1.0f) * 100.0f;
        avgDepth *= avgDepth;
        float ao = avgDepth / 10000.0f;

        ao = aoMin + ao/2.0f;
        ao *= multiply;
        ao = Clamp<float>(ao, aoMin, 1.0f);

        rad *= ao;
    }

    source.bakeMesh->ContributeRadiance(lightRay, rad, GLOW_LIGHTMODE_AMBIENT);
}

void ZoneBakeLight::SetZone(Zone* zone)
{
    node_ = zone->GetNode();
    zone_ = zone;
}


// Directional Lights

DirectionalBakeLight::DirectionalBakeLight(Context* context, SceneBaker* sceneBaker) : BakeLight(context, sceneBaker)
{
}

DirectionalBakeLight::~DirectionalBakeLight()
{

}

void DirectionalBakeLight::Light(LightRay* lightRay)
{
    RTCScene scene = sceneBaker_->GetEmbreeScene()->GetRTCScene();

    LightRay::SamplePoint& source = lightRay->samplePoint_;
    RTCRay& ray = lightRay->rtcRay_;

    float angle = direction_.DotProduct(source.normal);

    if (angle < 0.0f)
        return;

    lightRay->SetupRay(source.position, direction_);

    rtcOccluded(scene, ray);

    // obstructed?  TODO: glass, etc
    if (ray.geomID != RTC_INVALID_GEOMETRY_ID)
        return;

    Vector3 rad(color_.r_, color_.g_, color_.b_);

    rad*=angle;

    source.bakeMesh->ContributeRadiance(lightRay, rad);

}

void DirectionalBakeLight::SetLight(Atomic::Light* light)
{
    node_ = light->GetNode();

    color_ = light->GetColor();

    direction_ = -node_->GetWorldDirection();
    direction_.Normalize();

}

// Point Lights

PointBakeLight::PointBakeLight(Context* context, SceneBaker* sceneBaker) : BakeLight(context, sceneBaker)
{
}

PointBakeLight::~PointBakeLight()
{

}

void PointBakeLight::Light(LightRay* lightRay)
{
    RTCScene scene = sceneBaker_->GetEmbreeScene()->GetRTCScene();

    LightRay::SamplePoint& source = lightRay->samplePoint_;
    RTCRay& ray = lightRay->rtcRay_;

    Vector3 dir = position_ - source.position;

    float dist = dir.Length();

    if (range_ <= 0.0f || dist >= range_)
        return;

    dir.Normalize();

    float dot = dir.DotProduct(source.normal);

    if (dot < 0.0f)
        return;

    lightRay->SetupRay(source.position, dir, .001f, dist);

    rtcOccluded(scene, ray);

    // obstructed?  TODO: glass, etc
    if (ray.geomID != RTC_INVALID_GEOMETRY_ID)
        return;

    Vector3 rad(color_.r_, color_.g_, color_.b_);

    // lightOverBright 1.2 for example will pop light,
    // needs to be configurable per light, maybe with brightness modifer
    float lightOverBright = 1.0f;

    rad *= Max<float> (1.0f - ( dist * lightOverBright / range_), 0.0f);
    rad *= dot;

    // EXPERIMENTAL: if GI is enabled, dim point light a bit

    if (GlobalGlowSettings.giEnabled_)
    {
        rad *= 0.75f;
    }



    if (rad.Length() > M_EPSILON)
        source.bakeMesh->ContributeRadiance(lightRay, rad);

}

void PointBakeLight::SetLight(Atomic::Light* light)
{
    node_ = light->GetNode();

    color_ = light->GetColor();
    position_ = node_->GetWorldPosition();

    range_ = light->GetRange();

}

// Bounce Lights

Mutex BounceBakeLight::sortMutex_;

BounceBakeLight::BounceBakeLight(Context* context, SceneBaker* sceneBaker) : BakeLight(context, sceneBaker)
{
}

BounceBakeLight::~BounceBakeLight()
{

}

static Vector3 compareBouncePoint;
static inline bool CompareBounceSamples(const BounceSample* lhs, const BounceSample* rhs)
{
    Vector3 v1 =  lhs->position_ - compareBouncePoint;
    Vector3 v2 =  rhs->position_ - compareBouncePoint;
    return v1.LengthSquared() < v2.LengthSquared();
}

void BounceBakeLight::Light(LightRay* lightRay)
{
    RTCScene scene = sceneBaker_->GetEmbreeScene()->GetRTCScene();
    LightRay::SamplePoint& source = lightRay->samplePoint_;
    RTCRay& ray = lightRay->rtcRay_;

    const float maxDist = 5.0f;
    const float maxDistSq = maxDist * maxDist;

    const BounceSample* b;
    PODVector<const BounceSample*> samples;

    for (int i = 0; i < bounceSamples_.Size(); i++)
    {
        b = &bounceSamples_[i];

        // don't light self
        if (source.bakeMesh == bakeMesh_)
        {
            for (int j = 0; j < GLOW_MAX_BOUNCE_SAMPLE_TRIANGLES; j++)
            {
                if (b->triIndex_[j] == -1)
                    break;

                if (b->triIndex_[j] == source.triangle)
                    return;

            }
        }

        Vector3 dir =  b->position_ - source.position;

        if (dir.LengthSquared() > maxDistSq)
        {
            continue;
        }

        dir.Normalize();

        if (dir.DotProduct(source.normal) < M_EPSILON)
        {
            continue;
        }

        samples.Push(b);

    }

    if (!samples.Size())
        return;

    sortMutex_.Acquire();

    compareBouncePoint = source.position;

    Sort(samples.Begin(), samples.End(), CompareBounceSamples);

    sortMutex_.Release();


    int bestIndex = -1;
    float bestDist = M_INFINITY;
    for (unsigned i = 0; i < samples.Size(); i++)
    {
        b = samples[i];

        Vector3 dir = b->position_ - source.position;
        float dist = dir.Length();

        if (dist < bestDist)
        {
            dir.Normalize();

            lightRay->SetupRay(source.position, dir, 0.01f, dist * 1.01f);

            rtcIntersect(scene, ray);

            if (ray.geomID != bakeMesh_->GetGeomID())
            {
                continue;
            }

            // backface
            if ( (ray.Ng[0] * ray.dir[0]) +
                 (ray.Ng[1] * ray.dir[1]) +
                 (ray.Ng[2] * ray.dir[2]) < 0.0f)
            {
                continue;
            }

            bool found = false;
            for (int j = 0; j < GLOW_MAX_BOUNCE_SAMPLE_TRIANGLES; j++)
            {
                if (b->triIndex_[j] == -1)
                    break;

                if (b->triIndex_[j] == ray.primID)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
                continue;

            bestIndex = i;
            bestDist = dist;

            break;
        }

    }

    if (bestIndex == -1)
        return;

    b = samples[bestIndex];

    // weighted average of src color and radiance for bounce sample
    Vector3 rad = b->srcColor_ + (b->radiance_/b->hits_);
    rad /= 2.0f;

    float d = 1.0f - Clamp<float>(bestDist / maxDist, 0.01f, 1.0f);

    rad *= d;

    rad *= 0.15f;

    if (rad.x_ < 0.0f || rad.y_ < 0.0f || rad.z_ < 0.0f)
    {
        ATOMIC_LOGWARNING("BounceBakeLight::Light - negative rad factor");
        return;
    }

    source.bakeMesh->ContributeRadiance(lightRay, rad, GLOW_LIGHTMODE_INDIRECT);

}

void BounceBakeLight::SetBounceSamples(const PODVector<BounceSample>& bounceSamples)
{
    bounceSamples_ = bounceSamples;
}

void BounceBakeLight::SetBakeMesh(BakeMesh* bakeMesh)
{
    bakeMesh_ = bakeMesh;
    node_ = bakeMesh->GetNode();

    color_ = Color::MAGENTA;
    range_ = -1.0f;
    position_ = Vector3::ZERO;
}

void BounceBakeLight::SetLight(Atomic::Light* light)
{
    node_ = light->GetNode();

    color_ = light->GetColor();
    position_ = node_->GetWorldPosition();

    range_ = light->GetRange();

}



}
