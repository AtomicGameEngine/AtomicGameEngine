//
// Copyright (c) 2008-2015 the Urho3D project.
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

#include "JSComponentFile.h"

namespace Atomic
{

JSComponentFile::JSComponentFile(Context* context) :
    Resource(context)
{
}

JSComponentFile::~JSComponentFile()
{

}

void JSComponentFile::RegisterObject(Context* context)
{
    context->RegisterFactory<JSComponentFile>();
}

bool JSComponentFile::BeginLoad(Deserializer& source)
{
    // TODO: cache these for non-editor builds

    unsigned dataSize = source.GetSize();
    if (!dataSize && !source.GetName().Empty())
    {
        LOGERROR("Zero sized component file in " + source.GetName());
        return false;
    }

    SharedArrayPtr<char> buffer(new char[dataSize + 1]);
    if (source.Read(buffer.Get(), dataSize) != dataSize)
        return false;
    buffer[dataSize] = '\0';

    String text = buffer.Get();
    Vector<String> lines = text.Split('\n');

    for (unsigned i = 0; i < lines.Size(); i++)
    {
        String line = lines[i].Trimmed();

        if (line[0] != '\"')
            continue;

        unsigned pos = line.Find("\";");
        if (pos == String::NPOS)
            continue;

        text = line.Substring(1, pos - 1);

        Vector<String> field = text.Split(' ');

        if (field.Size() != 2 || !field[0].Length() || !field[1].Length())
            continue;

        String name = field[0];
        String stringType = field[1];

        VariantType variantType = VAR_NONE;

        if (stringType == "boolean")
        {
            variantType = VAR_BOOL;
        }
        else if (stringType == "string")
        {
            variantType = VAR_STRING;
        }
        else if (stringType == "number")
        {
            variantType = VAR_FLOAT;
        }
        else if (stringType == "Vector3")
        {
            variantType = VAR_VECTOR3;
        }

        if (variantType == VAR_NONE)
            continue;

        fields_[name] = variantType;

    }

    SetMemoryUse(dataSize);

    return true;
}

bool JSComponentFile::Save(Serializer& dest) const
{
    return true;
}



}
