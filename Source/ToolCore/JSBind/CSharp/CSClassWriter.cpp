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

#include "CSTypeHelper.h"
#include "CSClassWriter.h"
#include "CSFunctionWriter.h"

namespace ToolCore
{

CSClassWriter::CSClassWriter(JSBClass *klass) : JSBClassWriter(klass)
{

}

void CSClassWriter::WriteNativeFunctions(String& source)
{
    for (unsigned i = 0; i < klass_->functions_.Size(); i++)
    {
        JSBFunction* function = klass_->functions_.At(i);

        if (function->Skip())
            continue;

        if (function->IsDestructor())
            continue;

        if (CSTypeHelper::OmitFunction(function))
            continue;

        CSFunctionWriter writer(function);
        writer.GenerateNativeSource(source);
    }

}

void CSClassWriter::GenerateNativeSource(String& sourceOut)
{
    String source = "";

    if (klass_->IsNumberArray())
        return;

    JSBPackage* package = klass_->GetPackage();

    source.AppendWithFormat("ATOMIC_EXPORT_API ClassID csb_%s_%s_GetClassIDStatic()\n{\n", package->GetName().CString(),klass_->GetName().CString());
    source.AppendWithFormat("   return %s::GetClassIDStatic();\n}\n\n", klass_->GetNativeName().CString());

    WriteNativeFunctions(source);

    sourceOut += source;
}

void CSClassWriter::WriteManagedProperties(String& sourceOut)
{
    String source;

    if (klass_->HasProperties())
    {
        Vector<String> pnames;
        klass_->GetPropertyNames(pnames);

        for (unsigned j = 0; j < pnames.Size(); j++)
        {
            JSBProperty* prop = klass_->GetProperty(pnames[j]);

            JSBFunctionType* fType = NULL;
            JSBFunctionType* getType = NULL;
            JSBFunctionType* setType = NULL;

            if (CSTypeHelper::OmitFunction(prop->getter_) || CSTypeHelper::OmitFunction(prop->setter_))
                continue;

            if (prop->getter_ && !prop->getter_->Skip())
            {
                fType = getType = prop->getter_->GetReturnType();
            }
            if (prop->setter_ && !prop->setter_->Skip())
            {
                setType = prop->setter_->GetParameters()[0];

                if (!fType)
                    fType = setType;
                else if (fType->type_->ToString() != setType->type_->ToString())
                    continue;
            }

            if (!fType)
                continue;

            String line = "public ";

            JSBClass* baseClass = klass_->GetBaseClass();
            if (baseClass)
            {
                if (baseClass->MatchProperty(prop, true))
                {
                    // always new so we don't have to deal with virtual/override on properties
                    line += "new ";
                }
            }

            String type = CSTypeHelper::GetManagedTypeString(fType, false);
            line += ToString("%s %s\n", type.CString(), prop->name_.CString());
            source += IndentLine(line);
            source += IndentLine("{\n");

            Indent();

            if (prop->getter_)
            {
                source += IndentLine("get\n");
                source += IndentLine("{\n");

                Indent();

                source += IndentLine(ToString("return %s();\n", prop->getter_->GetName().CString()));

                Dedent();

                source += IndentLine("}\n");
            }

            if (prop->setter_)
            {
                source += IndentLine("set\n");
                source += IndentLine("{\n");

                Indent();

                source += IndentLine(ToString("%s(value);\n", prop->setter_->GetName().CString()));

                Dedent();

                source += IndentLine("}\n");
            }


            Dedent();

            source += IndentLine("}\n\n");
        }

    }

    sourceOut += source;

}

void CSClassWriter::GenerateManagedSource(String& sourceOut)
{
    String source = "";

    if (klass_->IsNumberArray())
        return;

    Indent();

    source += "\n";
    String line;

    if (klass_->GetBaseClass())
        line = "public partial class " + klass_->GetName() + " : " + klass_->GetBaseClass()->GetName() + "\n";
    else
        line = "public partial class " + klass_->GetName() + "\n";


    source += IndentLine(line);
    source += IndentLine("{\n");

    Indent();

    WriteManagedProperties(source);

    JSBPackage* package = klass_->GetPackage();

    // CoreCLR has pinvoke security demand code commented out, so we do not (currently) need this optimization:
    // https://github.com/dotnet/coreclr/issues/1605
    // line = "[SuppressUnmanagedCodeSecurity]\n";
    // source += IndentLine(line);

    line = "[DllImport (Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]\n";
    source += IndentLine(line);
    line = ToString("public static extern IntPtr csb_%s_%s_GetClassIDStatic();\n", package->GetName().CString(),klass_->GetName().CString());
    source += IndentLine(line);
    source += "\n";
    Dedent();

    // managed functions
    bool wroteConstructor = false;
    for (unsigned i = 0; i < klass_->functions_.Size(); i++)
    {
        JSBFunction* function = klass_->functions_.At(i);

        if (function->Skip())
            continue;

        if (function->IsDestructor())
            continue;

        if (CSTypeHelper::OmitFunction(function))
            continue;

        if (function->IsConstructor())
            wroteConstructor = true;

        CSFunctionWriter fwriter(function);
        fwriter.GenerateManagedSource(source);

    }

    // There are some constructors being skipped (like HTTPRequest as it uses a vector of strings in args)
    // Make sure we have at least a IntPtr version
    if (!wroteConstructor)
    {
        ATOMIC_LOGINFOF("WARNING: %s class didn't write a constructor, filling in generated native constructor", klass_->GetName().CString());

        line = ToString("public %s (IntPtr native) : base (native)\n", klass_->GetName().CString());
        source += IndentLine(line);
        source += IndentLine("{\n");
        source += IndentLine("}\n\n");
    }

    source += IndentLine("}\n");

    Dedent();

    sourceOut += source;
}


void CSClassWriter::GenerateSource(String& sourceOut)
{

}

}
