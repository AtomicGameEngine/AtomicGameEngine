//
// Copyright (c) 2008-2015 the Urho3D project.
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

#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/Graphics/Geometry.h>

#include "OpenAssetUtils.h"

namespace ToolCore
{

void CollectMeshes(const aiScene* scene, OutModel& model, aiNode* node)
{

    // skip LOD for now
    String name = node->mName.C_Str();
    if (name.Find("LOD1") != String::NPOS || name.Find("LOD2") != String::NPOS)
    {
        return;
    }

    for (unsigned i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        for (unsigned j = 0; j < model.meshes_.Size(); ++j)
        {
            if (mesh == model.meshes_[j])
            {
                PrintLine("Warning: same mesh found multiple times");
                break;
            }
        }

        model.meshIndices_.Insert(node->mMeshes[i]);
        model.meshes_.Push(mesh);
        model.meshNodes_.Push(node);
        model.totalVertices_ += mesh->mNumVertices;
        model.totalIndices_ += GetNumValidFaces(mesh) * 3;
    }

    for (unsigned i = 0; i < node->mNumChildren; ++i)
        CollectMeshes(scene, model, node->mChildren[i]);
}


void GetMeshesUnderNode(const aiScene* scene, Vector<Pair<aiNode*, aiMesh*> >& dest, aiNode* node)
{
    for (unsigned i = 0; i < node->mNumMeshes; ++i)
        dest.Push(MakePair(node, scene->mMeshes[node->mMeshes[i]]));
}

unsigned GetMeshIndex(const aiScene* scene, aiMesh* mesh)
{
    for (unsigned i = 0; i < scene->mNumMeshes; ++i)
    {
        if (scene->mMeshes[i] == mesh)
            return i;
    }
    return M_MAX_UNSIGNED;
}

unsigned GetBoneIndex(OutModel& model, const String& boneName)
{
    for (unsigned i = 0; i < model.bones_.Size(); ++i)
    {
        if (boneName == model.bones_[i]->mName.data)
            return i;
    }
    return M_MAX_UNSIGNED;
}

aiBone* GetMeshBone(OutModel& model, const String& boneName)
{
    for (unsigned i = 0; i < model.meshes_.Size(); ++i)
    {
        aiMesh* mesh = model.meshes_[i];
        for (unsigned j = 0; j < mesh->mNumBones; ++j)
        {
            aiBone* bone = mesh->mBones[j];
            if (boneName == bone->mName.data)
                return bone;
        }
    }
    return 0;
}

Matrix3x4 GetOffsetMatrix(OutModel& model, const String& boneName)
{
    for (unsigned i = 0; i < model.meshes_.Size(); ++i)
    {
        aiMesh* mesh = model.meshes_[i];
        aiNode* node = model.meshNodes_[i];
        for (unsigned j = 0; j < mesh->mNumBones; ++j)
        {
            aiBone* bone = mesh->mBones[j];
            if (boneName == bone->mName.data)
            {
                aiMatrix4x4 offset = bone->mOffsetMatrix;
                aiMatrix4x4 nodeDerivedInverse = GetMeshBakingTransform(node, model.rootNode_);
                nodeDerivedInverse.Inverse();
                offset *= nodeDerivedInverse;
                return ToMatrix3x4(offset);
            }
        }
    }
    return Matrix3x4::IDENTITY;
}


unsigned GetNumValidFaces(aiMesh* mesh)
{
    unsigned ret = 0;

    for (unsigned j = 0; j < mesh->mNumFaces; ++j)
    {
        if (mesh->mFaces[j].mNumIndices == 3)
            ++ret;
    }

    return ret;
}

void WriteShortIndices(unsigned short*& dest, aiMesh* mesh, unsigned index, unsigned offset)
{
    if (mesh->mFaces[index].mNumIndices == 3)
    {
        *dest++ = mesh->mFaces[index].mIndices[0] + offset;
        *dest++ = mesh->mFaces[index].mIndices[1] + offset;
        *dest++ = mesh->mFaces[index].mIndices[2] + offset;
    }
}

void WriteLargeIndices(unsigned*& dest, aiMesh* mesh, unsigned index, unsigned offset)
{
    if (mesh->mFaces[index].mNumIndices == 3)
    {
        *dest++ = mesh->mFaces[index].mIndices[0] + offset;
        *dest++ = mesh->mFaces[index].mIndices[1] + offset;
        *dest++ = mesh->mFaces[index].mIndices[2] + offset;
    }
}

void WriteVertex(float*& dest, aiMesh* mesh, unsigned index, unsigned elementMask, BoundingBox& box,
    const Matrix3x4& vertexTransform, const Matrix3& normalTransform, Vector<PODVector<unsigned char> >& blendIndices,
    Vector<PODVector<float> >& blendWeights)
{
    Vector3 vertex = vertexTransform * ToVector3(mesh->mVertices[index]);
    box.Merge(vertex);
    *dest++ = vertex.x_;
    *dest++ = vertex.y_;
    *dest++ = vertex.z_;
    if (elementMask & MASK_NORMAL)
    {
        Vector3 normal = normalTransform * ToVector3(mesh->mNormals[index]);
        *dest++ = normal.x_;
        *dest++ = normal.y_;
        *dest++ = normal.z_;
    }
    if (elementMask & MASK_COLOR)
    {
        *((unsigned*)dest) = Color(mesh->mColors[0][index].r, mesh->mColors[0][index].g, mesh->mColors[0][index].b,
            mesh->mColors[0][index].a).ToUInt();
        ++dest;
    }
    if (elementMask & MASK_TEXCOORD1)
    {
        Vector3 texCoord = ToVector3(mesh->mTextureCoords[0][index]);
        *dest++ = texCoord.x_;
        *dest++ = texCoord.y_;
    }
    if (elementMask & MASK_TEXCOORD2)
    {
        Vector3 texCoord = ToVector3(mesh->mTextureCoords[1][index]);
        *dest++ = texCoord.x_;
        *dest++ = texCoord.y_;
    }
    if (elementMask & MASK_TANGENT)
    {
        Vector3 tangent = normalTransform * ToVector3(mesh->mTangents[index]);
        Vector3 normal = normalTransform * ToVector3(mesh->mNormals[index]);
        Vector3 bitangent = normalTransform * ToVector3(mesh->mBitangents[index]);
        // Check handedness
        float w = 1.0f;
        if ((tangent.CrossProduct(normal)).DotProduct(bitangent) < 0.5f)
            w = -1.0f;

        *dest++ = tangent.x_;
        *dest++ = tangent.y_;
        *dest++ = tangent.z_;
        *dest++ = w;
    }
    if (elementMask & MASK_BLENDWEIGHTS)
    {
        for (unsigned i = 0; i < 4; ++i)
        {
            if (i < blendWeights[index].Size())
                *dest++ = blendWeights[index][i];
            else
                *dest++ = 0.0f;
        }
    }
    if (elementMask & MASK_BLENDINDICES)
    {
        unsigned char* destBytes = (unsigned char*)dest;
        ++dest;
        for (unsigned i = 0; i < 4; ++i)
        {
            if (i < blendIndices[index].Size())
                *destBytes++ = blendIndices[index][i];
            else
                *destBytes++ = 0;
        }
    }
}

unsigned GetElementMask(aiMesh* mesh)
{
    unsigned elementMask = MASK_POSITION;
    if (mesh->HasNormals())
        elementMask |= MASK_NORMAL;
    if (mesh->HasTangentsAndBitangents())
        elementMask |= MASK_TANGENT;
    if (mesh->GetNumColorChannels() > 0)
        elementMask |= MASK_COLOR;
    if (mesh->GetNumUVChannels() > 0)
        elementMask |= MASK_TEXCOORD1;
    if (mesh->GetNumUVChannels() > 1)
        elementMask |= MASK_TEXCOORD2;
    if (mesh->HasBones())
        elementMask |= (MASK_BLENDWEIGHTS | MASK_BLENDINDICES);
    return elementMask;
}

aiNode* GetNode(const String& name, aiNode* rootNode, bool caseSensitive)
{
    if (!rootNode)
        return 0;
    if (!name.Compare(rootNode->mName.data, caseSensitive))
        return rootNode;
    for (unsigned i = 0; i < rootNode->mNumChildren; ++i)
    {
        aiNode* found = GetNode(name, rootNode->mChildren[i], caseSensitive);
        if (found)
            return found;
    }
    return 0;
}

aiMatrix4x4 GetDerivedTransform(aiNode* node, aiNode* rootNode, bool rootInclusive)
{
    return GetDerivedTransform(node->mTransformation, node, rootNode, rootInclusive);
}

aiMatrix4x4 GetDerivedTransform(aiMatrix4x4 transform, aiNode* node, aiNode* rootNode, bool rootInclusive)
{
    // If basenode is defined, go only up to it in the parent chain
    while (node && node != rootNode)
    {
        node = node->mParent;
        if (!rootInclusive && node == rootNode)
            break;
        if (node)
            transform = node->mTransformation * transform;
    }
    return transform;
}

aiMatrix4x4 GetMeshBakingTransform(aiNode* meshNode, aiNode* modelRootNode)
{
    if (meshNode == modelRootNode)
        return aiMatrix4x4();
    else
        return GetDerivedTransform(meshNode, modelRootNode);
}

void GetPosRotScale(const aiMatrix4x4& transform, Vector3& pos, Quaternion& rot, Vector3& scale)
{
    aiVector3D aiPos;
    aiQuaternion aiRot;
    aiVector3D aiScale;
    transform.Decompose(aiScale, aiRot, aiPos);
    pos = ToVector3(aiPos);
    rot = ToQuaternion(aiRot);
    scale = ToVector3(aiScale);
}

bool GetBlendData(OutModel& model, aiMesh* mesh, PODVector<unsigned>& boneMappings, Vector<PODVector<unsigned char> >&
    blendIndices, Vector<PODVector<float> >& blendWeights, String& errorMessage, unsigned maxBones)
{
    blendIndices.Resize(mesh->mNumVertices);
    blendWeights.Resize(mesh->mNumVertices);
    boneMappings.Clear();

    // If model has more bones than can fit vertex shader parameters, write the per-geometry mappings
    if (model.bones_.Size() > maxBones)
    {
        if (mesh->mNumBones > maxBones)
        {
            errorMessage = "Geometry (submesh) has over " + String(maxBones) + " bone influences. Try splitting to more submeshes\n"
                "that each stay at " + String(maxBones) + " bones or below.";

            return false;
        }
        boneMappings.Resize(mesh->mNumBones);
        for (unsigned i = 0; i < mesh->mNumBones; ++i)
        {
            aiBone* bone = mesh->mBones[i];
            String boneName = FromAIString(bone->mName);
            unsigned globalIndex = GetBoneIndex(model, boneName);
            if (globalIndex == M_MAX_UNSIGNED)
            {
                errorMessage = "Bone " + boneName + " not found";
                return false;
            }
            boneMappings[i] = globalIndex;
            for (unsigned j = 0; j < bone->mNumWeights; ++j)
            {
                unsigned vertex = bone->mWeights[j].mVertexId;
                blendIndices[vertex].Push(i);
                blendWeights[vertex].Push(bone->mWeights[j].mWeight);
            }
        }
    }
    else
    {
        for (unsigned i = 0; i < mesh->mNumBones; ++i)
        {
            aiBone* bone = mesh->mBones[i];
            String boneName = FromAIString(bone->mName);
            unsigned globalIndex = GetBoneIndex(model, boneName);
            if (globalIndex == M_MAX_UNSIGNED)
            {
                errorMessage = "Bone " + boneName + " not found";
                return false;
            }

            for (unsigned j = 0; j < bone->mNumWeights; ++j)
            {
                unsigned vertex = bone->mWeights[j].mVertexId;
                blendIndices[vertex].Push(globalIndex);
                blendWeights[vertex].Push(bone->mWeights[j].mWeight);
            }
        }
    }

    // Normalize weights now if necessary, also remove too many influences
    for (unsigned i = 0; i < blendWeights.Size(); ++i)
    {
        if (blendWeights[i].Size() > 4)
        {
            PrintLine("Warning: more than 4 bone influences in vertex " + String(i));

            while (blendWeights[i].Size() > 4)
            {
                unsigned lowestIndex = 0;
                float lowest = M_INFINITY;
                for (unsigned j = 0; j < blendWeights[i].Size(); ++j)
                {
                    if (blendWeights[i][j] < lowest)
                    {
                        lowest = blendWeights[i][j];
                        lowestIndex = j;
                    }
                }
                blendWeights[i].Erase(lowestIndex);
                blendIndices[i].Erase(lowestIndex);
            }
        }

        float sum = 0.0f;
        for (unsigned j = 0; j < blendWeights[i].Size(); ++j)
            sum += blendWeights[i][j];
        if (sum != 1.0f && sum != 0.0f)
        {
            for (unsigned j = 0; j < blendWeights[i].Size(); ++j)
                blendWeights[i][j] /= sum;
        }
    }

    return true;
}

String FromAIString(const aiString& str)
{
    return String(str.data);
}

Vector3 ToVector3(const aiVector3D& vec)
{
    return Vector3(vec.x, vec.y, vec.z);
}

Vector2 ToVector2(const aiVector2D& vec)
{
    return Vector2(vec.x, vec.y);
}

Quaternion ToQuaternion(const aiQuaternion& quat)
{
    return Quaternion(quat.w, quat.x, quat.y, quat.z);
}

Matrix3x4 ToMatrix3x4(const aiMatrix4x4& mat)
{
    Matrix3x4 ret;
    memcpy(&ret.m00_, &mat.a1, sizeof(Matrix3x4));
    return ret;
}

String SanitateAssetName(const String& name)
{
    String fixedName = name;
    fixedName.Replace("<", "");
    fixedName.Replace(">", "");
    fixedName.Replace("?", "");
    fixedName.Replace("*", "");
    fixedName.Replace(":", "");
    fixedName.Replace("\"", "");
    fixedName.Replace("/", "");
    fixedName.Replace("\\", "");
    fixedName.Replace("|", "");

    return fixedName;
}

}
