// Copyright (c) 2014-2017, THUNDERBEAST GAMES LLC All rights reserved
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

#include <xmmintrin.h>
#include <pmmintrin.h>
#include <cmath>
#include <cfloat>

#include <ThirdParty/STB/stb_rect_pack.h>

#include <Atomic/Core/WorkQueue.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Graphics/Zone.h>
#include <Atomic/Graphics/Light.h>
#include <Atomic/Graphics/StaticModel.h>

#include "LightRay.h"
#include "BakeModel.h"
#include "BakeMesh.h"
#include "BakeLight.h"
#include "EmbreeScene.h"
#include "LightMapPacker.h"
#include "SceneBaker.h"
#include "Photons.h"

namespace AtomicGlow
{

SceneBaker::SceneBaker(Context* context, const String &projectPath) : Object(context),
    currentLightMode_(GLOW_LIGHTMODE_UNDEFINED),
    currentGIPass_(0),
    projectPath_(AddTrailingSlash(projectPath)),
    standaloneMode_(true)
{
    embreeScene_ = new EmbreeScene(context_);
}

SceneBaker::~SceneBaker()
{

}

bool SceneBaker::SaveLitScene()
{
    if (!standaloneMode_)
    {
        ATOMIC_LOGERROR("SceneBaker::SaveLitScene() - only supported in standalone mode");
        return false;
    }

    String sceneFilename = AddTrailingSlash(projectPath_) + "Resources/" + scene_->GetFileName();

    File saveFile(context_, sceneFilename, FILE_WRITE);
    return scene_->SaveXML(saveFile);
}

bool SceneBaker::WriteBakeData(VectorBuffer& buffer)
{
    buffer.Clear();

    // protocol is very simple right now, can easily be expanded

    buffer.WriteUInt(bakeMeshes_.Size());

    for (unsigned i = 0; i < bakeMeshes_.Size(); i++)
    {
        BakeMesh* bakeMesh = bakeMeshes_[i];
        Node* node = bakeMesh->GetNode();
        StaticModel* staticModel = bakeMesh->GetStaticModel();

        buffer.WriteUInt(node->GetID());
        buffer.WriteUInt(staticModel->GetID());
        buffer.WriteUInt(staticModel->GetLightMask());
        buffer.WriteUInt(staticModel->GetLightmapIndex());
        buffer.WriteVector4(staticModel->GetLightmapTilingOffset());
    }

    return true;
}

bool SceneBaker::GenerateLightmaps()
{
    ATOMIC_LOGINFO("Generating Lightmaps");

    for (unsigned i = 0; i < bakeMeshes_.Size(); i++)
    {
        BakeMesh* mesh = bakeMeshes_[i];
        mesh->GenerateRadianceMap();
    }

    SharedPtr<LightMapPacker> packer(new LightMapPacker(context_));

    for (unsigned i = 0; i < bakeMeshes_.Size(); i++)
    {
        BakeMesh* mesh = bakeMeshes_[i];

        SharedPtr<RadianceMap> radianceMap = mesh->GetRadianceMap();

        if (radianceMap.NotNull())
        {
            packer->AddRadianceMap(radianceMap);
        }

    }

    packer->Pack();

    if (!packer->SaveLightmaps(projectPath_, scene_->GetFileName()))
    {
        return false;
    }

    if (standaloneMode_)
    {
        if (!SaveLitScene())
            return false;
    }

    return WriteBakeData(bakeData_);

}

void SceneBaker::TraceRay(LightRay* lightRay, const PODVector<BakeLight*>& bakeLights)
{
    if (currentLightMode_ == GLOW_LIGHTMODE_DIRECT)
    {
        DirectLight(lightRay, bakeLights);
    }
    else
    {
        IndirectLight(lightRay);
    }

}

bool SceneBaker::LightDirect()
{
    // Direct Lighting
    currentLightMode_ = GLOW_LIGHTMODE_DIRECT;

    if (!bakeMeshes_.Size())
    {
        ATOMIC_LOGINFO("SceneBaker::LightDirect() - No bake meshes found");
        bakeLights_.Clear();
        return false;
    }

    for (unsigned i = 0; i < bakeMeshes_.Size(); i++)
    {
        BakeMesh* mesh = bakeMeshes_[i];
        mesh->Light(GLOW_LIGHTMODE_DIRECT);
    }

    return true;

}

void SceneBaker::LightDirectFinish()
{

}

bool SceneBaker::EmitPhotons()
{
    Photons photons(this, GlobalGlowSettings.photonPassCount_, GlobalGlowSettings.photonBounceCount_,
                          GlobalGlowSettings.photonEnergyThreshold_, GlobalGlowSettings.photonMaxDistance_);

    int numPhotons = photons.Emit(bakeLights_);

    ATOMIC_LOGINFOF("SceneBaker::EmitPhotons() - %i photons emitted", numPhotons);

    if (!numPhotons)
    {
        return false;
    }


    for( unsigned i = 0; i < bakeMeshes_.Size(); i++ )
    {
        if( PhotonMap* photons = bakeMeshes_[i]->GetPhotonMap() )
        {
            photons->Gather( GlobalGlowSettings.finalGatherRadius_ );
        }
    }

    return true;
}

bool SceneBaker::LightGI()
{
    // Indirect Lighting
    currentLightMode_ = GLOW_LIGHTMODE_INDIRECT;

    // We currently only need one GI pass
    if (!GlobalGlowSettings.giEnabled_ || currentGIPass_ >= 1)
    {
        return false;
    }

    ATOMIC_LOGINFOF("GI Pass #%i of %i", currentGIPass_ + 1, 1);

    bool photons = EmitPhotons();

    if (!photons)
        return false;

    for (unsigned i = 0; i < bakeMeshes_.Size(); i++)
    {
        BakeMesh* mesh = bakeMeshes_[i];
        mesh->Light(GLOW_LIGHTMODE_INDIRECT);
    }

    return true;

}

void SceneBaker::LightGIFinish()
{
    currentGIPass_++;
}


bool SceneBaker::Light(const GlowLightMode lightMode)
{
    if (lightMode == GLOW_LIGHTMODE_DIRECT)
    {
        if (!LightDirect())
        {
            currentLightMode_ = GLOW_LIGHTMODE_COMPLETE;
            ATOMIC_LOGINFO("Cycle: Direct Lighting - no work to be done");
            return false;
        }

        ATOMIC_LOGINFO("Cycle: Direct Lighting");

        return true;
    }

    if (lightMode == GLOW_LIGHTMODE_INDIRECT)
    {
        if (!LightGI())
        {
            currentLightMode_ = GLOW_LIGHTMODE_COMPLETE;

            // We currently only need one GI pass
            if (GlobalGlowSettings.giEnabled_ && currentGIPass_ == 0)
            {
                ATOMIC_LOGINFO("Cycle: GI - no work to be done");
            }

            return false;
        }

        return true;
    }


    return true;
}

void SceneBaker::LightFinishCycle()
{
    if (currentLightMode_ == GLOW_LIGHTMODE_DIRECT)
    {
        LightDirectFinish();
    }

    if (currentLightMode_ == GLOW_LIGHTMODE_INDIRECT)
    {
        LightGIFinish();
    }

}

void SceneBaker::QueryLights(const BoundingBox& bbox, PODVector<BakeLight*>& lights)
{
    lights.Clear();

    for (unsigned i = 0; i < bakeLights_.Size(); i++)
    {

        // TODO: filter on zone, range, groups
        lights.Push(bakeLights_[i]);

    }

}

bool SceneBaker::Preprocess()
{
    Vector<SharedPtr<BakeMesh>>::Iterator itr = bakeMeshes_.Begin();

    while (itr != bakeMeshes_.End())
    {
        (*itr)->Preprocess();
        itr++;
    }

    embreeScene_->Commit();

    return true;
}

bool SceneBaker::LoadScene(const String& filename)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    SharedPtr<File> file = cache->GetFile(filename);

    if (!file || !file->IsOpen())
    {
        return false;
    }

    scene_ = new Scene(context_);

    if (!scene_->LoadXML(*file))
    {
        scene_ = 0;
        return false;
    }

    // IMPORTANT!, if scene updates are enabled
    // the Octree component will add work queue items
    // and will call WorkQueue->Complete(), see Octree::HandleRenderUpdate
    scene_->SetUpdateEnabled(false);

    // Zones
    PODVector<Node*> zoneNodes;
    PODVector<Zone*> zones;
    scene_->GetChildrenWithComponent<Zone>(zoneNodes, true);

    for (unsigned i = 0; i < zoneNodes.Size(); i++)
    {        
        Zone* zone = zoneNodes[i]->GetComponent<Zone>();

        if (!zone->GetNode()->IsEnabled() || !zone->IsEnabled())
            continue;

        zones.Push(zone);;

        BakeLight* bakeLight = BakeLight::CreateZoneLight(this, zone->GetAmbientColor());
        bakeLights_.Push(SharedPtr<BakeLight>(bakeLight));
    }

    // Lights
    PODVector<Node*> lightNodes;
    scene_->GetChildrenWithComponent<Atomic::Light>(lightNodes, true);

    for (unsigned i = 0; i < lightNodes.Size(); i++)
    {
        Node* lightNode = lightNodes[i];
        Atomic::Light* light = lightNode->GetComponent<Atomic::Light>();

        if (!lightNode->IsEnabled() || !light->IsEnabled())
            continue;

        if (light->GetLightType() == LIGHT_DIRECTIONAL)
        {
            /*
            SharedPtr<DirectionalBakeLight> dlight(new DirectionalBakeLight(context_, this));
            dlight->SetLight(light);
            bakeLights_.Push(dlight);
            */
        }
        else if (light->GetLightType() == LIGHT_POINT)
        {
            BakeLight* bakeLight = BakeLight::CreatePointLight(this, lightNode->GetWorldPosition(), light->GetRange(), light->GetColor(), 1.0f, light->GetCastShadows());
            bakeLights_.Push(SharedPtr<BakeLight>(bakeLight));
        }

    }

    // Static Models
    PODVector<StaticModel*> staticModels;
    scene_->GetComponents<StaticModel>(staticModels, true);

    for (unsigned i = 0; i < staticModels.Size(); i++)
    {
        StaticModel* staticModel = staticModels[i];

        if (!staticModel->GetNode()->IsEnabled() || !staticModel->IsEnabled())
            continue;

        Vector3 center = staticModel->GetWorldBoundingBox().Center();
        int bestPriority = M_MIN_INT;
        Zone* newZone = 0;

        for (PODVector<Zone*>::Iterator i = zones.Begin(); i != zones.End(); ++i)
        {
            Zone* zone = *i;
            int priority = zone->GetPriority();
            if (priority > bestPriority && (staticModel->GetZoneMask() & zone->GetZoneMask()) && zone->IsInside(center))
            {
                newZone = zone;
                bestPriority = priority;
            }
        }

        staticModel->SetZone(newZone, false);

        if (staticModel->GetModel() && (staticModel->GetLightmap() ||staticModel->GetCastShadows()))
        {
            Model* model = staticModel->GetModel();

            for (unsigned i = 0; i < model->GetNumGeometries(); i++)
            {
                Geometry* geo = model->GetGeometry(i, 0);

                if (!geo)
                {
                    ATOMIC_LOGERRORF("SceneBaker::LoadScene - model without geometry: %s", model->GetName().CString());
                    return false;
                }

                const unsigned char* indexData = 0;
                unsigned indexSize = 0;
                unsigned vertexSize = 0;
                const unsigned char* vertexData = 0;
                const PODVector<VertexElement>* elements = 0;

                geo->GetRawData(vertexData, vertexSize, indexData, indexSize, elements);

                if (!indexData || !indexSize || !vertexData || !vertexSize || !elements)
                {
                    ATOMIC_LOGERRORF("SceneBaker::LoadScene - Unable to inspect geometry elements: %s",  model->GetName().CString());
                    return false;
                }

                int texcoords = 0;
                for (unsigned i = 0; i < elements->Size(); i++)
                {
                    const VertexElement& element = elements->At(i);

                    if (element.type_ == TYPE_VECTOR2 && element.semantic_ == SEM_TEXCOORD)
                    {
                        texcoords++;
                    }
                }

                if (texcoords < 2)
                {
                    ATOMIC_LOGERRORF("SceneBaker::LoadScene - Model without lightmap UV set, skipping: %s",  model->GetName().CString());
                    continue;
                }

            }

            SharedPtr<BakeMesh> meshMap (new BakeMesh(context_, this));
            meshMap->SetStaticModel(staticModel);
            bakeMeshes_.Push(meshMap);
        }

    }

    return Preprocess();
}

// DIRECT LIGHT

void SceneBaker::DirectLight( LightRay* lightRay, const PODVector<BakeLight*>& bakeLights )
{
    for (unsigned i = 0; i < bakeLights.Size(); i++)
    {
        BakeLight* bakeLight = bakeLights[i];

        Color influence;

        // if (light->GetVertexGenerator())
        //{
        // influence = DirectLightFromPointSet( lightRay, bakeLight );
        //}
        //else
        {
            influence = DirectLightFromPoint( lightRay, bakeLight );
        }

        if (influence.r_ || influence.g_ || influence.b_ )
        {
            lightRay->samplePoint_.bakeMesh->ContributeRadiance(lightRay, influence.ToVector3());
        }

    }
}

Color SceneBaker::DirectLightFromPoint( LightRay* lightRay, const BakeLight* light ) const
{
    float influence = DirectLightInfluenceFromPoint( lightRay, light->GetPosition(), light );

    if( influence > 0.0f )
    {
        return light->GetColor() * light->GetIntensity() * influence;
    }

    return Color::BLACK;
}

Color SceneBaker::DirectLightFromPointSet( LightRay* lightRay, const BakeLight* light ) const
{
    /*
    LightVertexGenerator* vertexGenerator = light->vertexGenerator();

    // ** No light vertices generated - just exit
    if( vertexGenerator->vertexCount() == 0 ) {
        return Rgb( 0, 0, 0 );
    }

    const LightVertexBuffer& vertices = vertexGenerator->vertices();
    Rgb                      color    = Rgb( 0, 0, 0 );

    for( int i = 0, n = vertexGenerator->vertexCount(); i < n; i++ ) {
        const LightVertex&  vertex    = vertices[i];
        float               influence = influenceFromPoint( lumel, vertex.m_position + light->position(), light );

        // ** We have a non-zero light influence - add a light color to final result
        if( influence > 0.0f ) {
            color += light->color() * light->intensity() * influence;
        }
    }

    return color / static_cast<float>( vertexGenerator->vertexCount() );
    */

    return Color::BLACK;
}

// ** DirectLight::influenceFromPoint
float SceneBaker::DirectLightInfluenceFromPoint(LightRay* lightRay, const Vector3 &point, const BakeLight* light ) const
{
    float inf       = 1.0f;
    float att       = 1.0f;
    float cut       = 1.0f;
    float distance  = 0.0f;

    // Calculate light influence.
    if( const LightInfluence* influence = light->GetInfluenceModel() )
    {
        inf = influence->Calculate( lightRay, point, distance );
    }

    // Calculate light cutoff.
    if( const LightCutoff* cutoff = light->GetCutoffModel() )
    {
        cut = cutoff->Calculate( lightRay->samplePoint_.position);
    }

    // Calculate light attenuation
    if( const LightAttenuation* attenuation = light->GetAttenuationModel() )
    {
        att = attenuation->Calculate( distance );
    }

    // Return final influence
    return inf * att * cut;
}

// INDIRECT LIGHT

void SceneBaker::IndirectLight( LightRay* lightRay)
{
    BakeMesh* bakeMesh = 0;
    LightRay::SamplePoint& source = lightRay->samplePoint_;

    Vector3 gathered;

    int nsamples = GlobalGlowSettings.finalGatherSamples_;
    float maxDistance = GlobalGlowSettings.finalGatherDistance_; // , settings.m_finalGatherRadius, settings.m_skyColor, settings.m_ambientColor

    for( int k = 0; k <nsamples; k++ )
    {
        Vector3 dir;
        Vector3::GetRandomHemisphereDirection(dir, source.normal);

        float influence = Max<float>( source.normal.DotProduct(dir), 0.0f );

        if (influence > 1.0f)
        {
            // ATOMIC_LOGINFO("This shouldn't happen");
        }

        RTCRay& ray = lightRay->rtcRay_;
        lightRay->SetupRay(source.position, dir, .001f, maxDistance);

        rtcIntersect(GetEmbreeScene()->GetRTCScene(), ray);

        if (ray.geomID == RTC_INVALID_GEOMETRY_ID)
        {
            // gathered += skyColor * influence + ambientColor;
            continue;
        }               

        bakeMesh = GetEmbreeScene()->GetBakeMesh(ray.geomID);

        if (!bakeMesh)
        {
            continue;
        }

        if (bakeMesh == source.bakeMesh && ray.primID == source.triangle)
        {
            // do not self light
            continue;
        }

        const BakeMesh::MMTriangle* tri = bakeMesh->GetTriangle(ray.primID);

        // TODO: interpolate normal, if artifacting
        if( dir.DotProduct(tri->normal_) >= 0.0f )
        {
            continue;
        }

        PhotonMap* photonMap = bakeMesh->GetPhotonMap();

        if (!photonMap)
        {
            continue;
        }

        Vector3 bary(ray.u, ray.v, 1.0f-ray.u-ray.v);
        Vector2 st;
        bakeMesh->GetST(ray.primID, 1, bary, st);

        int photons;
        Color pcolor;
        Color gcolor;

        if (!photonMap->GetPhoton( (int) ray.primID, st, pcolor, photons, gcolor, true))
        {
            continue;
        }

        gathered += gcolor.ToVector3() * influence;// + ambientColor;

    }

    gathered /= static_cast<float>( nsamples );

    if (gathered.x_ >= 0.01f || gathered.y_ >= 0.01f || gathered.z_ >= 0.01f )
    {
        source.bakeMesh->ContributeRadiance(lightRay, gathered, GLOW_LIGHTMODE_INDIRECT);
    }

}

}
