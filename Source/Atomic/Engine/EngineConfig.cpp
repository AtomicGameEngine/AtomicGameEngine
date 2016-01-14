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

#include "../IO/File.h"
#include "../Resource/JSONFile.h"
#include "../Graphics/GraphicsDefs.h"
#include "EngineConfig.h"

namespace Atomic
{

VariantMap EngineConfig::engineConfig_;

bool EngineConfig::GetBoolValue(const JSONValue& jvalue, bool defaultValue)
{
    bool value = defaultValue;

    if (jvalue.IsBool())
        value = jvalue.GetBool();

    return value;
}

int EngineConfig::GetIntValue(const JSONValue& jvalue, int defaultValue)
{
    int value = defaultValue;

    if (jvalue.IsNumber())
        value = jvalue.GetInt();

    return value;
}

String EngineConfig::GetStringValue(const JSONValue& jvalue, const String& defaultValue)
{
    String value = defaultValue;

    if (jvalue.IsString())
        value = jvalue.GetString();

    return value;
}

bool EngineConfig::LoadEngineConfig(const JSONValue& jengine)
{
    if (!jengine.IsObject())
        return false;

    for (JSONObject::ConstIterator i = jengine.Begin(); i != jengine.End(); ++i)
    {
        String key = i->first_.ToLower();
        const JSONValue& jvalue = i->second_;

        if (key == "workerthreads")
            engineConfig_["WorkerThreads"] = GetBoolValue(jvalue, true);
        else if (key == "logquiet")
            engineConfig_["LogQuiet"] = GetBoolValue(jvalue, false);
        else if (key == "loglevel")
            engineConfig_["LogLevel"] = GetIntValue(jvalue, 1);
    }

    return true;
}

bool EngineConfig::LoadGraphicsConfig(const JSONValue& jgraphics)
{
    if (!jgraphics.IsObject())
        return false;

    for (JSONObject::ConstIterator i = jgraphics.Begin(); i != jgraphics.End(); ++i)
    {
        String key = i->first_.ToLower();
        const JSONValue& jvalue = i->second_;

        if (key == "headless")
            engineConfig_["Headless"] = GetBoolValue(jvalue, false);
        else if (key == "framelimiter")
            engineConfig_["FrameLimiter"] = GetBoolValue(jvalue, true);
        else if (key == "flushgpu")
            engineConfig_["FlushGPU"] = GetBoolValue(jvalue, false);
        else if (key == "forcegl2")
            engineConfig_["ForceGL2"] = GetBoolValue(jvalue, false);
        else if (key == "orientations")
            engineConfig_["Orientations"] = GetStringValue(jvalue, "LandscapeLeft LandscapeRight");
        else if (key == "vsync")
            engineConfig_["VSync"] = GetBoolValue(jvalue, false);
        else if (key == "triplebuffer")
            engineConfig_["TripleBuffer"] = GetBoolValue(jvalue, false);
        else if (key == "multisample")
            engineConfig_["Multisample"] = GetIntValue(jvalue, 1);
        else if (key == "renderpath")
        {
            String renderPath = GetStringValue(jvalue, "forward").ToLower();

            if (renderPath == "forward")
                engineConfig_["RenderPath"] = "RenderPaths/Forward.xml";
            else if (renderPath == "prepass")
                engineConfig_["RenderPath"] = "RenderPaths/Prepass.xml";
            else if (renderPath == "deferred")
                engineConfig_["RenderPath"] = "RenderPaths/Deferred.xml";
        }
        else if (key == "shadows")
            engineConfig_["Shadows"] = GetBoolValue(jvalue, true);
        else if (key == "lowqualityshadows")
            engineConfig_["LowQualityShadows"] = GetBoolValue(jvalue, false);
        else if (key == "materialquality")
        {
            String quality = GetStringValue(jvalue, "high").ToLower();

            if (quality == "high")
                engineConfig_["MaterialQuality"] = QUALITY_HIGH;
            else if (quality == "medium")
                engineConfig_["MaterialQuality"] = QUALITY_MEDIUM;
            else if (quality == "low")
                engineConfig_["MaterialQuality"] = QUALITY_LOW;
        }
        else if (key == "texturequality")
        {
            String quality = GetStringValue(jvalue, "high").ToLower();

            if (quality == "high")
                engineConfig_["TextureQuality"] = QUALITY_HIGH;
            else if (quality == "medium")
                engineConfig_["TextureQuality"] = QUALITY_MEDIUM;
            else if (quality == "low")
                engineConfig_["TextureQuality"] = QUALITY_LOW;
        }
        else if (key == "texturefiltermode")
        {
            String mode = GetStringValue(jvalue, "trilinear").ToLower();

            if (mode == "trilinear")
                engineConfig_["TextureFilterMode"] = FILTER_TRILINEAR;
            else if (mode == "bilinear")
                engineConfig_["TextureFilterMode"] = FILTER_BILINEAR;
            else if (mode == "nearest")
                engineConfig_["TextureFilterMode"] = FILTER_NEAREST;
            else if (mode == "anisotropic")
                engineConfig_["TextureFilterMode"] = FILTER_ANISOTROPIC;
        }
        else if (key == "textureanisotropy")
        {
            engineConfig_["TextureAnisotropy"] = GetIntValue(jvalue, 4);
        }


    }

    return true;
}

bool EngineConfig::LoadWindowConfig(const JSONValue& jwindow)
{
    if (!jwindow.IsObject())
        return false;

    for (JSONObject::ConstIterator i = jwindow.Begin(); i != jwindow.End(); ++i)
    {
        String key = i->first_.ToLower();
        const JSONValue& jvalue = i->second_;

        if (key == "title")
            engineConfig_["WindowTitle"] = GetStringValue(jvalue, "Atomic");
        else if (key == "fullscreen")
            engineConfig_["FullScreen"] = GetBoolValue(jvalue, false);
        else if (key == "borderless")
            engineConfig_["Borderless"] = GetBoolValue(jvalue, false);
        else if (key == "resizable")
            engineConfig_["WindowResizable"] = GetBoolValue(jvalue, false);
        else if (key == "width")
            engineConfig_["WindowWidth"] = GetIntValue(jvalue, false);
        else if (key == "height")
            engineConfig_["WindowHeight"] = GetIntValue(jvalue, false);
        else if (key == "positionx")
            engineConfig_["WindowPositionX"] = GetIntValue(jvalue, false);
        else if (key == "positiony")
            engineConfig_["WindowPositionY"] = GetIntValue(jvalue, false);

    }

    return true;

}

bool EngineConfig::LoadSoundConfig(const JSONValue& jsound)
{
    if (!jsound.IsObject())
        return false;

    for (JSONObject::ConstIterator i = jsound.Begin(); i != jsound.End(); ++i)
    {
        String key = i->first_.ToLower();
        const JSONValue& jvalue = i->second_;

        if (key == "enabled")
            engineConfig_["Sound"] = GetBoolValue(jvalue, true);
        else if (key == "interpolation")
            engineConfig_["SoundInterpolation"] = GetBoolValue(jvalue, true);
        else if (key == "stereo")
            engineConfig_["SoundStereo"] = GetBoolValue(jvalue, true);
        else if (key == "bufferms")
            engineConfig_["SoundBuffer"] = GetIntValue(jvalue, 100);
        else if (key == "mixrate")
            engineConfig_["SoundMixRate"] = GetIntValue(jvalue, 44100);

    }

    return true;

}

bool EngineConfig::LoadInputConfig(const JSONValue& jinput)
{
    if (!jinput.IsObject())
        return false;

    for (JSONObject::ConstIterator i = jinput.Begin(); i != jinput.End(); ++i)
    {
        String key = i->first_.ToLower();
        const JSONValue& jvalue = i->second_;

        if (key == "touchemulation")
            engineConfig_["TouchEmulation"] = GetBoolValue(jvalue, false);
    }

    return true;

}


bool EngineConfig::LoadDesktopConfig(JSONValue root)
{
    const JSONValue& jdesktop = root["desktop"];

    if (!jdesktop.IsObject())
        return false;

    const JSONValue& jengine = jdesktop["engine"];
    if (jengine.IsObject())
        LoadEngineConfig(jengine);

    const JSONValue& jgraphics = jdesktop["graphics"];
    if (jgraphics.IsObject())
        LoadGraphicsConfig(jgraphics);

    const JSONValue& jwindow = jdesktop["window"];
    if (jwindow.IsObject())
        LoadWindowConfig(jwindow);

    const JSONValue& jsound = jdesktop["sound"];
    if (jsound.IsObject())
        LoadSoundConfig(jsound);

    const JSONValue& jinput = jdesktop["input"];
    if (jinput.IsObject())
        LoadInputConfig(jinput);

    return true;
}

bool EngineConfig::LoadFromJSON(const String& json)
{
    engineConfig_.Clear();

    JSONValue jroot;

    if (!JSONFile::ParseJSON(json, jroot))
        return false;

    if (!jroot.IsObject())
        return false;

    if (!LoadDesktopConfig(jroot))
        return false;

    return true;
}

bool EngineConfig::LoadFromFile(Context *context, const String& filename)
{
    SharedPtr<File> file(new File(context));

    if (!file->Open(filename))
        return false;

    String json;
    file->ReadText(json);

    return LoadFromJSON(json);
}

void EngineConfig::ApplyConfig(VariantMap& settings)
{
    VariantMap::ConstIterator itr = engineConfig_.Begin();
    while (itr != engineConfig_.End())
    {
        settings.InsertNew(itr->first_, itr->second_);
        itr++;
    }

}

}
