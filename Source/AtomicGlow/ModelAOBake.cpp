
#include <embree2/rtcore.h>
#include <embree2/rtcore_ray.h>
#include <xmmintrin.h>
#include <pmmintrin.h>
#include <cmath>
#include <cfloat>

#include <Atomic/IO/Log.h>
#include <Atomic/Resource/Image.h>
#include <Atomic/Resource/ResourceCache.h>

#include "ModelAOBake.h"

#ifdef __llvm__
double omp_get_wtime() { return 1; }
int omp_get_max_threads() { return 1; }
int omp_get_thread_num() { return 1; }
#else
#include <omp.h>
#endif


namespace AtomicGlow
{


static inline void GetBarycentricCoordinates(const Vector2& p1, const Vector2& p2, const Vector2& p3, const Vector2& p, Vector3& out)
{
    out.x_ = out.y_ = 0.0f;

    float denom = (-p1.x_ * p3.y_ - p2.x_ * p1.y_ + p2.x_ * p3.y_ + p1.y_ * p3.x_ + p2.y_ *p1.x_ - p2.y_ * p3.x_ );

    if (fabs(denom) > M_EPSILON)
    {
        out.x_ = (p2.x_ * p3.y_ - p2.y_ * p3.x_ - p.x_ * p3.y_ + p3.x_ * p.y_ - p2.x_ * p.y_ + p2.y_ * p.x_) / denom;
        out.y_ = -(-p1.x_ * p.y_ + p1.x_ * p3.y_ + p1.y_ * p.x_ - p.x_ * p3.y_ + p3.x_ * p.y_ - p1.y_ * p3.x_) / denom;
    }

    out.z_ = 1 - out.x_ - out.y_;

}

static inline int GetPixelCoordinate(float textureCoord, unsigned extent)
{
    if (!extent)
        return 0;

    int pixel = (int)(textureCoord * extent);

    if (pixel < 0)
        pixel = 0;

    if (pixel >= (int) extent)
        pixel = extent - 1;

    return pixel;
}

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

ModelAOBake::ModelAOBake(Context* context) : Object(context),
    numIndices_(0)
{

}

ModelAOBake::~ModelAOBake()
{

}

/*
unsigned ModelAOBake::GetImageSize(float pixelsPerUnit, bool powerOfTwo)
{
    if (!lmVertices_.Size())
        return 0;

    Vector2 tMin(999999, 999999);
    Vector2 tMax(-999999, -999999);

    Vector3 pMin(999999, 999999, 999999);
    Vector3 pMax(-999999, -999999, -999999);

    for (unsigned i = 0; i < lmVertices_.Size(); i++)
    {
        const LMVertex& v = lmVertices_[i];

        if (tMin.x_ > v.uv1_.x_)
            tMin.x_ = v.uv1_.x_;
        if (tMin.y_ > v.uv1_.y_)
            tMin.y_ = v.uv1_.y_;

        if (tMax.x_ < v.uv1_.x_)
            tMax.x_ = v.uv1_.x_;
        if (tMax.y_ < v.uv1_.y_)
            tMax.y_ = v.uv1_.y_;
    }

    ATOMIC_LOGINFOF("%f %f : %f %f", tMin.x_, tMin.y_, tMax.x_, tMax.y_);

    return 0;

}
*/

void ModelAOBake::TraceAORays(unsigned nsamples, float aoDepth, float multiply)
{
    // Intel says to do this, so we're doing it.
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

    // Create the embree device and scene.
    RTCDevice device = rtcNewDevice(NULL);
    assert(device && "Unable to create embree device.");
    RTCScene scene = rtcDeviceNewScene(device, RTC_SCENE_STATIC, RTC_INTERSECT1);
    assert(scene);

    // Create the embree mesh
    uint32_t gid = rtcNewTriangleMesh(scene, RTC_GEOMETRY_STATIC, numIndices_ / 3, lmVertices_.Size());

    // Populate vertices

    float* vertices = (float*) rtcMapBuffer(scene, gid, RTC_VERTEX_BUFFER);

    for (unsigned i = 0; i < lmVertices_.Size(); i++)
    {
        const LMVertex& v = lmVertices_[i];

        *vertices++ = v.position_.x_;
        *vertices++ = v.position_.y_;
        *vertices++ = v.position_.z_;

        vertices++;
    }

    rtcUnmapBuffer(scene, gid, RTC_VERTEX_BUFFER);

    uint32_t* triangles = (uint32_t*) rtcMapBuffer(scene, gid, RTC_INDEX_BUFFER);

    for (size_t i = 0; i < numIndices_; i++)
    {
        *triangles++ = indices_[i];
    }

    rtcUnmapBuffer(scene, gid, RTC_INDEX_BUFFER);

    rtcCommit(scene);

    // Iterate over each pixel in the light map, row by row.
    ATOMIC_LOGINFOF("Rendering ambient occlusion (%d threads)...", omp_get_max_threads());

    double begintime = omp_get_wtime();

    SharedArrayPtr<unsigned char> results(new unsigned char[lightmap_->GetWidth() * lightmap_->GetHeight()]);
    memset(&results[0], 0, sizeof(unsigned char) * lightmap_->GetWidth() * lightmap_->GetHeight());

    const unsigned npixels = lightmap_->GetWidth() * lightmap_->GetHeight();

    const float E = 0.001f;

#pragma omp parallel
    {
        srand(omp_get_thread_num());
        RTCRay ray;
        ray.primID = RTC_INVALID_GEOMETRY_ID;
        ray.instID = RTC_INVALID_GEOMETRY_ID;
        ray.mask = 0xFFFFFFFF;
        ray.time = 0.f;

#pragma omp for

        for (unsigned i = 0; i < npixels; i++)
        {
            LMLexel& lexel = lmLexels_[i];

            if (lexel.normal_ == Vector3::ZERO)
                continue;

            ray.org[0] = lexel.position_.x_;
            ray.org[1] = lexel.position_.y_;
            ray.org[2] = lexel.position_.z_;

            int nhits = 0;

            // Shoot rays through the differential hemisphere.
            for (unsigned nsamp = 0; nsamp < nsamples; nsamp++)
            {
                Vector3 rayDir;
                GetRandomDirection(rayDir);

                float dotp = lexel.normal_.x_ * rayDir.x_ +
                        lexel.normal_.y_ * rayDir.y_ +
                        lexel.normal_.z_ * rayDir.z_;

                if (dotp < 0)
                {
                    rayDir = -rayDir;
                }

                ray.dir[0] = rayDir.x_;
                ray.dir[1] = rayDir.y_;
                ray.dir[2] = rayDir.z_;

                ray.tnear = E;

                float variance = 0.0f;//(aoDepth * (float) rand() / (float) RAND_MAX);

                ray.tfar = aoDepth + variance;

                ray.geomID = RTC_INVALID_GEOMETRY_ID;
                rtcOccluded(scene, ray);

                if (ray.geomID == 0)
                {
                    nhits++;
                }
            }

            float ao = multiply * (1.0f - (float) nhits / nsamples);
            float result = Min<float>(1.0f, ao);
            lexel.color_ = Color(result, result, result);
        }
    }


    // Free all embree data.
    rtcDeleteGeometry(scene, gid);
    rtcDeleteScene(scene);
    rtcDeleteDevice(device);

    // Dilate the image by 2 pixels to allow bilinear texturing near seams.
    // Note that this still allows seams when mipmapping, unless mipmap levels
    // are generated very carefully.
    for (int step = 0; step < 2; step++)
    {
        SharedArrayPtr<Color> tmp(new Color[lightmap_->GetWidth() * lightmap_->GetHeight()]);
        memset (&tmp[0], 0, lightmap_->GetWidth() * lightmap_->GetHeight() * sizeof(Color));

        for (int y = 0; y < lightmap_->GetHeight() ; y++)
        {
            for (int x = 0; x < lightmap_->GetWidth(); x++)
            {
                int center = x + y * lightmap_->GetWidth();

                const LMLexel& lexel = lmLexels_[center];
                const Vector3& norm = lexel.normal_;

                tmp[center] = lexel.color_;

                if (norm.x_ == 0 && norm.y_ == 0 && norm.z_ == 0 && lexel.color_ == Color::BLACK)
                {
                    for (int k = 0; k < 9; k++)
                    {
                        int i = (k / 3) - 1, j = (k % 3) - 1;

                        if (i == 0 && j == 0)
                        {
                            continue;
                        }

                        i += x;
                        j += y;

                        if (i < 0 || j < 0 || i >= lightmap_->GetWidth() || j >=  lightmap_->GetHeight() )
                        {
                            continue;
                        }

                        const LMLexel& lexel2 = lmLexels_[i + j * lightmap_->GetWidth()];

                        if (lexel2.color_ != Color::BLACK)
                        {
                            tmp[center] = lexel2.color_;
                            break;
                        }
                    }
                }
            }
        }

        for (unsigned i = 0; i < lmLexels_.Size(); i++)
        {
            lmLexels_[i].color_ = tmp[i];
        }

    }

}

bool ModelAOBake::GenerateLexels()
{

    return true;
}

bool ModelAOBake::FillLexelsCallback(void* param, int x, int y, const Vector3& barycentric,const Vector3& dx, const Vector3& dy, float coverage)
{
    ShaderData* shaderData = (ShaderData*) param;
    ModelAOBake* bake = shaderData->bake_;

    LMLexel& lexel = bake->lmLexels_[ y * bake->lightmap_->GetWidth() + x];

    lexel.position_ = shaderData->triPositions_[0] * barycentric.x_ +
            shaderData->triPositions_[1] *  barycentric.y_ +
            shaderData->triPositions_[2] * barycentric.z_;

    lexel.normal_ = shaderData->faceNormal_;

    return true;
}

bool ModelAOBake::GenerateLODLevelAOMap(MPLODLevel *lodLevel)
{
    curLOD_ = lodLevel;

    if (curLOD_.Null())
    {
        curLOD_ = 0;
        return false;
    }

    // LOD must have LM coords

    if (!curLOD_->HasElement(TYPE_VECTOR2, SEM_TEXCOORD, 1))
    {
        curLOD_ = 0;
        return false;
    }

    unsigned totalVertices = 0;
    curLOD_->GetTotalCounts(totalVertices, numIndices_);

    if (!totalVertices || ! numIndices_)
    {
        curLOD_ = 0;
        return false;
    }

    // allocate

    lmVertices_.Resize(totalVertices);
    indices_ = new unsigned[numIndices_];

    LMVertex* vOut = &lmVertices_[0];

    unsigned vertexStart = 0;
    unsigned indexStart = 0;

    for (unsigned i = 0; i < curLOD_->mpGeometry_.Size(); i++)
    {
        MPGeometry* mpGeo = curLOD_->mpGeometry_[i];

        // Copy Vertices

        MPVertex* vIn = &mpGeo->vertices_[0];

        for (unsigned j = 0; j < mpGeo->vertices_.Size(); j++)
        {
            vOut->position_ = vIn->position_;
            vOut->normal_ = vIn->normal_;
            vOut->uv0_ = vIn->uv0_;
            vOut->uv1_ = vIn->uv1_;

            vOut++;
            vIn++;
        }

        // Copy Indices

        for (unsigned j = 0; j < mpGeo->numIndices_; j++)
        {
            indices_[j + indexStart] = mpGeo->indices_[j] + vertexStart;
        }

        indexStart  += mpGeo->numIndices_;
        vertexStart += mpGeo->vertices_.Size();
    }

    lightmap_ = new Image(context_);

    unsigned w, h;
    w = 4096;
    h = 4096;

    lightmap_->SetSize(w, h, 2, 3);

    lmLexels_.Resize(w * h);

    for (unsigned y = 0; y < h; y++)
    {
        for (unsigned x = 0; x < w ; x++)
        {
            LMLexel& lexel = lmLexels_[y * w + x];
            lexel.color_ = Color::BLACK;
            lexel.pixelCoord_.x_ = x;
            lexel.pixelCoord_.y_ = y;
            lexel.normal_ = Vector3(0, 0, 0);
            lexel.position_ = Vector3(0, 0, 0);
        }
    }

    // for all triangles

    Vector2 extents(lightmap_->GetWidth(), lightmap_->GetHeight());
    Vector2 triUV[3];

    ShaderData shaderData;

    shaderData.bake_ = this;

    for (unsigned i = 0; i < numIndices_; i += 3)
    {
        shaderData.triPositions_[0] = lmVertices_[indices_[i]].position_;
        shaderData.triPositions_[1] = lmVertices_[indices_[i + 1]].position_;
        shaderData.triPositions_[2] = lmVertices_[indices_[i + 2]].position_;

        shaderData.triNormals_[0] = lmVertices_[indices_[i]].normal_;
        shaderData.triNormals_[1] = lmVertices_[indices_[i + 1]].normal_;
        shaderData.triNormals_[2] = lmVertices_[indices_[i + 2]].normal_;

        triUV[0] = lmVertices_[indices_[i]].uv1_;
        triUV[1] = lmVertices_[indices_[i + 1]].uv1_;
        triUV[2] = lmVertices_[indices_[i + 2]].uv1_;

        triUV[0].x_ *= w;
        triUV[1].x_ *= w;
        triUV[2].x_ *= w;

        triUV[0].y_ *= h;
        triUV[1].y_ *= h;
        triUV[2].y_ *= h;

        Vector3 A = shaderData.triPositions_[1] - shaderData.triPositions_[0];
        Vector3 B = shaderData.triPositions_[2] - shaderData.triPositions_[0];
        shaderData.faceNormal_ = A.CrossProduct(B);
        shaderData.faceNormal_.Normalize();

        Raster::DrawTriangle(true, extents, true, triUV, FillLexelsCallback, &shaderData );

    }


    // Raytrace

    TraceAORays(128, 0.5f);

    for (unsigned i = 0; i < lmLexels_.Size(); i++)
    {
        const LMLexel& lexel = lmLexels_[i];
        lightmap_->SetPixelInt(lexel.pixelCoord_.x_, lexel.pixelCoord_.y_, 0, lexel.color_.ToUInt());
    }

    lightmap_->SavePNG("/Users/jenge/Dev/atomic/AtomicExamples/GlowTest/Resources/Textures/lightmap.png");


    // GetImageSize(32, false);

    curLOD_ = 0;

    return true;
}

bool ModelAOBake::LoadModel(const String& pathName)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    Model* model = cache->GetResource<Model>(pathName);

    if (!model)
        return false;

    modelPacker_ = new ModelPacker(context_);

    if (!modelPacker_->Unpack(model))
        return false;

    for (unsigned i = 0; i < modelPacker_->lodLevels_.Size(); i++)
    {
        GenerateLODLevelAOMap(modelPacker_->lodLevels_[i]);
    }

    return true;
}

}

