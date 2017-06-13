// Copyright (c) 2014-2017, THUNDERBEAST GAMES LLC All rights reserved
// Portions Copyright (c) 2015 Dmitry Sovetov
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

BakeLight::BakeLight(Context* context, SceneBaker* sceneBaker) : BakeNode(context, sceneBaker),
    color_(Color::WHITE),
    intensity_( 0.0f ),
    castsShadow_( false )
{
}

BakeLight::~BakeLight()
{
}

LightCutoff* BakeLight::GetCutoffModel( void ) const
{
    return cutoffModel_;
}

void BakeLight::SetCutoffModel( LightCutoff* value )
{
    cutoffModel_ = value;
}

LightInfluence* BakeLight::GetInfluenceModel( void ) const
{
    return influenceModel_;
}

void BakeLight::SetInfluenceModel( LightInfluence* value )
{
    influenceModel_ = value;
}

/*
LightVertexGenerator* BakeLight::vertexGenerator( void ) const
{
    return vertexGenerator_;
}

// ** BakeLight::setVertexGenerator
void BakeLight::setVertexGenerator( LightVertexGenerator* value )
{
    delete m_vertexGenerator;
    m_vertexGenerator = value;
}
*/

LightAttenuation* BakeLight::GetAttenuationModel( void ) const
{
    return attenuationModel_;
}

void BakeLight::SetAttenuationModel( LightAttenuation* value )
{
    attenuationModel_ = value;
}

PhotonEmitter* BakeLight::GetPhotonEmitter( void ) const
{
    return photonEmitter_;
}

void BakeLight::SetPhotonEmitter( PhotonEmitter* value )
{
    photonEmitter_ = value;
}

const Vector3& BakeLight::GetPosition( void ) const
{
    return position_;
}

void BakeLight::SetPosition( const Vector3& value )
{
    position_ = value;
}

const Color& BakeLight::GetColor( void ) const
{
    return color_;
}

void BakeLight::SetColor( const Color& value )
{
    color_ = value;
}

float BakeLight::GetIntensity( void ) const
{
    return intensity_;
}

void BakeLight::SetIntensity( float value )
{
    intensity_ = value;
}

bool BakeLight::GetCastsShadow( void ) const
{
    return castsShadow_;
}

void BakeLight::SetCastsShadow( bool value )
{
    castsShadow_ = value;
}

BakeLight* BakeLight::CreateZoneLight( SceneBaker* baker, const Color& color)
{
    BakeLight* light = new BakeLight(baker->GetContext(), baker);
    light->SetCastsShadow( false );
    light->SetColor( color );
    light->SetIntensity(1.0f);

    if (GlobalGlowSettings.aoEnabled_)
    {
        light->SetInfluenceModel( new AmbientOcclusionInfluence( light ) );
    }

    return light;
}

BakeLight* BakeLight::CreatePointLight( SceneBaker* baker, const Vector3& position, float radius, const Color& color, float intensity, bool castsShadow )
{
    BakeLight* light = new BakeLight(baker->GetContext(), baker);

    light->SetInfluenceModel( new LightInfluence( light ) );
    light->SetAttenuationModel( new LinearLightAttenuation( light, radius ) );
    light->SetCutoffModel( new LightCutoff( light ) );
    light->SetPhotonEmitter( new PhotonEmitter( light ) );

    light->SetCastsShadow( castsShadow );
    light->SetPosition( position );
    light->SetColor( color );
    light->SetIntensity( intensity );

    return light;
}

BakeLight* BakeLight::CreateSpotLight( SceneBaker* baker, const Vector3& position, const Vector3& direction, float cutoff, float radius, const Color& color, float intensity, bool castsShadow )
{
    BakeLight* light = new BakeLight(baker->GetContext(), baker);

    light->SetInfluenceModel( new LightInfluence( light ) );
    light->SetAttenuationModel( new LinearLightAttenuation( light, radius ) );
    light->SetCutoffModel( new LightSpotCutoff( light, direction, cutoff, 1.0f ) );
    light->SetPhotonEmitter( new PhotonEmitter( light ) );

    light->SetCastsShadow( castsShadow );
    light->SetPosition( position );
    light->SetColor( color );
    light->SetIntensity( intensity );

    return light;
}

BakeLight* BakeLight::CreateDirectionalLight( SceneBaker* baker, const Vector3& direction, const Color& color, float intensity, bool castsShadow )
{
    BakeLight* light = new BakeLight(baker->GetContext(), baker);

    light->SetInfluenceModel( new DirectionalLightInfluence( light, direction ) );
    light->SetPhotonEmitter( new DirectionalPhotonEmitter( light, direction ) );
    light->SetCastsShadow( castsShadow );
    light->SetColor( color );
    light->SetIntensity( intensity );

    return light;
}

/*
BakeLight* BakeLight::CreateAreaLight( SceneBaker* baker, const Mesh* mesh, const Vector3& position, const Rgb& color, float intensity, bool castsShadow )
{
    Light* light = new Light;

    light->SetInfluence( new LightInfluence( light ) );
    light->SetAttenuation( new LinearLightAttenuation( light, mesh->bounds().volume() ) );
    light->SetPhotonEmitter( new PhotonEmitter( light ) );
    light->SetCutoff( new LightCutoff( light ) );
    //light->SetVertexGenerator( new FaceLightVertexGenerator( mesh, true, 3 ) );
    //light->SetVertexGenerator( new FaceLightVertexGenerator( mesh, false, 0 ) );
    light->SetVertexGenerator( new FaceLightVertexGenerator( mesh, true, 0 ) );
    //  light->SetVertexGenerator( new LightVertexGenerator( mesh ) );
    light->SetCastsShadow( castsShadow );
    light->SetPosition( position );
    light->SetColor( color );
    light->SetIntensity( intensity );

    light->GetVertexGenerator()->Generate();

    return light;
}
*/

// ------------------------------------------------------- LightInfluence --------------------------------------------------------- //

LightInfluence::LightInfluence( const BakeLight* light ) :
    light_( light )
{

}

float LightInfluence::Calculate(LightRay* lightRay, const Vector3& light, float& distance ) const
{
    LightRay::SamplePoint& source = lightRay->samplePoint_;

    Vector3 direction = light - source.position;
    distance = direction.Length();
    direction.Normalize();

    // ** Calculate Lambert's cosine law intensity
    float intensity = GetLambert( direction, source.normal );

    if( intensity <= 0.001f ) {
        return 0.0f;
    }

    // ** Cast shadow to point
    if( light_->GetCastsShadow() )
    {
        LightRay::SamplePoint& source = lightRay->samplePoint_;

        // clean this mess up
        RTCScene scene = source.bakeMesh->GetSceneBaker()->GetEmbreeScene()->GetRTCScene();

        lightRay->SetupRay(source.position, direction, .001f, distance);

        rtcOccluded(scene, lightRay->rtcRay_);

        // obstructed?  TODO: glass, etc
        if (lightRay->rtcRay_.geomID != RTC_INVALID_GEOMETRY_ID)
        {
            return 0.0f;
        }
    }

    return intensity;
}

float LightInfluence::GetLambert( const Vector3& direction, const Vector3& normal )
{
    float dp = direction.DotProduct(normal);
    return dp < 0.0f ? 0.0f : dp;
}

// --------------------------------------------------- DirectionalLightInfluence -------------------------------------------------- //

DirectionalLightInfluence::DirectionalLightInfluence( const BakeLight* light, const Vector3& direction )
    : LightInfluence( light ), direction_( direction )
{

}

float DirectionalLightInfluence::Calculate(LightRay* lightRay, const Vector3& light, float& distance ) const
{
    LightRay::SamplePoint& source = lightRay->samplePoint_;

    float intensity = GetLambert( -direction_, source.normal );

    if( intensity <= 0.001f )
    {
        return 0.0f;
    }

    // ** Cast shadow to point
    if( light_->GetCastsShadow() )
    {
        // FIXME: tracer
        // intensity *= tracer->traceSegment( point, point - m_direction * 1000, rt::HitUseAlpha ) ? 0.0f : 1.0f;
    }

    return intensity;
}

// --------------------------------------------------- AmbientOcclusionInfluence -------------------------------------------------- //

AmbientOcclusionInfluence::AmbientOcclusionInfluence( const BakeLight* light )
    : LightInfluence( light )
{

}

float AmbientOcclusionInfluence::Calculate(LightRay* lightRay, const Vector3& light, float& distance ) const
{

    LightRay::SamplePoint& source = lightRay->samplePoint_;

    if (source.normal == Vector3::ZERO)
        return 1.0f;

    // clean this mess up
    RTCScene scene = source.bakeMesh->GetSceneBaker()->GetEmbreeScene()->GetRTCScene();

    const Color& color = light_->GetColor();

    Vector3 rad(color.r_, color.g_, color.b_);

    if (!GlobalGlowSettings.aoEnabled_)
    {
        return 1.0f;
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
        Vector3::GetRandomHemisphereDirection(rayDir, source.normal);

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

        return ao;
    }

    return 1.0f;
}

// --------------------------------------------------------- LightCutoff ---------------------------------------------------------- //

LightCutoff::LightCutoff( const BakeLight* light ) : light_( light )
{

}

float LightCutoff::Calculate( const Vector3& point ) const
{
    return 1.0f;
}

float LightCutoff::GetCutoffForDirection( const Vector3& direction ) const
{
    return 1.0f;
}

// ------------------------------------------------------- LightSpotCutoff -------------------------------------------------------- //

LightSpotCutoff::LightSpotCutoff( const BakeLight* light, const Vector3& direction, float cutoff, float exponent ) :
    LightCutoff( light ), direction_( direction ), cutoff_( cutoff ), exponent_( exponent )
{

}

float LightSpotCutoff::Calculate( const Vector3& point ) const
{
    Vector3 dir = point - light_->GetPosition();
    dir.Normalize();
    return GetCutoffForDirection( dir );
}

float LightSpotCutoff::GetCutoffForDirection( const Vector3& direction ) const
{
    float value = direction.DotProduct(direction_);

    if( value <= cutoff_ ) {
        return 0.0f;
    }

    value = (1.0f - (1.0f - value) * 1.0f / (1.0f - cutoff_));

    if( fabs( 1.0f - exponent_ ) > 0.01f ) {
        value = powf( value, exponent_ );
    }

    return value;
}

// ------------------------------------------------------- LightAttenuation ------------------------------------------------------- //

LightAttenuation::LightAttenuation( const BakeLight* light ) : light_( light )
{

}

// ---------------------------------------------------- LinearLightAttenuation ---------------------------------------------------- //


LinearLightAttenuation::LinearLightAttenuation( const BakeLight* light, float radius, float constant, float linear, float quadratic )
    : LightAttenuation( light ), radius_( radius ), constant_( constant ), linear_( linear ), quadratic_( quadratic )
{

}


float LinearLightAttenuation::Calculate( float distance ) const
{
    if (fabs(radius_) < 0.01f)
        return 0.0f;

    float r = distance / radius_;
    return Max<float>( 1.0f / (1.0f + constant_ + linear_ * r + quadratic_ * r * r), 0.0f );
}

// -------------------------------------------------------- PhotonEmitter --------------------------------------------------------- //

PhotonEmitter::PhotonEmitter( const BakeLight* light ) :
    light_( light )
{

}

int PhotonEmitter::GetPhotonCount( void ) const
{
    return static_cast<int>( light_->GetIntensity() * 25000 );
}

void PhotonEmitter::Emit( SceneBaker* sceneBaker, Vector3& position, Vector3& direction ) const
{
    position  = light_->GetPosition();
    Vector3::GetRandomDirection(direction);

}

// --------------------------------------------------- DirectinalPhotonEmitter ---------------------------------------------------- //

DirectionalPhotonEmitter::DirectionalPhotonEmitter( const BakeLight* light, const Vector3& direction ) :
    PhotonEmitter( light ), direction_( direction )
{
    plane_.Define(direction.Normalized(), light->GetPosition()); //  = Plane::calculate( direction, light->position() );
}

/*
void DirectionalPhotonEmitter::Emit( const Scene* scene, Vector3& position, Vector3& direction ) const
{
    const Bounds& bounds = scene->bounds();

    position  = m_plane * bounds.randomPointInside() - m_direction * 5;
    direction = m_direction;
}
*/


}
