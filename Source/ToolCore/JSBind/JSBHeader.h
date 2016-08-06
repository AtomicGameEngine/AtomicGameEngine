//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
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

    ATOMIC_OBJECT(JSBHeader, Object)

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
