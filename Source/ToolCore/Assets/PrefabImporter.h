//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include "AssetImporter.h"

namespace Atomic
{
class Scene;
}

namespace ToolCore
{

class PrefabImporter : public AssetImporter
{
    OBJECT(PrefabImporter);

public:
    /// Construct.
    PrefabImporter(Context* context, Asset* asset);
    virtual ~PrefabImporter();

    virtual void SetDefaults();

    virtual bool Preload();

    /// Instantiate a node from the asset
    Node* InstantiateNode(Node* parent, const String& name);

protected:

    bool Import();

    virtual bool LoadSettingsInternal(JSONValue& jsonRoot);
    virtual bool SaveSettingsInternal(JSONValue& jsonRoot);

private:

    void HandlePrefabSave(StringHash eventType, VariantMap& eventData);

   SharedPtr<Atomic::Scene> preloadResourceScene_;

};

}
