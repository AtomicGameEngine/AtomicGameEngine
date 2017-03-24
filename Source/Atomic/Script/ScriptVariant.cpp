//
// Copyright (c) 2014-2016, THUNDERBEAST GAMES LLC All rights reserved
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

#include "ScriptVariant.h"

namespace Atomic

{

Resource* ScriptVariant::GetResource() const
{
    if (variant_.GetType() == VAR_RESOURCEREF)
    {
        const ResourceRef& ref = variant_.GetResourceRef();

        if (!ref.name_.Length() || !ref.type_)
            return 0;

        ResourceCache* cache = ScriptSystem::GetContext()->GetSubsystem<ResourceCache>();

        return cache->GetResource(ref.type_, ref.name_);       
    }

    return 0;
}

void ScriptVariant::SetResource(Resource* resource)
{
    if (!resource)
    {
        variant_ = ResourceRef();
        return;
    }
    variant_ = ResourceRef(resource->GetType(), resource->GetName());
}

}