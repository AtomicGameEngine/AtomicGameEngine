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

#ifdef USE_SPIDERMONKEY
#pragma once

#include "Object.h"

using namespace Atomic;

struct JSRuntime;
struct JSContext;
class JSAutoRequest;
class JSAutoCompartment;

namespace AtomicEditor
{

// Notes: SpiderMonkey Reflect API is giving some bad loc's, which seems to be an issue with the
// C++ generator not passing in tokens for some nodes (MemberExpression.property for example)
// probably should use http://esprima.org/
class JSSpiderMonkeyVM : public Object
{
    OBJECT(JSSpiderMonkeyVM);

public:
    /// Construct.
    JSSpiderMonkeyVM(Context* context);
    /// Destruct.
    ~JSSpiderMonkeyVM();

    bool ExecuteFile(const String& path);

    bool ParseJavascriptToJSON(const char* source, String& json);

    bool ParseJavascriptToJSONWithSpiderMonkeyReflectAPI(const char* source, String& json);

private:

    bool ReadZeroTerminatedSourceFile(const String& path, String& source);

    // for access within duktape callbacks
    static WeakPtr<JSSpiderMonkeyVM> instance_;

    JSRuntime* runtime_;
    JSContext* jscontext_;
    JSAutoRequest* autorequest_;
    JSAutoCompartment* autocompartment_;

};


}

#endif
