
#include <Atomic/Atomic.h>
#include "JSBFunction.h"

void JSBFunction::WriteParameterMarshal(String& source)
{
    // generate args

    int cparam = 0;
    if (parameters_.Size())
    {
        for (unsigned int i = 0; i < parameters_.Size(); i++, cparam++)
        {
            JSBFunctionType * ptype = parameters_.At(i);


            // ignore "Context" parameters
            if (ptype->type_->asClassType())
            {
                JSBClassType* classType = ptype->type_->asClassType();
                JSBClass* klass = classType->class_;
                if (klass->GetClassName() == "Context")
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

                if (!klass->isNumberArray())
                {
                    if (init.Length())
                    {
                        source.AppendWithFormat("%s = duk_get_top(ctx) >= %i ? js_to_class_instance<%s>(ctx, %i, 0) : %s;\n",
                                                pstring.CString(), cparam + 1, klass->GetClassName().CString(), cparam, init.CString());
                    }
                    else
                    {
                        source.AppendWithFormat("%s = js_to_class_instance<%s>(ctx, %i, 0);\n",
                                                pstring.CString(), klass->GetClassName().CString(), cparam);
                    }
                }
                else
                {
                    int elements = klass->GetNumberArrayElements();
                    String elementType = klass->GetArrayElementType();
                    source.AppendWithFormat("%s arrayData%i[%i];\n", elementType.CString(), cparam, elements);

                    if (init.Length())
                    {
                        source.AppendWithFormat("const %s& defaultArg%i = %s;\n", klass->GetClassName().CString(), cparam,  init.CString());
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

                        source.AppendWithFormat("%s __arg%i(duk_get_top(ctx) >= %i ? arrayData%i : defaultArg%i);\n",
                                                klass->GetClassName().CString(), cparam, cparam + 1, cparam, cparam);
                    }
                    else
                    {
                        source.AppendWithFormat("%s __arg%i(arrayData%i);\n", klass->GetClassName().CString(), cparam, cparam);
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
                    source.AppendWithFormat("%s __arg%i = duk_get_top(ctx) >= %i ? ((%s) ((int) duk_to_number(ctx, %i))) : %s;\n", etype->enum_->name_.CString(),
                                            cparam,  cparam + 1, etype->enum_->name_.CString(),  cparam, init.CString());

                }
                else
                {
                    source.AppendWithFormat("%s __arg%i = (%s) ((int)duk_to_number(ctx, %i));\n", etype->enum_->name_.CString(),
                                            cparam, etype->enum_->name_.CString(),  cparam);

                }

            }

        }
    }
}

void JSBFunction::Process()
{
    if (skip_)
    {
        return;
    }


    // if not already marked as a getter
    if (!isGetter_)
    {
        if (!parameters_.Size() && returnType_)
        {
            if (name_.Length() > 3 && name_.StartsWith("Get") && isupper(name_[3]))
            {
                String pname = name_.Substring(3);
                class_->SetSkipFunction(pname);
                isGetter_ = true;
                propertyName_ = pname;

            }
        }
    }

    if (!isSetter_)
    {
        if (parameters_.Size() == 1 && !returnType_)
        {
            if (name_.Length() > 3 && name_.StartsWith("Set") && isupper(name_[3]))
            {

                String pname = name_.Substring(3);
                class_->SetSkipFunction(pname);
                isSetter_ = true;
                propertyName_ = pname;
            }
        }
    }

    if (isGetter_)
        class_->AddPropertyFunction(this);
    if (isSetter_)
        class_->AddPropertyFunction(this);

}

void JSBFunction::WriteConstructor(String& source)
{

    // TODO: refactor this

    if (name_ == "RefCounted")
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

    JSBClass* base = class_->GetBaseClass();

    // Constructor
    source.AppendWithFormat("duk_ret_t jsb_constructor_%s(duk_context* ctx)\n{\n", class_->GetName().CString());

    source.Append("   if (duk_is_constructor_call(ctx))\n   {\n");
    if (!class_->isAbstract() && !class_->isNumberArray())
    {

        String marshal;
        WriteParameterMarshal(marshal);

        String sparams;

        int cparam = 0;
        for (unsigned i = 0; i < parameters_.Size(); i++, cparam++)
        {
            JSBFunctionType * ptype = parameters_.At(i);

            String sarg;

            if (ptype->type_->asClassType())
            {
                JSBClassType* classType = ptype->type_->asClassType();
                JSBClass* klass = classType->class_;
                if (klass->GetClassName() == "Context")
                {
                    sarg = "JSVM::GetJSVM(ctx)->GetContext()";
                    cparam--;
                }

            }

            if (!sarg.Length())
            {
                sarg.AppendWithFormat("__arg%i", cparam);
            }

            sparams += sarg;

            if (i + 1 < parameters_.Size())
                sparams += ", ";

        }

        source.AppendWithFormat("if (!duk_get_top(ctx) || !duk_is_pointer(ctx, 0))\n"\
        "{\n"\
          "%s\n"\
          "%s* native = new %s(%s);\n" \
          "duk_push_this(ctx);\n" \
          "JSVM::GetJSVM(ctx)->AddObject(duk_get_heapptr(ctx, -1), native);\n"\
          "duk_pop(ctx);\n"\
          "}\n" \
        "else if (duk_is_pointer(ctx, 0))\n" \
        "{\n" \
            "duk_push_this(ctx);\n" \
            "JSVM::GetJSVM(ctx)->AddObject(duk_get_heapptr(ctx, -1), (RefCounted*) duk_get_pointer(ctx, 0));\n" \
            "duk_pop(ctx);\n" \
        "}\n", marshal.CString(), class_->GetClassName().CString(), class_->GetClassName().CString(), sparams.CString());
    }
    else
    {
        if (class_->isAbstract())
            source.Append("assert(0); // abstract class new'd\n");
        if (class_->isNumberArray())
            source.Append("assert(0); // number array class new'd\n");

    }
    source.Append("   }\n");

    if (base)
    {
        source.AppendWithFormat("   js_constructor_basecall(ctx, \"%s\");\n", base->GetName().CString());
    }

    if (name_ == "RefCounted")
    {
        source.Append("duk_push_this(ctx);\n "\
                  "duk_push_c_function(ctx, jsb_finalizer_RefCounted, 1);\n "\
                  "duk_set_finalizer(ctx, -2);\n "\
                  \
                  "RefCounted* ref = JSVM::GetJSVM(ctx)->GetObjectPtr(duk_get_heapptr(ctx, -1));\n "\
                  "ref->AddRef();\n "\
                  \
                  "duk_pop(ctx);\n");

    }

    source += "   return 0;";
    source += "\n}\n";

}

void JSBFunction::WriteFunction(String& source)
{
    source.AppendWithFormat("static int jsb_class_%s_%s(duk_context* ctx)\n{\n", class_->GetName().CString(), name_.CString());

    WriteParameterMarshal(source);

    source.Append("duk_push_this(ctx);\n");
    source.AppendWithFormat("%s* native = js_to_class_instance<%s>(ctx, -1, 0);\n", class_->GetClassName().CString(), class_->GetClassName().CString());

    // declare return value;
    bool returnDeclared = false;
    if (returnType_)
    {
        if (returnType_->type_->asStringType())
        {
            returnDeclared = true;
            source.Append("const String& retValue = ");
        }
        else if (returnType_->type_->asPrimitiveType())
        {
            returnDeclared = true;

            JSBPrimitiveType* prtype = returnType_->type_->asPrimitiveType();

            if (prtype->kind_ == JSBPrimitiveType::Bool)
            {
                source.Append("bool retValue = ");
            }
            else
            {
                source.Append("double retValue = ");
            }

        }
        else if (returnType_->type_->asClassType())
        {
            JSBClassType* klassType = returnType_->type_->asClassType();

            if (klassType->class_->isObject())
            {
                returnDeclared = true;
                source.Append("const Object* object = ");
            }
            else if (klassType->class_->isNumberArray())
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
        else if (returnType_->type_->asEnumType())
        {
            JSBEnumType* enumType = returnType_->type_->asEnumType();
            returnDeclared = true;
            source.AppendWithFormat("%s retValue = ", enumType->enum_->name_.CString());
        }
    }

    source.AppendWithFormat("native->%s(", name_.CString());

    for (unsigned int i = 0; i < parameters_.Size(); i++)
    {
        source.AppendWithFormat("__arg%i",  i);

        if (i != parameters_.Size() - 1)
        {
            source += ", ";
        }
    }

    source += ");\n";

    if (returnDeclared)
    {
        if (returnType_->type_->asStringType())
        {
            source.Append("duk_push_string(ctx, retValue.CString());\n");
        }
        else if (returnType_->type_->asPrimitiveType())
        {
            JSBPrimitiveType* prtype = returnType_->type_->asPrimitiveType();

            if (prtype->kind_ == JSBPrimitiveType::Bool)
            {
                source.Append("duk_push_boolean(ctx, retValue ? 1 : 0);\n");
            }
            else
            {
                source.Append("duk_push_number(ctx, retValue);\n");
            }

        }
        else if (returnType_->type_->asClassType())
        {
            JSBClassType* klassType = returnType_->type_->asClassType();

            if (klassType->class_->isObject())
            {
                returnDeclared = true;
                source.Append("js_push_class_object_instance(ctx, object);\n");
            }
            else if (klassType->class_->isNumberArray())
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
        else if (returnType_->type_->asEnumType())
        {
            returnDeclared = true;
            source.Append("duk_push_number(ctx, (double) retValue);\n");
        }


        source += "return 1;\n";
    }
    else
    {
        source += "return 0;\n";
    }

    source.Append("}\n");
}

void JSBFunction::Write(String& source)
{
    if (isConstructor_)
    {
        WriteConstructor(source);
    }
    else
    {
        WriteFunction(source);
    }
}

