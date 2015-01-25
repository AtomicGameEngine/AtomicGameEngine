
#pragma once

#include <Atomic/Container/Vector.h>
#include <Atomic/Container/Str.h>
#include <Atomic/Container/ArrayPtr.h>

using namespace Atomic;

namespace CPlusPlus
{
    class TranslationUnit;
    class Namespace;
}

class JSBModule;

class JSBHeader
{
    CPlusPlus::TranslationUnit* translationUnit_;
    CPlusPlus::Namespace* globalNamespace_;

    SharedArrayPtr<char> data_;

public:

    JSBHeader(JSBModule* module, const String& filepath) :
        translationUnit_(NULL),
        filepath_(filepath),
        module_(module)
    {

    }

    void Parse();

    void VisitPreprocess();
    void VisitHeader();

    // absolute path to source file
    String filepath_;
    JSBModule* module_;

};
