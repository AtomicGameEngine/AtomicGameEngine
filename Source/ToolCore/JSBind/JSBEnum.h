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

#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace ToolCore
{

class JSBPackage;
class JSBModule;
class JSBHeader;


class JSBEnum : public Object
{

    ATOMIC_OBJECT(JSBEnum, Object)

public:

    JSBEnum(Context* context, JSBModule* module, const String& name);
    virtual ~JSBEnum();

    const String& GetName() const { return name_; }
    JSBHeader* GetHeader() { return header_; }

    JSBPackage* GetPackage();

    HashMap<String, String>& GetValues() { return values_; }

    void SetHeader(JSBHeader* header) { header_ = header; }

    void Preprocess();

    void AddValue(const String& name, const String& constantValue = String::EMPTY) { values_[name] = constantValue; }

private:

    String name_;

    SharedPtr<JSBModule> module_;

    HashMap<String, String> values_;

    JSBHeader* header_;

};


}
