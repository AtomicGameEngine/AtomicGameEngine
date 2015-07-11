// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

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
