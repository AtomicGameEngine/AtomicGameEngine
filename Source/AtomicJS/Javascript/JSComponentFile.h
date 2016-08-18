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

#include <Atomic/Resource/Resource.h>
#include <Atomic/Container/ArrayPtr.h>
#include <Atomic/Container/List.h>

#include <Atomic/Script/ScriptComponentFile.h>

namespace Atomic
{

class JSComponent;

/// Script document resource.
class ATOMIC_API JSComponentFile : public ScriptComponentFile
{
    ATOMIC_OBJECT(JSComponentFile, ScriptComponentFile);

public:

    /// Construct.
    JSComponentFile(Context* context);
    /// Destruct.
    virtual ~JSComponentFile();
    /// Register object factory.
    static void RegisterObject(Context* context);

    /// Load resource from stream. May be called from a worker thread. Return true if successful.
    virtual bool BeginLoad(Deserializer& source);
    /// Save resource
    virtual bool Save(Serializer& dest) const;

    bool GetScriptClass() { return scriptClass_; }

    SharedPtr<JSComponent> CreateJSComponent();
    bool PushModule();

private:

    bool InitModule();

    bool scriptClass_;

};

}
