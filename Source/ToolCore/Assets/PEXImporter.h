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

class PEXImporter : public AssetImporter
{
    OBJECT(PEXImporter);

public:
    /// Construct.
    PEXImporter(Context* context, Asset* asset);
    virtual ~PEXImporter();

    virtual void SetDefaults();

    Resource* GetResource(const String& typeName);

protected:

    bool Import();

    virtual bool LoadSettingsInternal(JSONValue& jsonRoot);
    virtual bool SaveSettingsInternal(JSONValue& jsonRoot);

};

}
