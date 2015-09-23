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

#include "CSTypeHelper.h"
#include "CSFunctionWriter.h"

/*
 *
 C# getters/setters
 local instance storage so we're not constantly creating managed Vector3, etc
 Vector2/Vector3/BoundingBox, etc C# structs so assign by value
 Object lifetime

 C# enum of module types for type info?
 C# version of push class instance?

 new instance from C# needs constructor
 wrapping does not, wrapping doesn't use constructors at all (JS needs this for prototype)

 Store GCHandle to keep an object alive (Component, UI) C# side?

 typedef const void* ClassID;
 which changed based on address, so need register at startup
 so at package startup time, need to setup mapping between
 IntPtr and C# class, we also need to be able to new a class
 instance with existing native or create a native when new'ing from C#

 IntPtr to RefCounted native side is the "ID", like JSHeapPtr

 Lifetime:

 // you cannot derive from native engine classes, other than script components

 a C# instance can be new'd, handed to native, stored in native, the C# side could be GC'd
 future access to this instance would be a new instance

*/


/*

// struct marshal Vector2, Vector3, BoundingBox, etc
// RefCounted*
// primitive bool, int, uint, float, double
// String

RefCounted* csb_Node_Constructor()
{
    return new Node(AtomicSharp::GetContext());
}

void csb_Node_GetPosition(Node* self, Vector3* out)
{
    *out = self->GetPosition();
}

void csb_Node_SetPosition(Node* self, Vector3*__arg0)
{
    self->SetPosition(*__arg0);
}

void csb_Node_SetPosition(Node* self, Vector3*__arg0)
{
    self->SetPosition(*__arg0);
}

bool csb_Audio_Play(Audio* self)
{
    bool retValue = self->Play();
    return retValue;
}

const RefCounted* csb_Node_GetParent(Node* self)
{
    const RefCounted* retValue = self->GetParent();
    return RefCounted;
}

RefCounted* csb_ObjectAnimation_Constructor()
{
    return new ObjectAnimation(AtomicSharp::GetContext());
}

*/
namespace ToolCore
{

CSFunctionWriter::CSFunctionWriter(JSBFunction *function) : JSBFunctionWriter(function)
{

}

void CSFunctionWriter::WriteNativeParameterMarshal(String& source)
{

}

void CSFunctionWriter::WriteNativeConstructor(String& source)
{
    JSBClass* klass = function_->class_;

    if (klass->IsAbstract())
        return;

    // just object for now, as constructor takes a context
    if (!klass->IsObject())
        return;

    // more than context arg, don't marshal yet
    if (function_->GetParameters().Size() > 1)
    {
        return;
    }


    source.AppendWithFormat("RefCounted* csb_%s_Constructor()\n{\nreturn new %s(AtomicSharp::GetContext());\n}\n",
                            klass->GetName().CString(), klass->GetNativeName().CString());


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

void CSFunctionWriter::WriteNativeFunction(String& source)
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
            rTypeString = "const String&";
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



    source.AppendWithFormat("%s csb_%s_%s(%s* self%s)\n{\n", rTypeString == "const String&" ? "const char*" : rTypeString.CString(), klass->GetName().CString(),
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
        if (rTypeString == "const String&")
            source.AppendWithFormat("\nreturn retValue.CString();\n");
        else
            source.AppendWithFormat("\nreturn retValue;\n");
    }

    source.AppendWithFormat("}\n\n");

}

void CSFunctionWriter::GenerateNativeSource(String& sourceOut)
{
    String source = "";

    if (function_->IsConstructor())
    {
        WriteNativeConstructor(source);
    }
    else
    {
        WriteNativeFunction(source);
    }

    sourceOut += source;

}

// MANAGED----------------------------------------------------------------------------------------

void CSFunctionWriter::WriteManagedPInvokeFunctionSignature(String& source)
{
    source += "\n";

    String line = "[DllImport (LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]\n";
    source += IndentLine(line);
    JSBClass* klass = function_->GetClass();
    JSBPackage* package = klass->GetPackage();

    String returnType = CSTypeHelper::GetNativeTypeString(function_->GetReturnType());

    Vector<JSBFunctionType*>& parameters = function_->GetParameters();

    Vector<String> args;

    if (!function_->IsConstructor())
    {
        args.Push("IntPtr self");
    }

    if (parameters.Size())
    {
        for (unsigned int i = 0; i < parameters.Size(); i++)
        {
            JSBFunctionType* ptype = parameters.At(i);

            // ignore "Context" parameters
            if (ptype->type_->asClassType())
            {
                JSBClassType* classType = ptype->type_->asClassType();
                JSBClass* klass = classType->class_;
                if (klass->GetName() == "Context")
                {
                    continue;
                }

                if (klass->IsNumberArray())
                {
                    args.Push("ref " + klass->GetName() + " " + ptype->name_);
                }
                else
                {
                    args.Push("IntPtr " + ptype->name_);
                }
            }
            else
            {
                args.Push(CSTypeHelper::GetNativeTypeString(ptype) + " " + ptype->name_);
            }

        }
    }

    if (function_->GetReturnType())
    {
        if (function_->GetReturnType()->type_->asClassType())
        {
            JSBClass* retClass = function_->GetReturnType()->type_->asClassType()->class_;
            if (retClass->IsNumberArray())
            {
                args.Push("ref " + retClass->GetName() + " retValue");
            }

        }
    }

    String pstring;
    pstring.Join(args, ", ");

    String fname = function_->IsConstructor() ? "Constructor" : function_->GetName();
    line = ToString("private static extern %s csb_%s_%s_%s(%s);\n",
                    returnType.CString(), package->GetName().CString(), klass->GetName().CString(),
                    fname.CString(), pstring.CString());

    source += IndentLine(line);

    source += "\n";

}


void CSFunctionWriter::GenManagedFunctionParameters(String& sig)
{
    // generate args
    Vector<JSBFunctionType*>& parameters = function_->GetParameters();

    if (parameters.Size())
    {
        for (unsigned int i = 0; i < parameters.Size(); i++)
        {
            JSBFunctionType* ptype = parameters.At(i);

            // ignore "Context" parameters
            if (ptype->type_->asClassType())
            {
                JSBClassType* classType = ptype->type_->asClassType();
                JSBClass* klass = classType->class_;
                if (klass->GetName() == "Context")
                {
                    continue;
                }
            }

            sig += CSTypeHelper::GetManagedTypeString(ptype);

            if (i + 1 != parameters.Size())
                sig += ", ";
        }
    }
}

void CSFunctionWriter::WriteManagedConstructor(String& source)
{
    JSBClass* klass = function_->GetClass();
    JSBPackage* package = klass->GetPackage();

    // wrapping constructor

    String line;

    if (klass->GetName() != "RefCounted")
    {
        line = ToString("public %s (IntPtr native) : base (native)\n", klass->GetName().CString());
        source += IndentLine(line);
        source += IndentLine("{\n");
        source += IndentLine("}\n\n");
    }

    String sig;
    GenManagedFunctionParameters(sig);

    line = ToString("public %s (%s)\n", klass->GetName().CString(), sig.CString());

    source += IndentLine(line);

    source += IndentLine("{\n");

    Indent();

    source += IndentLine("if (nativeInstance == IntPtr.Zero)");
    source += IndentLine("{\n");

    Indent();

    String callSig;
    GenPInvokeCallParameters(callSig);

    line = ToString("nativeInstance = NativeCore.RegisterNative (csb_%s_%s_Constructor(%s), this);\n",
                     package->GetName().CString(), klass->GetName().CString(), callSig.CString());

    source += IndentLine(line);

    Dedent();

    source += IndentLine("}\n");

    Dedent();

    source += IndentLine("}\n");
}

void CSFunctionWriter::GenPInvokeCallParameters(String& sig)
{
    // generate args
    Vector<JSBFunctionType*>& parameters = function_->GetParameters();

    if (parameters.Size())
    {
        for (unsigned int i = 0; i < parameters.Size(); i++)
        {
            JSBFunctionType* ptype = parameters.At(i);

            // ignore "Context" parameters
            if (ptype->type_->asClassType())
            {
                JSBClassType* classType = ptype->type_->asClassType();
                JSBClass* klass = classType->class_;
                if (klass->GetName() == "Context")
                {
                    continue;
                }
            }

            if (ptype->type_->asClassType())
            {
                JSBClass* pclass = ptype->type_->asClassType()->class_;
                if (pclass->IsNumberArray())
                {
                    sig += "ref " + ptype->name_;
                }
                else
                {
                    sig += ptype->name_ + " == null ? IntPtr.Zero : " + ptype->name_+ ".nativeInstance";
                }

            }
            else
            {
                sig += ptype->name_;
            }

            if (i + 1 != parameters.Size())
                sig += ", ";
        }
    }

    // data marshaller
    if (function_->GetReturnType() && !CSTypeHelper::IsSimpleReturn(function_->GetReturnType()))
    {
        if (function_->GetReturnType()->type_->asClassType()->class_->IsNumberArray())
        {
            if (sig.Length())
                sig += ", ";

            JSBClass* klass = function_->GetClass();
            sig += ToString("ref %s%sReturnValue", klass->GetName().CString(), function_->GetName().CString());
        }
    }


}

void CSFunctionWriter::WriteManagedFunction(String& source)
{
    JSBClass* klass = function_->GetClass();
    JSBPackage* package = klass->GetPackage();

    String sig;
    String returnType = CSTypeHelper::GetManagedTypeString(function_->GetReturnType());

    GenManagedFunctionParameters(sig);

    String line = ToString("public %s %s (%s)\n", returnType.CString(), function_->GetName().CString(), sig.CString());

    source += IndentLine(line);

    source += IndentLine("{\n");

    Indent();

    line.Clear();

    if (function_->GetReturnType())
    {
        if (CSTypeHelper::IsSimpleReturn(function_->GetReturnType()))
            line += "return ";
        else
        {
            if (function_->GetReturnType()->type_->asClassType())
            {
                if (!function_->GetReturnType()->type_->asClassType()->class_->IsNumberArray())
                    line += "IntPtr retNativeInstance = ";
            }
        }
    }

    String callSig;
    GenPInvokeCallParameters(callSig);

    line += ToString("csb_%s_%s_%s(nativeInstance",
                     package->GetName().CString(), klass->GetName().CString(), function_->GetName().CString());

    if (callSig.Length())
    {
        line += ", " + callSig;
    }

    line += ");\n";

    source += IndentLine(line);

    if (function_->GetReturnType() && !CSTypeHelper::IsSimpleReturn(function_->GetReturnType()))
    {
        if (function_->GetReturnType()->type_->asClassType())
        {
            JSBClass* retClass = function_->GetReturnType()->type_->asClassType()->class_;
            JSBClass* klass = function_->GetClass();

            if (retClass->IsNumberArray())
            {
                line = ToString("return %s%sReturnValue;", klass->GetName().CString(), function_->GetName().CString());
            }
            else
            {
                line = ToString("return retNativeInstance == IntPtr.Zero ? null :  NativeCore.WrapNative<%s> (retNativeInstance);", retClass->GetName().CString());
            }

            source += IndentLine(line);
        }
    }

    source+= "\n";

    Dedent();

    source += IndentLine("}\n");

}

void CSFunctionWriter::GenerateManagedSource(String& sourceOut)
{

    String source = "";

    Indent();
    Indent();
    Indent();

    if (function_->IsConstructor())
        WriteManagedConstructor(source);
    else
        WriteManagedFunction(source);

    WriteManagedPInvokeFunctionSignature(source);

    // data marshaller
    if (function_->GetReturnType() && !CSTypeHelper::IsSimpleReturn(function_->GetReturnType()))
    {
        if (function_->GetReturnType()->type_->asClassType())
        {
            JSBClass* retClass = function_->GetReturnType()->type_->asClassType()->class_;
            if (retClass->IsNumberArray())
            {
                JSBClass* klass = function_->GetClass();
                String managedType = CSTypeHelper::GetManagedTypeString(function_->GetReturnType());
                String marshal = "private " + managedType + " ";

                marshal += ToString("%s%sReturnValue = new %s();\n", klass->GetName().CString(), function_->GetName().CString(), managedType.CString());

                sourceOut += IndentLine(marshal);
            }
        }

    }

    Dedent();
    Dedent();
    Dedent();

    sourceOut += source;
}


void CSFunctionWriter::GenerateSource(String& sourceOut)
{

}

}
