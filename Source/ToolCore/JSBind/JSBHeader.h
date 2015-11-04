//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Container/Vector.h>
#include <Atomic/Container/Str.h>
#include <Atomic/Container/ArrayPtr.h>

#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace CPlusPlus
{
    class TranslationUnit;
    class Namespace;
}

namespace ToolCore
{

class JSBModule;

class JSBHeader : public Object
{

    OBJECT(JSBHeader)

public:

    JSBHeader(Context* context, JSBModule* module, const String& filepath);
    virtual ~JSBHeader();

    const String& GetFilePath() { return filepath_; }
    JSBModule* GetModule() { return module_; }

    void Parse();

    void VisitPreprocess();
    void VisitHeader();

private:

    CPlusPlus::TranslationUnit* translationUnit_;
    CPlusPlus::Namespace* globalNamespace_;

    SharedArrayPtr<char> data_;

    // absolute path to source file
    String filepath_;
    SharedPtr<JSBModule> module_;

};

}
