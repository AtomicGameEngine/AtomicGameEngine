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

#include "../../Atomic/Core/Context.h"
#include "../../Atomic/IO/Log.h"
#include "../../Atomic/IO/File.h"
#include "../../Atomic/IO/FileSystem.h"
#include "../Atomic/Resource/JSONFile.h"
#include "../../Atomic/Graphics/GraphicsDefs.h"
#include "ImportConfig.h"

namespace Atomic
{

ImportConfig ImportConfig::importConfig_;

bool ImportConfig::LoadModelImporterConfig(const JSONValue& jModelImporterConfig)
{
    const JSONValue& jflags = jModelImporterConfig["aiFlagsDefault"];
    if (!jflags.IsObject())
        return false;

    for (JSONObject::ConstIterator i = jflags.Begin(); i != jflags.End(); ++i)
    {
        String key = i->first_;
        const JSONValue& jvalue = i->second_;

        if (key == "convertToLeftHanded")
            valueMap_["aiProcess_ConvertToLeftHanded"] = GetBoolValue(jvalue, true);
        else if (key == "joinIdenticalVertices")
            valueMap_["aiProcess_JoinIdenticalVertices"] = GetBoolValue(jvalue, true);
        else if (key == "triangulate")
            valueMap_["aiProcess_Triangulate"] = GetBoolValue(jvalue, true);
        else if (key == "genSmoothNormals")
            valueMap_["aiProcess_GenSmoothNormals"] = GetBoolValue(jvalue, true);
        else if (key == "limitBoneWeights")
            valueMap_["aiProcess_LimitBoneWeights"] = GetBoolValue(jvalue, true);
        else if (key == "improveCacheLocality")
            valueMap_["aiProcess_ImproveCacheLocality"] = GetBoolValue(jvalue, true);
        else if (key == "fixInFacingNormals")
            valueMap_["aiProcess_FixInfacingNormals"] = GetBoolValue(jvalue, true);
        else if (key == "findInvalidData")
            valueMap_["aiProcess_FindInvalidData"] = GetBoolValue(jvalue, true);
        else if (key == "genUVCoords")
            valueMap_["aiProcess_GenUVCoords"] = GetBoolValue(jvalue, true);
        else if (key == "findInstances")
            valueMap_["aiProcess_FindInstances"] = GetBoolValue(jvalue, true);
        else if (key == "optimizeMeshes")
            valueMap_["aiProcess_OptimizeMeshes"] = GetBoolValue(jvalue, true);
        else if (key == "importMaterials")
            valueMap_["ImportMaterials"] = GetBoolValue(jvalue, true);
    }

    return true;
}

bool ImportConfig::LoadTextureImporterConfig(const JSONValue& jTextureImporterConfig)
{
    for (JSONObject::ConstIterator i = jTextureImporterConfig.Begin(); i != jTextureImporterConfig.End(); ++i)
    {
        String key = i->first_;
        const JSONValue& jvalue = i->second_;

        if (key == "compressTextures")
            valueMap_["tiProcess_CompressTextures"] = GetBoolValue(jvalue, false);
    }

    return true;
}

bool ImportConfig::LoadDesktopConfig(JSONValue root)
{
    const JSONValue& jdesktop = root["desktop"];

    if (!jdesktop.IsObject())
        return false;

    const JSONValue& jModelImporterConfig = jdesktop["ModelImporter"];
    if (jModelImporterConfig.IsObject())
        LoadModelImporterConfig(jModelImporterConfig);
 
    const JSONValue& jTextureImporterConfig = jdesktop["TextureImporter"];
    if (jTextureImporterConfig.IsObject())
        LoadTextureImporterConfig(jTextureImporterConfig);

    return true;
}

}