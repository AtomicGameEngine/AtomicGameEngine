//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include "AssetImporter.h"

namespace ToolCore
{

class SceneImporter : public AssetImporter
{
    OBJECT(SceneImporter);

public:
    /// Construct.
    SceneImporter(Context* context, Asset *asset);
    virtual ~SceneImporter();

    virtual void SetDefaults();

    /// Set the scene camera's rotation
    void SetSceneCamRotation(const Quaternion& rotation) { sceneCamRotation_ = rotation; }
    /// Set the scene camera's position
    void SetSceneCamPosition(const Vector3& position) { sceneCamPosition_ = position; }

    /// Get the scene camera's rotation
    const Quaternion& GetSceneCamRotation() const { return sceneCamRotation_; }
    /// Get the scene camera's position
    const Vector3& GetSceneCamPosition() const { return sceneCamPosition_; }

protected:

    bool Import();

    virtual bool LoadSettingsInternal(JSONValue& jsonRoot);
    virtual bool SaveSettingsInternal(JSONValue& jsonRoot);

    Quaternion sceneCamRotation_;
    Vector3 sceneCamPosition_;

};

}
