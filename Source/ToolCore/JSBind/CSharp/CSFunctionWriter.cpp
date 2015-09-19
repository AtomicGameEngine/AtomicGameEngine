//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/IO/FileSystem.h>

#include "../JSBind.h"
#include "../JSBModule.h"
#include "../JSBPackage.h"
#include "../JSBEnum.h"
#include "../JSBClass.h"
#include "../JSBFunction.h"

#include "CSFunctionWriter.h"

namespace ToolCore
{

CSFunctionWriter::CSFunctionWriter(JSBFunction *function) : JSBFunctionWriter(function)
{

}

void CSFunctionWriter::WriteParameterMarshal(String& source)
{

}

void CSFunctionWriter::WriteConstructor(String& source)
{

}

void CSFunctionWriter::GenNativeFunctionSignature(String& sig)
{
    // generate args
    Vector<JSBFunctionType*>& parameters = function_->GetParameters();

    int cparam = 0;
    if (parameters.Size())
    {
        for (unsigned int i = 0; i < parameters.Size(); i++, cparam++)
        {
            JSBFunctionType * ptype = parameters.At(i);

            // ignore "Context" parameters
            if (ptype->type_->asClassType())
            {
                JSBClassType* classType = ptype->type_->asClassType();
                JSBClass* klass = classType->class_;
                if (klass->GetName() == "Context")
                {
                    cparam--;
                    continue;
                }
            }

            String pstring = ptype->ToArgString(cparam);

            if (ptype->type_->asClassType())
            {
                JSBClassType* classType = ptype->type_->asClassType();

                JSBClass* klass = classType->class_;

                if (!klass->IsNumberArray())
                {
                    sig.AppendWithFormat("%s", pstring.CString());
                }
                else
                {
                    sig.AppendWithFormat("%s __arg%i", klass->GetNativeName().CString(), cparam);
                }
            }
            else if (ptype->type_->asStringType() || ptype->type_->asStringHashType())
            {
                sig.AppendWithFormat("char* __arg%i", cparam);
            }
            else if (ptype->type_->asHeapPtrType())
            {
                assert(0);
            }
            else if (ptype->type_->asPrimitiveType())
            {
                JSBPrimitiveType* prtype = ptype->type_->asPrimitiveType();
                sig.AppendWithFormat("%s __arg%i", prtype->ToString().CString(), cparam);
            }
            else if (ptype->type_->asEnumType())
            {
                JSBEnumType* etype = ptype->type_->asEnumType();

                sig.AppendWithFormat("%s __arg%i", etype->enum_->GetName().CString(), cparam);

            }
            else if (ptype->type_->asVectorType())
            {
                // read only vector arguments
                if (ptype->isConst_)
                {
                    JSBVectorType* vtype = ptype->type_->asVectorType();
                    sig.AppendWithFormat("%s __arg%i", vtype->ToString().CString(), cparam);
                }
            }
            else
            {
                assert(0);
            }

            sig += ", ";

        }
    }

    if (sig.EndsWith(", "))
        sig = sig.Substring(0, sig.Length() - 2);

}

void CSFunctionWriter::WriteFunction(String& source)
{
    JSBClass* klass = function_->class_;

    String sig;

    GenNativeFunctionSignature(sig);

    JSBFunctionType* returnType = function_->returnType_;
    String rTypeString = "void";
    // if the marshalling local variable type is differnt
    // for example SharedPtr ->Object *
    String rMarshalTypeString;

    if (returnType)
    {
        if (returnType->type_->asStringType())
        {
            rTypeString = "String";
        }
        else if (returnType->type_->asPrimitiveType())
        {
            JSBPrimitiveType* prtype = returnType->type_->asPrimitiveType();
            rTypeString = prtype->ToString();
        }
        else if (returnType->type_->asClassType())
        {
            JSBClassType* klassType = returnType->type_->asClassType();

            if (returnType->isTemplate_)
            {
                if (klassType->class_->IsObject())
                    rTypeString = "const Object*";
                else
                    rTypeString = "const RefCounted*";

                rMarshalTypeString.AppendWithFormat("SharedPtr<%s>", klassType->class_->GetNativeName().CString());
            }
            else if (klassType->class_->IsObject())
            {
                rTypeString = "const Object*";
            }
            else if (klassType->class_->IsNumberArray())
            {
                rTypeString = klassType->class_->GetName().CString();
            }
            else
            {
                rTypeString = "const RefCounted*";
            }
        }
        else if (returnType->type_->asEnumType())
        {
            JSBEnumType* enumType = returnType->type_->asEnumType();
            rTypeString = enumType->enum_->GetName().CString();
        }
        else if (returnType->type_->asVectorType())
        {
            JSBVectorType* vtype = returnType->type_->asVectorType();
            rTypeString = "";
            rTypeString.AppendWithFormat("%s", vtype->ToString().CString());
        }

    }



    source.AppendWithFormat("%s csb_%s_%s(%s* self%s)\n{\n", rTypeString == "String" ? "const char*" : rTypeString.CString(), klass->GetName().CString(),
                            function_->name_.CString(), klass->GetNativeName().CString(), sig.Length() ? (", " + sig).CString() : "");

    if (rTypeString != "void")
    {
        source.AppendWithFormat("%s retValue = ", rMarshalTypeString.Length()? rMarshalTypeString.CString() : rTypeString.CString());
    }

    // call

    source.AppendWithFormat("self->%s(", function_->name_.CString());

    Vector<JSBFunctionType*>& parameters = function_->GetParameters();

    for (unsigned int i = 0; i < parameters.Size(); i++)
    {
        source.AppendWithFormat("__arg%i",  i);

        if (i != parameters.Size() - 1)
        {
            source += ", ";
        }
    }

    source += ");\n";


    if (rTypeString != "void")
    {
        if (rTypeString == "String")
            source.AppendWithFormat("\nreturn retValue.CString();\n");
        else
            source.AppendWithFormat("\nreturn retValue;\n");
    }

    source.AppendWithFormat("}\n\n");

}

void CSFunctionWriter::GenerateSource(String& sourceOut)
{
    String source = "";

    if (function_->IsConstructor())
    {
        WriteConstructor(source);
    }
    else
    {
        WriteFunction(source);
    }

    sourceOut += source;

}

}
