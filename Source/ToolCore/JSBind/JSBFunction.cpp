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
