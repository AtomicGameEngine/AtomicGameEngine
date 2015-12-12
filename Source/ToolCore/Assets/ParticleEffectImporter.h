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

class ParticleEffectImporter : public AssetImporter
{
    OBJECT(ParticleEffectImporter);

public:
    /// Construct.
    ParticleEffectImporter(Context* context, Asset* asset);
    virtual ~ParticleEffectImporter();

    virtual void SetDefaults();

    Resource* GetResource(const String& typeName);

protected:

    bool Import();

    virtual bool LoadSettingsInternal(JSONValue& jsonRoot);
    virtual bool SaveSettingsInternal(JSONValue& jsonRoot);

};

}
