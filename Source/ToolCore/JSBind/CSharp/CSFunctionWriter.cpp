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
    return new Node(NETCore::GetContext());
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
    return new ObjectAnimation(NETCore::GetContext());
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

void CSFunctionWriter::WriteNativeFunction(String& source)
{
    JSBClass* klass = function_->GetClass();
    JSBPackage* package = klass->GetPackage();
    String fname = function_->IsConstructor() ? "Constructor" : function_->GetName();

    String returnType;
    String functionSig = CSTypeHelper::GetNativeFunctionSignature(function_, returnType);

    String line;

    line = ToString("ATOMIC_EXPORT_API %s %s\n",
                    returnType.CString(), functionSig.CString());

    source += IndentLine(line);

    source += IndentLine("{\n");

    Indent();


    source += "\n";

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
            returnStatement = "return ";
        }
    }

    String callSig;
    GenNativeCallParameters(callSig);
    if (!function_->isConstructor_)
    {
        if (function_->IsStatic())
        {
            line = ToString("%s%s::%s(%s);\n", returnStatement.CString(), klass->GetNativeName().CString(), function_->GetName().CString(), callSig.CString());
        }
        else
        {
            line = ToString("%sself->%s(%s);\n", returnStatement.CString(), function_->GetName().CString(), callSig.CString());
        }

    }
    else
    {
        if (klass->IsAbstract())
        {
            line = "return 0; // Abstract Class\n";
        }
        else if (klass->IsObject())
        {
            if (callSig.Length())
                line = ToString("return new %s(NETCore::GetContext(), %s);\n", klass->GetNativeName().CString(), callSig.CString());
            else
                line = ToString("return new %s(NETCore::GetContext());\n", klass->GetNativeName().CString());
        }
        else
        {
            line = ToString("return new %s(%s);\n", klass->GetNativeName().CString(), callSig.CString());
        }
    }

    source += IndentLine(line);

    if (sharedPtrReturn)
    {
        source += IndentLine("if (returnValue.NotNull()) returnValue->AddRef();\n");
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

void CSFunctionWriter::WriteDefaultStructParameters(String& source)
{

    for (unsigned i = 0; i < defaultStructParameters_.Size(); i++)
    {
        const DefaultStructParameter& dparm = defaultStructParameters_[i];

        String line = ToString("if (default(%s).Equals(%s)) %s = %s;\n",
                               dparm.type.CString(), dparm.parameterName.CString(), dparm.parameterName.CString(),
                               dparm.assignment.CString());

        source += IndentLine(line);

    }

}

void CSFunctionWriter::WriteManagedPInvokeFunctionSignature(String& source)
{
    source += "\n";

    // CoreCLR has pinvoke security demand code commented out, so we do not (currently) need this optimization:
    // https://github.com/dotnet/coreclr/issues/1605
    // line = "[SuppressUnmanagedCodeSecurity]\n";
    // source += IndentLine(line);

    String line = "[DllImport (Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]\n";
    source += IndentLine(line);

    JSBClass* klass = function_->GetClass();
    JSBPackage* package = klass->GetPackage();

    String returnType = CSTypeHelper::GetPInvokeTypeString(function_->GetReturnType());

    if (returnType == "bool")
    {
        // default boolean marshal is 4 byte windows type BOOL and not 1 byte bool
        // https://blogs.msdn.microsoft.com/jaredpar/2008/10/14/pinvoke-and-bool-or-should-i-say-bool/        
        source += IndentLine("[return: MarshalAs(UnmanagedType.I1)]\n");
    }

    if (returnType == "string")
        returnType = "IntPtr";

    if (function_->IsConstructor())
        returnType = "IntPtr";

    Vector<JSBFunctionType*>& parameters = function_->GetParameters();

    Vector<String> args;

    if (!function_->IsConstructor() && !function_->IsStatic())
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
            else if (name == "readonly")
                name = "readOnly";
            else if (name == "params")
                name = "parameters";

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

            String init = ptype->initializer_;

            if (init.Length())
            {
                init = MapDefaultParameter(ptype);
                if (init.Length())
                    sig += " = " + init;

            }

            if (i + 1 != parameters.Size())
                sig += ", ";
        }
    }
}

void CSFunctionWriter::WriteManagedConstructor(String& source)
{
    JSBClass* klass = function_->GetClass();
    JSBPackage* package = klass->GetPackage();

    if (klass->GetName() == "RefCounted")
        return;

    // wrapping constructor

    String line;

    line = ToString("public %s (IntPtr native) : base (native)\n", klass->GetName().CString());
    source += IndentLine(line);
    source += IndentLine("{\n");
    source += IndentLine("}\n\n");

    String sig;
    GenManagedFunctionParameters(sig);

    line = ToString("public %s (%s)\n", klass->GetName().CString(), sig.CString());

    source += IndentLine(line);

    source += IndentLine("{\n");

    Indent();

    WriteDefaultStructParameters(source);

    source += IndentLine("if (nativeInstance == IntPtr.Zero)\n");
    source += IndentLine("{\n");

    Indent();

    source += IndentLine(ToString("var classType = typeof(%s);\n", klass->GetName().CString()));
    source += IndentLine("var thisType = this.GetType();\n");
    source += IndentLine("var nativeThisType = NativeCore.IsNativeType(thisType);\n");    
    source += IndentLine("var nativeBaseType = NativeCore.IsNativeType(thisType.BaseType);\n");
    source += IndentLine("if ( (nativeThisType && (thisType == classType)) || (!nativeThisType && (nativeBaseType && (thisType.BaseType == classType))))\n");
    source += IndentLine("{\n");

    Indent();

    String callSig;
    GenPInvokeCallParameters(callSig);

    source += IndentLine("IntPtr nativeInstanceOverride = NativeCore.NativeContructorOverride;\n");

    line = ToString("nativeInstance = NativeCore.RegisterNative (nativeInstanceOverride != IntPtr.Zero ? nativeInstanceOverride : csb_%s_%s_Constructor(%s), this);\n",
                     package->GetName().CString(), klass->GetName().CString(), callSig.CString());

    source += IndentLine(line);

    Dedent();

    source += IndentLine("}\n");

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
            else if (name == "readonly")
                name = "readOnly";
            else if (name == "params")
                name = "parameters";

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

    String line = "public ";

    if (function_->IsStatic())
    {
        line += "static ";
    }

    bool marked = false;
    JSBClass* baseClass = klass->GetBaseClass();
    if (baseClass)
    {
        JSBFunction* override = baseClass->MatchFunction(function_, true);
        if (override)
        {
            marked = true;
            if (override->IsVirtual())
                line += "override ";
            else
                line += "new ";
        }
    }

    if (!marked && function_->IsVirtual())
        line += "virtual ";

    line += ToString("%s %s (%s)\n", returnType.CString(), function_->GetName().CString(), sig.CString());

    source += IndentLine(line);

    source += IndentLine("{\n");

    Indent();

    WriteDefaultStructParameters(source);

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

    String nativeInstance;

    if (!function_->IsStatic())
        nativeInstance = "nativeInstance";

    line += ToString("csb_%s_%s_%s(%s",
                     package->GetName().CString(), klass->GetName().CString(), function_->GetName().CString(), nativeInstance.CString());

    if (callSig.Length())
    {
        if (nativeInstance.Length())
            line += ", " + callSig;
        else
            line += callSig;

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

            source+= "\n";
        }
    }

    Dedent();

    source += IndentLine("}\n");

}

void CSFunctionWriter::GenerateManagedSource(String& sourceOut)
{

    String source = "";

    Indent();
    Indent();

    if (function_->GetDocString().Length())
    {
        // monodocer -assembly:NETCore.dll -path:en -pretty
        // mdoc export-html -o htmldocs en
        source += IndentLine("/// <summary>\n");
        if (function_->GetDocString().Contains('\n'))
            source += IndentLine("/* " + function_->GetDocString() + "*/\n");
        else
            source += IndentLine("/// " + function_->GetDocString() + "\n");

        source += IndentLine("/// </summary>\n");
    }

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
                String marshal = "private ";

                if (function_->IsStatic())
                    marshal += "static ";

                marshal += managedType + " ";

                marshal += ToString("%s%sReturnValue = new %s();\n", klass->GetName().CString(), function_->GetName().CString(), managedType.CString());

                sourceOut += IndentLine(marshal);
            }
        }

    }

    Dedent();
    Dedent();

    sourceOut += source;
}


void CSFunctionWriter::GenerateSource(String& sourceOut)
{

}

String CSFunctionWriter::MapDefaultParameter(JSBFunctionType* parameter)
{

    String init = parameter->initializer_;

    if (!init.Length())
        return init;

    if (parameter->type_->asClassType())
    {
        if (init == "0")
            return "null";
    }

    if (parameter->type_->asEnumType())
    {
        return parameter->type_->asEnumType()->enum_->GetName() + "." + init;
    }

    if (function_->class_->GetPackage()->ContainsConstant(init))
        return "Constants." + init;

    if (init == "true" || init == "false")
        return init;

    if (init == "0.0f")
        return init;

    if (init == "1.0f")
        return init;

    if (init == "0.1f")
        return init;

    if (init == "0")
        return init;

    if (init == "3")
        return init;

    if (init == "-1")
        return init;

    if (init == "\"\\t\"")
        return init;

    if (init == "NULL")
        return "null";

    if (init == "M_MAX_UNSIGNED")
        return "0xffffffff";

    if (init == "String::EMPTY")
        return "\"\"";

    // this kind of sucks, can't define const structs
    // and default parameters need to be const :/

    DefaultStructParameter dparm;

    dparm.parameterName = parameter->name_;

    if (init == "Vector3::ZERO")
    {
        dparm.type = "Vector3";
        dparm.assignment = "Vector3.Zero";
        defaultStructParameters_.Push(dparm);

        return "default(Vector3)";
    }

    if (init == "Vector3::ONE")
    {
        dparm.type = "Vector3";
        dparm.assignment = "Vector3.One";
        defaultStructParameters_.Push(dparm);

        return "default(Vector3)";
    }

    if (init == "Vector3::UP")
    {
        dparm.type = "Vector3";
        dparm.assignment = "Vector3.Up";
        defaultStructParameters_.Push(dparm);

        return "default(Vector3)";
    }

    if (init == "IntVector2::ZERO")
    {
        dparm.type = "IntVector2";
        dparm.assignment = "IntVector2.Zero";
        defaultStructParameters_.Push(dparm);
        return "default(IntVector2)";
    }

    if (init == "Quaternion::IDENTITY")
    {
        dparm.type = "Quaternion";
        dparm.assignment = "Quaternion.Identity";
        defaultStructParameters_.Push(dparm);
        return "default(Quaternion)";
    }

    return String::EMPTY;
}

}
