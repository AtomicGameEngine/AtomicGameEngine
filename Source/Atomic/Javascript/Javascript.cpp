// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicEditor

#include "Precompiled.h"
#include "../Core/CoreEvents.h"
#include "../Engine/EngineEvents.h"
#include "../IO/File.h"
#include "../IO/Log.h"
#include "../Core/ProcessUtils.h"
#include "../Core/Profiler.h"
#include "../Resource/ResourceCache.h"
#include "../Scene/Scene.h"

#include "../Javascript/Javascript.h"
#include "../Javascript/JSComponent.h"
#include "../Javascript/JSVM.h"

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
    JSComponent::RegisterObject(context);
}

}
