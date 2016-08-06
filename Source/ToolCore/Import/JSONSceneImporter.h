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

#pragma once

#include <Atomic/Container/List.h>
#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace rapidjson
{
template<typename CharType> struct UTF8;
class CrtAllocator;
template <typename BaseAllocator> class MemoryPoolAllocator;
template <typename Encoding, typename Allocator> class GenericValue;
typedef GenericValue<UTF8<char>, MemoryPoolAllocator<CrtAllocator> > Value;
template <typename Encoding, typename Allocator> class GenericDocument;
typedef GenericDocument<UTF8<char>, MemoryPoolAllocator<CrtAllocator> > Document;
}


namespace ToolCore
{

class JSONSceneImporter;

class JSONResource
{
public:

    void SetName(const String& name)
    {
        name_ = name;
    }

    const String& GetName() const
    {
        return name_;
    }


protected:
    JSONResource(String name) : name_(name) {}

private:

    String name_;

};

class JSONTexture : public JSONResource
{
public:

    JSONTexture(const String& name) : JSONResource(name)
    {

    }

    unsigned char* GetPNGPixels(unsigned& length) const
    {
        length = length_;
        return pngPixels_.Get();
    }


    void SetPNGPixels(SharedArrayPtr<unsigned char>& pngPixels, unsigned length)
    {
        length_ = length;
        pngPixels_ = pngPixels;
    }


private:

    SharedArrayPtr<unsigned char> pngPixels_;
    unsigned length_;
};

class JSONLightmap : public JSONResource
{
public:

    JSONLightmap(const String& name) : JSONResource(name)
    {

    }

    unsigned char* GetPNGPixels(unsigned& length) const
    {
        length = length_;
        return pngPixels_.Get();
    }


    void SetPNGPixels(SharedArrayPtr<unsigned char>& pngPixels, unsigned length)
    {
        length_ = length;
        pngPixels_ = pngPixels;
    }


private:

    SharedArrayPtr<unsigned char> pngPixels_;
    unsigned length_;
};


class JSONShader: public JSONResource
{
public:

    JSONShader(const String& name, int renderQueue) :
        JSONResource(name), renderQueue_(renderQueue)
    {

    }

private:

    int renderQueue_;

};

class JSONMaterial : public JSONResource
{
public:

    JSONMaterial(const String& name) :
        JSONResource(name),
        mainTextureOffset_(0.0f, 0.0f),
        mainTextureScale_(1.0f, 1.0f),
        passCount_(1),
        color_(1, 1, 1, 1),
        renderQueue_(0)
    {

    }

    const String& GetShader() const
    {
        return shader_;
    }

    void SetShader(const String& shader)
    {
        shader_ = shader;
    }

    const String& GetMainTexture() const
    {
        return mainTexture_;
    }

    void SetMainTexture(const String& mainTexture)
    {
        mainTexture_ = mainTexture;
    }

    void SetMainTextureOffset(const Vector2& offset)
    {
        mainTextureOffset_ = offset;
    }

    void SetMainTextureScale(const Vector2& scale)
    {
        mainTextureScale_ = scale;
    }

    void SetColor(const Color& color)
    {
        color_ = color;
    }

    void SetPassCount(int count)
    {
        passCount_ = count;
    }

    void SetRenderQueue(int renderQueue)
    {
        renderQueue_ = renderQueue;
    }

private:

    String shader_;
    String mainTexture_;
    Vector2 mainTextureOffset_;
    Vector2 mainTextureScale_;
    int passCount_;
    Color color_;
    int renderQueue_;
    List<String> shaderKeywords_;
};


class JSONMesh : public JSONResource
{
public:

    struct BoneWeight
    {
        int indexes_[4];
        float weights_[4];
    };

    class Bone
    {
    public:
        Vector3 pos_;
        Vector3 scale_;
        Quaternion rot_;
        String name_;
        String parentName_;
    };

    JSONMesh(const String& name) :
        JSONResource(name)
    {

    }

    PODVector<int>& AddSubMesh()
    {
        triangles_.Resize(triangles_.Size() + 1);
        return triangles_.Back();
    }

    unsigned GetSubMeshCount()
    {
        return triangles_.Size();
    }

    PODVector<int>& GetSubMesh(unsigned index)
    {
        return triangles_.At(index);
    }

    unsigned GetVertexCount() const
    {
        return vertexPositions_.Size();
    }

    PODVector<Vector3>& GetVertexPositions()
    {
        return vertexPositions_;
    }

    PODVector<Vector3>& GetVertexNormals()
    {
        return vertexNormals_;
    }

    PODVector<Vector4>& GetVertexTangents()
    {
        return vertexTangents_;
    }

    unsigned GetNumUVSets() const
    {
        return vertexUV_.Size();
    }

    PODVector<Vector2>& GetUVSet(int idx)
    {
        while (vertexUV_.Size() <= idx)
        {
            AddUVSet();
        }

        return vertexUV_.At(idx);

    }

    PODVector<BoneWeight>& GetBoneWeights()
    {
        return boneWeights_;
    }

    Vector<Matrix4>& GetBindPoses()
    {
        return bindPoses_;
    }

    Vector<Bone>& GetBones()
    {
        return bones_;
    }

    const String& GetRootBone() const
    {
        return rootBone_;
    }

    void SetRootBone(const String& rootBone)
    {
        rootBone_ = rootBone;
    }

private:

    PODVector<Vector2>& AddUVSet()
    {
        vertexUV_.Resize(vertexUV_.Size() + 1);
        return vertexUV_.Back();
    }


    PODVector<Vector3> vertexPositions_;
    PODVector<Vector3> vertexNormals_;
    PODVector<Vector4> vertexTangents_;

    Vector<PODVector<Vector2> > vertexUV_;

    Vector<Matrix4> bindPoses_;
    Vector<Bone> bones_;
    PODVector<BoneWeight> boneWeights_;
    String rootBone_;

    //broken into submeshes
    Vector<PODVector<int> > triangles_;

};

class JSONComponent
{
public:

    const String& GetType() const
    {
        return type_;
    }

protected:

    JSONComponent(JSONSceneImporter* importer, const String& type) : type_(type), enabled_(true)
    {

    }


protected:

    bool Parse(const rapidjson::Value& value);

    String type_;
    JSONSceneImporter* importer_;

    bool enabled_;


};

class JSONTransform : public JSONComponent
{
public:

    JSONTransform(JSONSceneImporter* importer, const rapidjson::Value& value);

    const Vector3& GetLocalPosition() const
    {
        return localPosition_;
    }
    const Vector3& GetLocalScale() const
    {
        return localScale_;
    }
    const Quaternion& GetLocalRotation() const
    {
        return localRotation_;
    }

private:

    Vector3 localPosition_;
    Vector3 localScale_;
    Quaternion localRotation_;
};

class JSONMeshRenderer: public JSONComponent
{
public:

    JSONMeshRenderer(JSONSceneImporter* importer, const rapidjson::Value& value, const char *type = "MeshRenderer");

    const JSONMesh* GetMesh() const
    {
        return mesh_;
    }

    bool GetCastShadows() const
    {
        return castShadows_;
    }

    bool GetReceiveShadows() const
    {
        return receiveShadows_;
    }

    unsigned GetNumMaterials() const
    {
        return materials_.Size();
    }

    const JSONMaterial* GetMaterial(unsigned index) const
    {
        return materials_.At(index);
    }

    int GetLightmapIndex() const
    {
        return lightmapIndex_;
    }

    const Vector4& GetLightmapTilingOffset() const
    {
        return lightmapTilingOffset_;
    }

protected:

    JSONMesh* mesh_;
    bool castShadows_;
    bool receiveShadows_;
    int lightmapIndex_;
    Vector4 lightmapTilingOffset_;
    PODVector<JSONMaterial*> materials_;

};

class JSONSkinnedMeshRenderer: public JSONMeshRenderer
{
public:

    JSONSkinnedMeshRenderer(JSONSceneImporter* importer, const rapidjson::Value& value);

};

class JSONTimeOfDay : public JSONComponent
{
public:

    JSONTimeOfDay(JSONSceneImporter* importer, const rapidjson::Value& value);

    float GetTimeOn() const
    {
        return timeOn_;
    }

    void SetTimeOn(float value)
    {
        timeOn_ = value;
    }

    float GetTimeOff() const
    {
        return timeOff_;
    }

    void SetTimeOff(float value)
    {
        timeOff_ = value;
    }

private:

    float timeOn_;
    float timeOff_;


};


class JSONLight : public JSONComponent
{
public:

    JSONLight(JSONSceneImporter* importer, const rapidjson::Value& value);

    float GetRange() const
    {
        return range_;
    }

    void SetRange(float range)
    {
        range_ = range;
    }

    void SetLightType(const String& lightType)
    {
        lightType_ = lightType;
    }

    const String& GetLightType() const
    {
        return lightType_;
    }

    void SetColor(const Color& color)
    {
        color_ = color;
    }

    const Color& GetColor() const
    {
        return color_;
    }

    void SetCastsShadows(bool castsShadows)
    {
        castsShadows_ = castsShadows;
    }

    bool GetCastsShadows() const
    {
        return castsShadows_;
    }

    void SetRealtime(bool realtime)
    {
        realtime_ = realtime;
    }

    bool GetRealtime() const
    {
        return realtime_;
    }

private:

    String lightType_;
    float range_;
    Color color_;
    bool castsShadows_;
    bool realtime_;
};


class JSONRigidBody : public JSONComponent
{
public:

    JSONRigidBody(JSONSceneImporter* importer, const rapidjson::Value& value);

    float GetMass() const
    {
        return mass_;
    }

    void SetMass(float mass)
    {
        mass_ = mass;
    }

private:
    float mass_;
};

class JSONMeshCollider : public JSONComponent
{
public:

    JSONMeshCollider(JSONSceneImporter* importer, const rapidjson::Value& value);

};

class JSONBoxCollider : public JSONComponent
{
public:

    JSONBoxCollider(JSONSceneImporter* importer, const rapidjson::Value& value);

    const Vector3& GetCenter() const
    {
        return center_;
    }

    const Vector3& GetSize() const
    {
        return size_;
    }

    void SetCenter(const Vector3& center)
    {
        center_ = center;
    }

    void SetSize(const Vector3& size)
    {
        size_ = size;
    }

private:
    Vector3 center_;
    Vector3 size_;
};

class JSONAnimation : public JSONComponent
{
public:

    class Keyframe
    {
    public:
        Vector3 pos_;
        Vector3 scale_;
        Quaternion rot_;
        float time_;
    };

    class AnimationNode
    {
    public:
        String name_;
        Vector<Keyframe*> keyframes_;
    };

    class AnimationClip
    {
    public:
        String name_;
        Vector<AnimationNode*> nodes_;

        float GetDuration() const
        {
            float maxTime = -1.0f;
            for (unsigned i = 0 ; i < nodes_.Size(); i++)
            {
                if (nodes_[i]->keyframes_.Size())
                    if (nodes_[i]->keyframes_.Back()->time_ > maxTime)
                        maxTime = nodes_[i]->keyframes_.Back()->time_;
            }

            return maxTime;
        }

    };

    const Vector<AnimationClip*>& GetClips() const
    {
        return clips_;
    }

    JSONAnimation(JSONSceneImporter* importer, const rapidjson::Value& value);

private:

    Vector<AnimationClip*> clips_;

};

class JSONTerrain: public JSONComponent
{
public:

    JSONTerrain(JSONSceneImporter* importer, const rapidjson::Value& value);

    int GetHeightMapWidth() const
    {
        return heightmapWidth_;
    }

    int GetHeightMapHeight() const
    {
        return heightmapHeight_;
    }

    int GetHeightMapResolution() const
    {
        return heightmapResolution_;
    }

    const Vector3& GetHeightMapScale() const
    {
        return heightmapScale_;
    }

    const Vector3& GetHeightMapSize() const
    {
        return size_;
    }

    int GetAlphaMapWidth() const
    {
        return alphamapWidth_;
    }

    int GetAlphaMapHeight() const
    {
        return alphamapHeight_;
    }

    int GetAlphaMapLayers() const
    {
        return alphamapLayers_;
    }

    const float* GetHeightMap(unsigned& length) const
    {
        length = heightMapLength_;
        return heightMap_.Get();
    }

    const float* GetAlphaMap(unsigned& length) const
    {
        length = alphaMapLength_;
        return alphaMap_.Get();
    }

private:
    int heightmapHeight_;
    int heightmapWidth_;
    int heightmapResolution_;

    Vector3 heightmapScale_;
    Vector3 size_;

    int alphamapWidth_;
    int alphamapHeight_;
    int alphamapLayers_;

    SharedArrayPtr<float> heightMap_;
    unsigned heightMapLength_;
    SharedArrayPtr<float> alphaMap_;
    unsigned alphaMapLength_;

};

class JSONCamera: public JSONComponent
{
public:

    JSONCamera(JSONSceneImporter* importer, const rapidjson::Value& value);


private:

};


class JSONNode
{
public:

    JSONNode(JSONSceneImporter* importer, const rapidjson::Value& value);

    const String& GetName() const
    {
        return name_;
    }

    const PODVector<JSONComponent*>& GetComponents() const
    {
        return components_;

    }

    const PODVector<JSONNode*>& GetChildren() const
    {
        return children_;
    }

    void AddChild(JSONNode* child)
    {
        children_.Push(child);
    }

private:

    String name_;
    JSONSceneImporter* importer_;

    PODVector<JSONComponent*> components_;
    PODVector<JSONNode*> children_;

};


class Importer: public Object
{
    ATOMIC_OBJECT(Importer, Object);

public:

    Importer(Context* context) : Object(context) {}

private:

};


class JSONSceneImporter: public Importer
{
    ATOMIC_OBJECT(JSONSceneImporter, Importer);

public:

    JSONSceneImporter(Context* context);

    bool Import(const String& path);

    void ReadVector2FromArray(const rapidjson::Value& value, Vector2& v);
    void ReadVector3FromArray(const rapidjson::Value& value, Vector3& v);
    void ReadVector4FromArray(const rapidjson::Value& value, Vector4& v);
    void ReadQuaternionFromArray(const rapidjson::Value& value, Quaternion& q);
    void ReadMatrix4FromArray(const rapidjson::Value& value, Matrix4& m);
    void ReadColorFromArray(const rapidjson::Value& value, Color& color);

    JSONMesh* GetMesh(const String& name)
    {
        for (unsigned i = 0; i < meshes_.Size(); i++)
        {
            if (meshes_[i]->GetName() == name)
                return meshes_[i];
        }

        return NULL;
    }

    JSONMaterial* GetMaterial(const String& name)
    {
        for (unsigned i = 0; i < materials_.Size(); i++)
        {
            if (materials_[i]->GetName() == name)
                return materials_[i];
        }

        return NULL;
    }

    const PODVector<JSONTexture*>& GetTexture()
    {
        return textures_;
    }

    const PODVector<JSONLightmap*>& GetLightmaps()
    {
        return lightmaps_;
    }

    const PODVector<JSONShader*>& GetShaders()
    {
        return shaders_;
    }

    const PODVector<JSONMaterial*>& GetMaterials()
    {
        return materials_;
    }

    const PODVector<JSONTexture*>& GetTextures()
    {
        return textures_;
    }

    const PODVector<JSONNode*>& GetHierarchy() const
    {
        return hierarchy_;
    }


    PODVector<JSONMesh*>& GetMeshes()
    {
        return meshes_;
    }

    const String& GetSceneName() const
    {
        return sceneName_;
    }


    virtual ~JSONSceneImporter();

private:

    void AddTexture(JSONTexture* texture)
    {
        textures_.Push(texture);
    }

    void AddLightmap(JSONLightmap* lightmap)
    {
        lightmaps_.Push(lightmap);
    }

    void AddShader(JSONShader* shader)
    {
        shaders_.Push(shader);
    }

    void AddMaterial(JSONMaterial* material)
    {
        materials_.Push(material);
    }

    void AddMesh(JSONMesh* mesh)
    {
        meshes_.Push(mesh);
    }

    bool ParseShaders(const rapidjson::Value& value);
    bool ParseTextures(const rapidjson::Value& value);
    bool ParseLightmaps(const rapidjson::Value& value);
    bool ParseMaterials(const rapidjson::Value& value);
    bool ParseMeshes(const rapidjson::Value& value);

    bool ParseResources(const rapidjson::Value& value);

    bool ParseHierarchy(const rapidjson::Value& value);

    rapidjson::Document* document_;

    String sceneName_;

    PODVector<JSONTexture*> textures_;
    PODVector<JSONLightmap*> lightmaps_;
    PODVector<JSONShader*> shaders_;
    PODVector<JSONMaterial*> materials_;
    PODVector<JSONMesh*> meshes_;

    PODVector<JSONNode*> hierarchy_;

};



}
