//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
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

#include <Atomic/Core/Context.h>
#include <Atomic/IO/Deserializer.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Core/Profiler.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/IO/Serializer.h>

#include "NETComponentClass.h"
#include "NETAssemblyFile.h"

namespace Atomic
{

/*

"enums":[
      {
         "name":"BehaviorState",
         "values":{
            "Friendly":0,
            "Aggressive":10,
            "Neutral":11
         }
      }
   ],

*/
NETAssemblyFile::NETAssemblyFile(Context* context) :
    Resource(context)
{

}

NETAssemblyFile::~NETAssemblyFile()
{

}

NETComponentClass* NETAssemblyFile::GetComponentClass(const String& name)
{
    if (!componentClasses_.Contains(name))
        return 0;

    return componentClasses_[name];

}

bool NETAssemblyFile::ParseAssemblyJSON(const JSONValue& json)
{
    componentClasses_.Clear();
    enums_.Clear();

    const JSONArray& enums = json.Get("enums").GetArray();

    for (unsigned i = 0; i < enums.Size(); i++)
    {
        const JSONValue& ejson = enums.At(i);

        String enumName = ejson.Get("name").GetString();

        const JSONObject& evalues = ejson.Get("values").GetObject();

        JSONObject::ConstIterator itr = evalues.Begin();

        Vector<EnumInfo> values;

        while(itr != evalues.End())
        {
            EnumInfo info;
            info.name_ = itr->first_;
            info.value_ = itr->second_.GetInt();
            values.Push(info);
            itr++;
        }

        enums_[enumName] = values;
    }

    const JSONArray& components = json.Get("components").GetArray();

    for (unsigned i = 0; i < components.Size(); i++)
    {
        const JSONValue& cjson = components.At(i);

        SharedPtr<NETComponentClass> c(new NETComponentClass(context_, this));

        if (c->ParseJSON(cjson))
            componentClasses_[c->GetName()] = c;
    }

    return true;
}

void NETAssemblyFile::RegisterObject(Context* context)
{
    context->RegisterFactory<NETAssemblyFile>();
}

bool NETAssemblyFile::BeginLoad(Deserializer& source)
{
    return true;
}

bool NETAssemblyFile::Save(Serializer& dest) const
{
    return true;
}

}
