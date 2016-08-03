//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
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

#include <stdint.h>

#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>

#include <Atomic/Resource/Image.h>

#include <Atomic/Graphics/IndexBuffer.h>
#include <Atomic/Graphics/VertexBuffer.h>
#include <Atomic/Graphics/Geometry.h>

#include <Atomic/Graphics/Octree.h>
#include <Atomic/Graphics/Zone.h>
#include <Atomic/Graphics/AnimatedModel.h>
#include <Atomic/Graphics/StaticModel.h>
#include <Atomic/Graphics/LMStaticModel.h>
#include <Atomic/Graphics/Terrain.h>
#include <Atomic/Graphics/Animation.h>
#include <Atomic/Graphics/DebugRenderer.h>
#include <Atomic/Physics/CollisionShape.h>
#include <Atomic/Physics/RigidBody.h>
#include <Atomic/Physics/PhysicsWorld.h>
#include <Atomic/Environment/TimeOfDay.h>

#include "JSONSceneProcess.h"
#include "JSONSceneImporter.h"

namespace ToolCore
{

bool JSONSceneProcess::ProcessTextures()
{
    const PODVector<JSONTexture*>& textures = importer_->GetTextures();

    for (unsigned i = 0; i < textures.Size(); i++)
    {
        const JSONTexture* jtexture = textures.At(i);

        unsigned length;
        const unsigned char* pixels = jtexture->GetPNGPixels(length);

        String filename = resourcePath_ + "Textures/" + jtexture->GetName() + ".png";

        SharedPtr<File> file (new File(context_, filename, FILE_WRITE));

        file->Write(pixels, length);

        file->Close();

    }

    return true;
}

bool JSONSceneProcess::ProcessLightmaps()
{
    const PODVector<JSONLightmap*>& lightmaps = importer_->GetLightmaps();

    for (unsigned i = 0; i < lightmaps.Size(); i++)
    {
        const JSONLightmap* jlightmap = lightmaps.At(i);

        unsigned length;
        const unsigned char* pixels = jlightmap->GetPNGPixels(length);

        String filename = resourcePath_ + "Textures/" + jlightmap->GetName() + ".png";

        SharedPtr<File> file (new File(context_, filename, FILE_WRITE));

        file->Write(pixels, length);

        file->Close();

    }

    return true;
}

bool JSONSceneProcess::ProcessMaterials()
{
    const PODVector<JSONMaterial*>& materials = importer_->GetMaterials();

    ResourceCache* cache = GetSubsystem<ResourceCache>();

    for (unsigned i = 0; i < materials.Size(); i++)
    {
        const JSONMaterial* jmaterial = materials.At(i);

        const String& shader = jmaterial->GetShader();

        //LOGINFOF("Shader: %s", shader.CString());

        // TODO: factor in lightmaps
        bool lightmap = false;

        String technique("Diff.xml");

        if (!lightmap)
        {
            if (shader == "Transparent/Diffuse")
                technique = "DiffAlpha.xml";
            else if (shader == "Transparent/Cutout/Diffuse")
                technique = "DiffAlphaMask.xml";
            else if (shader == "Transparent/Cutout/Specular")
                technique = "DiffAlphaMask.xml";
            else if (shader == "Transparent/Specular")
                technique = "DiffAlpha.xml";
            else if (shader == "Hidden/Nature/Tree Creator Leaves Fast Optimized")
                technique = "DiffAlphaMask.xml";
        }
        else
        {
            technique = "DiffLightMap.xml";
            if (shader == "Transparent/Diffuse")
                technique = "DiffLightMapAlpha.xml";
            else if (shader == "Transparent/Cutout/Diffuse")
                technique = "DiffLightMapAlpha.xml";
            else if (shader == "Transparent/Cutout/Specular")
                technique = "DiffLightMapAlpha.xml";
            else if (shader == "Transparent/Specular")
                technique = "DiffLightMapAlpha.xml";
        }

        SharedPtr<Material> material;
        material = new Material(context_);

        material->SetName("Materials/" + jmaterial->GetName() + ".material");

        Technique* _technique = cache->GetResource<Technique>("Techniques/" + technique);
        assert(_technique);

        material->SetTechnique(0, _technique);

        const String& mainTexture = jmaterial->GetMainTexture();
        if (mainTexture.Length())
        {
            Texture2D* texture = cache->GetResource<Texture2D>("Textures/" + mainTexture + ".png");
            material->SetTexture(TU_DIFFUSE, texture);
        }

        materials_[jmaterial->GetName()] = material;

    }

    return true;
}

bool JSONSceneProcess::WriteMaterials()
{
    String materialFolder("Materials");
    HashMap<String, SharedPtr<Material> >::Iterator itr = materials_.Begin();
    while (itr != materials_.End())
    {
        Material* material = itr->second_;
        SharedPtr<File> file;
        file = new File(context_, resourcePath_ + material->GetName(), FILE_WRITE);
        material->Save(*file);
        itr++;
    }

    return true;
}

bool JSONSceneProcess::WriteModels()
{
    HashMap<String, SharedPtr<Model> >::Iterator itr = models_.Begin();
    while (itr != models_.End())
    {
        Model* model = itr->second_;
        SharedPtr<File> file;
        file = new File(context_, resourcePath_ + model->GetName(), FILE_WRITE);
        model->Save(*file);
        itr++;
    }

    return true;
}

bool JSONSceneProcess::WriteHierarchy(Scene* scene)
{


    List<SharedPtr<Node> >::Iterator itr = hierarchy_.Begin();
    while (itr != hierarchy_.End())
    {
        scene->AddChild(*itr);
        itr++;
    }

    File file(context_);

    bool useXML = true;

    String filename;

    filename.AppendWithFormat("%s", importer_->GetSceneName().CString());

    //if (!useXML)
        filename += ".scene";
    //else
        //filename += ".xml";

    filename = resourcePath_ + "Scenes/" + filename;

    if (!file.Open(filename, FILE_WRITE))
        ErrorExit("Could not open output file: Scenes/Test.bin");

    if (useXML)
        scene->SaveXML(file);
    else
        scene->Save(file);


    file.Close();


    return true;
}


bool JSONSceneProcess::ProcessModels()
{
    PODVector<JSONMesh*>& meshes = importer_->GetMeshes();

    ResourceCache* cache = GetSubsystem<ResourceCache>();

    for (unsigned i = 0; i < meshes.Size(); i++)
    {
        JSONMesh* jmesh = meshes.At(i);

        SharedPtr<Model> model(new Model(context_));

        model->SetName("Models/" + jmesh->GetName() + ".mdl");

        unsigned vertexElementMask = MASK_POSITION | MASK_NORMAL | MASK_TEXCOORD1;// | MASK_COLOR;

        if (jmesh->GetNumUVSets() == 2)
            vertexElementMask |= MASK_TEXCOORD2;

        PODVector<JSONMesh::BoneWeight>& boneWeights = jmesh->GetBoneWeights();

        if (boneWeights.Size())
            vertexElementMask |= (MASK_BLENDWEIGHTS | MASK_BLENDINDICES);

        // the model has to have 65535 or less indices to avoid 32 bit indices, check this
        SharedPtr<IndexBuffer> ib (new IndexBuffer(context_));
        SharedPtr<VertexBuffer> vb (new VertexBuffer(context_));

        // even though shadowing, need to "SetData()" on these below
        // could we SetData with the shadowed data?
        ib->SetShadowed(true);
        vb->SetShadowed(true);

        // Setup Vertex Data
        unsigned vertexCount = jmesh->GetVertexCount();

        vb->SetSize(vertexCount, vertexElementMask);

        BoundingBox bbox;

        PODVector<Vector3>& vpos =  jmesh->GetVertexPositions();
        PODVector<Vector3>& vnorm = jmesh->GetVertexNormals();
        PODVector<Vector4>& vtan = jmesh->GetVertexTangents();
        PODVector<Vector2>& uv0 = jmesh->GetUVSet(0);
        PODVector<Vector2>& uv1 = jmesh->GetUVSet(1);

        PODVector<float> vdata;

        unsigned vertexSize = 8;
        if (vertexElementMask & MASK_TEXCOORD2)
            vertexSize += 2;

        if (vertexElementMask & MASK_BLENDWEIGHTS)
            vertexSize += 5;

        vdata.Resize(vertexCount * vertexSize);

        // TODO: we should be identifying unique vertices
        float* v = &vdata[0];

        for (unsigned j = 0; j < vertexCount; j++)
        {
            if (j < vpos.Size())
            {
                *v = vpos[j].x_; v++;
                *v = vpos[j].y_; v++;
                *v = vpos[j].z_; v++;
            }
            else
            {
                *v = 0; v++;
                *v = 0; v++;
                *v = 0; v++;
            }

            if (j < vnorm.Size())
            {
                *v = vnorm[j].x_; v++;
                *v = vnorm[j].y_; v++;
                *v = vnorm[j].z_; v++;
            }
            else
            {
                *v = 0; v++;
                *v = 0; v++;
                *v = 0; v++;
            }

            if (j < uv0.Size())
            {
                *v = uv0[j].x_; v++;
                *v = -uv0[j].y_; v++;
            }
            else
            {
                *v = 0; v++;
                *v = 0; v++;
            }

            if (vertexElementMask &  MASK_TEXCOORD2)
            {
                if (j < uv1.Size())
                {
                    *v = uv1[j].x_; v++;
                    *v = -uv1[j].y_; v++;
                }
                else
                {
                    *v = 0; v++;
                    *v = 0; v++;
                }
            }

            if (vertexElementMask & MASK_BLENDWEIGHTS)
            {
                JSONMesh::BoneWeight& boneWeight = boneWeights[j];

                for (unsigned k = 0; k < 4; ++k)
                {
                    *v = boneWeight.weights_[k]; v++;
                }
            }
            if (vertexElementMask & MASK_BLENDINDICES)
            {
                JSONMesh::BoneWeight& boneWeight = boneWeights[j];

                unsigned char* destBytes = (unsigned char*)v;
                ++v;
                for (unsigned k = 0; k < 4; ++k)
                {
                    *destBytes++ = (unsigned char) boneWeight.indexes_[k];
                }
            }


            bbox.Merge(vpos[j]);

        }

        vb->SetData(&vdata[0]);

        unsigned numIndices = 0;
        PODVector<uint16_t> allIndices;

        unsigned numGeom = jmesh->GetSubMeshCount();
        for (unsigned j = 0; j < numGeom; j++)
        {
            PODVector<int>& submesh = jmesh->GetSubMesh(j);

            numIndices += submesh.Size();

            for (unsigned k = 0; k < submesh.Size(); k++)
            {
                allIndices.Push((uint16_t) submesh[k]);
            }
        }

        ib->SetSize(numIndices, false);

        ib->SetData(&allIndices[0]);

        Vector<SharedPtr<Geometry> > geometry;

        uint16_t start = 0;
        for (unsigned j = 0; j < numGeom; j++)
        {
            PODVector<int>& submesh = jmesh->GetSubMesh(j);

            SharedPtr<Geometry> geom(new Geometry(context_));
            geometry.Push(geom);

            geom->SetIndexBuffer(ib);
            geom->SetVertexBuffer(0, vb);
            geom->SetDrawRange(TRIANGLE_LIST, start, submesh.Size(), false);

            start += submesh.Size();
        }

        model->SetNumGeometries(geometry.Size());

        Vector<SharedPtr<VertexBuffer> > vbVector;
        Vector<SharedPtr<IndexBuffer> > ibVector;

        vbVector.Push(vb);
        ibVector.Push(ib);

        PODVector<unsigned> emptyMorphRange;
        model->SetVertexBuffers(vbVector, emptyMorphRange, emptyMorphRange);
        model->SetIndexBuffers(ibVector);

        Vector<JSONMesh::Bone>& jbones = jmesh->GetBones();

        // FIXME: we need to get the bounds or calculate propoer
        if (jbones.Size())
        {
            bbox.min_ *= 2;
            bbox.max_ *= 2;

        }

        // see fixme's for skeletal geo center/bounds
        for (unsigned j = 0; j < geometry.Size(); j++)
        {
            model->SetNumGeometryLodLevels(j, 1);
            model->SetGeometry(j, 0, geometry[j]);
            // this could be finer grained
            // also, fix this for skeletal
            model->SetGeometryCenter(j, jbones.Size() ? Vector3::ZERO : bbox.Center());//Vector3::ZERO);
        }

        // FIXME: skeletal bounds is off
        if (!jbones.Size())
            model->SetBoundingBox(bbox);
        else
            model->SetBoundingBox(BoundingBox(-1024, 1024));

        // Build skeleton if necessary
        if (jbones.Size())
        {
            Skeleton skeleton;
            Vector<Bone>& bones = skeleton.GetModifiableBones();

            unsigned rootindex = 0;

            const String& rootBoneName = jmesh->GetRootBone();

            for (unsigned j = 0; j < jbones.Size(); ++j)
            {
                JSONMesh::Bone& jbone = jbones[j];

                if (jbone.name_ == rootBoneName)
                {
                    rootindex = j;
                    break;
                }

            }

            for (unsigned j = 0; j < jbones.Size(); ++j)
            {
                JSONMesh::Bone& jbone = jbones[j];

                String boneName(jbone.name_);

                Bone newBone;
                newBone.name_ = boneName;
                newBone.initialPosition_ = jbone.pos_;
                newBone.initialRotation_ = jbone.rot_;
                newBone.initialScale_ = jbone.scale_;

                // Get offset information if exists
                newBone.offsetMatrix_ = Matrix3x4(jmesh->GetBindPoses()[j]);
                //newBone.radius_ = model.boneRadii_[i];
                //newBone.boundingBox_ = model.boneHitboxes_[i];
                //newBone.collisionMask_ = BONECOLLISION_SPHERE | BONECOLLISION_BOX;
                newBone.parentIndex_ = j;
                bones.Push(newBone);
            }

            // Set the bone hierarchy
            for (unsigned j = 0; j < jbones.Size(); ++j)
            {
                JSONMesh::Bone& jbone = jbones[j];

                String parentName = jbone.parentName_;

                if (!parentName.Length())
                {
                    bones[j].parentIndex_ = j;
                }
                else
                {
                    for (unsigned k = 0; k < bones.Size(); ++k)
                    {
                        if (bones[k].name_ == parentName)
                        {
                            bones[j].parentIndex_ = k;
                            break;
                        }
                    }

                }

            }

            skeleton.SetRootBoneIndex(rootindex);

            model->SetSkeleton(skeleton);
        }

        models_[jmesh->GetName()] = model;

    }

    return true;
}

bool JSONSceneProcess::ProcessComponent(Node* node, const JSONTransform* jtransform)
{
    node->SetPosition(jtransform->GetLocalPosition());
    node->SetRotation(jtransform->GetLocalRotation());
    node->SetScale(jtransform->GetLocalScale());

    return true;
}

bool JSONSceneProcess::ProcessComponent(Node* node, const JSONTimeOfDay* jtime )
{
    TimeOfDay* timeofday = node->CreateComponent<TimeOfDay>();
    timeofday->SetTimeOn(jtime->GetTimeOn());
    timeofday->SetTimeOff(jtime->GetTimeOff());
    return true;
}


bool JSONSceneProcess::ProcessComponent(Node* node, const JSONLight* jlight )
{
    if (!jlight->GetRealtime() || jlight->GetLightType() != "Point")
        return true;

    Light* light = node->CreateComponent<Light>();
    light->SetRange(jlight->GetRange());
    light->SetCastShadows(jlight->GetCastsShadows());
    light->SetColor(jlight->GetColor());
    light->SetLightType(LIGHT_POINT);

    return true;
}

bool JSONSceneProcess::ProcessComponent(Node* node, const JSONBoxCollider* jbox )
{
    CollisionShape* shape = node->CreateComponent<CollisionShape>();
    shape->SetBox(jbox->GetSize(), jbox->GetCenter());
    return true;
}

bool JSONSceneProcess::ProcessComponent(Node* node, const JSONMeshCollider* jmesh )
{
    // JSONMeshCollider should store the mesh, as this may not be on node
    CollisionShape* shape = node->CreateComponent<CollisionShape>();
    StaticModel* model = node->GetComponent<StaticModel>();

    if (!model || !model->GetModel())
    {
        ATOMIC_LOGWARNING("Missing model for MeshCollier");
        return true;
    }

    assert(model && model->GetModel());
    if (model)
        shape->SetTriangleMesh(model->GetModel());
    return true;
}

bool JSONSceneProcess::ProcessComponent(Node* node, const JSONRigidBody* jbody )
{
    RigidBody* body = node->CreateComponent<RigidBody>();
    body->SetMass(jbody->GetMass());
    return true;
}


bool JSONSceneProcess::ProcessComponent(Node* node, const JSONTerrain* jterrain)
{
    int heightmapHeight = jterrain->GetHeightMapHeight();
    int heightmapWidth = jterrain->GetHeightMapWidth();
    unsigned length;
    const float* heightmap = jterrain->GetHeightMap(length);

    const Vector3 heightmapScale = jterrain->GetHeightMapScale();
    const Vector3 heightmapSize = jterrain->GetHeightMapSize();

    PODVector<uint8_t> bytes;

    bytes.Resize(heightmapHeight * heightmapWidth * 4);

    uint8_t* out = &bytes[0];

    for (int y = 0; y < heightmapHeight; y++)
    {
        for (int x = 0; x < heightmapWidth; x++)
        {
            float h = heightmap[y * heightmapWidth + x];
            uint8_t hbyte = uint8_t(255.0 * h);
            *out++ = hbyte;
            *out++ = hbyte;
            *out++ = hbyte;
            *out++ = 255;
        }
    }

    SharedPtr<Image> image(new Image(context_));

    image->SetSize(heightmapWidth, heightmapHeight, 4);
    image->SetData(&bytes[0]);

    String heightMapPath = resourcePath_ + "Textures/TerrainHeightMap.png";
    image->SavePNG(heightMapPath);

    int alphamapWidth = jterrain->GetAlphaMapHeight();
    int alphamapHeight = jterrain->GetAlphaMapWidth();
    int alphamapLayers = jterrain->GetAlphaMapLayers();

    if (alphamapLayers > 3)
        alphamapLayers = 3;

    const float* alphamap = jterrain->GetAlphaMap(length);

    bytes.Resize( alphamapWidth * alphamapWidth * 4);

    memset (&bytes[0], 255, alphamapWidth * alphamapWidth * 4);

    for (int i = 0; i < alphamapLayers; i++)
    {
        uint8_t* out = &bytes[i];

        for (int y = 0; y < alphamapHeight; y++)
        {
            for (int x = 0; x < alphamapWidth; x++)
            {
                float w = alphamap[(i * (alphamapWidth * alphamapHeight)) + y * alphamapWidth + x ];
                uint8_t wbyte = uint8_t(255.0 * w);
                *out = wbyte;
                out += 4;
            }
        }
    }

    SharedPtr<Image> alphaWeights(new Image(context_));

    alphaWeights->SetSize(alphamapWidth, alphamapHeight, 4);
    alphaWeights->SetData(&bytes[0]);

    String alphaWeightsPath = resourcePath_ + "Textures/TerrainWeights.png";
    alphaWeights->SavePNG(alphaWeightsPath);

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Image* heightMap = cache->GetResource<Image>(heightMapPath);

    Material* material = cache->GetResource<Material>("Materials/DemoTerrain.material");

    Terrain* terrain = node->CreateComponent<Terrain>();

    terrain->SetHeightMap(heightMap);
    terrain->SetMaterial(material);
    terrain->SetSmoothing(true);

    //terrain->SetPatchSize(64);
    //terrain->SetSpacing(Vector3(2.0f, 0.5f, 2.0f)); // Spacing between vertices and vertical resolution of the height map
    //terrain->SetSmoothing(true);

    Vector3 spacing = heightmapScale;

    spacing.y_ = heightmapScale.y_ / 250.0f;

    terrain->SetSpacing(spacing);

    Vector3 pos = node->GetPosition();
    pos.x_ += heightmapSize.x_/2.0f;
    pos.y_  += .1f;
    pos.z_ += heightmapSize.z_/2.0f;

    node->SetPosition(pos);

    Quaternion rotation;
    rotation = node->GetRotation();

    rotation.FromEulerAngles(0, -90, 0);

    node->SetRotation(rotation);

    RigidBody* body = node->CreateComponent<RigidBody>();
    // use layer 2 for static
    body->SetCollisionLayer(2);
    body->SetFriction(1.0f);
    CollisionShape* shape = node->CreateComponent<CollisionShape>();
    shape->SetTerrain();

    return true;
}

bool JSONSceneProcess::ProcessComponent(Node* node, const JSONAnimation* janim )
{
    const Vector<JSONAnimation::AnimationClip*>& clips =  janim->GetClips();

    for (unsigned i = 0; i < clips.Size(); i++)
    {
        const JSONAnimation::AnimationClip* clip = clips[i];

        SharedPtr<Animation> outAnim(new Animation(context_));
        outAnim->SetAnimationName(clip->name_);
        outAnim->SetLength(clip->GetDuration());

        Vector<AnimationTrack> tracks;

        for (unsigned j = 0; j < clip->nodes_.Size(); ++j)
        {
            const JSONAnimation::AnimationNode* node = clip->nodes_[j];

            String channelName = node->name_;

            if (channelName.Contains('/'))
            {
                channelName = channelName.Split('/').Back();
            }

            AnimationTrack track;
            track.name_ = channelName;
            track.nameHash_ = channelName;
            // TODO: optimize channels
            track.channelMask_ |= (CHANNEL_POSITION | CHANNEL_ROTATION | CHANNEL_SCALE);

            //assert(node->keyframes_.Size());

            for (unsigned k = 0; k < node->keyframes_.Size(); k++)
            {
                const JSONAnimation::Keyframe* keyframe = node->keyframes_[k];

                AnimationKeyFrame key;

                key.time_ = keyframe->time_;
                key.position_ = keyframe->pos_;
                key.rotation_ = keyframe->rot_;
                key.scale_ = keyframe->scale_;

                track.keyFrames_.Push(key);

            }

            tracks.Push(track);

        }

        outAnim->SetTracks(tracks);

        String filename = resourcePath_;

        filename.AppendWithFormat("Models/FIXME_ANIMATION_PROCESSNAME_%s.ani", clip->name_.CString());

        File outFile(context_);
        if (!outFile.Open(filename, FILE_WRITE))
            ErrorExit("Could not open output file for animation");
        outAnim->Save(outFile);

    }


    return true;


}

bool JSONSceneProcess::ProcessComponent(Node* node, const JSONMeshRenderer* jmeshrenderer )
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    StaticModel* staticModel = NULL;
    int lightmapIndex = jmeshrenderer->GetLightmapIndex();

    if (lightmapIndex >= 0)
    {
        LMStaticModel* lmstatic = node->CreateComponent<LMStaticModel>();
        staticModel = lmstatic;

        const Vector4& tilingOffset = jmeshrenderer->GetLightmapTilingOffset();
        lmstatic->lightmapTilingOffset_ = tilingOffset;

        lmstatic->lightmapTilingOffset_.w_ = -lmstatic->lightmapTilingOffset_.w_;

        String lightmapName;

        lightmapName.AppendWithFormat("Textures/%s_Lightmap_%i.png", importer_->GetSceneName().CString(), lightmapIndex);

        Texture2D* texture = cache->GetResource<Texture2D>(lightmapName);
        assert(texture);
        lmstatic->SetLightmapTexure(texture);
    }
    else
        staticModel = node->CreateComponent<StaticModel>();

    const JSONMesh* mesh = jmeshrenderer->GetMesh();

    assert(models_.Contains(mesh->GetName()));

    Model* model = models_[mesh->GetName()];

    staticModel->SetModel(model);

    for (unsigned i = 0; i < jmeshrenderer->GetNumMaterials(); i++)
    {
        const JSONMaterial* jmat = jmeshrenderer->GetMaterial(i);

        assert(materials_.Contains(jmat->GetName()));

        staticModel->SetMaterial(i, materials_[jmat->GetName()]);

    }

    staticModel->SetCastShadows(jmeshrenderer->GetCastShadows());

    /*
    CollisionShape* shape = node->CreateComponent<CollisionShape>();
    shape->SetTriangleMesh(model, 0);
    node->CreateComponent<RigidBody>();
    */

    BoundingBox bbox = model->GetBoundingBox();
    bbox.Transform(node->GetWorldTransform());

    Vector3 size = bbox.Size();
    //staticModel->SetDrawDistance(size.Length() * 30.0f);
    //staticModel->SetShadowDistance(size.Length() * 10.0f);

    return true;
}

bool JSONSceneProcess::ProcessComponent(Node* node, const JSONSkinnedMeshRenderer* jmeshrenderer )
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    AnimatedModel* animatedModel =  node->CreateComponent<AnimatedModel>();

    const JSONMesh* mesh = jmeshrenderer->GetMesh();

    assert(models_.Contains(mesh->GetName()));

    Model* model = models_[mesh->GetName()];

    animatedModel->SetModel(model);

    for (unsigned i = 0; i < jmeshrenderer->GetNumMaterials(); i++)
    {
        const JSONMaterial* jmat = jmeshrenderer->GetMaterial(i);

        assert(materials_.Contains(jmat->GetName()));

        animatedModel->SetMaterial(i, materials_[jmat->GetName()]);

    }

    animatedModel->SetCastShadows(jmeshrenderer->GetCastShadows());

    return true;
}

Node* JSONSceneProcess::ProcessNode(const JSONNode* jnode, Node* parent)
{
    Node* node = parent->CreateChild(jnode->GetName());

    const PODVector<JSONComponent*>& components = jnode->GetComponents();

    for (unsigned i = 0; i < components.Size(); i++)
    {
        const JSONComponent* c = components.At(i);

        if (c->GetType() == "Transform")
        {
            ProcessComponent(node, (const JSONTransform*) c);
        }
        else if (c->GetType() == "MeshRenderer")
        {
            ProcessComponent(node, (const JSONMeshRenderer*) c);
        }
        else if (c->GetType() == "SkinnedMeshRenderer")
        {
            ProcessComponent(node, (const JSONSkinnedMeshRenderer*) c);
        }
        else if (c->GetType() == "Terrain")
        {
            ProcessComponent(node, (const JSONTerrain*) c);
        }
        else if (c->GetType() == "Animation")
        {
            ProcessComponent(node, (const JSONAnimation*) c);
        }
        else if (c->GetType() == "BoxCollider")
        {
            ProcessComponent(node, (const JSONBoxCollider*) c);
        }
        else if (c->GetType() == "MeshCollider")
        {
            ProcessComponent(node, (const JSONMeshCollider*) c);
        }
        else if (c->GetType() == "RigidBody")
        {
            ProcessComponent(node, (const JSONRigidBody*) c);
        }
        else if (c->GetType() == "Light")
        {
            ProcessComponent(node, (const JSONLight*) c);
        }
        else if (c->GetType() == "TimeOfDay")
        {
            ProcessComponent(node, (const JSONTimeOfDay*) c);
        }
    }

    if (node->GetComponent<CollisionShape>() && !node->GetComponent<RigidBody>())
    {
        RigidBody* body = node->CreateComponent<RigidBody>();
        body->SetCollisionLayer(2);

    }

    const PODVector<JSONNode*>& children = jnode->GetChildren();

    for (unsigned i = 0; i < children.Size(); i++)
    {
        node->AddChild(ProcessNode(children.At(i), node));
    }

    return node;
}

bool JSONSceneProcess::ProcessHierarchy(Scene* scene)
{
    const PODVector<JSONNode*>& hierarchy = importer_->GetHierarchy();

    for (unsigned i = 0; i < hierarchy.Size(); i++)
    {
        hierarchy_.Push(SharedPtr<Node>(ProcessNode(hierarchy[i], scene)));
    }

    return true;
}

bool JSONSceneProcess::Process(const String &resourcePath)
{
    resourcePath_ = resourcePath;
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    cache->AddResourceDir(resourcePath_);
    scene_ = new Scene(context_);
    scene_->CreateComponent<PhysicsWorld>();
    scene_->CreateComponent<Octree>();
    scene_->CreateComponent<DebugRenderer>();

    Node* zoneNode = scene_->CreateChild("Zone");
    Zone* zone = zoneNode->CreateComponent<Zone>();
    zone->SetBoundingBox(BoundingBox(-10000.0f, 10000.f));
    zone->SetAmbientColor(Color(.4f, .4f, .4f));

    ProcessTextures();
    ProcessLightmaps();
    ProcessMaterials();
    ProcessModels();
    ProcessHierarchy(scene_);

    return true;
}

bool JSONSceneProcess::Write()
{
    FileSystem* fs = GetSubsystem<FileSystem>();

    if (!fs->DirExists(resourcePath_ + "Scenes"))
        fs->CreateDir(resourcePath_ + "Scenes");
    if (!fs->DirExists(resourcePath_ + "Models"))
        fs->CreateDir(resourcePath_ + "Models");
    if (!fs->DirExists(resourcePath_ + "Materials"))
        fs->CreateDir(resourcePath_ + "Materials");
    if (!fs->DirExists(resourcePath_ + "Textures"))
        fs->CreateDir(resourcePath_ + "Textures");

    WriteMaterials();
    WriteModels();
    WriteHierarchy(scene_);

    return true;
}


}
