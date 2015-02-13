// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include "AEExternalTool.h"

using namespace Atomic;

namespace AtomicEditor
{

class AtomicTiled : public ExternalTool
{

    OBJECT(AtomicTiled);

public:

    /// Construct.
    AtomicTiled(Context* context);
    /// Destruct.
    virtual ~AtomicTiled();

    void Open(const String& fullpath);

    bool Launch(const String& args);

protected:

    void HandleEvent(StringHash eventType, VariantMap& eventData);

};

}
