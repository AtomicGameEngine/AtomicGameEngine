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

namespace ToolCore
{
bool CSFunctionWriter::wroteConstructor_ = false;

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

    const Vector<JSBFunctionType*>& parameters = function_->GetParameters();

    Vector<String> args;

    unsigned numParams = parameters.Size();

    if (function_->HasMutatedReturn())
        numParams--;

    if (numParams)
    {
        for (unsigned int i = 0; i < numParams; i++)
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

                if (klass->IsNumberArray() || ptype->isReference_)
                    args.Push(ToString("*%s", ptype->name_.CString()));
                else
                    args.Push(ToString("%s", ptype->name_.CString()));

            }
            else if (ptype->type_->asVectorType())
            {
                args.Push(ToString("%s__vector", ptype->name_.CString()));
            }
            else
            {
                if (ptype->type_->asStringType())
                {
                    args.Push(ToString("%s ? String(%s) : String::EMPTY", ptype->name_.CString(), ptype->name_.CString()));
                }
                else if (ptype->type_->asStringHashType())
                {
                    args.Push(ToString("StringHash(%s)", ptype->name_.CString()));
                }
                else
                {
                    args.Push(ToString("%s", ptype->name_.CString()));
                }
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

    if (function_->HasMutatedReturn())
    {
        line = ToString("if (!__retValue) return;\n");
        source += IndentLine(line);
        line = ToString("VariantVector __retValueVector;\n");
        source += IndentLine(line);
    }

    // vector marshal

    bool hasVectorMarshal = false;
    const Vector<JSBFunctionType*>& fparams = function_->GetParameters();

    for (unsigned i = 0; i < fparams.Size(); i++)
    {
        JSBFunctionType* ftype = fparams[i];

        // skip mutated input param
        if (function_->HasMutatedReturn() && i == fparams.Size() - 1)
            break;

        // Interface        
        JSBClass* interface = 0;
        if (ftype->type_->asClassType() && ftype->type_->asClassType()->class_->IsInterface())
        {
            // We need to downcast to the interface 
            // TODO: this assumes Object* is in hierarchy, how do we validate this?
            interface = ftype->type_->asClassType()->class_;
            line = ToString("%s = dynamic_cast<%s*>((Object*)%s);\n", ftype->name_.CString(), interface->GetNativeName().CString(), ftype->name_.CString());
            source += IndentLine(line);
        }

        // Vector
        JSBVectorType* vtype = ftype->type_->asVectorType();

        if (!vtype)
            continue;

        JSBClassType* classType = vtype->vectorType_->asClassType();

        if (!classType)
            continue;

        String className = classType->class_->GetName();

        String vectorMarshal;

        hasVectorMarshal = true;

        if (vtype->isVariantVector_)
        {
            const String& pname = ftype->name_;

            // TODO: handle early out with return value
            if (!function_->returnType_)
                source += IndentLine(ToString("if (!%s) return;\n", pname.CString()));

            source += IndentLine(ToString("VariantVector %s__vector;\n", pname.CString()));
            source += IndentLine(ToString("%s->AdaptToVector(%s__vector);\n", pname.CString(), pname.CString()));

        }
        else if (vtype->isPODVector_)
        {
            const String& pname = ftype->name_;
            source += IndentLine(ToString("PODVector<%s*> %s__vector;\n", className.CString(), pname.CString()));
            source += IndentLine(ToString("if (%s) %s->AdaptToVector<%s*>(%s__vector);\n", pname.CString(), pname.CString(), className.CString(), pname.CString()));
        }
        else
        {
            // vectorMarshal = ToString("PODVector<%s*> %s__vector", className.CString(), ftype->name_.CString());
        }

        if (vectorMarshal.Length())
        {
            source += IndentLine(vectorMarshal);
            vectorMarshal = String::EMPTY;
        }
    }

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
        returnStatement = ToString("SharedPtr<%s> returnValuePtr = ", function_->GetReturnClass()->GetNativeName().CString());
        sharedPtrReturn = true;
    }
    else if (function_->GetReturnType() && function_->GetReturnType()->type_->asVectorType())
    {
        // we have an out parameter
        JSBVectorType* vtype = function_->GetReturnType()->type_->asVectorType();

        if (!vtype->vectorTypeIsSharedPtr_ && !vtype->vectorTypeIsWeakPtr_)
        {
            returnStatement = ToString("%sVector<%s*> returnValue__vector = ", vtype->isPODVector_ ? "POD" : "", vtype->vectorType_->asClassType()->class_->GetName().CString());
        }
        else
        {
            returnStatement = ToString("%sVector<%s<%s>> returnValue__vector = ",  vtype->isPODVector_ ? "POD" : "", vtype->vectorTypeIsSharedPtr_ ? "SharedPtr" : "WeakPtr", vtype->vectorType_->asClassType()->class_->GetName().CString());
        }
    }
    else
    {
        if (returnType != "void" && !hasVectorMarshal)
        {
            returnStatement = "return ";
        }
        else if (returnType != "void")
        {
            returnStatement = ToString("%s returnValue = ", returnType.CString());
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
            if (function_->hasMutatedReturn_)
            {
                // this handles VariantVector case now, can be expanded
                line = ToString("__retValueVector = self->%s(%s);\n", function_->GetName().CString(), callSig.CString());
            }
            else
            {
                line = ToString("%sself->%s(%s);\n", returnStatement.CString(), function_->GetName().CString(), callSig.CString());
            }


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

    // Vector marshaling

    for (unsigned i = 0; i < fparams.Size(); i++)
    {
        JSBFunctionType* ftype = fparams[i];

        JSBVectorType* vtype = ftype->type_->asVectorType();

        if (!vtype)
            continue;

        if (function_->HasMutatedReturn() && i == fparams.Size() - 1)
        {
            source += IndentLine("__retValue->AdaptFromVector(__retValueVector);\n");
            break;
        }

        JSBClassType* classType = vtype->vectorType_->asClassType();

        if (!classType)
            continue;

        String className = classType->class_->GetName();

        String vectorMarshal;

        if (vtype->isPODVector_)
        {
            const String& pname = ftype->name_;
            source += IndentLine(ToString("if (%s) %s->AdaptFromVector<%s*>(%s__vector);\n", pname.CString(), pname.CString(), className.CString(), pname.CString()));
        }
        else
        {
            // vectorMarshal = ToString("PODVector<%s*> %s__vector", className.CString(), ftype->name_.CString());
        }

        if (vectorMarshal.Length())
        {
            source += IndentLine(vectorMarshal);
            vectorMarshal = String::EMPTY;
        }
    }


    if (sharedPtrReturn)
    {
        // We need to keep the shared ptr return value alive through the call, without adding an unwanted reference

        source += IndentLine(ToString("%s* returnValue = returnValuePtr;\n", function_->GetReturnClass()->GetNativeName().CString()));
        source += IndentLine("if (returnValue)\n");
        source += IndentLine("{\n");

        Indent();

        source += IndentLine("returnValue->AddRefSilent();\n");
        source += IndentLine("returnValuePtr = 0;\n");
        source += IndentLine("returnValue->ReleaseRefSilent();\n");

        Dedent();

        source += IndentLine("}\n");

        source += IndentLine("return returnValue;\n");
    }
    else if (returnType == "const char*")
    {
        source += IndentLine("return returnValue.CString();\n");
    }
    else if (function_->GetReturnType() && function_->GetReturnType()->type_->asVectorType())
    {
        // we have an out parameter
        JSBVectorType* vtype = function_->GetReturnType()->type_->asVectorType();
        source += IndentLine("if (returnValue) returnValue->AdaptFromVector(returnValue__vector);\n");

    }
    else if (returnType != "void" && hasVectorMarshal)
    {
        source += IndentLine("return returnValue;\n");
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
    JSBClass* klass = function_->GetClass();
    JSBPackage* package = klass->GetPackage();

    if (klass->IsInterface())
        return;

    source += "\n";

    // CoreCLR has pinvoke security demand code commented out, so we do not (currently) need this optimization:
    // https://github.com/dotnet/coreclr/issues/1605
    // line = "[SuppressUnmanagedCodeSecurity]\n";
    // source += IndentLine(line);

    String line = "[DllImport (Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]\n";
    source += IndentLine(line);

    String returnType = CSTypeHelper::GetPInvokeTypeString(function_->GetReturnType());

    // handled by out parameter
    if (function_->GetReturnType() && function_->GetReturnType()->type_->asVectorType())
        returnType = "void";

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

    const Vector<JSBFunctionType*>& parameters = function_->GetParameters();

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
    else if (function_->GetReturnType() && function_->GetReturnType()->type_->asVectorType())
    {
        args.Push("IntPtr returnValue");
    }

    String pstring;
    pstring.Join(args, ", ");

    String fname = function_->IsConstructor() ? "Constructor" : function_->GetName();
    line = ToString("private static extern %s csb_%s_%s_%s_%u(%s);\n",
                    returnType.CString(), package->GetName().CString(), klass->GetName().CString(),
                    fname.CString(), function_->GetID(), pstring.CString());

    source += IndentLine(line);

    source += "\n";

}



void CSFunctionWriter::GenManagedFunctionParameters(String& sig)
{
    // generate args
    const Vector<JSBFunctionType*>& parameters = function_->GetParameters();

    if (parameters.Size())
    {
        for (unsigned int i = 0; i < parameters.Size(); i++)
        {
            bool isStruct = false;
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

                // TODO: we should have a better system for struct type in general
                // This number array is really for JS
                if (klass->IsNumberArray())
                {
                    isStruct = true;
                }
            }

            String managedTypeString = CSTypeHelper::GetManagedTypeString(ptype);

            if (!ptype->isConst_ && (ptype->isReference_ && isStruct))
            {
                // pass by reference
                managedTypeString = "ref " + managedTypeString;
            }

            sig += managedTypeString;

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

    if (!wroteConstructor_)
    {
        line = ToString("public %s (IntPtr native) : base (native)\n", klass->GetName().CString());
        source += IndentLine(line);
        source += IndentLine("{\n");
        source += IndentLine("}\n\n");
    }

    // don't add wrapping constructor for overloads
    wroteConstructor_ = true;

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
    source += IndentLine("var thisTypeIsNative = NativeCore.IsNativeType(thisType);\n");    
    source += IndentLine("var nativeAncsestorType = NativeCore.GetNativeAncestorType(thisType);\n");
    source += IndentLine("if ( (thisTypeIsNative && (thisType == classType)) || (!thisTypeIsNative && (nativeAncsestorType == classType)))\n");
    source += IndentLine("{\n");

    Indent();

    String callSig;
    GenPInvokeCallParameters(callSig);

    line = ToString("nativeInstance = NativeCore.RegisterNative (csb_%s_%s_Constructor_%u(%s), this);\n",
                     package->GetName().CString(), klass->GetName().CString(), function_->GetID(), callSig.CString());

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
    const Vector<JSBFunctionType*>& parameters = function_->GetParameters();

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
                    sig += name + " == null ? IntPtr.Zero : " + name + ".NativeInstance";
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
            sig += ToString("ref %s%s%uReturnValue", klass->GetName().CString(), function_->GetName().CString(), function_->GetID());
        }
    }
    else if (!function_->IsStatic() && function_->GetReturnType() && function_->GetReturnType()->type_->asVectorType())
    {
        if (sig.Length())
            sig += ", ";

        JSBClass* klass = function_->GetClass();
        sig += "returnScriptVector";
    }

}

void CSFunctionWriter::WriteManagedFunction(String& source)
{

    JSBClass* klass = function_->GetClass();
    JSBPackage* package = klass->GetPackage();

    String sig;
    String returnType = CSTypeHelper::GetManagedTypeString(function_->GetReturnType());

    GenManagedFunctionParameters(sig);

    String line = klass->IsInterface() ? "" : "public ";

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

    if (!marked && function_->IsVirtual() && !klass->IsInterface())
        line += "virtual ";

    line += ToString("%s %s (%s)", returnType.CString(), function_->GetName().CString(), sig.CString());

    if (klass->IsInterface())
    {
        // If we're an interface we have no implementation
        line += ";\n\n";
        source += IndentLine(line);
        return;
    }
    else
        line += "\n";

    source += IndentLine(line);

    source += IndentLine("{\n");

    Indent();

    WriteDefaultStructParameters(source);

    line.Clear();

    if (function_->GetReturnType())
    {

        if (function_->GetReturnType()->type_->asStringType())
        {
            line += "return System.Runtime.InteropServices.Marshal.PtrToStringAnsi(";
        }
        else if (function_->GetReturnType()->type_->asStringHashType())
        {
            line += "return ";
        }
        else if (function_->GetReturnType()->type_->asVectorType())
        {
            JSBVectorType* vtype = function_->GetReturnType()->type_->asVectorType();
            
            String marshalName = ToString("%s%s%uReturnValue", function_->GetClass()->GetName().CString(), function_->GetName().CString(), function_->GetID());
            
            // Defer creation of ScriptVector return value until method is called
            if (vtype->vectorType_->asClassType())
            {
                String classname = vtype->vectorType_->asClassType()->class_->GetName();
                source += IndentLine(ToString("if (%s == null) %s = new Vector<%s>();\n", marshalName.CString(), marshalName.CString(), classname.CString()));
            }

            source += IndentLine(ToString("var returnScriptVector = %s.GetScriptVector();\n", marshalName.CString()));
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

    line += ToString("csb_%s_%s_%s_%u(%s",
                     package->GetName().CString(), klass->GetName().CString(), function_->GetName().CString(), function_->GetID(), nativeInstance.CString());

    if (callSig.Length())
    {
        if (nativeInstance.Length())
            line += ", " + callSig;
        else
            line += callSig;

    }

    if (function_->GetReturnType())
    {
        if (function_->GetReturnType()->type_->asStringType())
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
                line = ToString("return %s%s%uReturnValue;", klass->GetName().CString(), function_->GetName().CString(), function_->GetID());
            }
            else
            {
                line = ToString("return retNativeInstance == IntPtr.Zero ? null :  NativeCore.WrapNative<%s> (retNativeInstance);", retClass->GetName().CString());
            }

            source += IndentLine(line);

            source+= "\n";
        }
    }
    else if (function_->GetReturnType() && function_->GetReturnType()->type_->asVectorType())
    {
        if (!function_->IsStatic())
        {
            source += IndentLine(ToString("return %s%s%uReturnValue;", klass->GetName().CString(), function_->GetName().CString(), function_->GetID()));
            source+= "\n";
        }
    }

    Dedent();

    source += IndentLine("}\n");

}

void CSFunctionWriter::GenerateManagedSource(String& sourceOut)
{
    JSBClass* klass = function_->GetClass();

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

    if (!klass->IsInterface())
    {
        // data marshaller
        if (function_->GetReturnType() && !CSTypeHelper::IsSimpleReturn(function_->GetReturnType()))
        {
            if (function_->GetReturnClass())
            {
                JSBClass* retClass = function_->GetReturnClass();

                if (retClass->IsNumberArray())
                {
                    String managedType = CSTypeHelper::GetManagedTypeString(function_->GetReturnType());
                    String marshal = "private ";

                    if (function_->IsStatic())
                        marshal += "static ";

                    marshal += managedType + " ";

                    marshal += ToString("%s%s%uReturnValue;\n", klass->GetName().CString(), function_->GetName().CString(), function_->GetID());

                    sourceOut += IndentLine(marshal);
                }
            }
        }
        else if (!function_->IsStatic() && function_->GetReturnType() && function_->GetReturnType()->type_->asVectorType())
        {
            JSBVectorType* vtype = function_->GetReturnType()->type_->asVectorType();

            if (vtype->vectorType_->asClassType())
            {
                String classname = vtype->vectorType_->asClassType()->class_->GetName();
                String typestring = "Vector<" + classname + ">";

                String marshal = "private " + typestring + " ";

                marshal += ToString("%s%s%uReturnValue = null;\n", function_->GetClass()->GetName().CString(), function_->GetName().CString(), function_->GetID());

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

    if (init == "StringHash::ZERO")
    {
        dparm.type = "StringHash";
        dparm.assignment = "StringHash.Zero";
        defaultStructParameters_.Push(dparm);
        return "default(StringHash)";
    }

    return String::EMPTY;
}

}
