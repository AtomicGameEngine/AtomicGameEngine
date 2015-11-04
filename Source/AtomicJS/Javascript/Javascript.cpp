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

#include <Atomic/Core/CoreEvents.h>
#include <Atomic/Engine/EngineEvents.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/Core/Profiler.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Scene/Scene.h>

#include "Javascript.h"
#include "JSComponent.h"
#include "JSComponentFile.h"
#include "JSVM.h"

namespace Atomic
{

Javascript::Javascript(Context* context) :
    Object(context)
{
    RegisterJavascriptLibrary(context_);
}

Javascript::~Javascript()
{

}

JSVM* Javascript::InstantiateVM(const String& name)
{
    if (vms_.Contains(name))
        return NULL;

    JSVM* vm = new JSVM(context_);
    vms_[name] = vm;
    return vm;

}

void Javascript::ShutdownVM(const String& name)
{
    vms_.Erase(name);
}

void RegisterJavascriptLibrary(Context* context)
{
    JSComponentFile::RegisterObject(context);
    JSComponent::RegisterObject(context);
}

}
