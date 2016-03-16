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

#include <Atomic/IO/FileSystem.h>

#include "../JSBind.h"
#include "../JSBModule.h"
#include "../JSBPackage.h"
#include "../JSBEnum.h"
#include "../JSBClass.h"
#include "../JSBFunction.h"

#include "JSClassWriter.h"
#include "JSFunctionWriter.h"

namespace ToolCore
{

JSClassWriter::JSClassWriter(JSBClass *klass) : JSBClassWriter(klass)
{

}


void JSClassWriter::WriteFunctions(String& source)
{
    for (unsigned i = 0; i < klass_->functions_.Size(); i++)
    {
        JSBFunction* function = klass_->functions_.At(i);

        if (function->Skip())
            continue;

        if (function->IsDestructor())
            continue;

        JSFunctionWriter writer(function);
        writer.GenerateSource(source);
    }

}

void JSClassWriter::GenerateSource(String& sourceOut)
{
    String source = "";

    if (klass_->IsNumberArray())
        return;

    WriteFunctions(source);

    String packageName = klass_->GetModule()->GetPackage()->GetName();

    source.AppendWithFormat("static void jsb_class_define_%s(JSVM* vm)\n{\n", klass_->GetName().CString());
    source.Append("duk_context* ctx = vm->GetJSContext();\n");

    GenerateStaticFunctionsSource(source, packageName);

    GenerateNonStaticFunctionsSource(source, packageName);

    source.Append("}\n");

    sourceOut += source;

}

void JSClassWriter::GenerateStaticFunctionsSource(String& source, String& packageName)
{
    // Store function on both the constructor and prototype
    // so can access static functions from both the class constructor and an instance

    source.AppendWithFormat("js_class_get_constructor(ctx, \"%s\", \"%s\");\n", packageName.CString(), klass_->GetName().CString());
    source.AppendWithFormat("js_class_get_prototype(ctx, \"%s\", \"%s\");\n", packageName.CString(), klass_->GetName().CString());

    for (unsigned i = 0; i < klass_->functions_.Size(); i++)
    {
        JSBFunction* function = klass_->functions_.At(i);

        if (function->Skip())
            continue;

        if (function->IsConstructor() || function->IsDestructor())
            continue;

        if (!function->IsStatic())
            continue;

        if (function->FirstDefaultParameter() != -1)
        {
            source.AppendWithFormat("duk_push_c_function(ctx, jsb_class_%s_%s, DUK_VARARGS);\n", klass_->GetName().CString(), function->GetName().CString());
        }
        else
        {
            source.AppendWithFormat("duk_push_c_function(ctx, jsb_class_%s_%s, %i);\n", klass_->GetName().CString(), function->GetName().CString(), (int)function->GetParameters().Size());
        }

        String scriptName = function->GetName();
        scriptName[0] = tolower(scriptName[0]);

        source.Append("duk_dup(ctx, -1);\n");
        source.AppendWithFormat("duk_put_prop_string(ctx, -3, \"%s\");\n", scriptName.CString());
        source.AppendWithFormat("duk_put_prop_string(ctx, -3, \"%s\");\n", scriptName.CString());
    }

    source.Append("duk_pop_2(ctx);\n");
}

void JSClassWriter::GenerateNonStaticFunctionsSource(String& source, String& packageName)
{
    source.AppendWithFormat("js_class_get_prototype(ctx, \"%s\", \"%s\");\n", packageName.CString(), klass_->GetName().CString());

    for (unsigned i = 0; i < klass_->functions_.Size(); i++)
    {
        JSBFunction* function = klass_->functions_.At(i);

        if (function->Skip())
            continue;

        if (function->IsConstructor() || function->IsDestructor())
            continue;

        if (function->IsStatic())
            continue;

        if (function->FirstDefaultParameter() != -1)
        {
            source.AppendWithFormat("duk_push_c_function(ctx, jsb_class_%s_%s, DUK_VARARGS);\n", klass_->GetName().CString(), function->GetName().CString());
        }
        else
        {
            source.AppendWithFormat("duk_push_c_function(ctx, jsb_class_%s_%s, %i);\n", klass_->GetName().CString(), function->GetName().CString(), (int)function->GetParameters().Size());
        }

        String scriptName = function->GetName();
        scriptName[0] = tolower(scriptName[0]);
        source.AppendWithFormat("duk_put_prop_string(ctx, -2, \"%s\");\n", scriptName.CString());

    }
    source.Append("duk_pop(ctx);\n");
}


}
