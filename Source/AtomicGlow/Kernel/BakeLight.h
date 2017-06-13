// Copyright (c) 2014-2017, THUNDERBEAST GAMES LLC All rights reserved
// Portions Copyright (c) 2015 Dmitry Sovetov
// Portions Copyright 2009-2017 Intel Corporation
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

#include <Atomic/Core/Mutex.h>
#include <Atomic/Math/Plane.h>
#include "BakeNode.h"

namespace Atomic
{
class Light;
class Zone;
}

using namespace Atomic;

namespace AtomicGlow
{

class LightRay;
class BakeMesh;
class BakeLight;

/// Light cuttoff is used for configuring the light source influence direction (omni, directional, spotlight).
/// LightCutoff class is a base for all cutoff models and represents an omni-directional light.
class LightCutoff : public RefCounted
{
    ATOMIC_REFCOUNTED(LightCutoff)

public:

    /// Constructs a LightCutoff instance.
    LightCutoff( const BakeLight* light );
    virtual             ~LightCutoff( void ) {}

    /// Calculates a light influence to a given point.
    virtual float       Calculate( const Vector3& point ) const;

    /// Calculates a light cutoff for direction.
    virtual float       GetCutoffForDirection( const Vector3& direction ) const;

protected:

    /// Parent light instance.
    const BakeLight*        light_;
};

/// LightSpotCutoff is used for spot lights.
class LightSpotCutoff : public LightCutoff
{

    ATOMIC_REFCOUNTED(LightSpotCutoff)

public:

    // Constructs a LightSpotCutoff instance
    // param light Parent light source.
    // param direction Spot light direction.
    // param cutoff The cutoff value represents the maximum angle between the light direction and the light to pixel vector.
    // param exponent The cutoff exponent.
    LightSpotCutoff( const BakeLight* light, const Vector3& direction, float cutoff, float exponent );
    virtual ~LightSpotCutoff( void ) {}

    /// Calculates a light influence for a spot light.
    virtual float Calculate( const Vector3& point ) const;

    /// Calculates a light cutoff for direction.
    virtual float GetCutoffForDirection( const Vector3& direction ) const;

private:

    /// Light direction.
    Vector3 direction_;
    /// Light cutoff.
    float cutoff_;
    /// Light spot exponent.
    float exponent_;
};

/// Light distance attenuation model.
class LightAttenuation : public RefCounted
{
    ATOMIC_REFCOUNTED(LightAttenuation)

public:

    /// Constructs a LightAttenuation instance.
    LightAttenuation( const BakeLight* light );
    virtual ~LightAttenuation( void ) {}

    /// Calculates light attenuation by distance.
    /// An attenuation factor is a value in range [0;1], where 0 means
    /// that light is attenuated to zero, and 1 means no attenuation at all.
    virtual float Calculate( float distance ) const { return 0.0f; }

protected:

    /// Parent light source.
    const BakeLight* light_;
};

/// Linear light attenuation model.
class LinearLightAttenuation : public LightAttenuation
{

    ATOMIC_REFCOUNTED(LinearLightAttenuation)

public:
    /// Constructs a LinearLightAttenuation instance.
    LinearLightAttenuation( const BakeLight* light, float radius, float constant = 0.0f, float linear = 0.0f, float quadratic = 25.0f );

    virtual float Calculate( float distance ) const;

private:

    // Light influence radius.
    float radius_;
    // Constant light attenuation factor.
    float constant_;
    // Linear light attenuation factor.
    float linear_;
    // Quadratic light attenuation factor.
    float quadratic_;
};

/// Light to point influence model.
class LightInfluence : public RefCounted
{

    ATOMIC_REFCOUNTED(LightInfluence)

public:

    /// Constructs a LightInfluence instance.
    LightInfluence( const BakeLight* light );

    virtual ~LightInfluence( void ) {}

    /// Calculates omni light influence to a given point.
    virtual float Calculate(LightRay* lightRay, const Vector3 &light, float& distance ) const;

    /// Calculates a light influence by a Lambert's cosine law.
    static float GetLambert( const Vector3& direction, const Vector3& normal );

protected:

    /// Parent light instance.
    const BakeLight* light_;
};

///  Directional light influence model.
class DirectionalLightInfluence : public LightInfluence
{
    ATOMIC_REFCOUNTED(DirectionalLightInfluence)

public:

    /// Constructs a DirectionalLightInfluence instance.
    DirectionalLightInfluence( const BakeLight* light, const Vector3& direction );

    /// Calculates a directional light influence.
    virtual float Calculate( LightRay* lightRay, const Vector3& light, float& distance ) const;

private:

    /// Light source direction.
    Vector3 direction_;
};

///  Ambient occlusion influence model.
class AmbientOcclusionInfluence : public LightInfluence
{
    ATOMIC_REFCOUNTED(AmbientOcclusionInfluence)

public:

    /// Constructs a AmbientOcclusionInfluence instance.
    AmbientOcclusionInfluence( const BakeLight* light);

    /// Calculates a ambient occlusion light influence.
    virtual float Calculate( LightRay* lightRay, const Vector3& light, float& distance ) const;

private:

};



/// Photon emitter is used to determine an amount of photons to be emitted by a given light.
class PhotonEmitter : public RefCounted
{
    ATOMIC_REFCOUNTED(PhotonEmitter)

public:

    // Constructs a new PhotonEmitter instance.
    PhotonEmitter( const BakeLight* light );
    virtual ~PhotonEmitter( void ) {}

    // Calculates an amount of photons to be emitted.
    virtual int GetPhotonCount( void ) const;

    // Emits a new photon.
    virtual void Emit( SceneBaker* scene, Vector3& position, Vector3& direction ) const;

protected:

    // Parent light source.
    const BakeLight* light_;
};

/// Directional photon emitter emits photons with a given direction.
class DirectionalPhotonEmitter : public PhotonEmitter
{

    ATOMIC_REFCOUNTED(DirectionalPhotonEmitter)

public:

    // Constructs a new DirectionalPhotonEmitter instance.
    // param light Parent light source.
    // param direction Emission direction.
    DirectionalPhotonEmitter( const BakeLight* light, const Vector3& direction );

    /// Emits a new photon.
    // virtual void Emit( const Scene* scene, Vector3& position, Vector3& direction ) const;

private:

    /// Emission direction.
    Vector3 direction_;

    /// Emission plane.
    Plane plane_;
};


class BakeLight : public BakeNode
{
    ATOMIC_OBJECT(BakeLight, BakeNode)

    public:

    BakeLight(Context* context, SceneBaker *sceneBaker);
    virtual ~BakeLight();

    /*
    virtual void SetLight(Atomic::Light* light) = 0;
    virtual void Light(LightRay* ray) = 0;
    */

    // Returns an light influence model.
    LightInfluence* GetInfluenceModel( void ) const;

    // Sets an light influence model.
    void SetInfluenceModel( LightInfluence* value );

    // Returns an attenuation model.
    LightAttenuation* GetAttenuationModel( void ) const;

    // Sets an attenuation model.
    void SetAttenuationModel( LightAttenuation* value );

    // Returns a light vertex generator.
    // LightVertexGenerator* vertexGenerator( void ) const;

    // Sets an light vertex generator.
    // void SetVertexGenerator( LightVertexGenerator* value );

    // Returns an cutoff model.
    LightCutoff* GetCutoffModel( void ) const;

    // Sets an cutoff model.
    void SetCutoffModel( LightCutoff* value );

    // Returns a photon emitter.
    PhotonEmitter* GetPhotonEmitter( void ) const;

    // Sets a photon emitter.
    // By default each light has a photon emitter, light sources with no photon emitter
    // set won't make any influence to a global illumination.
    void SetPhotonEmitter( PhotonEmitter* value );

    // Returns a light position.
    const Vector3& GetPosition( void ) const;

    // Sets a light position.
    void SetPosition( const Vector3& value );

    // Returns a light color.
    const Color& GetColor( void ) const;

    // Sets a light color.
    void SetColor( const Color& value );

    // Returns a light intensity.
    float GetIntensity( void ) const;

    // Sets a light intensity.
    void SetIntensity( float value );

    // Returns true if this light casts shadow otherwise false.
    bool GetCastsShadow( void ) const;

    // Sets a castShadow flag.
    void SetCastsShadow( bool value );

    // Direct Lighting

    void DirectLight(LightRay* lightRay);

    // Creates a Zone light instance.
    static BakeLight* CreateZoneLight( SceneBaker* baker, const Color& color = Color::WHITE );

    // Creates a point light instance.
    static BakeLight* CreatePointLight( SceneBaker* baker, const Vector3& position, float radius, const Color& color = Color::WHITE, float intensity = 1.0f, bool castsShadow = true );

    // Creates a spot light instance.
    static BakeLight* CreateSpotLight( SceneBaker* baker, const Vector3& position, const Vector3& direction, float cutoff, float radius, const Color& color = Color::WHITE, float intensity = 1.0f, bool castsShadow = true );

    // Creates a directional light instance.
    static BakeLight* CreateDirectionalLight( SceneBaker* baker, const Vector3& direction, const Color& color = Color::WHITE, float intensity = 1.0f, bool castsShadow = true );

    // Creates an area light instance.
    // static BakeLight* CreateAreaLight( SceneBaker* baker, const Mesh* mesh, const Vector3& position, const Color& color = Color::WHITE, float intensity = 1.0f, bool castsShadow = true );

private:

    // Light position.
    Vector3 position_;

    // Light color.
    Color color_;

    // Light intensity.
    float intensity_;

    // Casts shadow flag.
    bool castsShadow_;

    // Light cutoff model.
    SharedPtr<LightCutoff> cutoffModel_;

    // Light attenuation model.
    SharedPtr<LightAttenuation>  attenuationModel_;

    // Light influence model.
    SharedPtr<LightInfluence> influenceModel_;

    // Light source photon emitter.
    SharedPtr<PhotonEmitter> photonEmitter_;

    // Light vertex sampler.
    // SharedPtr<LightVertexGenerator> vertexGenerator_;

};

/*

// Zone ambient, etc
class ZoneBakeLight : public BakeLight
{
    ATOMIC_OBJECT(ZoneBakeLight, BakeLight)

    public:

    ZoneBakeLight(Context* context, SceneBaker* sceneBaker);
    virtual ~ZoneBakeLight();

    void Light(LightRay* lightRay);
    void SetLight(Atomic::BakeLight* light) {}

    void SetZone(Zone* zone);

protected:

private:

    Zone* zone_;

};


class DirectionalBakeLight : public BakeLight
{
    ATOMIC_OBJECT(DirectionalBakeLight, BakeLight)

    public:

        DirectionalBakeLight(Context* context, SceneBaker* sceneBaker);
    virtual ~DirectionalBakeLight();

    void Set(const Vector3& direction, const Vector3& radiance, float cosAngle);

    void Light(LightRay* lightRay);

    void SetLight(Atomic::BakeLight* light);


protected:


private:

};

class PointBakeLight : public BakeLight
{
    ATOMIC_OBJECT(PointBakeLight, BakeLight)

    public:

        PointBakeLight(Context* context, SceneBaker* sceneBaker);
    virtual ~PointBakeLight();

    void Light(LightRay* lightRay);

    void SetLight(Atomic::BakeLight* light);

protected:


private:

};

*/


}
