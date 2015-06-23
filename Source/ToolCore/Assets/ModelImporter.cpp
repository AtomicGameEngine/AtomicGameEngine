
#include <Atomic/IO/Log.h>

#include "../Import/OpenAssetImporter.h"

#include "Asset.h"
#include "AssetDatabase.h"
#include "ModelImporter.h"

namespace ToolCore
{

/// Node + Model (static or animated)
ModelImporter::ModelImporter(Context* context) : AssetImporter(context)
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
    startTime_ = -1.0f;
    endTime_ = -1.0f;
    importAnimations_ = false;

}

bool ModelImporter::Import(const String& guid)
{
    AssetDatabase* db = GetSubsystem<AssetDatabase>();
    Asset* asset = db->GetAssetByGUID(guid);

    if (!asset)
        return false;

    SharedPtr<OpenAssetImporter> importer(new OpenAssetImporter(context_));

    //importer->SetVerboseLog(true);

    bool animationsOnly = false;

    String modelAssetFilename = asset->GetPath();

    if (modelAssetFilename.Contains("@"))
    {
        animationsOnly = true;
    }

    importer->SetScale(scale_);
    importer->SetStartTime(startTime_);
    importer->SetEndTime(endTime_);
    importer->SetExportAnimations(importAnimations_);

    if (importer->Load(asset->GetPath()))
    {
        String cachePath = db->GetCachePath();

        cachePath += guid;

        importer->ExportModel(cachePath, animationsOnly);

        const Vector<OpenAssetImporter::AnimationInfo>& infos = importer->GetAnimationInfos();

        for (unsigned i = 0; i < infos.Size(); i++)
        {
            const OpenAssetImporter::AnimationInfo& info = infos.At(i);

            LOGINFOF("Import Info: %s : %s", info.name_.CString(), info.cacheFilename_.CString());
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool ModelImporter::LoadSettingsInternal()
{
    if (!AssetImporter::LoadSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.GetChild("ModelImporter", JSON_OBJECT);

    SetDefaults();

    if (import.HasMember("scale"))
        scale_ = import.GetFloat("scale");
    if (import.HasMember("startTime"))
        startTime_ = import.GetFloat("startTime");
    if (import.HasMember("endTime"))
        endTime_ = import.GetFloat("endTime");
    if (import.HasMember("importAnimations"))
        importAnimations_ = import.GetBool("importAnimations");

    return true;
}

bool ModelImporter::SaveSettingsInternal()
{
    if (!AssetImporter::SaveSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.CreateChild("ModelImporter");

    import.SetFloat("scale", scale_);
    import.SetFloat("startTime", startTime_);
    import.SetFloat("endTime", endTime_);
    import.SetBool("importAnimations", importAnimations_);

    return true;
}


}
