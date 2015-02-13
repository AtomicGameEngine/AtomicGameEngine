// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicEditor

#pragma once

#include "../Core/Context.h"
#include "../Core/Object.h"
#include "../Javascript/JSVM.h"

namespace Atomic
{

class JSVM;

/// Javascript subsystem.
class ATOMIC_API Javascript : public Object
{
    OBJECT(Javascript);

public:
    /// Construct.
    Javascript(Context* context);
    /// Destruct.
    ~Javascript();

    /// Returns NULL if a VM with name already exists
    JSVM* InstantiateVM(const String& name);

    /// Gets a VM with the given name
    inline JSVM* GetVM(const String& name)
    {
        return vms_[name];
    }

    /// Closes the VM with the given name
    void ShutdownVM(const String& name);

private:

    HashMap<String, SharedPtr<JSVM> > vms_;

};

/// Register Javascript library objects.
void ATOMIC_API RegisterJavascriptLibrary(Context* context);

}
