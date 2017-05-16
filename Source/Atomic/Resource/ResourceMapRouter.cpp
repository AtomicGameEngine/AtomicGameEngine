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

#include <Atomic/IO/Log.h>
#include "JSONValue.h"
#include "JSONFile.h"
#include "ResourceCache.h"
#include "ResourceMapRouter.h"

namespace Atomic
{

    ResourceMapRouter::ResourceMapRouter(Context* context, const String& mapFile) : ResourceRouter(context)
    {
        if (mapFile.Length())
        {
            ResourceCache* cache = GetSubsystem<ResourceCache>();

            if (!cache)
            {
                ATOMIC_LOGERROR("ResourceMapRouter::ResourceMapRouter - ResourceCache subsystem not found");
                return;
            }

            SharedPtr<JSONFile> jsonFile = cache->GetTempResource<JSONFile>(mapFile);

            context_->RegisterSubsystem(this);

            if (jsonFile.NotNull())
            {
                if (Load(jsonFile->GetRoot()))
                {
                    cache->AddResourceRouter(this);
                }
                else
                {
                    ATOMIC_LOGERROR("ResourceMapRouter::ResourceMapRouter - Unable to load resource map json");
                }
            }
            else
            {
                ATOMIC_LOGERROR("ResourceMapRouter::ResourceMapRouter - Unable to parse resource map json");
            }

        }
    }

    bool ResourceMapRouter::Load(const JSONValue& json)
    {
        const JSONValue& assetMap = json.Get("assetMap");

        if (!assetMap.IsObject())
            return false;

        ConstJSONObjectIterator itr = assetMap.Begin();
        while (itr != assetMap.End())
        {
            StringVector tags = itr->first_.Split(';');

            if (tags.Size() == 2)
            {
                StringHash::RegisterSignificantString(tags[0]);
                StringHash::RegisterSignificantString(tags[1]);
                resourceMap_[tags[0]][tags[1]] = itr->second_.GetString();
            }

            itr++;
        }

        return true;
    }

    void ResourceMapRouter::Route(String& name, StringHash type, ResourceRequest requestType)
    {

        if (type == StringHash::ZERO)
            return;

        HashMap<StringHash, HashMap<StringHash, String>>::ConstIterator itr = resourceMap_.Find(type);
        if (itr == resourceMap_.End())
        {
            return;
        }

        HashMap<StringHash, String>::ConstIterator mitr = itr->second_.Find(name);
        if (mitr == itr->second_.End())
        {
            return;
        }


        name = mitr->second_;

    }

}
