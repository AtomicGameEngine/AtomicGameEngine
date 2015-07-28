// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Container/Str.h>
#include <Atomic/Core/Object.h>

#include "BuildSettings.h"

using namespace Atomic;

namespace AtomicEditor
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
    AndroidBuildSettings buildSettings_;

};

}
