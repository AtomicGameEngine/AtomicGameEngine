//
// Copyright (c) 2008-2015 the Atomic project.
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

// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>

#include <Atomic/Atomic3D/AnimatedModel.h>
#include <Atomic/Atomic3D/Animation.h>

#include <Atomic/Graphics/Geometry.h>
#include <Atomic/Graphics/IndexBuffer.h>
#include <Atomic/Graphics/VertexBuffer.h>

#include "OpenAssetImporter.h"

namespace ToolCore
{

OpenAssetImporter::OpenAssetImporter(Context* context) : Object(context) ,
    scene_(0),
    rootNode_(0),
    useSubdirs_(true),
    localIDs_(false),
    saveBinary_(false),
    createZone_(true),
    noAnimations_(false),
    noHierarchy_(false),
    noMaterials_(false),
    noTextures_(false),
    noMaterialDiffuseColor_(false),
    noEmptyNodes_(false),
    saveMaterialList_(false),
    includeNonSkinningBones_(false),
    verboseLog_(false),
    emissiveAO_(false),
    noOverwriteMaterial_(false),
    noOverwriteTexture_(false),
    noOverwriteNewerTexture_(false),
    checkUniqueModel_(true),
    maxBones_(64),
    defaultTicksPerSecond_(4800.0f)
{

    aiFlagsDefault_ =
        aiProcess_ConvertToLeftHanded |
        aiProcess_JoinIdenticalVertices |
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_LimitBoneWeights |
        aiProcess_ImproveCacheLocality |
        aiProcess_RemoveRedundantMaterials |
        aiProcess_FixInfacingNormals |
        aiProcess_FindInvalidData |
        aiProcess_GenUVCoords |
        aiProcess_FindInstances |
        aiProcess_OptimizeMeshes;

    aiCurrentFlags_ = aiFlagsDefault_;

}

OpenAssetImporter::~OpenAssetImporter()
{
    if (scene_)
        aiReleaseImport(scene_);
}

bool OpenAssetImporter::Load(const String &assetPath)
{
    if (verboseLog_)
        Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE, aiDefaultLogStream_STDOUT);

    PrintLine("Reading file " + assetPath);

    scene_ = aiImportFile(GetNativePath(assetPath).CString(), aiCurrentFlags_);

    if (!scene_)
        ErrorExit("Could not open or parse input file " + assetPath + ": " + String(aiGetErrorString()));

    if (verboseLog_)
        Assimp::DefaultLogger::kill();

    rootNode_ = scene_->mRootNode;

    // DumpNodes(rootNode_, 0);

    return true;

}

void OpenAssetImporter::ExportModel(const String& outName, bool animationOnly)
{
    if (outName.Empty())
        ErrorExit("No output file defined");

    OutModel model;
    model.rootNode_ = rootNode_;
    model.outName_ = outName;

    CollectMeshes(scene_, model, model.rootNode_);
    CollectBones(model, animationOnly);
    BuildBoneCollisionInfo(model);
    BuildAndSaveModel(model);
    if (!noAnimations_)
    {
        CollectAnimations(&model);
        BuildAndSaveAnimations(&model);

        // Save scene-global animations
        CollectAnimations();
        BuildAndSaveAnimations();
    }
}
void OpenAssetImporter::BuildAndSaveModel(OutModel& model)
{
    if (!model.rootNode_)
        ErrorExit("Null root node for model");
    String rootNodeName = FromAIString(model.rootNode_->mName);
    if (!model.meshes_.Size())
        ErrorExit("No geometries found starting from node " + rootNodeName);

    PrintLine("Writing model " + rootNodeName);

    SharedPtr<Model> outModel(new Model(context_));
    Vector<PODVector<unsigned> > allBoneMappings;
    BoundingBox box;

    unsigned numValidGeometries = 0;

    bool combineBuffers = true;
    // Check if buffers can be combined (same vertex element mask, under 65535 vertices)
    unsigned elementMask = GetElementMask(model.meshes_[0]);
    for (unsigned i = 0; i < model.meshes_.Size(); ++i)
    {
        if (GetNumValidFaces(model.meshes_[i]))
        {
            ++numValidGeometries;
            if (i > 0 && GetElementMask(model.meshes_[i]) != elementMask)
                combineBuffers = false;
        }
    }
    // Check if keeping separate buffers allows to avoid 32-bit indices
    if (combineBuffers && model.totalVertices_ > 65535)
    {
        bool allUnder65k = true;
        for (unsigned i = 0; i < model.meshes_.Size(); ++i)
        {
            if (GetNumValidFaces(model.meshes_[i]))
            {
                if (model.meshes_[i]->mNumVertices > 65535)
                    allUnder65k = false;
            }
        }
        if (allUnder65k == true)
            combineBuffers = false;
    }

    SharedPtr<IndexBuffer> ib;
    SharedPtr<VertexBuffer> vb;
    Vector<SharedPtr<VertexBuffer> > vbVector;
    Vector<SharedPtr<IndexBuffer> > ibVector;
    unsigned startVertexOffset = 0;
    unsigned startIndexOffset = 0;
    unsigned destGeomIndex = 0;

    outModel->SetNumGeometries(numValidGeometries);

    for (unsigned i = 0; i < model.meshes_.Size(); ++i)
    {
        aiMesh* mesh = model.meshes_[i];
        unsigned elementMask = GetElementMask(mesh);
        unsigned validFaces = GetNumValidFaces(mesh);
        if (!validFaces)
            continue;

        bool largeIndices;
        if (combineBuffers)
            largeIndices = model.totalIndices_ > 65535;
        else
            largeIndices = mesh->mNumVertices > 65535;

        // Create new buffers if necessary
        if (!combineBuffers || vbVector.Empty())
        {
            vb = new VertexBuffer(context_);
            ib = new IndexBuffer(context_);

            if (combineBuffers)
            {
                ib->SetSize(model.totalIndices_, largeIndices);
                vb->SetSize(model.totalVertices_, elementMask);
            }
            else
            {
                ib->SetSize(validFaces * 3, largeIndices);
                vb->SetSize(mesh->mNumVertices, elementMask);
            }

            vbVector.Push(vb);
            ibVector.Push(ib);
            startVertexOffset = 0;
            startIndexOffset = 0;
        }

        // Get the world transform of the mesh for baking into the vertices
        Matrix3x4 vertexTransform;
        Matrix3 normalTransform;
        Vector3 pos, scale;
        Quaternion rot;
        GetPosRotScale(GetMeshBakingTransform(model.meshNodes_[i], model.rootNode_), pos, rot, scale);
        vertexTransform = Matrix3x4(pos, rot, scale);
        normalTransform = rot.RotationMatrix();

        SharedPtr<Geometry> geom(new Geometry(context_));

        PrintLine("Writing geometry " + String(i) + " with " + String(mesh->mNumVertices) + " vertices " +
            String(validFaces * 3) + " indices");

        unsigned char* vertexData = vb->GetShadowData();
        unsigned char* indexData = ib->GetShadowData();

        // Build the index data
        if (!largeIndices)
        {
            unsigned short* dest = (unsigned short*)indexData + startIndexOffset;
            for (unsigned j = 0; j < mesh->mNumFaces; ++j)
                WriteShortIndices(dest, mesh, j, startVertexOffset);
        }
        else
        {
            unsigned* dest = (unsigned*)indexData + startIndexOffset;
            for (unsigned j = 0; j < mesh->mNumFaces; ++j)
                WriteLargeIndices(dest, mesh, j, startVertexOffset);
        }

        // Build the vertex data
        // If there are bones, get blend data
        Vector<PODVector<unsigned char> > blendIndices;
        Vector<PODVector<float> > blendWeights;
        PODVector<unsigned> boneMappings;
        if (model.bones_.Size())
            GetBlendData(model, mesh, boneMappings, blendIndices, blendWeights, maxBones_);

        float* dest = (float*)((unsigned char*)vertexData + startVertexOffset * vb->GetVertexSize());
        for (unsigned j = 0; j < mesh->mNumVertices; ++j)
            WriteVertex(dest, mesh, j, elementMask, box, vertexTransform, normalTransform, blendIndices, blendWeights);

        // Calculate the geometry center
        Vector3 center = Vector3::ZERO;
        if (validFaces)
        {
            for (unsigned j = 0; j < mesh->mNumFaces; ++j)
            {
                if (mesh->mFaces[j].mNumIndices == 3)
                {
                    center += vertexTransform * ToVector3(mesh->mVertices[mesh->mFaces[j].mIndices[0]]);
                    center += vertexTransform * ToVector3(mesh->mVertices[mesh->mFaces[j].mIndices[1]]);
                    center += vertexTransform * ToVector3(mesh->mVertices[mesh->mFaces[j].mIndices[2]]);
                }
            }

            center /= (float)validFaces * 3;
        }

        // Define the geometry
        geom->SetIndexBuffer(ib);
        geom->SetVertexBuffer(0, vb);
        geom->SetDrawRange(TRIANGLE_LIST, startIndexOffset, validFaces * 3, true);
        outModel->SetNumGeometryLodLevels(destGeomIndex, 1);
        outModel->SetGeometry(destGeomIndex, 0, geom);
        outModel->SetGeometryCenter(destGeomIndex, center);
        if (model.bones_.Size() > maxBones_)
            allBoneMappings.Push(boneMappings);

        startVertexOffset += mesh->mNumVertices;
        startIndexOffset += validFaces * 3;
        ++destGeomIndex;
    }

    // Define the model buffers and bounding box
    PODVector<unsigned> emptyMorphRange;
    outModel->SetVertexBuffers(vbVector, emptyMorphRange, emptyMorphRange);
    outModel->SetIndexBuffers(ibVector);
    outModel->SetBoundingBox(box);

    // Build skeleton if necessary
    if (model.bones_.Size() && model.rootBone_)
    {
        PrintLine("Writing skeleton with " + String(model.bones_.Size()) + " bones, rootbone " +
            FromAIString(model.rootBone_->mName));

        Skeleton skeleton;
        Vector<Bone>& bones = skeleton.GetModifiableBones();

        for (unsigned i = 0; i < model.bones_.Size(); ++i)
        {
            aiNode* boneNode = model.bones_[i];
            String boneName(FromAIString(boneNode->mName));

            Bone newBone;
            newBone.name_ = boneName;

            aiMatrix4x4 transform = boneNode->mTransformation;
            // Make the root bone transform relative to the model's root node, if it is not already
            if (boneNode == model.rootBone_)
                transform = GetDerivedTransform(boneNode, model.rootNode_);

            GetPosRotScale(transform, newBone.initialPosition_, newBone.initialRotation_, newBone.initialScale_);

            // Get offset information if exists
            newBone.offsetMatrix_ = GetOffsetMatrix(model, boneName);
            newBone.radius_ = model.boneRadii_[i];
            newBone.boundingBox_ = model.boneHitboxes_[i];
            newBone.collisionMask_ = BONECOLLISION_SPHERE | BONECOLLISION_BOX;
            newBone.parentIndex_ = i;
            bones.Push(newBone);
        }
        // Set the bone hierarchy
        for (unsigned i = 1; i < model.bones_.Size(); ++i)
        {
            String parentName = FromAIString(model.bones_[i]->mParent->mName);
            for (unsigned j = 0; j < bones.Size(); ++j)
            {
                if (bones[j].name_ == parentName)
                {
                    bones[i].parentIndex_ = j;
                    break;
                }
            }
        }

        outModel->SetSkeleton(skeleton);
        if (model.bones_.Size() > maxBones_)
            outModel->SetGeometryBoneMappings(allBoneMappings);
    }

    File outFile(context_);
    if (!outFile.Open(model.outName_, FILE_WRITE))
        ErrorExit("Could not open output file " + model.outName_);
    outModel->Save(outFile);

    // If exporting materials, also save material list for use by the editor
    if (!noMaterials_ && saveMaterialList_)
    {
        String materialListName = ReplaceExtension(model.outName_, ".txt");
        File listFile(context_);
        if (listFile.Open(materialListName, FILE_WRITE))
        {
            for (unsigned i = 0; i < model.meshes_.Size(); ++i)
                listFile.WriteLine(GetMeshMaterialName(model.meshes_[i]));
        }
        else
            PrintLine("Warning: could not write material list file " + materialListName);
    }
}

String OpenAssetImporter::GetMeshMaterialName(aiMesh* mesh)
{
    aiMaterial* material = scene_->mMaterials[mesh->mMaterialIndex];
    aiString matNameStr;
    material->Get(AI_MATKEY_NAME, matNameStr);
    String matName = SanitateAssetName(FromAIString(matNameStr));
    if (matName.Trimmed().Empty())
        matName = GenerateMaterialName(material);

    return (useSubdirs_ ? "Materials/" : "") + matName + ".xml";
}

String OpenAssetImporter::GenerateMaterialName(aiMaterial* material)
{
    for (unsigned i = 0; i < scene_->mNumMaterials; ++i)
    {
        if (scene_->mMaterials[i] == material)
            return inputName_ + "_Material" + String(i);
    }

    // Should not go here
    return String::EMPTY;
}

String OpenAssetImporter::GetMaterialTextureName(const String& nameIn)
{
    // Detect assimp embedded texture
    if (nameIn.Length() && nameIn[0] == '*')
        return GenerateTextureName(ToInt(nameIn.Substring(1)));
    else
        return (useSubdirs_ ? "Textures/" : "") + nameIn;
}

String OpenAssetImporter::GenerateTextureName(unsigned texIndex)
{
    if (texIndex < scene_->mNumTextures)
    {
        // If embedded texture contains encoded data, use the format hint for file extension. Else save RGBA8 data as PNG
        aiTexture* tex = scene_->mTextures[texIndex];
        if (!tex->mHeight)
            return (useSubdirs_ ? "Textures/" : "") + inputName_ + "_Texture" + String(texIndex) + "." + tex->achFormatHint;
        else
            return (useSubdirs_ ? "Textures/" : "") + inputName_ + "_Texture" + String(texIndex) + ".png";
    }

    // Should not go here
    return String::EMPTY;
}

void OpenAssetImporter::CollectSceneModels(OutScene& scene, aiNode* node)
{
    Vector<Pair<aiNode*, aiMesh*> > meshes;
    GetMeshesUnderNode(scene_, meshes, node);

    if (meshes.Size())
    {
        OutModel model;
        model.rootNode_ = node;
        model.outName_ = resourcePath_ + (useSubdirs_ ? "Models/" : "") + SanitateAssetName(FromAIString(node->mName)) + ".mdl";
        for (unsigned i = 0; i < meshes.Size(); ++i)
        {
            aiMesh* mesh = meshes[i].second_;
            unsigned meshIndex = GetMeshIndex(scene_, mesh);
            model.meshIndices_.Insert(meshIndex);
            model.meshes_.Push(mesh);
            model.meshNodes_.Push(meshes[i].first_);
            model.totalVertices_ += mesh->mNumVertices;
            model.totalIndices_ += GetNumValidFaces(mesh) * 3;
        }

        // Check if a model with identical mesh indices already exists. If yes, do not export twice
        bool unique = true;
        if (checkUniqueModel_)
        {
            for (unsigned i = 0; i < scene.models_.Size(); ++i)
            {
                if (scene.models_[i].meshIndices_ == model.meshIndices_)
                {
                    PrintLine("Added node " + FromAIString(node->mName));
                    scene.nodes_.Push(node);
                    scene.nodeModelIndices_.Push(i);
                    unique = false;
                    break;
                }
            }
        }
        if (unique)
        {
            PrintLine("Added model " + model.outName_);
            PrintLine("Added node " + FromAIString(node->mName));
            CollectBones(model);
            BuildBoneCollisionInfo(model);
            if (!noAnimations_)
            {
                CollectAnimations(&model);
                BuildAndSaveAnimations(&model);
            }

            scene.models_.Push(model);
            scene.nodes_.Push(node);
            scene.nodeModelIndices_.Push(scene.models_.Size() - 1);
        }
    }

    for (unsigned i = 0; i < node->mNumChildren; ++i)
        CollectSceneModels(scene, node->mChildren[i]);
}

void OpenAssetImporter::CollectBones(OutModel& model, bool animationOnly)
{
    HashSet<aiNode*> necessary;
    HashSet<aiNode*> rootNodes;

    for (unsigned i = 0; i < model.meshes_.Size(); ++i)
    {
        aiMesh* mesh = model.meshes_[i];
        aiNode* meshNode = model.meshNodes_[i];
        aiNode* meshParentNode = meshNode->mParent;
        aiNode* rootNode = 0;

        for (unsigned j = 0; j < mesh->mNumBones; ++j)
        {
            aiBone* bone = mesh->mBones[j];
            String boneName(FromAIString(bone->mName));
            aiNode* boneNode = GetNode(boneName, scene_->mRootNode, true);
            if (!boneNode)
                ErrorExit("Could not find scene node for bone " + boneName);
            necessary.Insert(boneNode);
            rootNode = boneNode;

            for (;;)
            {
                boneNode = boneNode->mParent;
                if (!boneNode || ((boneNode == meshNode || boneNode == meshParentNode) && !animationOnly))
                    break;
                rootNode = boneNode;
                necessary.Insert(boneNode);
            }

            if (rootNodes.Find(rootNode) == rootNodes.End())
                rootNodes.Insert(rootNode);
        }
    }

    // If we find multiple root nodes, try to remedy by using their parent instead
    if (rootNodes.Size() > 1)
    {
        aiNode* commonParent = (*rootNodes.Begin())->mParent;
        for (HashSet<aiNode*>::Iterator i = rootNodes.Begin(); i != rootNodes.End(); ++i)
        {
            if (*i != commonParent)
            {
                if (!commonParent || (*i)->mParent != commonParent)
                    ErrorExit("Skeleton with multiple root nodes found, not supported");
            }
        }
        rootNodes.Clear();
        rootNodes.Insert(commonParent);
        necessary.Insert(commonParent);
    }

    if (rootNodes.Empty())
        return;

    model.rootBone_ = *rootNodes.Begin();
    CollectBonesFinal(model.bones_, necessary, model.rootBone_);
    // Initialize the bone collision info
    model.boneRadii_.Resize(model.bones_.Size());
    model.boneHitboxes_.Resize(model.bones_.Size());
    for (unsigned i = 0; i < model.bones_.Size(); ++i)
    {
        model.boneRadii_[i] = 0.0f;
        model.boneHitboxes_[i] = BoundingBox(0.0f, 0.0f);
    }
}

void OpenAssetImporter::CollectBonesFinal(PODVector<aiNode*>& dest, const HashSet<aiNode*>& necessary, aiNode* node)
{
    bool includeBone = necessary.Find(node) != necessary.End();
    String boneName = FromAIString(node->mName);

    // Check include/exclude filters for non-skinned bones
    if (!includeBone && includeNonSkinningBones_)
    {
        // If no includes specified, include by default but check for excludes
        if (nonSkinningBoneIncludes_.Empty())
            includeBone = true;

        // Check against includes/excludes
        for (unsigned i = 0; i < nonSkinningBoneIncludes_.Size(); ++i)
        {
            if (boneName.Contains(nonSkinningBoneIncludes_[i], false))
            {
                includeBone = true;
                break;
            }
        }
        for (unsigned i = 0; i < nonSkinningBoneExcludes_.Size(); ++i)
        {
            if (boneName.Contains(nonSkinningBoneExcludes_[i], false))
            {
                includeBone = false;
                break;
            }
        }

        if (includeBone)
            PrintLine("Including non-skinning bone " + boneName);
    }

    if (includeBone)
        dest.Push(node);

    for (unsigned i = 0; i < node->mNumChildren; ++i)
        CollectBonesFinal(dest, necessary, node->mChildren[i]);
}

void OpenAssetImporter::CollectAnimations(OutModel* model)
{
    const aiScene* scene = scene_;
    for (unsigned i = 0; i < scene->mNumAnimations; ++i)
    {
        aiAnimation* anim = scene->mAnimations[i];
        if (allAnimations_.Contains(anim))
            continue;

        if (model)
        {
            bool modelBoneFound = false;
            for (unsigned j = 0; j < anim->mNumChannels; ++j)
            {
                aiNodeAnim* channel = anim->mChannels[j];
                String channelName = FromAIString(channel->mNodeName);
                if (GetBoneIndex(*model, channelName) != M_MAX_UNSIGNED)
                {
                    modelBoneFound = true;
                    break;
                }
            }
            if (modelBoneFound)
            {
                model->animations_.Push(anim);
                allAnimations_.Insert(anim);
            }
        }
        else
        {
            sceneAnimations_.Push(anim);
            allAnimations_.Insert(anim);
        }
    }

    /// \todo Vertex morphs are ignored for now
}

void OpenAssetImporter::BuildBoneCollisionInfo(OutModel& model)
{
    for (unsigned i = 0; i < model.meshes_.Size(); ++i)
    {
        aiMesh* mesh = model.meshes_[i];
        for (unsigned j = 0; j < mesh->mNumBones; ++j)
        {
            aiBone* bone = mesh->mBones[j];
            String boneName = FromAIString(bone->mName);
            unsigned boneIndex = GetBoneIndex(model, boneName);
            if (boneIndex == M_MAX_UNSIGNED)
                continue;
            for (unsigned k = 0; k < bone->mNumWeights; ++k)
            {
                float weight = bone->mWeights[k].mWeight;
                // Require skinning weight to be sufficiently large before vertex contributes to bone hitbox
                if (weight > 0.33f)
                {
                    aiVector3D vertexBoneSpace = bone->mOffsetMatrix * mesh->mVertices[bone->mWeights[k].mVertexId];
                    Vector3 vertex = ToVector3(vertexBoneSpace);
                    float radius = vertex.Length();
                    if (radius > model.boneRadii_[boneIndex])
                        model.boneRadii_[boneIndex] = radius;
                    model.boneHitboxes_[boneIndex].Merge(vertex);
                }
            }
        }
    }
}

void OpenAssetImporter::BuildAndSaveAnimations(OutModel* model)
{
    const PODVector<aiAnimation*>& animations = model ? model->animations_ : sceneAnimations_;

    for (unsigned i = 0; i < animations.Size(); ++i)
    {
        aiAnimation* anim = animations[i];

        float duration = (float)anim->mDuration;
        String animName = FromAIString(anim->mName);
        String animOutName;

        if (animName.Empty())
            animName = "Anim" + String(i + 1);
        if (model)
            animOutName = GetPath(model->outName_) + GetFileName(model->outName_) + "_" + SanitateAssetName(animName) + ".ani";
        else
            animOutName = outPath_ + SanitateAssetName(animName) + ".ani";

        float ticksPerSecond = (float)anim->mTicksPerSecond;
        // If ticks per second not specified, it's probably a .X file. In this case use the default tick rate
        if (ticksPerSecond < M_EPSILON)
            ticksPerSecond = defaultTicksPerSecond_;
        float tickConversion = 1.0f / ticksPerSecond;

        // Find out the start time of animation from each channel's first keyframe for adjusting the keyframe times
        // to start from zero
        float startTime = duration;
        for (unsigned j = 0; j < anim->mNumChannels; ++j)
        {
            aiNodeAnim* channel = anim->mChannels[j];
            if (channel->mNumPositionKeys > 0)
                startTime = Min(startTime, (float)channel->mPositionKeys[0].mTime);
            if (channel->mNumRotationKeys > 0)
                startTime = Min(startTime, (float)channel->mRotationKeys[0].mTime);
            if (channel->mScalingKeys > 0)
                startTime = Min(startTime, (float)channel->mScalingKeys[0].mTime);
        }
        duration -= startTime;

        SharedPtr<Animation> outAnim(new Animation(context_));
        outAnim->SetAnimationName(animName);
        outAnim->SetLength(duration * tickConversion);

        PrintLine("Writing animation " + animName + " length " + String(outAnim->GetLength()));
        Vector<AnimationTrack> tracks;
        for (unsigned j = 0; j < anim->mNumChannels; ++j)
        {
            aiNodeAnim* channel = anim->mChannels[j];
            String channelName = FromAIString(channel->mNodeName);
            aiNode* boneNode = 0;
            bool isRootBone = false;

            if (model)
            {
                unsigned boneIndex = GetBoneIndex(*model, channelName);
                if (boneIndex == M_MAX_UNSIGNED)
                {
                    PrintLine("Warning: skipping animation track " + channelName + " not found in model skeleton");
                    continue;
                }
                boneNode = model->bones_[boneIndex];
                isRootBone = boneIndex == 0;
            }
            else
            {
                boneNode = GetNode(channelName, scene_->mRootNode);
                if (!boneNode)
                {
                    PrintLine("Warning: skipping animation track " + channelName + " whose scene node was not found");
                    continue;
                }
            }

            // To export single frame animation, check if first key frame is identical to bone transformation
            aiVector3D bonePos, boneScale;
            aiQuaternion boneRot;
            boneNode->mTransformation.Decompose(boneScale, boneRot, bonePos);

            bool posEqual = true;
            bool scaleEqual = true;
            bool rotEqual = true;

            if (channel->mNumPositionKeys > 0 && !ToVector3(bonePos).Equals(ToVector3(channel->mPositionKeys[0].mValue)))
                posEqual = false;
            if (channel->mNumScalingKeys > 0 && !ToVector3(boneScale).Equals(ToVector3(channel->mScalingKeys[0].mValue)))
                scaleEqual = false;
            if (channel->mNumRotationKeys > 0 && !ToQuaternion(boneRot).Equals(ToQuaternion(channel->mRotationKeys[0].mValue)))
                rotEqual = false;

            AnimationTrack track;
            track.name_ = channelName;
            track.nameHash_ = channelName;

            // Check which channels are used
            track.channelMask_ = 0;
            if (channel->mNumPositionKeys > 1 || !posEqual)
                track.channelMask_ |= CHANNEL_POSITION;
            if (channel->mNumRotationKeys > 1 || !rotEqual)
                track.channelMask_ |= CHANNEL_ROTATION;
            if (channel->mNumScalingKeys > 1 || !scaleEqual)
                track.channelMask_ |= CHANNEL_SCALE;
            // Check for redundant identity scale in all keyframes and remove in that case
            if (track.channelMask_ & CHANNEL_SCALE)
            {
                bool redundantScale = true;
                for (unsigned k = 0; k < channel->mNumScalingKeys; ++k)
                {
                    float SCALE_EPSILON = 0.000001f;
                    Vector3 scaleVec = ToVector3(channel->mScalingKeys[k].mValue);
                    if (fabsf(scaleVec.x_ - 1.0f) >= SCALE_EPSILON || fabsf(scaleVec.y_ - 1.0f) >= SCALE_EPSILON ||
                            fabsf(scaleVec.z_ - 1.0f) >= SCALE_EPSILON)
                    {
                        redundantScale = false;
                        break;
                    }
                }
                if (redundantScale)
                    track.channelMask_ &= ~CHANNEL_SCALE;
            }

            if (!track.channelMask_)
                PrintLine("Warning: skipping animation track " + channelName + " with no keyframes");

            // Currently only same amount of keyframes is supported
            // Note: should also check the times of individual keyframes for match
            if ((channel->mNumPositionKeys > 1 && channel->mNumRotationKeys > 1 && channel->mNumPositionKeys != channel->mNumRotationKeys) ||
                    (channel->mNumPositionKeys > 1 && channel->mNumScalingKeys > 1 && channel->mNumPositionKeys != channel->mNumScalingKeys) ||
                    (channel->mNumRotationKeys > 1 && channel->mNumScalingKeys > 1 && channel->mNumRotationKeys != channel->mNumScalingKeys))
            {
                PrintLine("Warning: differing amounts of channel keyframes, skipping animation track " + channelName);
                continue;
            }

            unsigned keyFrames = channel->mNumPositionKeys;
            if (channel->mNumRotationKeys > keyFrames)
                keyFrames = channel->mNumRotationKeys;
            if (channel->mNumScalingKeys > keyFrames)
                keyFrames = channel->mNumScalingKeys;

            for (unsigned k = 0; k < keyFrames; ++k)
            {
                AnimationKeyFrame kf;
                kf.time_ = 0.0f;
                kf.position_ = Vector3::ZERO;
                kf.rotation_ = Quaternion::IDENTITY;
                kf.scale_ = Vector3::ONE;

                // Get time for the keyframe. Adjust with animation's start time
                if (track.channelMask_ & CHANNEL_POSITION && k < channel->mNumPositionKeys)
                    kf.time_ = ((float)channel->mPositionKeys[k].mTime - startTime) * tickConversion;
                else if (track.channelMask_ & CHANNEL_ROTATION && k < channel->mNumRotationKeys)
                    kf.time_ = ((float)channel->mRotationKeys[k].mTime - startTime) * tickConversion;
                else if (track.channelMask_ & CHANNEL_SCALE && k < channel->mNumScalingKeys)
                    kf.time_ = ((float)channel->mScalingKeys[k].mTime - startTime) * tickConversion;

                // Make sure time stays positive
                kf.time_ = Max(kf.time_, 0.0f);

                // Start with the bone's base transform
                aiMatrix4x4 boneTransform = boneNode->mTransformation;
                aiVector3D pos, scale;
                aiQuaternion rot;
                boneTransform.Decompose(scale, rot, pos);
                // Then apply the active channels
                if (track.channelMask_ & CHANNEL_POSITION && k < channel->mNumPositionKeys)
                    pos = channel->mPositionKeys[k].mValue;
                if (track.channelMask_ & CHANNEL_ROTATION && k < channel->mNumRotationKeys)
                    rot = channel->mRotationKeys[k].mValue;
                if (track.channelMask_ & CHANNEL_SCALE && k < channel->mNumScalingKeys)
                    scale = channel->mScalingKeys[k].mValue;

                // If root bone, transform with the model root node transform
                if (model && isRootBone)
                {
                    aiMatrix4x4 transMat, scaleMat, rotMat;
                    aiMatrix4x4::Translation(pos, transMat);
                    aiMatrix4x4::Scaling(scale, scaleMat);
                    rotMat = aiMatrix4x4(rot.GetMatrix());
                    aiMatrix4x4 tform = transMat * rotMat * scaleMat;
                    tform = GetDerivedTransform(tform, boneNode, model->rootNode_);
                    tform.Decompose(scale, rot, pos);
                }

                if (track.channelMask_ & CHANNEL_POSITION)
                    kf.position_ = ToVector3(pos);
                if (track.channelMask_ & CHANNEL_ROTATION)
                    kf.rotation_ = ToQuaternion(rot);
                if (track.channelMask_ & CHANNEL_SCALE)
                    kf.scale_ = ToVector3(scale);

                track.keyFrames_.Push(kf);
            }

            tracks.Push(track);
        }

        outAnim->SetTracks(tracks);

        File outFile(context_);
        if (!outFile.Open(animOutName, FILE_WRITE))
            ErrorExit("Could not open output file " + animOutName);
        outAnim->Save(outFile);
    }
}

void OpenAssetImporter::DumpNodes(aiNode* rootNode, unsigned level)
{
    if (!rootNode)
        return;

    String indent(' ', level * 2);
    Vector3 pos, scale;
    Quaternion rot;
    aiMatrix4x4 transform = GetDerivedTransform(rootNode, rootNode_);
    GetPosRotScale(transform, pos, rot, scale);

    PrintLine(indent + "Node " + FromAIString(rootNode->mName) + " pos " + String(pos));

    if (rootNode->mNumMeshes == 1)
        PrintLine(indent + "  " + String(rootNode->mNumMeshes) + " geometry");
    if (rootNode->mNumMeshes > 1)
        PrintLine(indent + "  " + String(rootNode->mNumMeshes) + " geometries");

    for (unsigned i = 0; i < rootNode->mNumChildren; ++i)
        DumpNodes(rootNode->mChildren[i], level + 1);
}

}

