//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include <Atomic/Core/Context.h>
#include <Atomic/Core/Object.h>
#include "JSVM.h"

namespace Atomic
{

class JSVM;

/// Javascript subsystem.
class ATOMIC_API Javascript : public Object
{
    ATOMIC_OBJECT(Javascript, Object);

public:
    /// Construct.
    Javascript(Context* context);
    /// Destruct.
    virtual ~Javascript();

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
