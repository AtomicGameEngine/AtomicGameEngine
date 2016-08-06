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

#include "OpenAssetUtils.h"

#include <Atomic/Core/Object.h>
#include <Atomic/Scene/Node.h>

using namespace Atomic;

namespace ToolCore
{

class OpenAssetImporter : public Object
{
    ATOMIC_OBJECT(OpenAssetImporter, Object);

public:

    struct AnimationInfo
    {
        String name_;
        String cacheFilename_;
    };

    OpenAssetImporter(Context* context);
    virtual ~OpenAssetImporter();

    bool Load(const String& assetPath);

    const String& GetErrorMessage() { return errorMessage_; }

    bool ExportModel(const String& outName, const String& animName = String::EMPTY, bool animationOnly = false);

    void SetImportNode(Node* node) { importNode_ = node; }
    void SetStartTime(float startTime) { startTime_ = startTime; }
    void SetEndTime(float endTime) { endTime_ = endTime; }
    void SetScale(float scale) { scale_ = scale; }
    void SetExportAnimations(bool exportAnimations) { noAnimations_ = !exportAnimations; }
    void SetImportMaterials(bool importMaterials) { importMaterials_ = importMaterials; }
    void SetVerboseLog(bool verboseLog) { verboseLog_ = verboseLog; }

    bool GetImportMaterialsDefault() { return importMaterialsDefault_; }

    const Vector<AnimationInfo>& GetAnimationInfos() { return animationInfos_; }

private:

    void ApplyScale();
    void ApplyScale(aiNode* node);

    bool BuildAndSaveModel(OutModel& model);
    bool BuildAndSaveAnimations(OutModel* model = 0, const String& animNameOverride = String::EMPTY);

    void ExportMaterials(HashSet<String>& usedTextures);
    bool BuildAndSaveMaterial(aiMaterial* material, HashSet<String>& usedTextures);

    void CollectSceneModels(OutScene& scene, aiNode* node);
    bool CollectBones(OutModel& model, bool animationOnly = false);
    void CollectBonesFinal(PODVector<aiNode*>& dest, const HashSet<aiNode*>& necessary, aiNode* node);
    void BuildBoneCollisionInfo(OutModel& model);
    void CollectAnimations(OutModel* model = 0);

    void ApplyProjectImportConfig();
    void SetOveriddenFlags(VariantMap& aiFlagParameters);
    void ApplyFlag(int processStep, bool active);

    String GetMeshMaterialName(aiMesh* mesh);
    String GenerateMaterialName(aiMaterial* material);
    String GetMaterialTextureName(const String& nameIn);
    String GenerateTextureName(unsigned texIndex);

    // TODO: See AssetImporter
    // void CombineLods(const PODVector<float>& lodDistances, const Vector<String>& modelNames, const String& outName)

    void DumpNodes(aiNode* rootNode, unsigned level);

    String errorMessage_;

    const aiScene* scene_;
    aiNode* rootNode_;

    String inputName_;
    String sourceAssetPath_;
    String resourcePath_;
    String outPath_;
    bool useSubdirs_;
    bool importMaterials_;
    bool importMaterialsDefault_;
    bool localIDs_;
    bool saveBinary_;
    bool createZone_;
    bool noAnimations_;
    bool noHierarchy_;
    bool noMaterials_;
    bool noTextures_;
    bool noMaterialDiffuseColor_;
    bool noEmptyNodes_;
    bool saveMaterialList_;
    bool includeNonSkinningBones_;
    bool verboseLog_;
    bool emissiveAO_;
    bool noOverwriteMaterial_;
    bool noOverwriteTexture_;
    bool noOverwriteNewerTexture_;
    bool checkUniqueModel_;
    float scale_;
    unsigned maxBones_;

    unsigned aiFlagsDefault_;
    unsigned aiCurrentFlags_;

    Vector<String> nonSkinningBoneIncludes_;
    Vector<String> nonSkinningBoneExcludes_;

    HashSet<aiAnimation*> allAnimations_;
    PODVector<aiAnimation*> sceneAnimations_;

    Vector<AnimationInfo> animationInfos_;

    SharedPtr<Node> importNode_;

    float defaultTicksPerSecond_;

    float startTime_;
    float endTime_;

};

}
