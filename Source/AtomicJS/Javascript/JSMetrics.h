// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Core/Context.h>
#include <Atomic/Core/Object.h>
#include <Atomic/Container/List.h>

namespace Atomic
{

class JSVM;

class JSMetrics : public Object
{
    OBJECT(JSMetrics);

public:

    /// Construct.
    JSMetrics(Context* context, JSVM* vm);
    /// Destruct.
    virtual ~JSMetrics();

    void Capture();

    void Dump();

private:

    WeakPtr<JSVM> vm_;

    // Object
    List<String> classNames_;
    HashMap<StringHash, int> classInstances_;

};

}
