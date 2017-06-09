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

#include "BakeModel.h"
#include "BakeMesh.h"
#include "BakeLight.h"
#include "EmbreeScene.h"
#include "LightMapPacker.h"
#include "SceneBaker.h"

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

void SceneBaker::TraceRay(LightRay* lightRay, const PODVector<BakeLight*>& bakeLights_)
{
    for (unsigned i = 0; i < bakeLights_.Size(); i++)
    {
        bakeLights_[i]->Light(lightRay);
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
    bakeLights_.Clear();
}

bool SceneBaker::LightGI()
{
    // Indirect Lighting
    currentLightMode_ = GLOW_LIGHTMODE_INDIRECT;

    if (!GlobalGlowSettings.giEnabled_ || currentGIPass_ >= GlobalGlowSettings.giMaxBounces_)
    {
        return false;
    }

    ATOMIC_LOGINFOF("GI Pass #%i of %i", currentGIPass_ + 1, GlobalGlowSettings.giMaxBounces_);

    BounceBakeLight* blight;
    unsigned totalBounceSamples = 0;

    for (unsigned i = 0; i < bakeMeshes_.Size(); i++)
    {
        BakeMesh* mesh = bakeMeshes_[i];
        blight = mesh->GenerateBounceBakeLight();
        if (blight)
        {
            bakeLights_.Push(SharedPtr<BakeLight>(blight));
            totalBounceSamples += blight->GetNumBounceSamples();
        }
    }

    if (!bakeLights_.Size())
        return false;

    ATOMIC_LOGINFOF("IMPORTANT: Optimize allocation of samples! Indirect lighting with %u bounce lights and %u samples", bakeLights_.Size(), totalBounceSamples);

    for (unsigned i = 0; i < bakeMeshes_.Size(); i++)
    {
        BakeMesh* mesh = bakeMeshes_[i];
        mesh->Light(GLOW_LIGHTMODE_INDIRECT);
    }

    return true;

}

void SceneBaker::LightGIFinish()
{
    bakeLights_.Clear();

    currentGIPass_++;

    if (currentGIPass_ >= GlobalGlowSettings.giMaxBounces_)
    {
    }
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
            ATOMIC_LOGINFO("Cycle: GI - no work to be done");
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
        SharedPtr<ZoneBakeLight> zlight(new ZoneBakeLight(context_, this));
        zlight->SetZone(zone);
        bakeLights_.Push(zlight);
    }

    // Lights
    PODVector<Node*> lightNodes;
    scene_->GetChildrenWithComponent<Atomic::Light>(lightNodes, true);

    for (unsigned i = 0; i < lightNodes.Size(); i++)
    {
        Atomic::Light* light = lightNodes[i]->GetComponent<Atomic::Light>();

        if (!light->GetNode()->IsEnabled()|| !light->IsEnabled())
            continue;

        if (light->GetLightType() == LIGHT_DIRECTIONAL)
        {
            SharedPtr<DirectionalBakeLight> dlight(new DirectionalBakeLight(context_, this));
            dlight->SetLight(light);
            bakeLights_.Push(dlight);
        }
        else if (light->GetLightType() == LIGHT_POINT)
        {
            SharedPtr<PointBakeLight> dlight(new PointBakeLight(context_, this));
            dlight->SetLight(light);
            bakeLights_.Push(dlight);
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

}
