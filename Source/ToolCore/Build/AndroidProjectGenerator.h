//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Container/Str.h>
#include <Atomic/Core/Object.h>

#include "BuildSettings.h"

using namespace Atomic;

namespace ToolCore
{

class AndroidProjectGenerator : public Object
{

    OBJECT(AndroidProjectGenerator);

public:

    AndroidProjectGenerator(Context* context);
    virtual ~AndroidProjectGenerator();

    void SetBuildPath(const String& buildpath) { buildPath_ = buildpath; }

    bool Generate();

    const String& GetErrorText() { return errorText_; }

private:

    bool GenerateAndroidManifest();
    bool GenerateStringXML();
    bool GenerateLocalProperties();
    bool GenerateProjectProperties();
    bool GenerateActivitySource();

    String buildPath_;
    String errorText_;

};

}
