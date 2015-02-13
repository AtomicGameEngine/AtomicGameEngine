// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicEditor

#include <Atomic/Atomic.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Core/ProcessUtils.h>
#include "JSBClass.h"
#include "JSBFunction.h"

Vector<JSBClass*> JSBClass::allClasses_;

JSBFunctionOverride::JSBFunctionOverride(const String &name, const Vector<String>& sig) : parsed_(false)
{
    name_ = name;
    sig_ = sig;
}

void JSBFunctionOverride::Parse()
{
    if (parsed_)
        return;

    parsed_ = true;

    for (unsigned i = 0; i < sig_.Size(); i++)
    {
        JSBType* type = JSBType::Parse(sig_.At(i));
        if (!type)
        {
            ErrorExit("Unable to parse override type");
        }
        types_.Push(type);
    }

}

void JSBClass::WriteForwardDeclarations(String& source)
{
    if (isNumberArray())
        return;

    source.AppendWithFormat("static duk_ret_t jsb_constructor_%s(duk_context* ctx);\n", GetName().CString());
    source.AppendWithFormat("static void jsb_class_define_%s(JSVM* vm);\n", GetName().CString());
}

void JSBClass::WriteFunctions(String& source)
{
    for (unsigned i = 0; i < functions_.Size(); i++)
    {
        JSBFunction* function = functions_.At(i);

        if (function->Skip())
            continue;

        if (function->isDestructor_)
            continue;

        functions_.At(i)->Write(source);
    }

}

void JSBClass::WriteDefinition(String& source)
{
    if (isNumberArray())
        return;

    WriteFunctions(source);
    source.AppendWithFormat("static void jsb_class_define_%s(JSVM* vm)\n{\n", GetName().CString());
    source.Append("duk_context* ctx = vm->GetJSContext();\n");
    source.AppendWithFormat("js_class_get_prototype(ctx, \"%s\");\n", GetName().CString());

    for (unsigned i = 0; i < functions_.Size(); i++)
    {
        JSBFunction* function = functions_.At(i);

        if (function->Skip())
            continue;

        if (function->isConstructor_ || function->isDestructor_)
            continue;

        if (function->FirstDefaultParameter() != -1)
        {
            source.AppendWithFormat("duk_push_c_function(ctx, jsb_class_%s_%s, DUK_VARARGS);\n", GetName().CString(), function->name_.CString());
        }
        else
        {
            source.AppendWithFormat("duk_push_c_function(ctx, jsb_class_%s_%s, %i);\n", GetName().CString(), function->name_.CString(), (int) function->parameters_.Size());
        }

        String scriptName =  function->name_;
        scriptName[0] = tolower(scriptName[0]);
        source.AppendWithFormat("duk_put_prop_string(ctx, -2, \"%s\");\n", scriptName.CString());

    }

    source.Append("duk_pop(ctx);\n");
    source.Append("}\n");
}


void JSBClass::Write(String& source)
{    
    WriteDefinition(source);
}


void JSBClass::AddFunction(JSBFunction* function)
{
    functions_.Push(function);
}

void JSBClass::AddFunctionOverride(JSBFunctionOverride* functionOverride)
{
    overrides_.Push(functionOverride);
}


void JSBClass::Dump()
{
    if (name_ != className_)
    {
        LOGINFOF("Class: %s (%s)", name_.CString(), className_.CString());
    }
    else
    {
        LOGINFOF("Class: %s", name_.CString());
    }

    LOGINFOF("   IsObject: %s", isObject() ? "true" : "false");

    if (baseClasses_.Size())
    {
        LOGINFOF("   Bases:");
        for (unsigned i = 0; i < baseClasses_.Size(); i++)
        {

            LOGINFOF("      %s", baseClasses_.At(i)->GetClassName().CString());
        }
    }

    if (functions_.Size())
    {
        for (unsigned i = 0; i < functions_.Size(); i++)
        {
            functions_.At(i)->Dump();
        }
    }

}

void JSBClass::WriteProtoTypeRecursive(String &source, JSBClass* klass,  Vector<JSBClass*>& written)
{
    if (written.Contains(klass))
        return;

    Vector<JSBClass*>::Iterator itr = klass->baseClasses_.End() - 1 ;
    while (itr != klass->baseClasses_.Begin() - 1)
    {
        WriteProtoTypeRecursive(source, (*itr), written);
        itr--;
    }

    JSBClass* base = klass->baseClasses_.Size() ? klass->baseClasses_[0] : NULL;

    if (!klass->isNumberArray())
        source.AppendWithFormat("   js_setup_prototype(vm, \"%s\", \"%s\", %s);\n",
                                klass->GetName().CString(), base ? base->GetName().CString() : "",
                                klass->hasProperties() ? "true" : "false");

    written.Push(klass);

}

void JSBClass::WriteProtoTypeSetup(String& source)
{
    Vector<JSBClass*> written;

    for (unsigned i = 0; i < allClasses_.Size(); i++)
    {
        JSBClass* klass = allClasses_.At(i);

        WriteProtoTypeRecursive(source, klass, written);
    }
}

void JSBClass::AddPropertyFunction(JSBFunction* function)
{
    hasProperties_ = true;

    if (!function->propertyName_.Length())
    {
        ErrorExit("Function has no property name");
    }

    if (!function->isGetter_ && !function->isSetter_)
    {
        ErrorExit("Function is not a getter or setter");
    }


    JSBProperty* prop = NULL;
    if (properties_.Contains(function->propertyName_))
    {
        prop = properties_[function->propertyName_];
    }

    if (!prop)
    {
        prop = new JSBProperty();
        properties_[function->propertyName_] = prop;
    }

    if (prop->getter_ && function->isGetter_)
    {
        ErrorExit("getter already registered");
    }

    if (prop->setter_ && function->isSetter_)
    {
        ErrorExit("setter already registered");
    }

    if (function->isSetter_)
        prop->setter_ = function;
    else
        prop->getter_ = function;

}

void JSBClass::SetSkipFunction(const String& name, bool skip)
{
    for (unsigned i = 0; i < functions_.Size(); i++)
    {
        if (functions_[i]->isConstructor_)
            continue;

        if (functions_[i]->name_ == name)
            functions_[i]->SetSkip(skip);
    }

}

JSBFunction* JSBClass::GetFunction(const String& name)
{
    for (unsigned i = 0; i < functions_.Size(); i++)
    {
        if (functions_[i]->name_ == name)
            return functions_[i];
    }

    return 0;
}


void JSBClass::Process()
{
    for (unsigned i = 0; i < allClasses_.Size(); i++)
    {
        JSBClass* klass = allClasses_[i];

        for (unsigned j = 0; j < klass->overrides_.Size(); j++)
        {
            klass->overrides_.At(j)->Parse();
        }

        // detect overridden functions, only works for in class atm (not baseclasses)
        for (unsigned j = 0; j < klass->functions_.Size(); j++)
        {
            JSBFunction* function = klass->functions_[j];

            if (klass->isNumberArray())
            {
                function->SetSkip(true);
                continue;
            }

            // skip function if only one parameter of type Context, if not Constuctor
            if (!function->isConstructor_)
            if (function->parameters_.Size() == 1 && function->parameters_.At(0)->type_->asClassType())
            {
                if (function->parameters_.At(0)->type_->asClassType()->class_->GetClassName() == "Context")
                {
                    function->SetSkip(true);
                    continue;
                }
            }


            if (function->isOverride_)
                continue;

            for (unsigned k = 0; k < klass->functions_.Size(); k++)
            {
                if (j == k)
                    continue;

                JSBFunction* function2 = klass->functions_[k];

                if (function->name_ == function2->name_)
                {
                    function->isOverride_ = true;
                    function2->isOverride_ = true;
                    // initially set all overridden functions to skip
                    function->SetSkip(true);
                    function2->SetSkip(true);
                    break;
                }
            }

        }

        // mark overrides
        for (unsigned j = 0; j < klass->functions_.Size(); j++)
        {
            JSBFunction* function = klass->functions_[j];

            if (function->isOverride_)
            {
                for (unsigned k = 0; k < klass->overrides_.Size(); k++)
                {
                    JSBFunctionOverride* override =  klass->overrides_[k];

                    if (override->name_ != function->name_)
                        continue;

                    if (override->types_.Size() != function->parameters_.Size())
                        continue;

                    bool match = true;
                    for (unsigned x = 0; x < function->parameters_.Size(); x++)
                    {
                        JSBType* ot = override->types_[x];
                        JSBType* pt = function->parameters_[x]->type_;

                        // should add an == operator
                        if ((ot->asPrimitiveType() == NULL) != (pt->asPrimitiveType() == NULL) ||
                            (ot->asClassType() == NULL) != (pt->asClassType() == NULL) )
                        {
                            match = false;
                            break;
                        }

                        if (ot->asPrimitiveType())
                        {
                            JSBPrimitiveType* pot = ot->asPrimitiveType();
                            JSBPrimitiveType* ppt = pt->asPrimitiveType();

                            if (pot->kind_ != ppt->kind_)
                            {
                                match = false;
                                break;
                            }
                        }
                        if (ot->asClassType())
                        {
                            JSBClassType* cot = ot->asClassType();
                            JSBClassType* cpt = pt->asClassType();

                            if (cot->class_ != cpt->class_)
                            {
                                match = false;
                                break;
                            }
                        }
                        if (ot->asStringType())
                        {
                            if (!pt->asStringType())
                            {
                                match = false;
                                break;
                            }
                        }
                        if (ot->asStringHashType())
                        {
                            if (!pt->asStringHashType())
                            {
                                match = false;
                                break;
                            }
                        }


                    }
                    if (match)
                    {
                        function->SetSkip(false);
                        break;
                    }

                }

            }

        }

    }

    for (unsigned i = 0; i < allClasses_.Size(); i++)
    {
        JSBClass* klass = allClasses_[i];

        for (unsigned j = 0; j < klass->functions_.Size(); j++)
        {
            JSBFunction* function = klass->functions_[j];
            function->Process();
        }

    }
}

JSBFunction* JSBClass::GetConstructor()
{
    {
        for (unsigned i = 0; i < functions_.Size(); i++)
            if (functions_[i]->isConstructor_ && !functions_[i]->Skip())
                return functions_[i];
    }

    return NULL;

}

void JSBClass::Preprocess()
{
    for (unsigned i = 0; i < allClasses_.Size(); i++)
    {
        JSBClass* klass = allClasses_[i];

        RecursiveAddBaseClass(klass, klass->baseClasses_);
    }

    for (unsigned i = 0; i < allClasses_.Size(); i++)
    {
        JSBClass* klass = allClasses_[i];

        if (klass->GetClassName() == "Object")
            klass->setObject(true);

        for (unsigned j = 0; j < klass->baseClasses_.Size(); j++)
        {
            if (klass->baseClasses_[j]->GetClassName() == "Object")
                klass->setObject(true);

        }

    }

}
