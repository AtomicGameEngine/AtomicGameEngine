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

}

void CSFunctionWriter::GenNativeCallParameters(String& sig)
{
    JSBClass* klass = function_->GetClass();

    Vector<JSBFunctionType*>& parameters = function_->GetParameters();

    Vector<String> args;

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
                    args.Push(ToString("*%s", ptype->name_.CString()));
                else
                    args.Push(ToString("%s", ptype->name_.CString()));

            }
            else
            {
                args.Push(ToString("%s", ptype->name_.CString()));
            }

        }
    }

    sig.Join(args, ", ");
}

void CSFunctionWriter::GenNativeFunctionSignature(String& sig)
{
    JSBClass* klass = function_->GetClass();

    Vector<JSBFunctionType*>& parameters = function_->GetParameters();

    Vector<String> args;

    if (!function_->IsConstructor())
    {
        args.Push(ToString("%s* self", klass->GetNativeName().CString()));
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

                args.Push(ToString("%s* %s", klass->GetNativeName().CString(), ptype->name_.CString()));
            }
            else
            {
                args.Push(CSTypeHelper::GetNativeTypeString(ptype) + " " + ptype->name_);
            }

        }
    }

    if (function_->GetReturnClass() && function_->GetReturnClass()->IsNumberArray())
    {
        args.Push(ToString("%s* returnValue", function_->GetReturnClass()->GetNativeName().CString()));
    }

    sig.Join(args, ", ");

}

void CSFunctionWriter::WriteNativeFunction(String& source)
{
    JSBClass* klass = function_->GetClass();
    JSBPackage* package = klass->GetPackage();
    String fname = function_->IsConstructor() ? "Constructor" : function_->GetName();

    String returnType = "void";

    bool simpleReturn = true;

    if (function_->IsConstructor())
    {
        returnType = "RefCounted*";
    }
    else if (function_->GetReturnType())
    {
        if (function_->IsConstructor())
        {
            returnType = ToString("%s*", klass->GetNativeName().CString());
        }
        else if (function_->GetReturnClass())
        {
            if (!function_->GetReturnClass()->IsNumberArray())
            {
                returnType = ToString("const %s*", function_->GetReturnClass()->GetNativeName().CString());
            }
        }
        else if (function_->GetReturnType()->type_->asStringHashType())
        {
            returnType = "unsigned";
        }
        else
        {
            returnType = ToString("%s", CSTypeHelper::GetNativeTypeString(function_->GetReturnType()).CString());
        }
    }

    String line;
    String sig;
    GenNativeFunctionSignature(sig);

    line = ToString("%s csb_%s_%s_%s(%s)\n",
                    returnType.CString(), package->GetName().CString(), klass->GetName().CString(),
                    fname.CString(), sig.CString());

    source += IndentLine(line);

    source += IndentLine("{\n");

    Indent();

    bool returnValue = false;
    bool sharedPtrReturn = false;

    String returnStatement;

    if (returnType == "const char*")
    {
        returnValue = true;
        source += IndentLine("static String returnValue;\n");
        returnStatement = "returnValue = ";
    }
    else if (function_->GetReturnClass() && function_->GetReturnClass()->IsNumberArray())
    {
        returnStatement = "*returnValue = ";
    }
    else if (function_->GetReturnClass() && function_->GetReturnType()->isSharedPtr_)
    {
        returnStatement = ToString("SharedPtr<%s> returnValue = ", function_->GetReturnClass()->GetNativeName().CString());
        sharedPtrReturn = true;
    }
    else
    {
        if (returnType != "void")
        {
            if (simpleReturn)
                returnStatement = "return ";
        }
    }

    String callSig;
    GenNativeCallParameters(callSig);
    if (!function_->isConstructor_)
        line = ToString("%sself->%s(%s);\n", returnStatement.CString(), function_->GetName().CString(), callSig.CString());
    else
    {
        if (klass->IsAbstract())
        {
            line = "return 0; // Abstract Class\n";
        }
        else if (klass->IsObject())
        {
            if (callSig.Length())
                line = ToString("return new %s(AtomicSharp::GetContext(), %s);\n", klass->GetNativeName().CString(), callSig.CString());
            else
                line = ToString("return new %s(AtomicSharp::GetContext());\n", klass->GetNativeName().CString());
        }
        else
        {
            line = ToString("return new %s(%s);\n", klass->GetNativeName().CString(), callSig.CString());
        }
    }

    source += IndentLine(line);

    if (sharedPtrReturn)
    {
        source += IndentLine("returnValue->AddRef();\n");
        source += IndentLine("return returnValue;\n");
    }
    else if (returnType == "const char*")
    {
        source += IndentLine("return returnValue.CString();\n");
    }

    Dedent();

    source += IndentLine("}\n");

    source += "\n";

}

void CSFunctionWriter::GenerateNativeSource(String& sourceOut)
{
    String source = "";

    WriteNativeFunction(source);

    sourceOut += source;

}

// MANAGED----------------------------------------------------------------------------------------

void CSFunctionWriter::WriteManagedPInvokeFunctionSignature(String& source)
{
    source += "\n";

    String line = "[DllImport (Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]\n";
    source += IndentLine(line);
    JSBClass* klass = function_->GetClass();
    JSBPackage* package = klass->GetPackage();

    String returnType = CSTypeHelper::GetPInvokeTypeString(function_->GetReturnType());

    if (returnType == "string")
        returnType = "IntPtr";

    if (function_->IsConstructor())
        returnType = "IntPtr";

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

            String name = ptype->name_;

            if (name == "object")
                name = "_object";

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
                    args.Push("ref " + klass->GetName() + " " + name);
                }
                else
                {
                    args.Push("IntPtr " + name);
                }
            }
            else
            {
                args.Push(CSTypeHelper::GetPInvokeTypeString(ptype) + " " + name);
            }

        }
    }

    if (function_->GetReturnClass())
    {
        JSBClass* retClass = function_->GetReturnClass();
        if (retClass->IsNumberArray())
        {
            args.Push("ref " + retClass->GetName() + " retValue");
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

            // hack for Drawable as this causes a compilation error (need to add default params)
            if (sig.EndsWith("drawableFlags"))
                sig += " = '\\0'";

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

    source += IndentLine(ToString("if (typeof(%s) == this.GetType())\n", klass->GetName().CString()));
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

            String name = ptype->name_;

            if (name == "object")
                name = "_object";

            if (ptype->type_->asClassType())
            {
                JSBClass* pclass = ptype->type_->asClassType()->class_;
                if (pclass->IsNumberArray())
                {
                    sig += "ref " + name;
                }
                else
                {
                    sig += name + " == null ? IntPtr.Zero : " + name + ".nativeInstance";
                }

            }
            else
            {
                sig += name;
            }

            if (i + 1 != parameters.Size())
                sig += ", ";
        }
    }

    // data marshaller
    if (function_->GetReturnType() && !CSTypeHelper::IsSimpleReturn(function_->GetReturnType()))
    {
        if (function_->GetReturnClass()->IsNumberArray())
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
        if (function_->GetReturnType()->type_->asStringType() || function_->GetReturnType()->type_->asStringHashType())
        {
            line += "return System.Runtime.InteropServices.Marshal.PtrToStringAnsi(";
        }
        else if (CSTypeHelper::IsSimpleReturn(function_->GetReturnType()))
            line += "return ";
        else
        {
            if (function_->GetReturnClass())
            {
                if (!function_->GetReturnClass()->IsNumberArray())
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

    if (function_->GetReturnType())
    {
        if (function_->GetReturnType()->type_->asStringType() || function_->GetReturnType()->type_->asStringHashType())
            line += ")";
    }

    line += ");\n";

    source += IndentLine(line);

    if (function_->GetReturnType() && !CSTypeHelper::IsSimpleReturn(function_->GetReturnType()))
    {
        if (function_->GetReturnType()->type_->asClassType())
        {
            JSBClass* retClass = function_->GetReturnClass();
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
        if (function_->GetReturnClass())
        {
            JSBClass* retClass = function_->GetReturnClass();
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
