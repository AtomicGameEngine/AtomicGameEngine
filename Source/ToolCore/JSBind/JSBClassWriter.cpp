


#include <Atomic/IO/FileSystem.h>

#include "JSBind.h"
#include "JSBModule.h"
#include "JSBPackage.h"
#include "JSBEnum.h"
#include "JSBClass.h"
#include "JSBFunction.h"

#include "JSBClassWriter.h"
#include "JSBFunctionWriter.h"

namespace ToolCore
{

JSBClassWriter::JSBClassWriter(JSBClass *klass) : klass_(klass)
{

}


void JSBClassWriter::WriteFunctions(String& source)
{
    for (unsigned i = 0; i < klass_->functions_.Size(); i++)
    {
        JSBFunction* function = klass_->functions_.At(i);

        if (function->Skip())
            continue;

        if (function->IsDestructor())
            continue;

        JSBFunctionWriter writer(function);
        writer.GenerateSource(source);
    }

}

void JSBClassWriter::GenerateSource(String& sourceOut)
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

void JSBClassWriter::GenerateStaticFunctionsSource(String& source, String& packageName)
{
    source.AppendWithFormat("js_class_get_constructor(ctx, \"%s\", \"%s\");\n", packageName.CString(), klass_->GetName().CString());

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
        source.AppendWithFormat("duk_put_prop_string(ctx, -2, \"%s\");\n", scriptName.CString());
    }
    source.Append("duk_pop(ctx);\n");
}

void JSBClassWriter::GenerateNonStaticFunctionsSource(String& source, String& packageName)
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

