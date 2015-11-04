//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include "JSBFunction.h"

namespace ToolCore
{
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

bool JSBFunction::Match(JSBFunction* func)
{
    if (func->Skip() || func->GetName() != name_)
        return false;

    if (!returnType_)
    {
        if (func->GetReturnType())
            return false;
    }
    else
    {
        if (!returnType_->Match(func->GetReturnType()))
            return false;
    }

    Vector<JSBFunctionType*>& fparams = func->GetParameters();

    if (parameters_.Size() != fparams.Size())
        return false;

    for ( unsigned j = 0; j < fparams.Size(); j++)
    {
        if (!parameters_[j]->Match(fparams[j]))
            return false;
    }

    return true;
}

JSBClass* JSBFunction::GetReturnClass()
{
    if (!returnType_)
        return 0;

    if (!returnType_->type_->asClassType())
        return 0;

    return returnType_->type_->asClassType()->class_;

}

}
