// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Core/Object.h>
#include "AEExternalTool.h"

using namespace Atomic;

namespace AtomicEditor
{

/// External tools subsystem

class ExternalTooling : public Object {

    OBJECT(ExternalTooling);

public:

    /// Construct.
    ExternalTooling(Context* context);
    /// Destruct.
    virtual ~ExternalTooling();

    String GetToolApplicationPath();

    void LaunchOrOpen(const String& name, const String& fullpath);

    void ToolExited(const String& name);

private:

    void HandleEditorShutdown(StringHash eventType, VariantMap& eventData);

    HashMap<String, SharedPtr<ExternalTool> > tools_;



};

}
