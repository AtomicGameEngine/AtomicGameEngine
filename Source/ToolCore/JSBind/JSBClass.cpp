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

#include <Atomic/Math/Plane.h>
#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/Resource/JSONFile.h>

#include "JSBType.h"
#include "JSBModule.h"
#include "JSBClass.h"
#include "JSBFunction.h"

namespace ToolCore
{

JSBFunctionSignature::JSBFunctionSignature(const String &name, const Vector<String>& sig) : parsed_(false)
{
    name_ = name;
    sig_ = sig;
}

void JSBFunctionSignature::Parse()
{
    if (parsed_)
        return;

    parsed_ = true;

    for (unsigned i = 0; i < sig_.Size(); i++)
    {
        JSBType* type = JSBType::Parse(sig_.At(i));
        if (!type)
        {
            ErrorExit(ToString("Unable to parse override type: %s", sig_.At(i).CString()));
        }
        types_.Push(type);
    }

}


bool JSBFunctionSignature::Match(JSBFunction* function)
{

    if (name_ != function->GetName())
        return false;

    const Vector<JSBFunctionType*>& parameters = function->GetParameters();

    if (types_.Size() != parameters.Size())
        return false;

    for (unsigned x = 0; x < parameters.Size(); x++)
    {
        JSBType* ot = types_[x];
        JSBType* pt = parameters[x]->type_;

        // should add an == operator
        if ((ot->asPrimitiveType() == NULL) != (pt->asPrimitiveType() == NULL) ||
                (ot->asClassType() == NULL) != (pt->asClassType() == NULL) )
        {
            return false;
        }

        if (ot->asPrimitiveType())
        {
            JSBPrimitiveType* pot = ot->asPrimitiveType();
            JSBPrimitiveType* ppt = pt->asPrimitiveType();

            if (pot->kind_ != ppt->kind_)
            {
                return false;
            }
        }
        if (ot->asClassType())
        {
            JSBClassType* cot = ot->asClassType();
            JSBClassType* cpt = pt->asClassType();

            if (cot->class_ != cpt->class_)
            {
                return false;
            }
        }
        if (ot->asStringType())
        {
            if (!pt->asStringType())
            {
                return false;
            }
        }
        if (ot->asStringHashType())
        {
            if (!pt->asStringHashType())
            {
                return false;
            }
        }


    }

    return true;

}

JSBClass::JSBClass(Context* context, JSBModule *module, const String& name, const String& nativeName, bool interface) : Object(context),
    module_(module), name_(name), nativeName_(nativeName),
    isAbstract_(false), isObject_(false), isGeneric_(false), isInterface_(interface),
    numberArrayElements_(0), arrayElementType_("float"),
    hasProperties_(false)
{

    if (nativeName == "Object")
        SetObject(true);

    // detect array type, TODO: this could go in module json

    if (name_ == "Color")
        numberArrayElements_ = 4;
    else if (name_ == "Vector2")
        numberArrayElements_ = 2;
    else if (name_ == "Vector3")
        numberArrayElements_ = 3;
    else if (name_ == "Vector4")
        numberArrayElements_ = 4;
    else if (name_ == "Quaternion")
        numberArrayElements_ = 4;
    else if (name_ == "BoundingBox")
        numberArrayElements_ = 6;
    else if (name_ == "Rect")
        numberArrayElements_ = 4;
    else if (name_ == "IntRect")
    {
        numberArrayElements_ = 4;
        arrayElementType_ = "int";
    }
    else if (name_ == "IntVector2")
    {
        numberArrayElements_ = 2;
        arrayElementType_ = "int";
    }
    else if (name_ == "Frustum")
    {
        // NUM_FRUSTUM_PLANES = 6;
        numberArrayElements_ = (int) ((sizeof(Plane) * 6) / sizeof(float));
        // NUM_FRUSTUM_VERTICES = 8;
        numberArrayElements_ += (int) ((sizeof(Vector3) * 8) / sizeof(float));
    }

}

JSBClass::~JSBClass()
{

}

void JSBClass::AddFunction(JSBFunction* function)
{
    functions_.Push(function);
}

JSBClass* JSBClass::GetBaseClass()
{
    if (!baseClasses_.Size())
        return 0;

    return baseClasses_[0];

}

void JSBClass::GetAllFunctions(PODVector<JSBFunction*>& functions)
{
    if (baseClasses_.Size())
        baseClasses_[0]->GetAllFunctions(functions);

    functions += functions_;

}

JSBFunction* JSBClass::GetConstructor(BindingLanguage bindingLanguage)
{
    {
        for (unsigned i = 0; i < functions_.Size(); i++)
            if (functions_[i]->IsConstructor() && !functions_[i]->Skip(bindingLanguage))
                return functions_[i];
    }

    return NULL;

}

void JSBClass::SetSkipFunction(const String& name, bool skip)
{
    for (unsigned i = 0; i < functions_.Size(); i++)
    {
        if (functions_[i]->IsConstructor())
            continue;

        if (functions_[i]->GetName() == name)
            functions_[i]->SetSkip(skip);
    }

}

bool JSBClass::MatchProperty(JSBProperty* property, bool includeBases)
{

    HashMap<String, JSBProperty*>::Iterator itr;
    for (itr = properties_.Begin(); itr != properties_.End(); itr++)
    {
        if (itr->first_ != property->name_)
            continue;

        return true;
    }

    if (!includeBases || !baseClasses_.Size())
        return 0;

    return baseClasses_[0]->MatchProperty(property, true);
}

JSBFunction* JSBClass::MatchFunction(JSBFunction *function, bool includeBases)
{
    for (unsigned i = 0; i < functions_.Size(); i++)
    {
        JSBFunction* func = functions_[i];

        if (func->Match(function))
            return func;
    }

    if (!includeBases || !baseClasses_.Size())
        return 0;

    return baseClasses_[0]->MatchFunction(function, true);

}

void JSBClass::SetBaseClass(JSBClass *baseClass)
{
    // we can't hook up chain here, as base class may not have been visited yet

    assert(!baseClasses_.Size());
    baseClasses_.Push(baseClass);
}

void JSBClass::RecursiveAddBaseClass(PODVector<JSBClass*>& baseClasses)
{
    for (unsigned i = 0; i < baseClasses.Size(); i++)
    {
        JSBClass* base = baseClasses.At(i);

        if (base->IsObject())
            SetObject(true);

        if (!baseClasses_.Contains(base))
            baseClasses_.Push(base);

        RecursiveAddBaseClass(base->baseClasses_);
    }
}

void JSBClass::MergeInterface(JSBClass* interface)
{
    if (!interface->IsInterface())
    {
        ATOMIC_LOGERRORF("JSBClass::MergeInterface - attempting to merge non-interface class %s", interface->GetName().CString());
        return;
    }

    // We merge interfaces via cloning the method for those which aren't implemented in the class itself
    // We don't use a common native wrapper, as we need to cast (sometimes using multiple inheritance, and this avoids an error prone dynamic cast)

    for (unsigned i = 0; i < interface->functions_.Size(); i++)
    {
        JSBFunction* src = interface->functions_[i];

        if (src->Skip() || src->IsConstructor() || src->IsDestructor())
            continue;

        if (!MatchFunction(src))
        {
            JSBFunction* function = src->Clone(this);
            function->SetInheritedInterface(true);
            AddFunction(function);
        }
    }
}

void JSBClass::Preprocess()
{

    // merge interfaces
    for (unsigned i = 0; i < interfaces_.Size(); i++)
    {
        MergeInterface(interfaces_[i]);
    }

    RecursiveAddBaseClass(baseClasses_);
}

void JSBClass::Process()
{
    for (unsigned j = 0; j < overrides_.Size(); j++)
    {
        overrides_.At(j)->Parse();
    }

    for (unsigned j = 0; j < excludes_.Size(); j++)
    {
        excludes_.At(j)->Parse();
    }


    // detect skips

    for (unsigned j = 0; j < functions_.Size(); j++)
    {

        for (unsigned k = 0; k < excludes_.Size(); k++)
        {
            JSBFunctionSignature* sig = excludes_[k];

            if (sig->Match(functions_[j]))
            {
                if (!sig->associatedBindings_.Size())
                {
                    functions_[j]->SetSkip(true);
                }
                else
                {
                    for (unsigned x = 0; x < sig->associatedBindings_.Size(); x++)
                    {
                        // This shouldn't happen
                        if (sig->associatedBindings_[x] == BINDINGLANGUAGE_ANY)
                            continue;

                        functions_[j]->SetSkipLanguage(sig->associatedBindings_[x]);
                    }
                }

                break;
            }

        }
    }

    // detect overridden functions, only works for in class atm (not baseclasses)
    for (unsigned j = 0; j < functions_.Size(); j++)
    {
        JSBFunction* function = functions_[j];

        if (IsNumberArray())
        {
            function->SetSkip(true);
            continue;
        }

        // skip function if only one parameter of type Context, if not Constuctor
        if (!function->IsConstructor())
        {
            const Vector<JSBFunctionType*>& parameters = function->GetParameters();

            if (parameters.Size() == 1 && parameters.At(0)->type_->asClassType())
            {
                if (parameters.At(0)->type_->asClassType()->class_->GetName() == "Context")
                {
                    function->SetSkip(true);
                    continue;
                }
            }
        }


        if (function->IsOverload())
            continue;

        for (unsigned k = 0; k < functions_.Size(); k++)
        {
            if (j == k)
                continue;

            JSBFunction* function2 = functions_[k];

            if (function->GetName() == function2->GetName())
            {
                function->SetOverload();
                function2->SetOverload();
                // initially set all overridden functions to skip (for JavaScript)
                function->SetSkipLanguage(BINDINGLANGUAGE_JAVASCRIPT);
                function2->SetSkipLanguage(BINDINGLANGUAGE_JAVASCRIPT);
                break;
            }
        }

    }

    // mark overrides
    for (unsigned j = 0; j < functions_.Size(); j++)
    {
        JSBFunction* function = functions_[j];

        if (function->IsOverload())
        {
            for (unsigned k = 0; k < overrides_.Size(); k++)
            {
                JSBFunctionSignature* override =  overrides_[k];

                if (!override->Match(function))
                    continue;

                function->SetSkipLanguage(BINDINGLANGUAGE_JAVASCRIPT, false);

                break;

            }

        }

    }

}

void JSBClass::PostProcess()
{
    for (unsigned j = 0; j < functions_.Size(); j++)
    {
        JSBFunction* function = functions_[j];
        function->Process();
    }
}

void JSBClass::AddPropertyFunction(JSBFunction* function)
{
    hasProperties_ = true;

    if (!function->GetPropertyName().Length())
    {
        ErrorExit("Function has no property name");
    }

    if (!function->IsGetter() && !function->IsSetter())
    {
        ErrorExit("Function is not a getter or setter");
    }

    JSBProperty* prop = NULL;

    if (properties_.Contains(function->GetPropertyName()))
    {
        prop = properties_[function->GetPropertyName()];
    }

    if (!prop)
    {
        prop = new JSBProperty();
        prop->name_ = function->GetPropertyName();
        properties_[function->GetPropertyName()] = prop;
    }

    if (prop->getter_ && function->IsGetter())
    {
        ErrorExit("getter already registered");
    }

    if (prop->setter_ && function->IsSetter())
    {
        ErrorExit("setter already registered");
    }

    if (function->IsSetter())
        prop->setter_ = function;
    else
        prop->getter_ = function;

}

void JSBClass::Dump()
{
    if (name_ != nativeName_)
    {
        ATOMIC_LOGINFOF("Class: %s (%s)", name_.CString(), nativeName_.CString());
    }
    else
    {
        ATOMIC_LOGINFOF("Class: %s", name_.CString());
    }

    ATOMIC_LOGINFOF("   IsObject: %s", IsObject() ? "true" : "false");

    if (baseClasses_.Size())
    {
        ATOMIC_LOGINFOF("   Bases:");
        for (unsigned i = 0; i < baseClasses_.Size(); i++)
        {

            ATOMIC_LOGINFOF("      %s", baseClasses_.At(i)->GetName().CString());
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


}
