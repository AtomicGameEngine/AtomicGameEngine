// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include "AEImportJSON.h"

using namespace rapidjson;

namespace AtomicEditor
{

static unsigned char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static int _base64Decode(const unsigned char *input, unsigned int input_len, unsigned char *output, unsigned int *output_len )
{
    static char inalphabet[256], decoder[256];
    int i, bits, c = 0, char_count, errors = 0;
    unsigned int input_idx = 0;
    unsigned int output_idx = 0;

    for (i = (sizeof alphabet) - 1; i >= 0 ; i--) {
        inalphabet[alphabet[i]] = 1;
        decoder[alphabet[i]] = i;
    }

    char_count = 0;
    bits = 0;
    for( input_idx=0; input_idx < input_len ; input_idx++ ) {
        c = input[ input_idx ];
        if (c == '=')
            break;
        if (c > 255 || ! inalphabet[c])
            continue;
        bits += decoder[c];
        char_count++;
        if (char_count == 4) {
            output[ output_idx++ ] = (bits >> 16);
            output[ output_idx++ ] = ((bits >> 8) & 0xff);
            output[ output_idx++ ] = ( bits & 0xff);
            bits = 0;
            char_count = 0;
        } else {
            bits <<= 6;
        }
    }

    if( c == '=' ) {
        switch (char_count) {
        case 1:
            errors++;
            break;
        case 2:
            output[ output_idx++ ] = ( bits >> 10 );
            break;
        case 3:
            output[ output_idx++ ] = ( bits >> 16 );
            output[ output_idx++ ] = (( bits >> 8 ) & 0xff);
            break;
        }
    } else if ( input_idx < input_len ) {
        if (char_count) {
            errors++;
        }
    }

    *output_len = output_idx;
    return errors;
}

bool JSONComponent::Parse(const rapidjson::Value& value)
{
    for (Value::ConstMemberIterator oitr = value.MemberBegin();
         oitr != value.MemberEnd(); ++oitr)
    {
        if (!strcmp(oitr->name.GetString(), "enabled"))
        {
            enabled_ = oitr->value.IsTrue();
        }
    }

    return true;

}

JSONTransform::JSONTransform(JSONSceneImporter* importer, const rapidjson::Value& value) :
    JSONComponent(importer, "Transform")
{

    JSONComponent::Parse(value);

    for (Value::ConstMemberIterator oitr = value.MemberBegin();
         oitr != value.MemberEnd(); ++oitr)
    {
        if (!strcmp(oitr->name.GetString(), "localPosition"))
        {
            importer->ReadVector3FromArray(oitr->value, localPosition_);
        }
        else if (!strcmp(oitr->name.GetString(), "localScale"))
        {
            importer->ReadVector3FromArray(oitr->value, localScale_);
        }
        else if (!strcmp(oitr->name.GetString(), "localRotation"))
        {
            importer->ReadQuaternionFromArray(oitr->value, localRotation_);
        }
    }
}


JSONMeshRenderer::JSONMeshRenderer(JSONSceneImporter* importer, const rapidjson::Value& value, const char* type) :
    JSONComponent(importer, type)
  , mesh_(0)
  , castShadows_(false)
  , receiveShadows_(false)
  , lightmapIndex_(-1)
{
    JSONComponent::Parse(value);

    for (Value::ConstMemberIterator oitr = value.MemberBegin();
         oitr != value.MemberEnd(); ++oitr)
    {
        if (!strcmp(oitr->name.GetString(), "mesh"))
        {
            mesh_ = importer->GetMesh(oitr->value.GetString());
        }
        else if (!strcmp(oitr->name.GetString(), "castShadows"))
        {
            castShadows_ = oitr->value.IsTrue();
        }
        else if (!strcmp(oitr->name.GetString(), "receiveShadows"))
        {
            receiveShadows_ = oitr->value.IsTrue();
        }
        else if (!strcmp(oitr->name.GetString(), "lightmapIndex"))
        {
            lightmapIndex_ = oitr->value.GetInt();
        }
        else if (!strcmp(oitr->name.GetString(), "lightmapTilingOffset"))
        {
            importer->ReadVector4FromArray(oitr->value, lightmapTilingOffset_);
        }
        else if (!strcmp(oitr->name.GetString(), "materials"))
        {
            for (Value::ConstValueIterator mitr = oitr->value.Begin(); mitr != oitr->value.End(); mitr++)
            {
                JSONMaterial* material = importer->GetMaterial((*mitr).GetString());
                assert(material);
                materials_.Push(material);
            }

        }
    }
}

JSONSkinnedMeshRenderer::JSONSkinnedMeshRenderer(JSONSceneImporter* importer, const rapidjson::Value& value) :
    JSONMeshRenderer(importer, value, "SkinnedMeshRenderer")
{

}

JSONAnimation::JSONAnimation(JSONSceneImporter* importer, const rapidjson::Value& value) :
    JSONComponent(importer, "Animation")
{
    JSONComponent::Parse(value);

    const Value::Member* jclips = value.FindMember("clips");

    assert(jclips);

    for (Value::ConstValueIterator clipitr = jclips->value.Begin(); clipitr != jclips->value.End(); clipitr++)
    {
        const Value::Member* clipname = clipitr->FindMember("name");
        const Value::Member* clipnodes = clipitr->FindMember("nodes");

        AnimationClip* aclip = new AnimationClip();
        aclip->name_ = clipname->value.GetString();

        for (Value::ConstValueIterator nodeitr = clipnodes->value.Begin(); nodeitr != clipnodes->value.End(); nodeitr++)
        {
            AnimationNode* node = new AnimationNode();
            const Value::Member* nodename = nodeitr->FindMember("name");
            const Value::Member* keyframes = nodeitr->FindMember("keyframes");

            node->name_ = nodename->value.GetString();

            for (Value::ConstValueIterator keyitr = keyframes->value.Begin(); keyitr != keyframes->value.End(); keyitr++)
            {
                Keyframe* keyframe = new Keyframe();

                const Value::Member* jpos = keyitr->FindMember("pos");
                const Value::Member* jscale = keyitr->FindMember("scale");
                const Value::Member* jrot = keyitr->FindMember("rot");
                const Value::Member* jtime = keyitr->FindMember("time");

                keyframe->time_ = (float) jtime->value.GetDouble();
                importer_->ReadVector3FromArray(jpos->value, keyframe->pos_);
                importer_->ReadVector3FromArray(jscale->value, keyframe->scale_);
                importer_->ReadQuaternionFromArray(jrot->value, keyframe->rot_);

                node->keyframes_.Push(keyframe);
            }

            aclip->nodes_.Push(node);

        }

        clips_.Push(aclip);

    }

}

JSONTimeOfDay::JSONTimeOfDay(JSONSceneImporter* importer, const rapidjson::Value& value) :
    JSONComponent(importer, "TimeOfDay"),
    timeOn_(0),
    timeOff_(0)
{
    JSONComponent::Parse(value);

    const Value::Member* jtimeOn = value.FindMember("timeOn");
    if (jtimeOn)
        timeOn_ = (float) jtimeOn->value.GetDouble();

    const Value::Member* jtimeOff = value.FindMember("timeOff");
    if (jtimeOff)
        timeOff_ = (float) jtimeOff->value.GetDouble();
}

JSONLight::JSONLight(JSONSceneImporter* importer, const rapidjson::Value& value) :
    JSONComponent(importer, "Light"),
    lightType_("Point"),
    range_(10),
    color_(1, 1, 1, 1),
    castsShadows_(false),
    realtime_(false)
{
    JSONComponent::Parse(value);

    const Value::Member* jlightType = value.FindMember("lightType");
    if (jlightType)
        lightType_ = jlightType->value.GetString();

    const Value::Member* jrange = value.FindMember("range");
    if (jrange)
        range_ = (float) jrange->value.GetDouble();

    const Value::Member* jcolor = value.FindMember("color");
    if (jcolor)
        importer->ReadColorFromArray(jcolor->value, color_);

    const Value::Member* jcastsShadows = value.FindMember("castsShadows");
    if (jcastsShadows)
        castsShadows_ = jcastsShadows->value.GetBool();

    const Value::Member* jrealtime = value.FindMember("realtime");
    if (jrealtime)
        realtime_ = jrealtime->value.GetBool();
}


JSONRigidBody::JSONRigidBody(JSONSceneImporter* importer, const rapidjson::Value& value) :
    JSONComponent(importer, "RigidBody"),
    mass_(0)
{
    JSONComponent::Parse(value);

    const Value::Member* jmass = value.FindMember("mass");
    if (jmass)
        mass_ = (float) jmass->value.GetDouble();
}


JSONMeshCollider::JSONMeshCollider(JSONSceneImporter* importer, const rapidjson::Value& value) :
    JSONComponent(importer, "MeshCollider")
{
    JSONComponent::Parse(value);

}

JSONBoxCollider::JSONBoxCollider(JSONSceneImporter* importer, const rapidjson::Value& value) :
    JSONComponent(importer, "BoxCollider")
{
    JSONComponent::Parse(value);

    const Value::Member* jcenter= value.FindMember("center");
    if (jcenter)
        importer->ReadVector3FromArray(jcenter->value, center_);

    const Value::Member* jsize= value.FindMember("size");
    if (jsize)
        importer->ReadVector3FromArray(jsize->value, size_);
}

JSONTerrain::JSONTerrain(JSONSceneImporter* importer, const rapidjson::Value& value) :
    JSONComponent(importer, "Terrain")
  , heightmapHeight_(0)
  , heightmapWidth_(0)
  , heightmapResolution_(0)
  , alphamapWidth_(0)
  , alphamapHeight_(0)
  , alphamapLayers_(0)
  , alphaMapLength_(0)
  , heightMapLength_(0)
{
    JSONComponent::Parse(value);

    String base64Height;
    String base64Alpha;

    for (Value::ConstMemberIterator oitr = value.MemberBegin();
         oitr != value.MemberEnd(); ++oitr)
    {
        if (!strcmp(oitr->name.GetString(), "heightmapHeight"))
        {
            heightmapHeight_ = oitr->value.GetInt();
        }
        else if (!strcmp(oitr->name.GetString(), "heightmapWidth"))
        {
            heightmapWidth_ = oitr->value.GetInt();
        }
        else if (!strcmp(oitr->name.GetString(), "heightmapResolution"))
        {
            heightmapResolution_ = oitr->value.GetInt();
        }
        else if (!strcmp(oitr->name.GetString(), "alphamapWidth"))
        {
            alphamapWidth_ = oitr->value.GetInt();
        }
        else if (!strcmp(oitr->name.GetString(), "alphamapHeight"))
        {
            alphamapHeight_ = oitr->value.GetInt();
        }
        else if (!strcmp(oitr->name.GetString(), "alphamapLayers"))
        {
            alphamapLayers_ = oitr->value.GetInt();
        }
        else if (!strcmp(oitr->name.GetString(), "base64HeightLength"))
        {
            heightMapLength_ = oitr->value.GetInt();
        }
        else if (!strcmp(oitr->name.GetString(), "base64AlphaLength"))
        {
            alphaMapLength_ = oitr->value.GetInt();
        }
        else if (!strcmp(oitr->name.GetString(), "base64Height"))
        {
            base64Height = oitr->value.GetString();
        }
        else if (!strcmp(oitr->name.GetString(), "base64Alpha"))
        {
            base64Alpha = oitr->value.GetString();
        }
        else if (!strcmp(oitr->name.GetString(), "heightmapScale"))
        {
            importer->ReadVector3FromArray(oitr->value, heightmapScale_);
        }
        else if (!strcmp(oitr->name.GetString(), "size"))
        {
            importer->ReadVector3FromArray(oitr->value, size_);
        }

    }

    heightMap_ = new float[heightMapLength_];
    alphaMap_ = new float[alphaMapLength_];

    unsigned int length = 0;
    int errors;

    errors = _base64Decode((const unsigned char*)base64Height.CString(), base64Height.Length(), (unsigned char*) heightMap_.Get(), &length);

    assert(!errors);
    assert(length == heightMapLength_);

    length = 0;

    errors = _base64Decode((const unsigned char*)base64Alpha.CString(), base64Alpha.Length(),  (unsigned char*) alphaMap_.Get(), &length);

    assert(!errors);
    assert(length == alphaMapLength_);

}

JSONCamera::JSONCamera(JSONSceneImporter* importer, const rapidjson::Value& value) :
    JSONComponent(importer, "Camera")
{
    JSONComponent::Parse(value);
}

JSONNode::JSONNode(JSONSceneImporter* importer, const rapidjson::Value& value) : importer_(importer)

{
    for (Value::ConstMemberIterator oitr = value.MemberBegin();
         oitr != value.MemberEnd(); ++oitr)
    {
        if (!strcmp(oitr->name.GetString(), "name"))
        {
            name_ = oitr->value.GetString();
        }
        else if (!strcmp(oitr->name.GetString(), "children"))
        {
            for (Value::ConstValueIterator citr = oitr->value.Begin(); citr != oitr->value.End(); citr++)
            {
                if (!(*citr).IsObject())
                    continue;

                AddChild(new JSONNode(importer, *citr));
            }
        }
        else if (!strcmp(oitr->name.GetString(), "components"))
        {
            for (Value::ConstValueIterator citr = oitr->value.Begin(); citr != oitr->value.End(); citr++)
            {
                if (!(*citr).IsObject())
                    continue;

                const Value::Member* jtype = citr->FindMember("type");

                if (!jtype)
                    continue;

                String type = jtype->value.GetString();

                if (type == "Transform")
                {
                    components_.Push(new JSONTransform(importer_, *citr));
                }
                else if (type == "MeshRenderer")
                {
                    components_.Push(new JSONMeshRenderer(importer_, *citr));
                }
                else if (type == "SkinnedMeshRenderer")
                {
                    components_.Push(new JSONSkinnedMeshRenderer(importer_, *citr));
                }
                else if (type == "Animation")
                {
                    components_.Push(new JSONAnimation(importer_, *citr));
                }
                else if (type == "Camera")
                {
                    components_.Push(new JSONCamera(importer_, *citr));
                }
                else if (type == "Terrain")
                {
                    components_.Push(new JSONTerrain(importer_, *citr));
                }
                else if (type == "RigidBody")
                {
                    components_.Push(new JSONRigidBody(importer_, *citr));
                }
                else if (type == "MeshCollider")
                {
                    components_.Push(new JSONMeshCollider(importer_, *citr));
                }
                else if (type == "BoxCollider")
                {
                    components_.Push(new JSONBoxCollider(importer_, *citr));
                }
                else if (type == "Light")
                {
                    components_.Push(new JSONLight(importer_, *citr));
                }
                else if (type == "TimeOfDay")
                {
                    components_.Push(new JSONTimeOfDay(importer_, *citr));
                }

            }
        }
    }
}

JSONSceneImporter::JSONSceneImporter(Context* context) : Importer(context)
  , document_(new Document())

{

}

void JSONSceneImporter::ReadColorFromArray(const rapidjson::Value& value, Color& color)
{
    if (!value.IsArray() || (value.Size() < 3 || value.Size() > 4))
        return;

    color.r_ = color.g_ = color.b_ = color.a_ = 1.0f;

    color.r_ = (float) value[SizeType(0)].GetDouble();
    color.g_ = (float) value[SizeType(1)].GetDouble();
    color.b_ = (float) value[SizeType(2)].GetDouble();
    if (value.Size() ==  4)
        color.a_ = (float) value[SizeType(3)].GetDouble();

}

void JSONSceneImporter::ReadVector2FromArray(const rapidjson::Value& value, Vector2& v)
{
    if (!value.IsArray() || value.Size() != 2)
        return;

    v.x_ = (float) value[SizeType(0)].GetDouble();
    v.y_ = (float) value[SizeType(1)].GetDouble();

}

void JSONSceneImporter::ReadVector3FromArray(const rapidjson::Value& value, Vector3& v)
{
    if (!value.IsArray() || value.Size() != 3)
        return;

    v.x_ = (float) value[SizeType(0)].GetDouble();
    v.y_ = (float) value[SizeType(1)].GetDouble();
    v.z_ = (float) value[SizeType(2)].GetDouble();
}

void JSONSceneImporter::ReadVector4FromArray(const rapidjson::Value& value, Vector4& v)
{
    if (!value.IsArray() || value.Size() != 4)
        return;

    v.x_ = (float) value[SizeType(0)].GetDouble();
    v.y_ = (float) value[SizeType(1)].GetDouble();
    v.z_ = (float) value[SizeType(2)].GetDouble();
    v.w_ = (float) value[SizeType(3)].GetDouble();
}

void JSONSceneImporter::ReadQuaternionFromArray(const rapidjson::Value& value, Quaternion& q)
{
    if (!value.IsArray() || value.Size() != 4)
        return;

    q.x_ = (float) value[SizeType(0)].GetDouble();
    q.y_ = (float) value[SizeType(1)].GetDouble();
    q.z_ = (float) value[SizeType(2)].GetDouble();
    q.w_ = (float) value[SizeType(3)].GetDouble();

}

void JSONSceneImporter::ReadMatrix4FromArray(const rapidjson::Value& value, Matrix4& m)
{
    if (!value.IsArray() || value.Size() != 16)
        return;

    float mf[16];
    for (SizeType i = 0; i < 16; i++)
        mf[i] = (float) value[i].GetDouble();

    m = Matrix4(mf);

}


bool JSONSceneImporter::ParseMaterials(const rapidjson::Value& value)
{
    const Value::Member* jmaterials = value.FindMember("materials");
    if (jmaterials && jmaterials->value.IsArray())
    {
        for (Value::ConstValueIterator itr = jmaterials->value.Begin(); itr != jmaterials->value.End(); itr++)
        {
            if ((*itr).IsObject())
            {
                String name = "Anonymous Material";
                String shader;
                String mainTexture;
                Vector2 mainTextureOffset(0.0f, 0.0f);
                Vector2 mainTextureScale(1.0f, 1.0f);
                int passCount = 1;
                int renderQueue = 0;
                Color color(1, 1, 1, 1);

                for (Value::ConstMemberIterator oitr = (*itr).MemberBegin();
                     oitr != (*itr).MemberEnd(); ++oitr)
                {

                    if (!strcmp(oitr->name.GetString(), "name"))
                    {
                        name = oitr->value.GetString();
                    }
                    else if (!strcmp(oitr->name.GetString(), "shader"))
                    {
                        shader = oitr->value.GetString();
                    }
                    else if (!strcmp(oitr->name.GetString(), "mainTexture"))
                    {
                        mainTexture = oitr->value.GetString();
                    }
                    else if (!strcmp(oitr->name.GetString(), "mainTextureOffset"))
                    {
                        ReadVector2FromArray(oitr->value, mainTextureOffset);
                    }
                    else if (!strcmp(oitr->name.GetString(), "mainTextureScale"))
                    {
                        ReadVector2FromArray(oitr->value, mainTextureScale);
                    }
                    else if (!strcmp(oitr->name.GetString(), "renderQueue"))
                    {
                        renderQueue = oitr->value.GetInt();
                    }
                    else if (!strcmp(oitr->name.GetString(), "passCount"))
                    {
                        passCount = oitr->value.GetInt();
                    }
                    else if (!strcmp(oitr->name.GetString(), "color"))
                    {
                        ReadColorFromArray(oitr->value, color);
                    }
                }

                JSONMaterial* material = new JSONMaterial(name);

                material->SetColor(color);
                material->SetMainTexture(mainTexture);
                material->SetMainTextureOffset(mainTextureOffset);
                material->SetMainTextureScale(mainTextureScale);
                material->SetPassCount(passCount);
                material->SetRenderQueue(renderQueue);
                material->SetShader(shader);

                AddMaterial(material);
            }
        }
    }

    return true;
}

bool JSONSceneImporter::ParseMeshes(const rapidjson::Value& value)
{
    const Value::Member* jmeshes = value.FindMember("meshes");
    if (jmeshes && jmeshes->value.IsArray())
    {
        for (Value::ConstValueIterator itr = jmeshes->value.Begin(); itr != jmeshes->value.End(); itr++)
        {
            if ((*itr).IsObject())
            {
                JSONMesh* mesh = new JSONMesh("Anonymous Mesh");

                for (Value::ConstMemberIterator oitr = (*itr).MemberBegin();
                     oitr != (*itr).MemberEnd(); ++oitr)
                {

                    if (!strcmp(oitr->name.GetString(), "name"))
                    {
                        mesh->SetName(oitr->value.GetString());
                    }
                    else if (!strcmp(oitr->name.GetString(), "triangles"))
                    {
                        if (oitr->value.IsArray())
                        {
                            for (Value::ConstValueIterator triangleArrayItr = oitr->value.Begin();
                                 triangleArrayItr != oitr->value.End(); triangleArrayItr++)
                            {
                                PODVector<int>& triangles = mesh->AddSubMesh();

                                for (Value::ConstValueIterator vertexItr = triangleArrayItr->Begin();
                                     vertexItr != triangleArrayItr->End(); vertexItr++)
                                {
                                    triangles.Push((*vertexItr).GetInt());
                                }

                            }
                        }
                    }
                    else if (!strcmp(oitr->name.GetString(), "vertexPositions"))
                    {
                        Vector3 pos;
                        PODVector<Vector3>& vpos = mesh->GetVertexPositions();
                        for (Value::ConstValueIterator vertexItr = oitr->value.Begin();
                             vertexItr != oitr->value.End(); vertexItr++)
                        {

                            ReadVector3FromArray(*vertexItr, pos);
                            vpos.Push(pos);
                        }

                    }
                    else if (!strcmp(oitr->name.GetString(), "vertexNormals"))
                    {
                        Vector3 pos;
                        PODVector<Vector3>& vnorm = mesh->GetVertexNormals();
                        for (Value::ConstValueIterator vertexItr = oitr->value.Begin();
                             vertexItr != oitr->value.End(); vertexItr++)
                        {

                            ReadVector3FromArray(*vertexItr, pos);
                            vnorm.Push(pos);
                        }

                    }
                    else if (!strcmp(oitr->name.GetString(), "vertexTangents"))
                    {
                        Vector4 tangent;
                        PODVector<Vector4>& vtangents = mesh->GetVertexTangents();
                        for (Value::ConstValueIterator vertexItr = oitr->value.Begin();
                             vertexItr != oitr->value.End(); vertexItr++)
                        {

                            ReadVector4FromArray(*vertexItr, tangent);
                            vtangents.Push(tangent);
                        }

                    }
                    else if (!strcmp(oitr->name.GetString(), "boneWeights"))
                    {
                        PODVector<JSONMesh::BoneWeight>& boneWeights = mesh->GetBoneWeights();

                        for (Value::ConstValueIterator bitr = oitr->value.Begin();
                             bitr != oitr->value.End(); bitr++)
                        {
                            JSONMesh::BoneWeight bw;
                            const Value::Member* indexes = bitr->FindMember("indexes");
                            const Value::Member* weights = bitr->FindMember("weights");

                            for (int i = 0; i < 4; i++)
                            {
                                bw.indexes_[i] = indexes->value[SizeType(i)].GetInt();
                                bw.weights_[i] = (float) weights->value[SizeType(i)].GetDouble();
                            }

                            boneWeights.Push(bw);

                        }

                    }
                    else if (!strcmp(oitr->name.GetString(), "rootBone"))
                    {
                        mesh->SetRootBone(oitr->value.GetString());
                    }
                    else if (!strcmp(oitr->name.GetString(), "bindPoses"))
                    {
                        Vector<Matrix4>& bindPoses = mesh->GetBindPoses();

                        for (Value::ConstValueIterator bitr = oitr->value.Begin();
                             bitr != oitr->value.End(); bitr++)
                        {
                            Matrix4 m;
                            ReadMatrix4FromArray(*bitr, m);
                            bindPoses.Push(m);
                        }

                    }
                    else if (!strcmp(oitr->name.GetString(), "bones"))
                    {
                        Vector<JSONMesh::Bone>& bones = mesh->GetBones();

                        if (!oitr->value.IsNull())
                        {
                            for (Value::ConstValueIterator bitr = oitr->value.Begin();
                                 bitr != oitr->value.End(); bitr++)
                            {
                                const Value::Member* pos = bitr->FindMember("localPosition");
                                const Value::Member* rot = bitr->FindMember("localRotation");
                                const Value::Member* scale = bitr->FindMember("localScale");
                                const Value::Member* name = bitr->FindMember("name");
                                const Value::Member* parentName = bitr->FindMember("parentName");

                                JSONMesh::Bone bone;

                                ReadVector3FromArray(pos->value, bone.pos_);
                                ReadVector3FromArray(scale->value, bone.scale_);
                                ReadQuaternionFromArray(rot->value, bone.rot_);
                                bone.name_ = name->value.GetString();
                                bone.parentName_ = parentName->value.GetString();

                                bones.Push(bone);
                            }
                        }

                    }
                    else if (!strcmp(oitr->name.GetString(), "vertexUV") ||
                             !strcmp(oitr->name.GetString(), "vertexUV2"))
                    {
                        Vector2 uv;
                        PODVector<Vector2>& uvs = !strcmp(oitr->name.GetString(), "vertexUV") ?
                                    mesh->GetUVSet(0) : mesh->GetUVSet(1);

                        for (Value::ConstValueIterator uvItr = oitr->value.Begin();
                             uvItr != oitr->value.End(); uvItr++)
                        {

                            ReadVector2FromArray(*uvItr, uv);
                            uvs.Push(uv);
                        }

                    }

                }

                AddMesh(mesh);
            }
        }
    }

    return true;
}

bool JSONSceneImporter::ParseShaders(const rapidjson::Value& value)
{
    const Value::Member* jshaders = value.FindMember("shaders");
    if (jshaders && jshaders->value.IsArray())
    {
        for (Value::ConstValueIterator itr = jshaders->value.Begin(); itr != jshaders->value.End(); itr++)
        {
            if ((*itr).IsObject())
            {
                String name = "Anonymous Shader";
                int renderQueue = 0;

                for (Value::ConstMemberIterator oitr = (*itr).MemberBegin();
                     oitr != (*itr).MemberEnd(); ++oitr)
                {
                    if (!strcmp(oitr->name.GetString(), "name"))
                    {
                        name = oitr->value.GetString();
                    }
                    else if (!strcmp(oitr->name.GetString(), "renderQueue"))
                    {
                        renderQueue = oitr->value.GetInt();
                    }

                }

                AddShader(new JSONShader(name, renderQueue));
            }
        }
    }

    return true;
}

bool JSONSceneImporter::ParseTextures(const rapidjson::Value& value)
{
    const Value::Member* jtextures = value.FindMember("textures");
    if (jtextures && jtextures->value.IsArray())
    {
        for (Value::ConstValueIterator itr = jtextures->value.Begin(); itr != jtextures->value.End(); itr++)
        {
            String name;
            String base64PNG;
            int base64PNGLength = 0;

            if ((*itr).IsObject())
            {
                const Value::Member* jname = (*itr).FindMember("name");
                if (jname && jname->value.IsString())
                {
                    name = jname->value.GetString();
                }
                const Value::Member* jbase64PNG = (*itr).FindMember("base64PNG");
                if (jbase64PNG)
                {
                    base64PNG = jbase64PNG->value.GetString();
                }
                const Value::Member* jbase64PNGLength = (*itr).FindMember("base64PNGLength");
                if (jbase64PNGLength)
                {
                    base64PNGLength = jbase64PNGLength->value.GetInt();
                }

            }

            SharedArrayPtr<unsigned char> pngPixels;
            pngPixels = new unsigned char[base64PNGLength];

            unsigned int length = 0;
            int errors = _base64Decode((const unsigned char*)base64PNG.CString(), base64PNG.Length(), pngPixels.Get(), &length);

            assert(!errors);
            assert(length == base64PNGLength);

            JSONTexture* texture = new JSONTexture(name);
            texture->SetPNGPixels(pngPixels, base64PNGLength);
            AddTexture(texture);

        }
    }

    return true;
}

bool JSONSceneImporter::ParseLightmaps(const rapidjson::Value& value)
{
    const Value::Member* jlightmaps = value.FindMember("lightmaps");
    if (jlightmaps && jlightmaps->value.IsArray())
    {
        for (Value::ConstValueIterator itr = jlightmaps->value.Begin(); itr != jlightmaps->value.End(); itr++)
        {
            String name;
            String base64PNG;
            int base64PNGLength = 0;

            if ((*itr).IsObject())
            {
                const Value::Member* jname = (*itr).FindMember("filename");
                if (jname && jname->value.IsString())
                {
                    name = jname->value.GetString();
                }
                const Value::Member* jbase64PNG = (*itr).FindMember("base64PNG");
                if (jbase64PNG)
                {
                    base64PNG = jbase64PNG->value.GetString();
                }
                const Value::Member* jbase64PNGLength = (*itr).FindMember("base64PNGLength");
                if (jbase64PNGLength)
                {
                    base64PNGLength = jbase64PNGLength->value.GetInt();
                }

            }

            SharedArrayPtr<unsigned char> pngPixels;
            pngPixels = new unsigned char[base64PNGLength];

            unsigned int length = 0;
            int errors = _base64Decode((const unsigned char*)base64PNG.CString(), base64PNG.Length(), pngPixels.Get(), &length);

            assert(!errors);
            assert(length == base64PNGLength);

            JSONLightmap* lightmap = new JSONLightmap(name);
            lightmap->SetPNGPixels(pngPixels, base64PNGLength);
            AddLightmap(lightmap);

        }
    }

    return true;
}

bool JSONSceneImporter::ParseResources(const rapidjson::Value& value)
{

    ParseTextures(value);
    ParseLightmaps(value);
    ParseShaders(value);
    ParseMaterials(value);
    ParseMeshes(value);

    return true;
}

bool JSONSceneImporter::ParseHierarchy(const rapidjson::Value& value)
{
    for (Value::ConstValueIterator itr = value.Begin(); itr != value.End(); itr++)
    {
        if ((*itr).IsObject())
        {
            hierarchy_.Push(new JSONNode(this, *itr));
        }
    }

    return true;
}

bool JSONSceneImporter::Import(const String& path)
{
    File jsonFile(context_, path);

    String json;

    jsonFile.ReadText(json);

    if (document_->Parse<0>(json.CString()).HasParseError())
    {
        LOGERRORF("Could not parse JSON data from %s", path.CString());
        return false;
    }
    const Value::Member* name = document_->FindMember("name");
    if (name)
        sceneName_ = name->value.GetString();

    const Value::Member* jresources = document_->FindMember("resources");
    if (jresources)
        ParseResources(jresources->value);

    const Value::Member* jhierarchy = document_->FindMember("hierarchy");
    if (jhierarchy)
        ParseHierarchy(jhierarchy->value);

    return true;
}

JSONSceneImporter::~JSONSceneImporter()
{
    if (document_)
        delete document_;
}

}
