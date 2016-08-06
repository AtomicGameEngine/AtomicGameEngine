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
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Scene/Node.h>

#include <Atomic/Graphics/AnimatedModel.h>
#include <Atomic/Graphics/Animation.h>
#include <Atomic/Graphics/AnimationController.h>
#include <Atomic/Graphics/StaticModel.h>
#include <Atomic/Graphics/Model.h>

#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/XMLFile.h>

#include "../Import/OpenAssetImporter.h"

#include "Asset.h"
#include "AssetDatabase.h"
#include "ModelImporter.h"

namespace ToolCore
{

/// Node + Model (static or animated)
ModelImporter::ModelImporter(Context* context, Asset *asset) : AssetImporter(context, asset)
{
    SetDefaults();
}

ModelImporter::~ModelImporter()
{

}

void ModelImporter::SetDefaults()
{
    AssetImporter::SetDefaults();
    SharedPtr<OpenAssetImporter> importer(new OpenAssetImporter(context_));

    scale_ = 1.0;
    importAnimations_ = false;
    importMaterials_ = importer->GetImportMaterialsDefault();
    animationInfo_.Clear();

}

bool ModelImporter::ImportModel()
{

    ATOMIC_LOGDEBUGF("Importing Model: %s", asset_->GetPath().CString());

    SharedPtr<OpenAssetImporter> importer(new OpenAssetImporter(context_));

    importer->SetVerboseLog(true);

    importer->SetScale(scale_);
    importer->SetExportAnimations(false);
    importer->SetImportNode(importNode_);
    importer->SetImportMaterials(importMaterials_);

    if (importer->Load(asset_->GetPath()))
    {
        importer->ExportModel(asset_->GetCachePath());

        return true;
    }
    else
    {
        asset_->PostImportError(importer->GetErrorMessage());
    }

    return false;
}

/*void ModelImporter::SetImportMaterials(bool importMat)
{
    LOGDEBUGF("Importing Materials of: %s", asset_->GetPath().CString());
    SharedPtr<OpenAssetImporter> importer(new OpenAssetImporter(context_));
    importer->SetImportMaterials(importMat);
}*/

bool ModelImporter::ImportAnimation(const String& filename, const String& name, float startTime, float endTime)
{
    SharedPtr<OpenAssetImporter> importer(new OpenAssetImporter(context_));

    //importer->SetVerboseLog(true);

    importer->SetScale(scale_);
    importer->SetExportAnimations(true);
    importer->SetStartTime(startTime);
    importer->SetEndTime(endTime);

    if (importer->Load(filename))
    {
        importer->ExportModel(asset_->GetCachePath(), name, true);

        const Vector<OpenAssetImporter::AnimationInfo>& infos = importer->GetAnimationInfos();

        for (unsigned i = 0; i < infos.Size(); i++)
        {
            const OpenAssetImporter::AnimationInfo& info = infos.At(i);

            String pathName, fileName, extension;

            SplitPath(info.cacheFilename_, pathName, fileName, extension);

            ResourceCache* cache = GetSubsystem<ResourceCache>();

            AnimatedModel* animatedModel = importNode_->GetComponent<AnimatedModel>();
            AnimationController* controller = importNode_->GetComponent<AnimationController>();
            if (animatedModel && controller)
            {
                SharedPtr<Animation> animation = cache->GetTempResource<Animation>(fileName + extension);
                if (animation)
                    controller->AddAnimationResource(animation);
            }

            ATOMIC_LOGINFOF("Import Info: %s : %s", info.name_.CString(), fileName.CString());
        }

        return true;
    }

    return false;

}

bool ModelImporter::ImportAnimations()
{
    if (!animationInfo_.Size())
    {
        if (!ImportAnimation(asset_->GetPath(), "RootAnim"))
            return false;
    }

    // embedded animations
    for (unsigned i = 0; i < animationInfo_.Size(); i++)
    {
        const SharedPtr<AnimationImportInfo>& info = animationInfo_[i];
        if (!ImportAnimation(asset_->GetPath(), info->GetName(), info->GetStartTime(), info->GetEndTime()))
            return false;
    }

    // add @ animations

    FileSystem* fs = GetSubsystem<FileSystem>();
    String pathName, fileName, ext;
    SplitPath(asset_->GetPath(), pathName, fileName, ext);

    Vector<String> results;

    fs->ScanDir(results, pathName, ext, SCAN_FILES, false);

    for (unsigned i = 0; i < results.Size(); i++)
    {
        const String& result = results[i];

        if (result.Contains("@"))
        {
            Vector<String> components = GetFileName(result).Split('@');

            if (components.Size() == 2 && components[1].Length() && components[0] == fileName)
            {
                String animationName = components[1];
                AssetDatabase* db = GetSubsystem<AssetDatabase>();
                Asset* asset = db->GetAssetByPath(pathName + result);
                assert(asset);
                assert(asset->GetImporter()->GetType() == ModelImporter::GetTypeStatic());

                ModelImporter* importer = (ModelImporter*) asset->GetImporter();

                if (!importer->animationInfo_.Size())
                {
                    if (!ImportAnimation(asset->GetPath(), animationName))
                        return false;
                }
                else
                {
                    // embedded animations
                    for (unsigned i = 0; i < importer->animationInfo_.Size(); i++)
                    {
                        const SharedPtr<AnimationImportInfo>& info = importer->animationInfo_[i];
                        if (!ImportAnimation(asset->GetPath(), info->GetName(), info->GetStartTime(), info->GetEndTime()))
                            return false;
                    }
                }


            }
        }
    }



    return true;
}

bool ModelImporter::Import()
{

    String ext = asset_->GetExtension();
    String modelAssetFilename = asset_->GetPath();

    importNode_ = new Node(context_);

    if (ext == ".mdl")
    {
        FileSystem* fs = GetSubsystem<FileSystem>();
        ResourceCache* cache = GetSubsystem<ResourceCache>();

        // mdl files are native file format that doesn't need to be converted
        // doesn't allow scale, animations legacy primarily for ToonTown

        if (!fs->Copy(asset_->GetPath(), asset_->GetCachePath() + ".mdl"))
        {
            importNode_= 0;
            return false;
        }

        Model* mdl = cache->GetResource<Model>( asset_->GetCachePath() + ".mdl");

        if (!mdl)
        {
            importNode_= 0;
            return false;
        }

        // Force a reload, though file watchers will catch this delayed and load again
        cache->ReloadResource(mdl);

        importNode_->CreateComponent<StaticModel>()->SetModel(mdl);
    }
    else
    {
        // skip external animations, they will be brought in when importing their
        // corresponding model

        if (!modelAssetFilename.Contains("@"))
        {
            ImportModel();

            if (importAnimations_)
            {
                ImportAnimations();
            }

            SetImportMaterials(importMaterials_);
        }
    }

    File outFile(context_);

    if (!outFile.Open(asset_->GetCachePath(), FILE_WRITE))
        ErrorExit("Could not open output file " + asset_->GetCachePath());

    importNode_->SaveXML(outFile);

    importNode_ = 0;

    return true;
}

unsigned ModelImporter::GetAnimationCount()
{
    return animationInfo_.Size();
}

void ModelImporter::SetAnimationCount(unsigned count)
{
    if (animationInfo_.Size() >= count)
    {
        animationInfo_.Resize(count);
    }
    else
    {
        for (unsigned i = animationInfo_.Size(); i < count; i++)
        {
            SharedPtr<AnimationImportInfo> info(new AnimationImportInfo(context_));
            animationInfo_.Push(info);
        }

    }

}

void ModelImporter::GetAnimations(PODVector<Animation*>& animations)
{
    animations.Clear();

    SharedPtr<File> file(new File(context_, asset_->GetCachePath()));
    SharedPtr<XMLFile> xml(new XMLFile(context_));

    if (!xml->Load(*file))
        return;

    SharedPtr<Node> node(new Node(context_));
    node->LoadXML(xml->GetRoot());

    AnimationController* controller = node->GetComponent<AnimationController>();

    if (!controller)
        return;

    const Vector<SharedPtr<Animation>>& animresources = controller->GetAnimationResources();

    for (unsigned i = 0; i < animresources.Size(); i++)
    {
        if (animresources[i].NotNull())
        {
            animations.Push(animresources[i]);
        }
    }

}

bool ModelImporter::LoadSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::LoadSettingsInternal(jsonRoot))
        return false;

    JSONValue import = jsonRoot.Get("ModelImporter");

    SetDefaults();

    if (import.Get("scale").IsNumber())
        scale_ = import.Get("scale").GetDouble();

    if (import.Get("importAnimations").IsBool())
        importAnimations_ = import.Get("importAnimations").GetBool();

    if (import.Get("importMaterials").IsBool())
    {
        importMaterials_ = import.Get("importMaterials").GetBool();
    }
    else
    {

    }

    if (import.Get("animInfo").IsArray())
    {
        JSONArray animInfo = import.Get("animInfo").GetArray();
        for (unsigned i = 0; i < animInfo.Size(); i++)
        {
            JSONValue anim = animInfo[i];

            SharedPtr<AnimationImportInfo> info(new AnimationImportInfo(context_));

            info->name_ = anim.Get("name").GetString();
            info->SetStartTime(anim.Get("startTime").GetFloat());
            info->SetEndTime(anim.Get("endTime").GetFloat());

            animationInfo_.Push(info);

        }
    }

    return true;
}

bool ModelImporter::SaveSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::SaveSettingsInternal(jsonRoot))
        return false;

    JSONValue save;
    save.Set("scale", scale_);
    save.Set("importAnimations", importAnimations_);
    save.Set("importMaterials", importMaterials_);

    JSONArray animInfo;

    for (unsigned i = 0; i < animationInfo_.Size(); i++)
    {
        const SharedPtr<AnimationImportInfo>& info = animationInfo_[i];
        JSONValue jinfo;
        jinfo.Set("name", info->GetName());
        jinfo.Set("startTime", info->GetStartTime());
        jinfo.Set("endTime", info->GetEndTime());
        animInfo.Push(jinfo);
    }

    save.Set("animInfo", animInfo);

    jsonRoot.Set("ModelImporter", save);

    return true;
}

Resource* ModelImporter::GetResource(const String& typeName)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    Model* model = cache->GetResource<Model>(asset_->GetCachePath() + ".mdl");

    return model;

}

Node* ModelImporter::InstantiateNode(Node* parent, const String& name)
{
    SharedPtr<File> file(new File(context_, asset_->GetCachePath()));
    SharedPtr<XMLFile> xml(new XMLFile(context_));

    if (!xml->Load(*file))
        return 0;

    Node* node = parent->CreateChild(name);

    node->LoadXML(xml->GetRoot());
    node->SetName(asset_->GetName());

    return node;
}

}
