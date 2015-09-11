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
}
