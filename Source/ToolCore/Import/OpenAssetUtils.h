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

#pragma once

#include <ThirdParty/Assimp/include/assimp/cimport.h>
#include <ThirdParty/Assimp/include/assimp/scene.h>
#include <ThirdParty/Assimp/include/assimp/postprocess.h>
#include <ThirdParty/Assimp/include/assimp/DefaultLogger.hpp>

#include <cstring>

#include <Atomic/Container/Str.h>
#include <Atomic/Container/Sort.h>
#include <Atomic/Container/Hash.h>
#include <Atomic/Container/HashSet.h>

#include <Atomic/Core/StringUtils.h>

#include <Atomic/Math/Vector3.h>
#include <Atomic/Math/Quaternion.h>
#include <Atomic/Math/Matrix3x4.h>
#include <Atomic/Math/BoundingBox.h>

using namespace Atomic;

namespace ToolCore
{

struct OutModel
{
    OutModel() :
        rootBone_(0),
        totalVertices_(0),
        totalIndices_(0)
    {
    }

    String outName_;
    aiNode* rootNode_;
    HashSet<unsigned> meshIndices_;
    PODVector<aiMesh*> meshes_;
    PODVector<aiNode*> meshNodes_;
    PODVector<aiNode*> bones_;
    PODVector<aiAnimation*> animations_;
    PODVector<float> boneRadii_;
    PODVector<BoundingBox> boneHitboxes_;
    aiNode* rootBone_;
    unsigned totalVertices_;
    unsigned totalIndices_;
};

struct OutScene
{
    String outName_;
    aiNode* rootNode_;
    Vector<OutModel> models_;
    PODVector<aiNode*> nodes_;
    PODVector<unsigned> nodeModelIndices_;
};

void WriteShortIndices(unsigned short*& dest, aiMesh* mesh, unsigned index, unsigned offset);
void WriteLargeIndices(unsigned*& dest, aiMesh* mesh, unsigned index, unsigned offset);
void WriteVertex(float*& dest, aiMesh* mesh, unsigned index, unsigned elementMask, BoundingBox& box,
    const Matrix3x4& vertexTransform, const Matrix3& normalTransform, Vector<PODVector<unsigned char> >& blendIndices,
    Vector<PODVector<float> >& blendWeights);
unsigned GetElementMask(aiMesh* mesh);

aiNode* GetNode(const String& name, aiNode* rootNode, bool caseSensitive = true);
aiMatrix4x4 GetDerivedTransform(aiNode* node, aiNode* rootNode, bool rootInclusive = true);
aiMatrix4x4 GetDerivedTransform(aiMatrix4x4 transform, aiNode* node, aiNode* rootNode, bool rootInclusive = true);
aiMatrix4x4 GetMeshBakingTransform(aiNode* meshNode, aiNode* modelRootNode);
void GetPosRotScale(const aiMatrix4x4& transform, Vector3& pos, Quaternion& rot, Vector3& scale);

String FromAIString(const aiString& str);
Vector3 ToVector3(const aiVector3D& vec);
Vector2 ToVector2(const aiVector2D& vec);
Quaternion ToQuaternion(const aiQuaternion& quat);
Matrix3x4 ToMatrix3x4(const aiMatrix4x4& mat);
String SanitateAssetName(const String& name);

void GetMeshesUnderNode(const aiScene* scene, Vector<Pair<aiNode*, aiMesh*> >& dest, aiNode* node);
unsigned GetMeshIndex(const aiScene* scene, aiMesh* mesh);
unsigned GetBoneIndex(OutModel& model, const String& boneName);
aiBone* GetMeshBone(OutModel& model, const String& boneName);
Matrix3x4 GetOffsetMatrix(OutModel& model, const String& boneName);
unsigned GetNumValidFaces(aiMesh* mesh);

bool GetBlendData(OutModel& model, aiMesh* mesh, PODVector<unsigned>& boneMappings, Vector<PODVector<unsigned char> >&
    blendIndices, Vector<PODVector<float> >& blendWeights, String &errorMessage, unsigned maxBones = 64);

void CollectMeshes(const aiScene* scene, OutModel& model, aiNode* node);

void DumpNodes(aiNode* rootNode, unsigned level);

}
