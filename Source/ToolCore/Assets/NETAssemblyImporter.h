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

class NETAssemblyImporter : public AssetImporter
{
    OBJECT(NETAssemblyImporter);

public:
    /// Construct.
    NETAssemblyImporter(Context* context, Asset* asset);
    virtual ~NETAssemblyImporter();

    virtual void SetDefaults();

    Resource* GetResource(const String& typeName = String::EMPTY);

protected:

    bool Import();

    virtual bool LoadSettingsInternal(JSONValue& jsonRoot);
    virtual bool SaveSettingsInternal(JSONValue& jsonRoot);

    JSONValue assemblyJSON_;

};

}
