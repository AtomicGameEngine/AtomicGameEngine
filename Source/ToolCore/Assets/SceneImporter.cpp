//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/Core/StringUtils.h>

#include "Asset.h"
#include "AssetDatabase.h"
#include "SceneImporter.h"

namespace ToolCore
{

SceneImporter::SceneImporter(Context* context, Asset* asset) : AssetImporter(context, asset)
{

}

SceneImporter::~SceneImporter()
{

}

void SceneImporter::SetDefaults()
{
    AssetImporter::SetDefaults();

    sceneCamRotation_ = Quaternion::IDENTITY;
    sceneCamPosition_ = Vector3::ZERO;
}

bool SceneImporter::Import()
{
    return true;
}

bool SceneImporter::LoadSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::LoadSettingsInternal(jsonRoot))
        return false;

    JSONValue import = jsonRoot.Get("SceneImporter");

    SetDefaults();

    if (import.Get("sceneCamRotation").IsString())
        sceneCamRotation_ = ToQuaternion(import.Get("sceneCamRotation").GetString());

    if (import.Get("sceneCamPosition").IsString())
        sceneCamPosition_ = ToVector3(import.Get("sceneCamPosition").GetString());

    return true;
}

bool SceneImporter::SaveSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::SaveSettingsInternal(jsonRoot))
        return false;

    JSONValue save;

    save.Set("sceneCamRotation", sceneCamRotation_.ToString());
    save.Set("sceneCamPosition", sceneCamPosition_.ToString());

    jsonRoot.Set("SceneImporter", save);

    return true;
}


}
