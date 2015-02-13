// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#ifdef USE_SPIDERMONKEY

#include "AtomicEditor.h"
#include "/Users/josh/Desktop/SpiderMonkey/include/mozjs-37a1/jsapi.h"
#include "js/RootingAPI.h"
#include "JSSpiderMonkeyVM.h"

using namespace JS;

#include "Context.h"
#include "FileSystem.h"
#include "ResourceCache.h"
#include "JSFile.h"

static RootedObject* __global = NULL;

// The class of the global object.
static JSClass globalClass = {
    "global",
    JSCLASS_GLOBAL_FLAGS,
    JS_PropertyStub,
    JS_DeletePropertyStub,
    JS_PropertyStub,
    JS_StrictPropertyStub,
    JS_EnumerateStub,
    JS_ResolveStub,
    JS_ConvertStub,
    nullptr, nullptr, nullptr, nullptr,
    JS_GlobalObjectTraceHook
};

// The error reporter callback.
void reportError(JSContext *cx, const char *message, JSErrorReport *report) {
    fprintf(stderr, "%s:%u:%s\n",
            report->filename ? report->filename : "[no filename]",
            (unsigned int) report->lineno,
            message);
}


namespace AtomicEditor
{



WeakPtr<JSSpiderMonkeyVM> JSSpiderMonkeyVM::instance_;

static bool
print(JSContext *cx, unsigned argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);

    for (unsigned i = 0; i < args.length(); i++) {
        JSString *str = JS::ToString(cx, args[i]);
        if (!str)
            return false;
        char *bytes = JS_EncodeString(cx, str);
        if (!bytes)
            return false;
        printf("%s%s", i ? " " : "", bytes);
        JS_free(cx, bytes);
    }

    putchar('\n');
    fflush(stdout);
    args.rval().setUndefined();
    return true;
}

JSSpiderMonkeyVM::JSSpiderMonkeyVM(Context* context) :
    Object(context), runtime_(0), jscontext_(0),
    autorequest_(0), autocompartment_(0)
{
    context_->RegisterSubsystem(this);
    instance_ = this;

    // Initialize the JS engine.
    if (!JS_Init())
    {
        assert(0);
    }

    // Create a JS runtime.
    runtime_ = JS_NewRuntime(32L * 1024L * 1024L);

    if (runtime_)
    {
        jscontext_ = JS_NewContext(runtime_, 8192);
        if (jscontext_)
        {
            JS_SetErrorReporter(runtime_, reportError);
        }
        else
        {
            assert(0);
        }

    }
    else
    {
        assert(0);
    }

    // Enter a request before running anything in the context.
    autorequest_ = new JSAutoRequest(jscontext_);

    // Create the global object and a new compartment.
    __global = new RootedObject(jscontext_);
    *__global = JS_NewGlobalObject(jscontext_, &globalClass, nullptr,
                                   JS::DontFireOnNewGlobalHook);
    // Enter the new global object's compartment.
    autocompartment_ = new JSAutoCompartment(jscontext_, *__global);

    // Populate the global object with the standard globals, like Object and
    // Array.
    if (!JS_InitStandardClasses(jscontext_, *__global))
    {
        assert(0);
    }

    if (!JS_InitReflect(jscontext_, *__global))
    {
        assert(0);
    }

    JS_DefineFunction(jscontext_, *__global, "print", (JSNative) print, 0, 0);

    ExecuteFile("AtomicEditor/javascript/modules/acorn_spidermonkey.js");

}

JSSpiderMonkeyVM::~JSSpiderMonkeyVM()
{
    instance_ = NULL;

    if (autocompartment_)
        delete autocompartment_;

    if (__global)
    {
        delete __global;
        __global = NULL;
    }

    if (autorequest_)
        delete autorequest_;

    // Shut everything down.
    if (context_)
        JS_DestroyContext(jscontext_);
    if (runtime_)
        JS_DestroyRuntime(runtime_);

    JS_ShutDown();

}

bool JSSpiderMonkeyVM::ExecuteFile(const String& path)
{
    SharedPtr<JSFile> jsfile;
    jsfile = GetSubsystem<ResourceCache>()->GetResource<JSFile>(path);

    JS::RootedValue returnValue(jscontext_);

    OwningCompileOptions options(jscontext_);

    options.setFileAndLine(jscontext_, path.CString(), 0);
    options.setCompileAndGo(true);
    bool ok = JS::Evaluate(jscontext_, *__global, options,
                           jsfile->GetSource(), strlen(jsfile->GetSource()), &returnValue);

    return ok;

}

bool JSSpiderMonkeyVM::ParseJavascriptToJSON(const char* source, String& json)
{
    json.Clear();

    bool ok = true;

    JS::Rooted<JS::Value> v(jscontext_);
    JS::RootedValue returnValue(jscontext_);
    JS::RootedValue jsource(jscontext_, JS::StringValue(JS_NewStringCopyZ(jscontext_, source)));
    JS_CallFunctionName(jscontext_, *__global, "__atomic_parse_to_json", HandleValueArray(jsource), &returnValue);

    JS::RootedString hm(jscontext_);
    hm = JS::ToString(jscontext_, returnValue);

    const char* strjson = JS_EncodeStringToUTF8(jscontext_, hm);

    json = strjson;

    //printf("%s\n", strjson);

    JS_free(jscontext_, (void *) strjson);

    return ok;

}


bool JSSpiderMonkeyVM::ParseJavascriptToJSONWithSpiderMonkeyReflectAPI(const char* source, String& json)
{
    json.Clear();

    bool ok = true;

    JS::Rooted<JS::Value> v(jscontext_);
    JS::RootedValue returnValue(jscontext_);
    JS::RootedValue jsource(jscontext_, JS::StringValue(JS_NewStringCopyZ(jscontext_, source)));

    // get the Reflect object
    if (!JS_GetProperty(jscontext_, *__global, "Reflect", &v))
        return false;

    JS::RootedObject reflectObject(jscontext_);
    JS_ValueToObject(jscontext_, v, &reflectObject);

    JS_CallFunctionName(jscontext_, reflectObject, "parse", HandleValueArray(jsource), &returnValue);

    JS::RootedObject jsonObject(jscontext_);
    if (!JS_GetProperty(jscontext_, *__global, "JSON", &v))
        return false;

    JS_ValueToObject(jscontext_, v, &jsonObject);

    // pretty printed, can remove for speed
    JS::AutoValueArray<3> args(jscontext_);
    args[0].set(returnValue);
    args[1].set(JS::NullValue());
    args[2].setNumber(3.0);

    JS::RootedValue jjson(jscontext_);
    JS_CallFunctionName(jscontext_, jsonObject, "stringify", HandleValueArray(args), &jjson);

    JS::RootedString hm(jscontext_);
    hm = JS::ToString(jscontext_, jjson);

    const char* strjson = JS_EncodeStringToUTF8(jscontext_, hm);

    json = strjson;

    //printf("%s\n", strjson);

    JS_free(jscontext_, (void *) strjson);

    return ok;

}

bool JSSpiderMonkeyVM::ReadZeroTerminatedSourceFile(const String& path, String& source)
{
    File file(instance_->GetContext());
    file.Open(path);

    unsigned size = file.GetSize();

    SharedArrayPtr<char> data;
    data = new char[size + 1];
    data[size] = '\0';
    file.Read(data, size);

    source = data;

    return true;

}


}

int run(JSContext *cx) {
    // Enter a request before running anything in the context.
    JSAutoRequest ar(cx);

    // Create the global object and a new compartment.
    RootedObject global(cx);
    global = JS_NewGlobalObject(cx, &globalClass, nullptr,
                                JS::DontFireOnNewGlobalHook);
    if (!global)
        return 1;

    // Enter the new global object's compartment.
    JSAutoCompartment ac(cx, global);

    // Populate the global object with the standard globals, like Object and
    // Array.
    if (!JS_InitStandardClasses(cx, global))
        return 1;

    // Your application code here. This may include JSAPI calls to create your
    // own custom JS objects and run scripts.

    return 0;
}

#endif

