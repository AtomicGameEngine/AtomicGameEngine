// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Core/Object.h>

#include <Duktape/duktape.h>

using namespace Atomic;

namespace AtomicEditor
{

class JSErrorChecker;

struct JSError
{
    String fullpath;
    String message;
    int line;
    int column;
    int tokenPos;
};

// This is the Javascript subsystem for the editor
// the embedded player uses a separate VM
class AEJavascript : public Object
{
    OBJECT(AEJavascript);

public:
    /// Construct.
    AEJavascript(Context* context);
    /// Destruct.
    ~AEJavascript();

    bool ExecuteFile(const String& path);

    bool ParseJavascriptToJSON(const char* source, String& json, bool loose = true);
    bool BeautifyJavascript(const char* source, String& output);

    bool CheckJSErrors(bool fullCheck = false);
    const Vector<JSError>& GetJSErrors() { return errors_; }

private:

    bool ReadZeroTerminatedSourceFile(const String& path, String& source);
    void HandleFileChanged(StringHash eventType, VariantMap& eventData);
    void HandleEditorShutdown(StringHash eventType, VariantMap& eventData);

    static int js_module_search(duk_context* ctx);
    static int js_read_source(duk_context* ctx);


    duk_context* ctx_;

    Vector<JSError> errors_;
    SharedPtr<JSErrorChecker> errorChecker_;

    // for access within duktape callbacks
    static WeakPtr<AEJavascript> instance_;

    Vector<String> modifiedJS_;

};


}
