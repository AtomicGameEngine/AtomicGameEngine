//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
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

#include "Precompiled.h"
#include "../Core/Context.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/GraphicsEvents.h"
#include "../Graphics/View.h"
#include "../Scene/Scene.h"
#include "../Graphics/Camera.h"
#include "../Scene/SceneEvents.h"
#include "../Graphics/Light.h"
#include "../Resource/ResourceCache.h"
#include "../Graphics/Technique.h"
#include "../Environment/ProcSky.h"
#include "../Graphics/Renderer.h"

#if defined(_MSC_VER)
#include "stdint.h"
#endif

#if defined(EMSCRIPTEN) || defined(ATOMIC_PLATFORM_LINUX)
#include <stdint.h>
#endif

namespace Atomic
{

extern const char* GEOMETRY_CATEGORY;

float ProcSky::timeOfDay_ = 0.0f;

ProcSky::ProcSky(Context* context) :
    Drawable(context, DRAWABLE_GEOMETRY),
    geometry_(new Geometry(context)),
    vertexBuffer_(new VertexBuffer(context_)),
    indexBuffer_(new IndexBuffer(context_)),
    autoUpdate_(false),
    dayTime_(0.0f),
    lastDayTimeUpdate_(-1.0f),
    shadowFade_(1.0f),
    customWorldTransform_(Matrix3x4::IDENTITY),
    initialized_(false),
    flipped_(false)
{

}

ProcSky::~ProcSky()
{

}

void ProcSky::RegisterObject(Context* context)
{
    context->RegisterFactory<ProcSky>(GEOMETRY_CATEGORY);

    ATOMIC_COPY_BASE_ATTRIBUTES(Drawable);
}


void ProcSky::UpdateBatches(const FrameInfo& frame)
{
    if (!initialized_)
        Initialize();

    distance_ = 0.0f;

    // Follow only the camera rotation, not position
    Matrix3x4 customView(Vector3::ZERO, frame.camera_->GetNode()->GetWorldRotation().Inverse(), Vector3::ONE);
    customWorldTransform_ = customView * node_->GetWorldTransform();

    for (unsigned i = 0; i < batches_.Size(); ++i)
    {
        batches_[i].worldTransform_ = &customWorldTransform_;
        batches_[i].numWorldTransforms_ = 1;
        batches_[i].distance_ = 0.0f;
    }

}

void ProcSky::UpdateGeometry(const FrameInfo& frame)
{
    if (!initialized_)
        Initialize();

    if ( indexBuffer_->IsDataLost())
        UpdateIndexBuffer();

    UpdateVertexBuffer(frame);

}

UpdateGeometryType ProcSky::GetUpdateGeometryType()
{
    // updates vertex buffer per frame
    return UPDATE_MAIN_THREAD;
}

void ProcSky::OnWorldBoundingBoxUpdate()
{
    // The skybox is supposed to be visible everywhere, so set a humongous bounding box
    worldBoundingBox_.Define(-M_LARGE_VALUE, M_LARGE_VALUE);
}

void ProcSky::OnNodeSet(Node* node)
{
    Drawable::OnNodeSet(node);

    if (node && node->GetScene())
    {
        SubscribeToEvent(node->GetScene(), E_SCENEUPDATE, ATOMIC_HANDLER(ProcSky, HandleSceneUpdate));
        SubscribeToEvent(E_BEGINVIEWUPDATE, ATOMIC_HANDLER(ProcSky, HandleBeginViewUpdate));
    }

}

void ProcSky::HandleBeginViewUpdate(StringHash eventType, VariantMap& eventData)
{
#ifdef ATOMIC_OPENGL
    flipped_ = false;
    View* view = static_cast<View*>(eventData[BeginViewUpdate::P_VIEW].GetPtr());
    if (view && view->GetRenderTarget())
        flipped_ = true;

#endif

}

void ProcSky::HandleSceneUpdate(StringHash eventType, VariantMap& eventData)
{

    using namespace SceneUpdate;

    if (!initialized_)
        Initialize();

    float timeStep = eventData[P_TIMESTEP].GetFloat();

    if (autoUpdate_)
    {
        dayTime_ += timeStep * .1f;
        SetDayTime(dayTime_);
    }

}

float ProcSky::SetDayTime(float time)
{
    if (!initialized_)
        Initialize();

    sunAngle_ = (time+0.5f) * M_PI;

    int degrees = int(sunAngle_ * M_RADTODEG);

    degrees %= 360;

    timeOfDay_ = ((degrees + 90) % 360) / 15.0f;

    if (degrees >= 150 || degrees <= 15)
    {
        // fade out
        if (shadowFade_ < 0.0f || shadowFade_ != 1.0f)
        {
            shadowFade_ += .05f;

            if (shadowFade_ > 1.0f)
                shadowFade_ = 1.0f;

            sunlight_->SetShadowIntensity(shadowFade_);

            if (!sunlight_->GetCastShadows())
                sunlight_->SetCastShadows(true);

        }
        else
        {
            if (sunlight_->GetCastShadows())
                sunlight_->SetCastShadows(false);
        }

    }
    else
    {
        // fade in
        if (shadowFade_ < 0.0f || shadowFade_ != 0.0f)
        {
            shadowFade_ -= .05f;

            if (shadowFade_ < 0.0f)
                shadowFade_ = 0.0f;

            sunlight_->SetShadowIntensity(shadowFade_);

            if (!sunlight_->GetCastShadows())
                sunlight_->SetCastShadows(true);

        }
        else
        {
            if (!sunlight_->GetCastShadows())
                sunlight_->SetCastShadows(true);
        }
    }

    Quaternion rot(degrees, Vector3::RIGHT);

    node_->SetRotation(rot);


    float daylight = cosf( time * M_PI ) * 0.8f + 0.4f;

    if( daylight > 1.0f )
        daylight = 1.0f;

    if( daylight < 0.0f )
        daylight = 0.0f;

    daylight = sqrtf( daylight );

    float dawnGlow = sinf( time * M_PI * 1.0f );
    int glowX = dawnGlow > 0.0f ? 0 : 7;
    dawnGlow = Atomic::Max( 0.0f, fabs( dawnGlow * dawnGlow * dawnGlow ) * 1.1f - 0.1f );

    Color top = Color(0,0,0,1).Lerp(topColor_, daylight);
    Color horiz = Color(.1,.1,.1,1).Lerp(horizColor_, daylight);

    uint8_t skytex[32 * 8 * 4 ];
    uint8_t* p;

    // bottom row is black
    p = &skytex[31*8*4];

    for( uint32_t x = 0; x < 8; x++ )
    {
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 255;
    }

    p = skytex;
    for( uint32_t z = 0; z < 31; z++ )
    {
        for( uint32_t x = 0; x < 8; x++ )
        {
            float xGlow = Atomic::Max( 0, -abs((int)x-glowX) + 5 ) / 5.0f;
            float hGlow = z/31.0f;
            float glow = (dawnGlow * hGlow * hGlow * sqrtf( xGlow ));

            Color baseColor = top.Lerp(horiz, Atomic::Min( 1.0f, float (z*11) / 255.0f));
            baseColor = baseColor.Lerp(lerpColor_, glow );

            // pic a color from the mid - horizon (this is cheesy!)
            if (z == 28 && x == 4)
            {
                fogColor_ = baseColor;
                zone_->SetFogColor(fogColor_);
                Color sunColor (.3f, .2f, .1f);
                sunColor += fogColor_;

                sunlight_->SetColor(sunColor);

                Color ambient = sunColor * .75f;

                if (ambient.r_ < .25f)
                    ambient.r_ = .25f;
                if (ambient.g_ < .25f)
                    ambient.g_ = .25f;
                if (ambient.b_ < .25f)
                    ambient.b_ = .25f;

                zone_->SetAmbientColor(ambient);

            }

            unsigned r = Clamp(((int)(baseColor.r_ * 255.0f)), 0, 255);
            unsigned g = Clamp(((int)(baseColor.g_ * 255.0f)), 0, 255);
            unsigned b = Clamp(((int)(baseColor.b_ * 255.0f)), 0, 255);

            *p++ = (uint8_t) r;
            *p++ = (uint8_t) g;
            *p++ = (uint8_t) b;
            *p++ = 255;
        }
    }

    //if (fabs(lastDayTimeUpdate_ - dayTime_) > .005f)
    {
        skyTexture_->SetData(0,0,0,8,32,skytex);
        lastDayTimeUpdate_ = dayTime_;
    }



    return Atomic::Max( 4.0f/15.0f, daylight );
}

void ProcSky::UpdateIndexBuffer()
{
    unsigned short* dest = (unsigned short*)indexBuffer_->Lock(0, 6, true);
    *dest++ = 2; *dest++ = 1; *dest++ = 0;
    *dest++ = 0; *dest++ = 3; *dest++ = 2;

    indexBuffer_->Unlock();
    indexBuffer_->ClearDataLost();
}

void ProcSky::UpdateVertexBuffer(const FrameInfo &frame)
{

    float* vdest = (float*)vertexBuffer_->Lock(0, 4, true);

    const Frustum& frustum = frame.camera_->GetFrustum();

    // Upper left corner
    Vector3 n;

    Vector3 planes[4];
    planes[0] = frustum.planes_[PLANE_LEFT].normal_;
    planes[1] = frustum.planes_[PLANE_RIGHT].normal_;
    planes[2] = frustum.planes_[PLANE_UP].normal_;
    planes[3] = frustum.planes_[PLANE_DOWN].normal_;


    float top = 1.0f;
    float bottom = -1.0f;

#ifdef ATOMIC_OPENGL
if (flipped_)
{
    top = -1.0f;
    bottom = 1.0f;
}
#endif

    // 0 2
    n = planes[0].CrossProduct(planes[2]);
    *vdest++ = -1.0f;
    *vdest++ =  top;
    *vdest++ =  1.0f;

    *vdest++ = n.x_;
    *vdest++ = -n.y_;
    *vdest++ = n.z_ ;

    // Lower left corner
    // 3 0
    n = planes[3].CrossProduct(planes[0]);
    *vdest++ = -1.0f;
    *vdest++ = bottom;
    *vdest++ =  1.0f;

    *vdest++ = n.x_;
    *vdest++ = -n.y_;
    *vdest++ = n.z_;

    // Lower right corner
    // 1 3
    n = planes[1].CrossProduct(planes[3]);
    *vdest++ = 1.0f;
    *vdest++ = bottom;
    *vdest++ =  1.0f;

    *vdest++ = n.x_;
    *vdest++ = -n.y_;
    *vdest++ = n.z_;

    // Upper right corner

    // 2 1
    n = planes[2].CrossProduct(planes[1]);
    *vdest++ = 1.0f;
    *vdest++ = top;
    *vdest++ = 1.0f;

    *vdest++ = n.x_;
    *vdest++ = -n.y_;
    *vdest++ = n.z_;

    vertexBuffer_->Unlock();
    vertexBuffer_->ClearDataLost();
}


void ProcSky::Initialize()
{
    initialized_ = true;

    topColor_.b_ = float(0x76) / 255.0f;
    topColor_.g_ = float(0x4a) / 255.0f;
    topColor_.r_ = float(0x1f) / 255.0f;
    topColor_.a_ = 1.0f;

    horizColor_.b_ = float(0xca) / 255.0f;
    horizColor_.g_ = float(0x9b) / 255.0f;
    horizColor_.r_ = float(0x65) / 255.0f;
    horizColor_.a_ = 1.0f;

    lerpColor_.b_ = float(0x16) / 255.0f;
    lerpColor_.g_ = float(0x40) / 255.0f;
    lerpColor_.r_ = float(0x9b) / 255.0f;
    lerpColor_.a_ = 1.0f;

    sunlight_ = node_->CreateComponent<Light>();
    sunlight_->SetLightType(LIGHT_DIRECTIONAL);
    sunlight_->SetCastShadows(true);


    sunlight_->SetShadowBias(BiasParameters(0.0001f, 0.5f));
    // Set cascade splits at 10, 50 and 200 world units, fade shadows out at 80% of maximum shadow distance
#if ANDROID
    sunlight_->SetShadowCascade(CascadeParameters(50.0f, 0.0f, 0.0f, 0.0f, 0.8f));
#else
    sunlight_->SetShadowCascade(CascadeParameters(30.0f, 60.0f, 120.0f, 200.0f, 0.8f));
#endif

    ResourceCache* cache = GetSubsystem<ResourceCache>();

    skyMaterial_ = cache->GetResource<Material>("Materials/ProcSky.xml");

    geometry_->SetVertexBuffer(0, vertexBuffer_);
    geometry_->SetIndexBuffer(indexBuffer_);

    indexBuffer_->SetSize( 6, false);
    UpdateIndexBuffer();

    // updated per frame
    vertexBuffer_->SetSize(4, MASK_POSITION | MASK_NORMAL , true);

    batches_.Resize(1);
    batches_[0].geometry_ = geometry_;
    batches_[0].geometryType_ = GEOM_STATIC_NOINSTANCING;
    batches_[0].worldTransform_ = &customWorldTransform_;
    batches_[0].material_ = skyMaterial_;
    batches_[0].geometry_->SetDrawRange(TRIANGLE_LIST, 0, 6, false);

    skyTexture_ = new Texture2D(context_);

    skyMaterial_->SetTexture(TU_DIFFUSE , skyTexture_);

    skyTexture_->SetMipsToSkip(QUALITY_LOW, 0); // No quality reduction
    skyTexture_->SetNumLevels(1); // No mipmaps
    skyTexture_->SetAddressMode(COORD_U, ADDRESS_CLAMP);
    skyTexture_->SetAddressMode(COORD_V, ADDRESS_CLAMP);

    int width = 8;
    int height = 32;

    skyTexture_->SetSize(width, height, Graphics::GetRGBAFormat(), TEXTURE_DYNAMIC);
    SharedArrayPtr<unsigned char> emptyBitmap(new unsigned char[width * height * 4]);
    memset(emptyBitmap.Get(), 0, width * height * 4);
    skyTexture_->SetData(0, 0, 0, width, height, emptyBitmap.Get());

    PODVector<Component*> components;
    node_->GetScene()->GetComponents(components, Zone::GetTypeStatic(), true);
    zone_ = (Zone*)components[0];

}

void ProcSky::ProcessRayQuery(const RayOctreeQuery& query, PODVector<RayQueryResult>& results)
{
    // Do not record a raycast result for a skybox, as it would block all other results
}




}
