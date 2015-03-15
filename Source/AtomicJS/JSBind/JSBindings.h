// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Container/Vector.h>
#include <Atomic/Container/HashMap.h>
#include "JSBClass.h"

using namespace Atomic;

class JSBModule;

class JSBindings
{
    Vector<JSBModule*> modules_;
    HashMap<String, JSBClass*> classes_;
    HashMap<String, JSBEnum*> enums_;
    HashMap<String, String> enumNames_;

    void EmitJSModules(const String& rootpath);

    static JSBindings* instance_;

public:

    JSBindings()
    {
        instance_ = this;
    }

    static JSBindings* Instance() { return instance_; }

    JSBClass* GetClass(const String& classname)
    {
        if (!classes_.Contains(classname))
            return NULL;

        return classes_[classname];
    }

    JSBEnum* GetEnum(const String& enumname)
    {
        if (!enums_.Contains(enumname))
            return NULL;

        return enums_[enumname];
    }

    unsigned GetModuleCount() { return modules_.Size(); }
    JSBModule* GetModule(unsigned idx) { return modules_.At(idx); }
    JSBModule* GetModuleByName(const String& name);


    void RegisterClass(const String& classname, const String& rename = String::EMPTY);
    void RegisterEnum(JSBEnum* jenum);

    void Initialize();
    void ParseHeaders();

};
