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
#include "JSONFile.h"
#include "../Graphics/GraphicsDefs.h"
#include "ImportConfig.h"

namespace Atomic
{

VariantMap ImportConfig::importConfig_;
String ImportConfig::importConfigFilename_;

bool ImportConfig::GetBoolValue(const JSONValue& jvalue, bool defaultValue)
{
    bool value = defaultValue;

    if (jvalue.IsBool())
        value = jvalue.GetBool();

    return value;
}

bool ImportConfig::LoadFromJSON(const String& json)
{
    importConfig_.Clear();

    JSONValue jroot;

    if (!JSONFile::ParseJSON(json, jroot))
    {
        LOGERRORF("ImportConfig::LoadFromJSON - Unable to parse config file JSON: %s", importConfigFilename_.CString());
        return false;
    }

    if (!jroot.IsObject())
        return false;

    if (!LoadDesktopConfig(jroot))
        return false;

    return true;
}

bool ImportConfig::LoadFromFile(Context *context, const String& filename)
{

    FileSystem* fileSystem = context->GetSubsystem<FileSystem>();

    if (!fileSystem->FileExists(filename))
        return false;

    importConfigFilename_ = filename;

    SharedPtr<File> file(new File(context));

    if (!file->Open(filename))
    {
        LOGERRORF("ImportConfig::LoadFromFile - Unable to open config file %s", filename.CString());
        return false;
    }

    String json;
    file->ReadText(json);

    return LoadFromJSON(json);
}

void ImportConfig::ApplyConfig(VariantMap& settings, bool overwrite)
{
    VariantMap::ConstIterator itr = importConfig_.Begin();
    if (overwrite)
    {
        while (itr != importConfig_.End())
        {
            settings[itr->first_] = itr->second_;
            itr++;
        }
    }
    else
    {
        while (itr != importConfig_.End())
        {
            settings.InsertNew(itr->first_, itr->second_);
            itr++;
        }
    }

}

bool ImportConfig::LoadAIFlagsDefaultConfig(const JSONValue& jflags)
{
    if (!jflags.IsObject())
        return false;

    for (JSONObject::ConstIterator i = jflags.Begin(); i != jflags.End(); ++i)
    {
        String key = i->first_;
        const JSONValue& jvalue = i->second_;

        if (key == "convertToLeftHanded")
            importConfig_["aiProcess_ConvertToLeftHanded"] = GetBoolValue(jvalue, true);
        else if (key == "joinIdenticalVertices")
            importConfig_["aiProcess_JoinIdenticalVertices"] = GetBoolValue(jvalue, true);
        else if (key == "triangulate")
            importConfig_["aiProcess_Triangulate"] = GetBoolValue(jvalue, true);
        else if (key == "genSmoothNormals")
            importConfig_["aiProcess_GenSmoothNormals"] = GetBoolValue(jvalue, true);
        else if (key == "limitBoneWeights")
            importConfig_["aiProcess_LimitBoneWeights"] = GetBoolValue(jvalue, true);
        else if (key == "improveCacheLocality")
            importConfig_["aiProcess_ImproveCacheLocality"] = GetBoolValue(jvalue, true);
        else if (key == "fixInFacingNormals")
            importConfig_["aiProcess_FixInfacingNormals"] = GetBoolValue(jvalue, true);
        else if (key == "fixInfacingNormals")
            importConfig_["aiProcess_FixInfacingNormals"] = GetBoolValue(jvalue, true);
        else if (key == "findInvalidData")
            importConfig_["aiProcess_FindInvalidData"] = GetBoolValue(jvalue, true);
        else if (key == "genUVCoords")
            importConfig_["aiProcess_GenUVCoords"] = GetBoolValue(jvalue, true);
        else if (key == "findInstances")
            importConfig_["aiProcess_FindInstances"] = GetBoolValue(jvalue, true);
        else if (key == "optimizeMeshes")
            importConfig_["aiProcess_OptimizeMeshes"] = GetBoolValue(jvalue, true);
    }

    return true;
}

bool ImportConfig::LoadDesktopConfig(JSONValue root)
{
    const JSONValue& jdesktop = root["desktop"];

    if (!jdesktop.IsObject())
        return false;

    const JSONValue& jflags = jdesktop["aiFlagsDefault"];
    if (jflags.IsObject())
        LoadAIFlagsDefaultConfig(jflags);
 
    return true;
}

}