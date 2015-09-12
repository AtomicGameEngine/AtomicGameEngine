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

class JavascriptImporter : public AssetImporter
{
    OBJECT(JavascriptImporter);

public:
    /// Construct.
    JavascriptImporter(Context* context, Asset* asset);
    virtual ~JavascriptImporter();

    virtual void SetDefaults();

    bool IsComponentFile() { return isComponentFile_; }

    Resource* GetResource(const String& typeName = String::EMPTY);

protected:

    bool Import();

    bool isComponentFile_;

    virtual bool LoadSettingsInternal(JSONValue& jsonRoot);
    virtual bool SaveSettingsInternal(JSONValue& jsonRoot);

};

}
