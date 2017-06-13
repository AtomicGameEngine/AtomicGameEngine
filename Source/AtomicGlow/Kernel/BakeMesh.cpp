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

#include "EmbreeScene.h"

#include <Atomic/Core/WorkQueue.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Graphics/Zone.h>

#include <AtomicGlow/Common/GlowSettings.h>

#include "Raster.h"
#include "LightRay.h"
#include "SceneBaker.h"
#include "RadianceMap.h"
#include "BakeLight.h"
#include "BakeMesh.h"

namespace AtomicGlow
{

BakeMesh::BakeMesh(Context* context, SceneBaker *sceneBaker) : BakeNode(context, sceneBaker),
    numVertices_(0),
    numTriangles_(0),
    radianceHeight_(0),
    radianceWidth_(0),
    embreeGeomID_(RTC_INVALID_GEOMETRY_ID),
    numWorkItems_(0),
    ambientColor_(Color::BLACK)
{

}

BakeMesh::~BakeMesh()
{

}

void BakeMesh::Pack(unsigned lightmapIdx, Vector4 tilingOffset)
{
    if (radianceMap_.NotNull())
    {
        radianceMap_->packed_ = true;
    }

    // modify the in memory scene, which will either become data for the editor
    // or in standalone mode, saved out the scene
    if (staticModel_)
    {
        staticModel_->SetLightMask(0);
        staticModel_->SetLightmapIndex(lightmapIdx);
        staticModel_->SetLightmapTilingOffset(tilingOffset);
    }

}


bool BakeMesh::FillLexelsCallback(void* param, int x, int y, const Vector3& barycentric,const Vector3& dx, const Vector3& dy, float coverage)
{
    return ((ShaderData*) param)->bakeMesh_->LightPixel((ShaderData*) param, x, y, barycentric, dx, dy, coverage);
}

bool BakeMesh::LightPixel(ShaderData* shaderData, int x, int y, const Vector3& barycentric,const Vector3& dx, const Vector3& dy, float coverage)
{

    if (x >= radianceWidth_ || y >= radianceHeight_)
        return true;

    meshMutex_.Acquire();
    bool accept = radiancePassAccept_[y * radianceWidth_ + x];

    // check whether we've already lit this pixel
    if (accept)
    {
        meshMutex_.Release();
        return true;
    }

    radiancePassAccept_[y * radianceWidth_ + x] = true;

    meshMutex_.Release();

    MMTriangle* tri = &triangles_[shaderData->triangleIdx_];
    MMVertex* verts[3];

    verts[0] = &vertices_[tri->indices_[0]];
    verts[1] = &vertices_[tri->indices_[1]];
    verts[2] = &vertices_[tri->indices_[2]];

    LightRay ray;
    LightRay::SamplePoint& sample = ray.samplePoint_;

    sample.bakeMesh = this;

    sample.triangle = shaderData->triangleIdx_;
    sample.barycentric = barycentric;

    sample.radianceX = x;
    sample.radianceY = y;

    sample.position = verts[0]->position_ * barycentric.x_ +
                      verts[1]->position_ * barycentric.y_ +
                      verts[2]->position_ * barycentric.z_;

    sample.normal = verts[0]->normal_ * barycentric.x_ +
                    verts[1]->normal_ * barycentric.y_ +
                    verts[2]->normal_ * barycentric.z_;

    sample.uv0  = verts[0]->uv0_ * barycentric.x_ +
                  verts[1]->uv0_ * barycentric.y_ +
                  verts[2]->uv0_ * barycentric.z_;

    sample.uv1  = verts[0]->uv1_ * barycentric.x_ +
                  verts[1]->uv1_ * barycentric.y_ +
                  verts[2]->uv1_ * barycentric.z_;


    sceneBaker_->TraceRay(&ray, bakeLights_);

    return true;
}

void BakeMesh::LightTrianglesWork(const WorkItem* item, unsigned threadIndex)
{
    ShaderData shaderData;
    BakeMesh* bakeMesh = shaderData.bakeMesh_ = (BakeMesh*) item->aux_;

    shaderData.lightMode_ = bakeMesh->GetSceneBaker()->GetCurrentLightMode();
    Vector2 triUV1[3];

    Vector2 extents(bakeMesh->radianceWidth_, bakeMesh->radianceHeight_);

    MMTriangle* start = reinterpret_cast<MMTriangle*>(item->start_);
    MMTriangle* end = reinterpret_cast<MMTriangle*>(item->end_);

    while (start <= end)
    {
        MMTriangle* tri = start;

        shaderData.triangleIdx_ = tri - &bakeMesh->triangles_[0];

        start++;

        for (unsigned j = 0; j < 3; j++)
        {
            unsigned idx = tri->indices_[j];

            triUV1[j] = bakeMesh->vertices_[idx].uv1_;
            triUV1[j].x_ *= float(bakeMesh->radianceWidth_);
            triUV1[j].y_ *= float(bakeMesh->radianceHeight_);

            triUV1[j].x_ = Clamp<float>(triUV1[j].x_, 0.0f, bakeMesh->radianceWidth_);
            triUV1[j].y_ = Clamp<float>(triUV1[j].y_, 0.0f, bakeMesh->radianceHeight_);
        }

        Raster::DrawTriangle(true, extents, false, triUV1, FillLexelsCallback, &shaderData );

    }

}


void BakeMesh::ContributeRadiance(const LightRay* lightRay, const Vector3& radiance, GlowLightMode lightMode)
{
    MutexLock lock(meshMutex_);

    const LightRay::SamplePoint& source = lightRay->samplePoint_;

    unsigned radX = source.radianceX;
    unsigned radY = source.radianceY;

    const Vector3& v = radiance_[radY * radianceWidth_ + radX];

    radianceTriIndices_[radY * radianceWidth_ + radX]  = source.triangle;

    if (v.x_ < 0.0f)
    {
        radiance_[radY * radianceWidth_ + radX] = radiance;
    }
    else
    {
        radiance_[radY * radianceWidth_ + radX] += radiance;
    }

}

void BakeMesh::GenerateRadianceMap()
{
    if (radianceMap_.NotNull())
        return;

    radianceMap_ = new RadianceMap(context_, this);
}


void BakeMesh::Light(GlowLightMode mode)
{
    if (mode == GLOW_LIGHTMODE_INDIRECT && !GlobalGlowSettings.giEnabled_)
        return;

    if (!GetLightmap() || !radianceWidth_ || !radianceHeight_ || !numTriangles_)
        return;

    bakeLights_.Clear();
    sceneBaker_->QueryLights(boundingBox_, bakeLights_);

    // for all triangles

    for (unsigned i = 0; i < radianceWidth_ * radianceHeight_; i++)
    {
        radiancePassAccept_[i] = false;
    }

    WorkQueue* queue = GetSubsystem<WorkQueue>();

    unsigned numTrianglePerItem = numTriangles_ / 4 ? numTriangles_ / 4 : numTriangles_;

    unsigned curIdx = 0;
    numWorkItems_ = 0;

    while (true)
    {
        SharedPtr<WorkItem> item = queue->GetFreeItem();
        item->priority_ = M_MAX_UNSIGNED;
        item->workFunction_ = LightTrianglesWork;
        item->aux_ = this;

        item->start_ = &triangles_[curIdx];

        unsigned endIdx = curIdx + numTrianglePerItem;

        if (endIdx < numTriangles_)
        {
            item->end_ = &triangles_[endIdx];
        }
        else
        {
            item->end_ = &triangles_[numTriangles_ - 1];
        }

        item->sendEvent_ = true;

        queue->AddWorkItem(item);

        numWorkItems_++;

        if (item->end_ == &triangles_[numTriangles_ - 1])
            break;

        curIdx += numTrianglePerItem + 1;
    }


}

static unsigned CalcLightMapSize(unsigned sz)
{
    // highest multiple of 8, note rasterizer requires a multiple of 8!
    sz = (sz + 8) & ~7;

    if (sz > 512 && !IsPowerOfTwo(sz))
    {
        sz = NextPowerOfTwo(sz)/2;
    }

    sz = Clamp<unsigned>(sz, 32, GlobalGlowSettings.lightmapSize_);

    return sz;

}

void BakeMesh::Preprocess()
{

    RTCScene scene = sceneBaker_->GetEmbreeScene()->GetRTCScene();

    if (staticModel_ && staticModel_->GetZone())
    {
        ambientColor_ = staticModel_->GetZone()->GetAmbientColor();
    }

    if (staticModel_ && staticModel_->GetCastShadows())
    {
        // Create the embree mesh
        embreeGeomID_ = rtcNewTriangleMesh(scene, RTC_GEOMETRY_STATIC, numTriangles_, numVertices_);
        rtcSetUserData(scene, embreeGeomID_, this);
        rtcSetOcclusionFilterFunction(scene, embreeGeomID_, OcclusionFilter);
        rtcSetIntersectionFilterFunction(scene, embreeGeomID_, IntersectFilter);

        // Populate vertices

        float* vertices = (float*) rtcMapBuffer(scene, embreeGeomID_, RTC_VERTEX_BUFFER);

        MMVertex* vIn = &vertices_[0];

        for (unsigned i = 0; i < numVertices_; i++, vIn++)
        {
            *vertices++ = vIn->position_.x_;
            *vertices++ = vIn->position_.y_;
            *vertices++ = vIn->position_.z_;

            // Note that RTC_VERTEX_BUFFER is 16 byte aligned, thus extra component
            // which isn't used, though we'll initialize it
            *vertices++ = 0.0f;
        }

        rtcUnmapBuffer(scene, embreeGeomID_, RTC_VERTEX_BUFFER);

        uint32_t* triangles = (uint32_t*) rtcMapBuffer(scene, embreeGeomID_, RTC_INDEX_BUFFER);

        for (size_t i = 0; i < numTriangles_; i++)
        {
            MMTriangle* tri = &triangles_[i];

            *triangles++ = tri->indices_[0];
            *triangles++ = tri->indices_[1];
            *triangles++ = tri->indices_[2];
        }

        rtcUnmapBuffer(scene, embreeGeomID_, RTC_INDEX_BUFFER);

        sceneBaker_->GetEmbreeScene()->RegisterBakeMesh(embreeGeomID_, this);
    }

    float lmScale = staticModel_->GetLightmapScale();

    // if we aren't lightmapped, we just contribute to occlusion
    if (!GetLightmap() || lmScale <= 0.0f)
        return;

    unsigned lmSize = staticModel_->GetLightmapSize();

    if (!lmSize)
    {
        float totalarea = 0.0f;

        for (unsigned i = 0; i < numTriangles_; i++)
        {
            MMTriangle* tri = &triangles_[i];

            MMVertex* v0 = &vertices_[tri->indices_[0]];
            MMVertex* v1 = &vertices_[tri->indices_[1]];
            MMVertex* v2 = &vertices_[tri->indices_[2]];

            totalarea += AreaOfTriangle(v0->position_,
                                        v1->position_,
                                        v2->position_);
        }

        totalarea = Clamp<float>(totalarea, 1, 64.0f);

        lmSize = CalcLightMapSize(totalarea * 64.0f * lmScale * GlobalGlowSettings.lexelDensity_ * GlobalGlowSettings.sceneLexelDensityScale_);

    }

    if (lmSize < 32)
        lmSize = 32;

    if (lmSize > 2048)
        lmSize = 2048;

    radianceWidth_ = lmSize;
    radianceHeight_ = lmSize;

    // init radiance
    radiance_ = new Vector3[radianceWidth_ * radianceHeight_];
    radianceTriIndices_ = new int[radianceWidth_ * radianceHeight_];
    radiancePassAccept_ = new bool[radianceWidth_ * radianceHeight_];

    Vector3 v(-1, -1, -1);
    for (unsigned i = 0; i < radianceWidth_ * radianceHeight_; i++)
    {
        radiance_[i] = v;
        radianceTriIndices_[i] = -1;
        radiancePassAccept_[i] = false;
    }

    // If GI is enabled, setup bounce metrics
    if (GlobalGlowSettings.giEnabled_)
    {
        photonMap_ = new PhotonMap(radianceWidth_, radianceHeight_);
    }
}


bool BakeMesh::SetStaticModel(StaticModel* staticModel)
{
    if (!staticModel || !staticModel->GetNode())
        return false;

    staticModel_ = staticModel;
    node_ = staticModel_->GetNode();

    bakeModel_ = GetSubsystem<BakeModelCache>()->GetBakeModel(staticModel_->GetModel());

    if (bakeModel_.Null())
    {
        return false;
    }

    ModelPacker* modelPacker = bakeModel_->GetModelPacker();

    if (modelPacker->lodLevels_.Size() < 1)
    {
        return false;
    }

    MPLODLevel* lodLevel = modelPacker->lodLevels_[0];

    // LOD must have LM coords

    if (!lodLevel->HasElement(TYPE_VECTOR2, SEM_TEXCOORD, 1))
    {
        return false;
    }

    // materials

    if (staticModel_->GetNumGeometries() != lodLevel->mpGeometry_.Size())
    {
        ATOMIC_LOGERROR("BakeMesh::Preprocess() - Geometry mismatch");
        return false;
    }

    for (unsigned i = 0; i < staticModel_->GetNumGeometries(); i++)
    {
        BakeMaterial* bakeMaterial = GetSubsystem<BakeMaterialCache>()->GetBakeMaterial(staticModel_->GetMaterial(i));
        bakeMaterials_.Push(bakeMaterial);
    }

    // allocate

    numVertices_ = 0;
    unsigned totalIndices = 0;

    lodLevel->GetTotalCounts(numVertices_, totalIndices);

    if (!numVertices_ || ! totalIndices)
    {
        return false;
    }

    numTriangles_ = totalIndices/3;

    vertices_ = new MMVertex[numVertices_];
    triangles_ = new MMTriangle[numTriangles_];

    MMVertex* vOut = &vertices_[0];
    MMTriangle* tri = &triangles_[0];

    unsigned vertexStart = 0;
    unsigned indexStart = 0;

    const Matrix3x4& wtransform = node_->GetWorldTransform();

    for (unsigned i = 0; i < lodLevel->mpGeometry_.Size(); i++)
    {
        MPGeometry* mpGeo = lodLevel->mpGeometry_[i];

        // Setup Vertices

        MPVertex* vIn = &mpGeo->vertices_[0];

        for (unsigned j = 0; j < mpGeo->vertices_.Size(); j++)
        {
            vOut->position_ = wtransform * vIn->position_;
            vOut->normal_ = wtransform.Rotation() * vIn->normal_;
            vOut->uv0_ = vIn->uv0_;
            vOut->uv1_ = vIn->uv1_;

            boundingBox_.Merge(vOut->position_);

            vOut++;
            vIn++;
        }

        // Setup Triangles

        for (unsigned j = 0; j < mpGeo->numIndices_; j+=3, tri++)
        {
            tri->materialIndex_ = i;
            tri->indices_[0] = mpGeo->indices_[j] + vertexStart;
            tri->indices_[1] = mpGeo->indices_[j + 1] + vertexStart;
            tri->indices_[2] = mpGeo->indices_[j + 2] + vertexStart;

            tri->normal_ = vertices_[tri->indices_[0]].normal_;
            tri->normal_ += vertices_[tri->indices_[1]].normal_;
            tri->normal_ += vertices_[tri->indices_[2]].normal_;
            tri->normal_ /= 3.0f;
            tri->normal_.Normalize();

        }

        indexStart  += mpGeo->numIndices_;
        vertexStart += mpGeo->vertices_.Size();
    }

    return true;

}

void BakeMesh::GetST(int triIndex, int channel, const Vector3& barycentric, Vector2& st) const
{
    if (triIndex < 0 || triIndex >= numTriangles_)
        return;

    const MMTriangle* tri = &triangles_[triIndex];

    const Vector2& st0 = channel == 0 ? vertices_[tri->indices_[0]].uv0_ : vertices_[tri->indices_[0]].uv1_;
    const Vector2& st1 = channel == 0 ? vertices_[tri->indices_[1]].uv0_ : vertices_[tri->indices_[1]].uv1_;
    const Vector2& st2 = channel == 0 ? vertices_[tri->indices_[2]].uv0_ : vertices_[tri->indices_[2]].uv1_;

    st = barycentric.z_*st0 + barycentric.x_*st1 + barycentric.y_*st2;

}

bool BakeMesh::GetUV0Color(int triIndex, const Vector3& barycentric, Color& colorOut) const
{
    colorOut = Color::BLACK;

    if (triIndex < 0 || triIndex >= numTriangles_)
        return false;

    const MMTriangle* tri = &triangles_[triIndex];
    const BakeMaterial* material = bakeMaterials_[tri->materialIndex_];
    const Image* diffuse = material->GetDiffuseTexture();

    if (!diffuse)
    {
        return false;
    }

    Vector2 st;

    GetST(triIndex, 0, barycentric, st);

    int x = diffuse->GetWidth() * st.x_;
    int y = diffuse->GetHeight() * st.y_;

    if (x < 0)
        x = diffuse->GetWidth() + x;

    if (y < 0)
        y = diffuse->GetWidth() + y;

    colorOut = diffuse->GetPixel(x, y);

    return true;
}

void BakeMesh::IntersectFilter(void* ptr, RTCRay& ray)
{
    CommonFilter(static_cast<BakeMesh*>(ptr), ray);
}

void BakeMesh::OcclusionFilter(void* ptr, RTCRay& ray)
{
    CommonFilter(static_cast<BakeMesh*>(ptr), ray);
}

bool BakeMesh::CommonFilter(const BakeMesh* bakeMesh, RTCRay& ray)
{
    if (ray.primID >= (unsigned) bakeMesh->numTriangles_)
        return false;

    const MMTriangle* tri = &bakeMesh->triangles_[ray.primID];
    const BakeMaterial* material = bakeMesh->bakeMaterials_[tri->materialIndex_];

    if (!material)
        return false;

    if (!material->GetOcclusionMasked())
        return false;

    Color color;
    if (bakeMesh->GetUV0Color(ray.primID, Vector3(ray.u, ray.v, 1.0f-ray.u-ray.v), color))
    {
        if (color.a_ < 0.5f)
        {
            ray.geomID = RTC_INVALID_GEOMETRY_ID;
            return true;
        }
    }

    return false;
}


}
