//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
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

#include <Atomic/Core/Context.h>
#include <Atomic/IO/Deserializer.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Core/Profiler.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/IO/Serializer.h>

#include "JSComponentFile.h"
#include "JSComponent.h"
#include "JSVM.h"

namespace Atomic
{

JSComponentFile::JSComponentFile(Context* context) :
    ScriptComponentFile(context),
    scriptClass_(false)
{
}

JSComponentFile::~JSComponentFile()
{

}

void JSComponentFile::RegisterObject(Context* context)
{
    context->RegisterFactory<JSComponentFile>();
}


bool JSComponentFile::PushModule()
{
    if (context_->GetEditorContext())
        return false;

    JSVM* vm = JSVM::GetJSVM(NULL);

    duk_context* ctx = vm->GetJSContext();

    const String& path = GetName();
    String pathName, fileName, ext;
    SplitPath(path, pathName, fileName, ext);

    if (path.Contains('/') || path.Contains('\\'))
        pathName += "/" + fileName;
    else
        pathName = fileName;

    duk_get_global_string(ctx, "require");
    duk_push_string(ctx, pathName.CString());

    if (duk_pcall(ctx, 1) != 0)
    {
        vm->SendJSErrorEvent();
        return false;
    }

    return true;

}

SharedPtr<JSComponent> JSComponentFile::CreateJSComponent()
{
    SharedPtr<JSComponent> component;

    if (!scriptClass_)
    {
        component =  new JSComponent(context_);
    }
    else
    {

        JSVM* vm = JSVM::GetJSVM(NULL);
        duk_context* ctx = vm->GetJSContext();

        PushModule();

        // Check to see if the module exposes a "default" constructor which is used by TS and ES2015
        duk_get_prop_string(ctx, -1, "default");
        if (!duk_is_function(ctx, -1))
        {
            // We are not a "default" style object, so reset the stack
            duk_pop(ctx);
        }

        duk_new(ctx, 0);

        if (duk_is_object(ctx, -1))
        {
            component = js_to_class_instance<JSComponent>(ctx, -1, 0);
            component->scriptClassInstance_ = true;
        }

        duk_pop(ctx);

    }

    if (component.Null())
    {
        ATOMIC_LOGERRORF("Failed to create script class from component file %s", GetName().CString());
        component =  new JSComponent(context_);
    }

    component->SetComponentFile(this);

    return component;

}

bool JSComponentFile::InitModule()
{
    if (context_->GetEditorContext())
        return true;

    JSVM* vm = JSVM::GetJSVM(NULL);

    duk_context* ctx = vm->GetJSContext();

    duk_idx_t top = duk_get_top(ctx);

    if (!PushModule())
        return false;

    if (duk_is_function(ctx, -1))
    {
        // constructor export
        scriptClass_ = true;
    }
    else if (duk_is_object(ctx, -1))
    {
        // Look for "default" constructor which is used by TypeScript and ES2015
        duk_get_prop_string(ctx, -1, "default");
        if (!duk_is_function(ctx, -1))
        {
            duk_pop(ctx);
            duk_get_prop_string(ctx, -1, "component");

            if (!duk_is_function(ctx, -1))
            {
                ATOMIC_LOGERRORF("Component file export object does not export a key \"component\" function: %s", GetName().CString());
                duk_set_top(ctx, top);
                return false;
            }

            scriptClass_ = false;
        } else {
            // this is a TS or ES2015 default class
            scriptClass_ = true;
        }

    }

    duk_set_top(ctx, top);

    return true;
}


bool JSComponentFile::BeginLoad(Deserializer& source)
{
    if (!InitModule())
        return false;

    // TODO: move the inspector field parsing to the JavascriptImporter
    // which will work with obfusication, minimization, or bytecode dumps

    unsigned dataSize = source.GetSize();
    if (!dataSize && !source.GetName().Empty())
    {
        ATOMIC_LOGERROR("Zero sized component file in " + source.GetName());
        return false;
    }

    SharedArrayPtr<char> buffer(new char[dataSize + 1]);
    if (source.Read(buffer.Get(), dataSize) != dataSize)
        return false;
    buffer[dataSize] = '\0';

    String text = buffer.Get();
    text.Replace("\r", "");
    Vector<String> lines = text.Split('\n');

    String eval;
    bool valid = false;
    int leftBracketCount = 0;
    int rightBracketCount = 0;
    for (unsigned i = 0; i < lines.Size(); i++)
    {
        String line = lines[i];

        bool added = false;

        if (!eval.Length())
        {
            line = line.Trimmed();

            if (line.StartsWith("inspectorFields"))
            {
                added = true;
                eval = line + "\n";
            }
            else if (line.StartsWith("this.inspectorFields"))
            {
                added = true;
                eval = line.Substring(5) + "\n";
            }
            else if (line.StartsWith("var inspectorFields"))
            {
                added = true;
                eval = line.Substring(4) + "\n";
            }

        }
        else
        {
            added = true;
            eval += line + "\n";
        }

        if (added) {

            for (unsigned j = 0; j < line.Length(); j++)
            {
                if (line.At(j) == '{')
                    leftBracketCount++;

                else if (line.At(j) == '}')
                    rightBracketCount++;

            }

        }

        if (eval.Length() && leftBracketCount && leftBracketCount == rightBracketCount)
        {
            valid = true;
            break;
        }

    }

    if (valid)
    {
        JSVM* vm = JSVM::GetJSVM(NULL);

        if (!vm)
            return true;

        duk_context* ctx = vm->GetJSContext();

        int top = duk_get_top(ctx);

        duk_push_string(ctx, eval.CString());
        duk_push_string(ctx, "eval");

        if (duk_pcompile(ctx, DUK_COMPILE_EVAL) != 0)
        {

            // couldn't eval the inspector fields
            duk_set_top(ctx, top);
            return true;

        }
        else if (duk_is_function(ctx, -1) && duk_pcall(ctx, 0) == DUK_EXEC_SUCCESS)
        {
            if (duk_is_object(ctx, -1))
            {
                duk_enum(ctx, -1, DUK_ENUM_OWN_PROPERTIES_ONLY);

                while (duk_next(ctx, -1, 1)) {

                    String name = duk_get_string(ctx, -2);

                    VariantType variantType = VAR_NONE;
                    Variant defaultValue;

                    if (duk_is_string(ctx, -1))
                    {
                        variantType = VAR_STRING;
                        defaultValue = duk_to_string(ctx, -1);
                    }
                    else if (duk_is_number(ctx, -1))
                    {
                        variantType = VAR_FLOAT;
                        defaultValue = (float) duk_to_number(ctx, -1);
                    }
                    else if (duk_is_array(ctx, -1))
                    {
                        int length = duk_get_length(ctx, -1);

                        if (length > 0)
                        {

                            duk_get_prop_index(ctx, -1, 0);

                            // resource ref detection
                            if (duk_is_string(ctx, -1))
                            {
                                const char* classname = duk_to_string(ctx, -1);

                                duk_pop(ctx);

                                const char* name = NULL;

                                if (length > 1)
                                {
                                    duk_get_prop_index(ctx, -1, 1);
                                    name = duk_require_string(ctx, -1);
                                    duk_pop(ctx);

                                }

                                ResourceRef resourceRef(classname);
                                if (name)
                                    resourceRef.name_ = name;

                                variantType = VAR_RESOURCEREF;
                                defaultValue = resourceRef;

                            }
                            else
                            {
                                variantType = (VariantType) ((int)duk_require_number(ctx, -1));

                                duk_pop(ctx);

                                // detect int enum
                                if (length > 1 && (variantType == VAR_INT || variantType == VAR_FLOAT
                                                   || variantType == VAR_DOUBLE))
                                {
                                    duk_get_prop_index(ctx, -1, 1);

                                    if (duk_is_number(ctx, -1))
                                    {
                                        js_to_variant(ctx, -1, defaultValue);
                                    }
                                    else if (duk_is_array(ctx, -1))
                                    {
                                        int enumLength = duk_get_length(ctx, -1);

                                        for (unsigned i = 0; i < enumLength; i++)
                                        {
                                            duk_get_prop_index(ctx, -1, i);
                                            String enumName = duk_require_string(ctx, -1);
                                            AddEnum(name, EnumInfo(enumName, Variant(float(i))));
                                            duk_pop(ctx);
                                        }

                                    }

                                    duk_pop(ctx);

                                    if (length > 2)
                                    {
                                        duk_get_prop_index(ctx, -1, 2);
                                        // default value
                                        js_to_variant(ctx, -1, defaultValue);
                                        duk_pop(ctx);
                                    }

                                }
                                else if (length > 1)
                                {
                                    duk_get_prop_index(ctx, -1, 1);
                                    // default value
                                    js_to_variant(ctx, -1, defaultValue);
                                    duk_pop(ctx);
                                }

                            }

                        }

                    }
                    else if (duk_is_boolean(ctx, -1))
                    {
                        variantType = VAR_BOOL;
                        defaultValue = duk_to_boolean(ctx, -1) ? true : false;
                    }


                    if (defaultValue.GetType() != VAR_NONE)
                    {
                        AddDefaultValue(name, defaultValue);

                    }

                    if (variantType != VAR_NONE)
                    {
                        AddField(name, variantType);
                    }

                    duk_pop_2(ctx);  // pop key value
                }

                duk_pop(ctx);  // pop enum object

            }

        }

        duk_set_top(ctx, top);
    }

    SetMemoryUse(0);

    return true;
}

bool JSComponentFile::Save(Serializer& dest) const
{
    return true;
}



}
