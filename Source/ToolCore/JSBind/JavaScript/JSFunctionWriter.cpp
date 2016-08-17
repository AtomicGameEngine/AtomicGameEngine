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

#include "JSFunctionWriter.h"

namespace ToolCore
{

JSFunctionWriter::JSFunctionWriter(JSBFunction *function) : JSBFunctionWriter(function)
{

}

void JSFunctionWriter::WriteParameterMarshal(String& source)
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
            const String& init = ptype->initializer_;

            if (ptype->type_->asClassType())
            {
                JSBClassType* classType = ptype->type_->asClassType();

                JSBClass* klass = classType->class_;

                if (!klass->IsNumberArray())
                {
                    if (init.Length())
                    {
                        source.AppendWithFormat("%s = duk_get_top(ctx) >= %i ? js_to_class_instance<%s>(ctx, %i, 0) : %s;\n",
                                                pstring.CString(), cparam + 1, klass->GetNativeName().CString(), cparam, init.CString());
                    }
                    else
                    {
                        source.AppendWithFormat("%s = js_to_class_instance<%s>(ctx, %i, 0);\n",
                                                pstring.CString(), klass->GetNativeName().CString(), cparam);
                    }
                }
                else
                {
                    int elements = klass->GetNumberArrayElements();
                    String elementType = klass->GetArrayElementType();
                    source.AppendWithFormat("%s arrayData%i[%i];\n", elementType.CString(), cparam, elements);

                    if (init.Length())
                    {
                        source.AppendWithFormat("const %s& defaultArg%i = %s;\n", klass->GetNativeName().CString(), cparam,  init.CString());
                        source.AppendWithFormat("if (duk_get_top(ctx) >= %i) {\n", cparam + 1);
                    }

                    for (int j = 0; j < elements; j++)
                    {
                        source.AppendWithFormat("duk_get_prop_index(ctx, %i, %i);\n", cparam, j);
                        source.AppendWithFormat("arrayData%i[%i] = (%s) duk_to_number(ctx, -1);\n", cparam, j, elementType.CString());
                    }

                    source.AppendWithFormat("duk_pop_n(ctx, %i);\n", elements);

                    if (init.Length())
                    {
                        source.Append("}\n");

                        source.AppendWithFormat("%s __arg%i(duk_get_top(ctx) >= %i ? (const %s *) arrayData%i : defaultArg%i.Data());\n",
                                                klass->GetNativeName().CString(), cparam, cparam + 1, elementType.CString(), cparam, cparam);
                    }
                    else
                    {
                        source.AppendWithFormat("%s __arg%i(arrayData%i);\n", klass->GetNativeName().CString(), cparam, cparam);
                    }

                }

            }
            else if (ptype->type_->asStringType() || ptype->type_->asStringHashType())
            {
                if (init.Length())
                {
                    source.AppendWithFormat("%s = duk_get_top(ctx) >= %i ? duk_to_string(ctx, %i) : %s;\n", pstring.CString(), cparam + 1, cparam, init.CString());
                }
                else
                {
                    source.AppendWithFormat("%s = duk_to_string(ctx, %i);\n", pstring.CString(),  cparam);

                }
            }
            else if (ptype->type_->asHeapPtrType())
            {
                if (init.Length())
                {
                    source.AppendWithFormat("%s = duk_get_top(ctx) >= %i ? duk_get_heapptr(ctx, %i) : %s;\n", pstring.CString(), cparam + 1, cparam, init.CString());
                }
                else
                {
                    source.AppendWithFormat("%s = duk_get_heapptr(ctx, %i);\n", pstring.CString(),  cparam);

                }
            }
            else if (ptype->type_->asPrimitiveType())
            {
                JSBPrimitiveType* prtype = ptype->type_->asPrimitiveType();

                if (prtype->kind_ == JSBPrimitiveType::Bool)
                {
                    if (init.Length())
                    {
                        source.AppendWithFormat("bool __arg%i = duk_get_top(ctx) >= %i ? (duk_to_boolean(ctx, %i) ? true : false) : %s;\n",
                                                cparam,  cparam + 1,   cparam, init.CString());
                    }
                    else
                    {
                        source.AppendWithFormat("bool __arg%i = duk_to_boolean(ctx, %i) ? true : false;\n",  cparam,  cparam);
                    }
                }
                else
                {
                    if (init.Length())
                    {
                        source.AppendWithFormat("double __arg%i = duk_get_top(ctx) >= %i ? (duk_to_number(ctx, %i)) : %s;\n",
                                                cparam,  cparam + 1,   cparam, init.CString());

                    }
                    else
                    {
                        source.AppendWithFormat("double __arg%i = duk_to_number(ctx, %i);\n",  cparam,  cparam);
                    }
                }

            }
            else if (ptype->type_->asEnumType())
            {
                JSBEnumType* etype = ptype->type_->asEnumType();

                if (init.Length())
                {
                    source.AppendWithFormat("%s __arg%i = duk_get_top(ctx) >= %i ? ((%s) ((int) duk_to_number(ctx, %i))) : %s;\n", etype->enum_->GetName().CString(),
                                            cparam,  cparam + 1, etype->enum_->GetName().CString(),  cparam, init.CString());

                }
                else
                {
                    source.AppendWithFormat("%s __arg%i = (%s) ((int)duk_to_number(ctx, %i));\n", etype->enum_->GetName().CString(),
                                            cparam, etype->enum_->GetName().CString(),  cparam);

                }

            }
            else if (ptype->type_->asVectorType())
            {
                // read only vector arguments
                if (ptype->isConst_)
                {
                    JSBVectorType* vtype = ptype->type_->asVectorType();
                    source.AppendWithFormat("%s __arg%i;\n", vtype->ToString().CString(), cparam);

                    source.AppendWithFormat("if (duk_get_top(ctx) >= %i)\n{\n", cparam + 1);
                    source.AppendWithFormat("duk_require_object_coercible(ctx, %i);\n", cparam);
                    source.AppendWithFormat("unsigned sz = duk_get_length(ctx, %i);\n", cparam);
                    source.AppendWithFormat("for (unsigned i = 0; i < sz; i++)\n{\n");

                    source.AppendWithFormat("duk_get_prop_index(ctx, 2, i);\n");

                    if (vtype->vectorType_->asStringType() || vtype->vectorType_->asStringHashType() )
                    {
                        source.AppendWithFormat("__arg%i.Push(duk_get_string(ctx, -1));\n", cparam);
                    }

                    source.AppendWithFormat("duk_pop(ctx);\n");

                    source.AppendWithFormat("\n}\n");

                    source.AppendWithFormat("\n}\n");

                }
            }

        }
    }
}


void JSFunctionWriter::WriteConstructor(String& source)
{

    // TODO: refactor this

    if (function_->name_ == "RefCounted")
    {
        source.Append("// finalizer may be called more than once\n" \
                      "static int jsb_finalizer_RefCounted(duk_context *ctx)\n" \
                      "{\n" \
                      "JSVM* vm =  JSVM::GetJSVM(ctx);\n" \
                      \
                      "duk_get_prop_index(ctx, 0, JS_INSTANCE_INDEX_FINALIZED);\n" \
                      \
                      "if (!duk_is_boolean(ctx, -1))\n" \
                      "{\n" \
                      "RefCounted* ref = vm->GetObjectPtr(duk_get_heapptr(ctx, 0));\n" \
                      "vm->RemoveObject(ref);\n" \
                      "ref->ReleaseRef();\n" \
                      "duk_push_boolean(ctx, 1);\n" \
                      "duk_put_prop_index(ctx, 0, JS_INSTANCE_INDEX_FINALIZED);\n" \
                      "}\n" \
                      \
                      "return 0;\n" \
                      "}\n");
    }

    JSBClass* klass = function_->class_;
    JSBClass* base = klass->GetBaseClass();


    // Constructor
    source.AppendWithFormat("duk_ret_t jsb_constructor_%s(duk_context* ctx)\n{\n", klass->GetName().CString());

    source.Append( "\nJSVM* vm = JSVM::GetJSVM(ctx);\n" \
                   "duk_push_this(ctx);\n" \
                   "void *ptr = duk_get_heapptr(ctx, -1);\n" \
                   "duk_pop(ctx);\n\n");

    source.Append("   if (!vm->GetObjectPtr(ptr, true))\n   {\n");

    if (!klass->IsAbstract() && !klass->IsNumberArray())
    {

        String marshal;
        WriteParameterMarshal(marshal);

        String sparams;
        int cparam = 0;

        Vector<JSBFunctionType*>& parameters = function_->GetParameters();

        for (unsigned i = 0; i < parameters.Size(); i++, cparam++)
        {
            JSBFunctionType * ptype = parameters.At(i);

            String sarg;

            if (ptype->type_->asClassType())
            {
                JSBClassType* classType = ptype->type_->asClassType();
                JSBClass* klass = classType->class_;
                if (klass->GetName() == "Context")
                {
                    sarg = "vm->GetContext()";
                    cparam--;
                }

            }

            if (!sarg.Length())
            {
                sarg.AppendWithFormat("__arg%i", cparam);
            }

            sparams += sarg;

            if (i + 1 < parameters.Size())
                sparams += ", ";

        }

        source.AppendWithFormat("if (!duk_get_top(ctx) || !duk_is_pointer(ctx, 0))\n"\
            "{\n"\
            "%s\n"\
            "%s* native = new %s(%s);\n" \
            "vm->AddObject(ptr, native, INSTANTIATION_JAVASCRIPT);\n"\
            "}\n" \
            "else if (duk_is_pointer(ctx, 0))\n" \
            "{\n" \
            "RefCounted* rc = (RefCounted*) duk_get_pointer(ctx, 0);\n" \
            "vm->AddObject(ptr, rc, rc->GetInstantiationType());\n" \
            "}\n", marshal.CString(), klass->GetNativeName().CString(), klass->GetNativeName().CString(), sparams.CString());
    }
    else
    {
        if (klass->IsAbstract())
            source.Append("assert(0); // abstract class new'd\n");

        if (klass->IsNumberArray())
            source.Append("assert(0); // number array class new'd\n");

    }
    source.Append("   }\n");

    if (base)
    {
        String basePackage = base->GetModule()->GetPackage()->GetName();
        source.AppendWithFormat("   js_constructor_basecall(ctx, \"%s\", \"%s\");\n", basePackage.CString(), base->GetName().CString());
    }

    if (function_->name_ == "RefCounted")
    {
        source.Append("duk_push_this(ctx);\n "\
                      "duk_push_c_function(ctx, jsb_finalizer_RefCounted, 1);\n "\
                      "duk_set_finalizer(ctx, -2);\n "\
                      "duk_pop(ctx);\n");
    }

    source += "   return 0;";
    source += "\n}\n";

}

void JSFunctionWriter::WriteFunction(String& source)
{
    JSBClass* klass = function_->class_;

    source.AppendWithFormat("static int jsb_class_%s_%s(duk_context* ctx)\n{\n", klass->GetName().CString(), function_->name_.CString());

    WriteParameterMarshal(source);

    if (!function_->IsStatic())
    {
        source.Append("duk_push_this(ctx);\n");
        source.AppendWithFormat("%s* native = js_to_class_instance<%s>(ctx, -1, 0);\n", klass->GetNativeName().CString(), klass->GetNativeName().CString());
    }

    // declare return value;
    bool returnDeclared = false;

    JSBFunctionType* returnType = function_->returnType_;

    if (returnType)
    {
        if (returnType->type_->asStringType())
        {
            returnDeclared = true;
            source.Append("const String& retValue = ");
        }
        else if (returnType->type_->asPrimitiveType())
        {
            returnDeclared = true;

            JSBPrimitiveType* prtype = returnType->type_->asPrimitiveType();

            if (prtype->kind_ == JSBPrimitiveType::Bool)
            {
                source.Append("bool retValue = ");
            }
            else
            {
                source.Append("double retValue = ");
            }

        }
        else if (returnType->type_->asClassType())
        {
            JSBClassType* klassType = returnType->type_->asClassType();

            if (returnType->isTemplate_)
            {
                returnDeclared = true;
                source.AppendWithFormat("SharedPtr<%s> object = ", klassType->class_->GetNativeName().CString());
            }
            else if (klassType->class_->IsObject())
            {
                returnDeclared = true;
                source.Append("const Object* object = ");
            }
            else if (klassType->class_->IsNumberArray())
            {
                returnDeclared = true;
                source.AppendWithFormat("const %s& retValue = ", klassType->class_->GetName().CString());
            }
            else
            {
                returnDeclared = true;
                source.Append("const RefCounted* object = ");
            }
        }
        else if (returnType->type_->asEnumType())
        {
            JSBEnumType* enumType = returnType->type_->asEnumType();
            returnDeclared = true;
            source.AppendWithFormat("%s retValue = ", enumType->enum_->GetName().CString());
        }
        else if (returnType->type_->asVectorType())
        {
            returnDeclared = true;
            JSBVectorType* vtype = returnType->type_->asVectorType();
            source.AppendWithFormat("const %s& retValue = ", vtype->ToString().CString());
        }

    }

    if (function_->IsStatic())
    {
        source.AppendWithFormat("%s::%s(", klass->GetNativeName().CString(), function_->name_.CString());
    }
    else
    {
        source.AppendWithFormat("native->%s(", function_->name_.CString());
    }

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

    if (returnDeclared)
    {
        if (returnType->type_->asStringType())
        {
            source.Append("duk_push_string(ctx, retValue.CString());\n");
        }
        else if (returnType->type_->asPrimitiveType())
        {
            JSBPrimitiveType* prtype = returnType->type_->asPrimitiveType();

            if (prtype->kind_ == JSBPrimitiveType::Bool)
            {
                source.Append("duk_push_boolean(ctx, retValue ? 1 : 0);\n");
            }
            else
            {
                source.Append("duk_push_number(ctx, retValue);\n");
            }

        }
        else if (returnType->type_->asClassType())
        {
            JSBClassType* klassType = returnType->type_->asClassType();

            if (klassType->class_->IsObject())
            {
                returnDeclared = true;
                source.Append("js_push_class_object_instance(ctx, object);\n");
            }
            else if (klassType->class_->IsNumberArray())
            {
                returnDeclared = true;
                String elementType = klassType->class_->GetArrayElementType();
                source.AppendWithFormat("const %s* arrayData = retValue.Data();\n", elementType.CString());
                source.Append("duk_push_array(ctx);\n");
                for (int i = 0; i < klassType->class_->GetNumberArrayElements(); i++)
                {
                    source.AppendWithFormat("duk_push_number(ctx, arrayData[%i]);\n", i);
                    source.AppendWithFormat("duk_put_prop_index(ctx, -2, %i);\n", i);
                }
            }
            else
            {
                returnDeclared = true;
                source.AppendWithFormat("js_push_class_object_instance(ctx, object, \"%s\");\n", klassType->class_->GetName().CString());
            }
        }
        else if (returnType->type_->asEnumType())
        {
            returnDeclared = true;
            source.Append("duk_push_number(ctx, (double) retValue);\n");
        }
        else if (returnType->type_->asVectorType())
        {
            source.Append("duk_push_array(ctx);\n");
            source.Append("for (unsigned i = 0; i < retValue.Size(); i++)\n{\n");
            source.Append("duk_push_string(ctx, retValue[i].CString());\n");
            source.Append("duk_put_prop_index(ctx, -2, i);\n}\n");
        }



        source += "return 1;\n";
    }
    else
    {
        source += "return 0;\n";
    }

    source.Append("}\n");
}

void JSFunctionWriter::GenerateSource(String& sourceOut)
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
