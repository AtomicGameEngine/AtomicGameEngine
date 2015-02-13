// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

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
