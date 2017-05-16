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

#pragma once

#include "ResourceCache.h"

namespace Atomic
{

class JSONValue;

// Resource router which maps resource requests to processed cache files, different files can be mapped based on type of request
class ATOMIC_API ResourceMapRouter : public ResourceRouter
{
    ATOMIC_OBJECT(ResourceMapRouter, ResourceRouter)

public:
    /// Construct.
    ResourceMapRouter(Context* context, const String& mapFile = String::EMPTY);

    void Route(String& name, StringHash type, ResourceRequest requestType);

    const HashMap<StringHash, String>* GetTypeCacheMap(StringHash hash) const { return resourceMap_[hash]; }

private:

    bool Load(const JSONValue& json);

    // Type -> FromPath (hash) -> CachePath
    HashMap<StringHash, HashMap<StringHash, String>> resourceMap_;
};

}
