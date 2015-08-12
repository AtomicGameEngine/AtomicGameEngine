
#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Scene/Node.h>

#include <Atomic/Atomic3D/AnimationController.h>
#include <Atomic/Atomic3D/Animation.h>
#include <Atomic/Atomic3D/Model.h>

#include <Atomic/Resource/ResourceCache.h>

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

    scale_ = 1.0f;
    importAnimations_ = false;
    animationInfo_.Clear();

}

bool ModelImporter::ImportModel()
{
    SharedPtr<OpenAssetImporter> importer(new OpenAssetImporter(context_));

    //importer->SetVerboseLog(true);

    importer->SetScale(scale_);
    importer->SetExportAnimations(false);
    importer->SetImportNode(importNode_);

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

            AnimationController* controller = importNode_->GetComponent<AnimationController>();

            if (controller)
            {
                SharedPtr<Animation> animation = cache->GetTempResource<Animation>(fileName + extension);
                if (animation)
                    controller->AddAnimationResource(animation);
            }

            LOGINFOF("Import Info: %s : %s", info.name_.CString(), fileName.CString());
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
    String modelAssetFilename = asset_->GetPath();

    importNode_ = new Node(context_);

    // skip external animations, they will be brought in when importing their
    // corresponding model
    if (!modelAssetFilename.Contains("@"))
    {
        ImportModel();

        if (importAnimations_)
        {
            ImportAnimations();
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

bool ModelImporter::LoadSettingsInternal()
{
    if (!AssetImporter::LoadSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.GetChild("ModelImporter", JSON_OBJECT);

    SetDefaults();

    if (import.HasMember("scale"))
        scale_ = import.GetFloat("scale");

    if (import.HasMember("importAnimations"))
        importAnimations_ = import.GetBool("importAnimations");

    if (import.HasMember("animInfo"))
    {
        JSONValue animInfo = import.GetChild("animInfo");
        for (unsigned i = 0; i < animInfo.GetSize(); i++)
        {
            JSONValue anim = animInfo.GetChild(i);

            SharedPtr<AnimationImportInfo> info(new AnimationImportInfo(context_));

            info->name_ = anim.GetString("name");
            info->SetStartTime(anim.GetFloat("startTime"));
            info->SetEndTime(anim.GetFloat("endTime"));

            animationInfo_.Push(info);

        }
    }

    return true;
}

bool ModelImporter::SaveSettingsInternal()
{
    if (!AssetImporter::SaveSettingsInternal())
        return false;

    JSONValue save = jsonRoot_.CreateChild("ModelImporter");

    save.SetFloat("scale", scale_);
    save.SetBool("importAnimations", importAnimations_);

    JSONValue animInfo = save.CreateChild("animInfo", JSON_ARRAY);

    for (unsigned i = 0; i < animationInfo_.Size(); i++)
    {
        const SharedPtr<AnimationImportInfo>& info = animationInfo_[i];
        JSONValue jinfo = animInfo.CreateChild();
        jinfo.SetString("name", info->GetName());
        jinfo.SetFloat("startTime", info->GetStartTime());
        jinfo.SetFloat("endTime", info->GetEndTime());
    }

    return true;
}

Resource* ModelImporter::GetResource()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    Model* model = cache->GetResource<Model>(asset_->GetCachePath() + ".mdl");

    return model;

}


}
