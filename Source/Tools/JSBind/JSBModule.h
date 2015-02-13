// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Container/Vector.h>
#include <Atomic/Container/Str.h>

using namespace Atomic;

class JSBindings;
class JSBClass;
class JSBHeader;
class JSBEnum;

class JSBModule
{

    void WriteClassDeclaration(String& source);
    void WriteClassDefine(String& source);
    void WriteIncludes(String& source);
    void WriteModulePreInit(String& source);
    void WriteModuleInit(String& source);    

public:

    JSBModule(JSBindings* bindings) :
        bindings_(bindings)
    {

    }

    void Load(const String& moduleJSONFilename);
    void AddClass(JSBClass* klass) { classes_.Push(klass); }

    void ParseHeaders();
    void PreprocessHeaders();
    void VisitHeaders();

    void EmitSource(const String& filepath);

    void RegisterConstant(const String& name)
    {
        if (!constants_.Contains(name))
            constants_.Push(name);
    }

    String name_;
    Vector<String> headerFiles_;

    Vector<String> includes_;
    Vector<JSBHeader*> headers_;
    Vector<JSBClass*> classes_;
    Vector<JSBEnum*> enums_;
    Vector<String> constants_;
    JSBindings* bindings_;

    String source_;

};
