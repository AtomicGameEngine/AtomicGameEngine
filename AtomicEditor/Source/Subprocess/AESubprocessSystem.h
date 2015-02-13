// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicEditor

#pragma once

#include <Atomic/Core/Object.h>

#include "AESubprocess.h"

using namespace Atomic;

namespace AtomicEditor
{

class SubprocessSystem : public Object
{
    OBJECT(SubprocessSystem);

public:
    /// Construct.
    SubprocessSystem(Context* context);
    /// Destruct.
    virtual ~SubprocessSystem();


    Subprocess* Launch(const String& command, const Vector<String>& args, const String& initialDirectory = "");

private:

    void HandleUpdate(StringHash eventType, VariantMap& eventData);

    Vector<SharedPtr<Subprocess> > processes_;
    float updateTimer_;

};

}
