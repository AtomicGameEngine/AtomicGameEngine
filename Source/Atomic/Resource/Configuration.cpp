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


#include "../Core/Context.h"
#include "../IO/Log.h"
#include "../IO/File.h"
#include "../IO/FileSystem.h"
#include "../Resource/JSONFile.h"
#include "../Graphics/GraphicsDefs.h"
#include "Configuration.h"

namespace Atomic
{

    bool Configuration::GetBoolValue(const JSONValue& jvalue, bool defaultValue)
    {
        bool value = defaultValue;

        if (jvalue.IsBool())
            value = jvalue.GetBool();

        return value;
    }

    int Configuration::GetIntValue(const JSONValue& jvalue, int defaultValue)
    {
        int value = defaultValue;

        if (jvalue.IsNumber())
            value = jvalue.GetInt();

        return value;
    }

    String Configuration::GetStringValue(const JSONValue& jvalue, const String& defaultValue)
    {
        String value = defaultValue;

        if (jvalue.IsString())
            value = jvalue.GetString();

        return value;
    }

    void Configuration::GetArrayValue(const JSONArray & jarray, StringVector& value)
    {
        value.Clear();

        for (JSONArray::ConstIterator it = jarray.Begin(); it != jarray.End(); it++)
        {
            if (it->IsString())
            {
                value.Push(it->GetString());
            }
        }

    }

    Configuration::Configuration() :
        isLoaded_(false)
    {
    }

    bool Configuration::LoadFromFile(Context *context, const String& filename)
    {

        FileSystem* fileSystem = context->GetSubsystem<FileSystem>();

        if (!fileSystem->FileExists(filename))
            return false;

        filename_ = filename;

        SharedPtr<File> file(new File(context));

        if (!file->Open(filename))
        {
            ATOMIC_LOGERRORF("Configuration::LoadFromFile - Unable to open config file %s", filename.CString());
            return false;
        }

        String json;
        file->ReadText(json);

        return LoadFromJSON(json);
    }

    void Configuration::ApplyConfig(VariantMap& settings, bool overwrite)
    {
        if (!isLoaded_) {
            ATOMIC_LOGERRORF("Configuration::ApplyConfig - Applying a config that has not yet been populated");
            return;
        }

        VariantMap::ConstIterator itr = valueMap_.Begin();
        if (overwrite)
        {
            while (itr != valueMap_.End())
            {
                settings[itr->first_] = itr->second_;
                itr++;
            }
        }
        else
        {
            while (itr != valueMap_.End())
            {
                settings.InsertNew(itr->first_, itr->second_);
                itr++;
            }
        }

    }

    bool Configuration::LoadFromJSON(const String& json)
    {
        Clear();

        JSONValue jroot;

        if (!JSONFile::ParseJSON(json, jroot))
        {
            ATOMIC_LOGERRORF("Configuration::LoadFromJSON - Unable to parse config file JSON: %s", filename_.CString());
            return false;
        }

        if (!jroot.IsObject())
            return false;

        if (!LoadDesktopConfig(jroot))
            return false;

        isLoaded_ = true;
        return true;
    }

    void Configuration::Clear()
    {
        valueMap_.Clear();
    }

}
