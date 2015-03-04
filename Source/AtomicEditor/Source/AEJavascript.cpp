// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <Duktape/duktape.h>

#include <Atomic/IO/Log.h>
#include <Atomic/Core/Context.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/ResourceEvents.h>
#include <Atomic/Resource/ResourceCache.h>

#include "AEEditor.h"
#include "AEEvents.h"
#include "Project/AEProject.h"

#include "AEJavascript.h"
#include "Javascript/JSErrorChecker.h"

namespace AtomicEditor
{

WeakPtr<AEJavascript> AEJavascript::instance_;

AEJavascript::AEJavascript(Context* context) :
    Object(context)
{
    context->RegisterSubsystem(this);
    instance_ = this;
    ctx_ = duk_create_heap_default();

    duk_get_global_string(ctx_, "Duktape");
    duk_push_c_function(ctx_, js_module_search, 1);
    duk_put_prop_string(ctx_, -2, "modSearch");
    duk_push_c_function(ctx_, js_read_source, 1);
    duk_put_prop_string(ctx_, -2, "readSource");
    duk_pop(ctx_);

    errorChecker_ = new JSErrorChecker(context, ctx_);

    SubscribeToEvent(E_FILECHANGED, HANDLER(AEJavascript, HandleFileChanged));
    SubscribeToEvent(E_EDITORSHUTDOWN, HANDLER(AEJavascript, HandleEditorShutdown));

    ExecuteFile("AtomicEditor/javascript/AtomicEditor.js");
}

AEJavascript::~AEJavascript()
{
    duk_destroy_heap(ctx_);
    instance_ = NULL;
}

bool AEJavascript::ExecuteFile(const String& path)
{

    SharedPtr<File> file (GetSubsystem<ResourceCache>()->GetFile(path));

    if (file.Null())
        return false;

    String source;

    file->ReadText(source);

    duk_push_string(ctx_, file->GetFullPath().CString());
    if (duk_eval_raw(ctx_, source.CString(), 0,
                     DUK_COMPILE_EVAL | DUK_COMPILE_SAFE | DUK_COMPILE_NOSOURCE | DUK_COMPILE_STRLEN) != 0)
    {
        if (duk_is_object(ctx_, -1))
        {
            LOGERRORF("Error: %s\n", duk_safe_to_string(ctx_, -1));
        }

        assert(0);

        duk_pop(ctx_);
        return false;
    }

    duk_pop(ctx_);

    return true;

}

void AEJavascript::HandleFileChanged(StringHash eventType, VariantMap& eventData)
{

    Editor* editor = GetSubsystem<Editor>();
    Project* project = editor->GetProject();

    if (!project)
        return;

    using namespace FileChanged;
    const String& fileName = eventData[P_FILENAME].GetString();
    //const String& resourceName = eventData[P_RESOURCENAME].GetString();

    if (GetExtension(fileName) != ".js")
        return;

    if (modifiedJS_.Contains(fileName))
        return;

    String componentsPath = AddTrailingSlash(project->GetComponentsPath());
    String scriptsPath = AddTrailingSlash(project->GetScriptsPath());
    String modulesPath = AddTrailingSlash(project->GetModulesPath());

    if (fileName.StartsWith(componentsPath) || fileName.StartsWith(scriptsPath) || fileName.StartsWith(modulesPath))
    {
        modifiedJS_.Push(fileName);
    }

}

bool AEJavascript::CheckJSErrors(bool fullCheck)
{
    errors_.Clear();

    Editor* editor = GetSubsystem<Editor>();
    Project* project = editor->GetProject();

    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    if (!project)
    {
        modifiedJS_.Clear();
        return true;
    }

    Vector<String>& filesToCheck = modifiedJS_;

    Vector<String> allFiles;

    if (fullCheck)
    {
        filesToCheck = allFiles;

        String componentsPath = AddTrailingSlash(project->GetComponentsPath());
        String scriptsPath = AddTrailingSlash(project->GetScriptsPath());
        String modulesPath = AddTrailingSlash(project->GetModulesPath());

        Vector<String> files;
        fileSystem->ScanDir(files, componentsPath, "*.js", SCAN_FILES, true );
        for (unsigned i = 0; i < files.Size(); i++)
            allFiles.Push(componentsPath + files[i]);

        fileSystem->ScanDir(files, scriptsPath, "*.js", SCAN_FILES, true );
        for (unsigned i = 0; i < files.Size(); i++)
            allFiles.Push(scriptsPath + files[i]);

        fileSystem->ScanDir(files, modulesPath, "*.js", SCAN_FILES, true );
        for (unsigned i = 0; i < files.Size(); i++)
            allFiles.Push(modulesPath + files[i]);

    }

    bool ok = true;

    for (unsigned j = 0; j < filesToCheck.Size(); j++)
    {
        String source;
        String fullpath = filesToCheck[j];
        ReadZeroTerminatedSourceFile(fullpath, source);

        duk_get_global_string(ctx_, "__atomic_parse_error_check");
        duk_push_string(ctx_, source.CString());

        if (duk_pcall(ctx_, 1))
        {
            printf("Error: %s\n", duk_safe_to_string(ctx_, -1));
        }
        else
        {
            if (duk_is_boolean(ctx_, -1))
            {
                // error
                if (duk_to_boolean(ctx_, -1))
                    ok = false;
            }
            else if (duk_is_object(ctx_, -1))
            {
                ok = false;
                JSError error;

                error.fullpath = fullpath;

                duk_get_prop_string(ctx_, -1, "message");
                error.message = duk_to_string(ctx_, -1);
                duk_pop(ctx_);

                duk_get_prop_string(ctx_, -1, "loc");
                duk_get_prop_string(ctx_, -1, "line");
                error.line = duk_to_number(ctx_, -1);
                duk_get_prop_string(ctx_, -2, "column");
                error.column = duk_to_number(ctx_, -1);
                duk_get_prop_string(ctx_, -4, "raisedAt");
                error.tokenPos = duk_to_number(ctx_, -1);

                duk_pop_3(ctx_);

                errors_.Push(error);
            }
            else
            {
                // what to do?
            }

        }

        // ignore result
        duk_pop(ctx_);

    }

    modifiedJS_.Clear();


    return !ok;

}

bool AEJavascript::ParseJavascriptToJSON(const char* source, String& json, bool loose)
{
    json.Clear();

    // forcing loose to false, as issues with MSVC release - 3/3/2015 First Early Access Release
    loose = false;

    duk_get_global_string(ctx_, loose ? "__atomic_parse_to_json_loose" : "__atomic_parse_to_json");
    duk_push_string(ctx_, source);
    bool ok = true;

    if (duk_pcall(ctx_, 1))
    {
        ok = false;
        printf("Error: %s\n", duk_safe_to_string(ctx_, -1));
    }
    else
    {
        json = duk_to_string(ctx_, -1);
    }

    // ignore result
    duk_pop(ctx_);

    return ok;

}

bool AEJavascript::BeautifyJavascript(const char* source, String& output)
{
    output.Clear();

    duk_get_global_string(ctx_, "__atomic_js_beautify");
    duk_push_string(ctx_, source);
    bool ok = true;

    if (duk_pcall(ctx_, 1))
    {
        ok = false;
        printf("Error: %s\n", duk_safe_to_string(ctx_, -1));
    }
    else
    {
        output = duk_to_string(ctx_, -1);
    }

    // ignore result
    duk_pop(ctx_);

    return ok;

}

bool AEJavascript::ReadZeroTerminatedSourceFile(const String& path, String& source)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    SharedPtr<File> file = cache->GetFile(path);

    if (file.Null() || !file->IsOpen())
        return false;

    file->ReadText(source);

    return true;

}

int AEJavascript::js_read_source(duk_context* ctx)
{
    String path = duk_to_string(ctx, 0);

    String source;

    instance_->ReadZeroTerminatedSourceFile(path, source);

    duk_push_string(ctx, source.CString());

    return 1;

}

int AEJavascript::js_module_search(duk_context* ctx)
{
    String path = duk_to_string(ctx, 0);

    path = "AtomicEditor/javascript/modules/" + path + ".js";

    String source;

    instance_->ReadZeroTerminatedSourceFile(path, source);

    duk_push_string(ctx, source.CString());

    return 1;
}

void AEJavascript::HandleEditorShutdown(StringHash eventType, VariantMap& eventData)
{
    context_->RemoveSubsystem(GetType());
}

}
