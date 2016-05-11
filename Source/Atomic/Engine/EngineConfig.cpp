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
#include "EngineConfig.h"

namespace Atomic
{

EngineConfig EngineConfig::engineConfig_;

bool EngineConfig::LoadEngineConfig(const JSONValue& jengine)
{
    if (!jengine.IsObject())
        return false;

    for (JSONObject::ConstIterator i = jengine.Begin(); i != jengine.End(); ++i)
    {
        String key = i->first_.ToLower();
        const JSONValue& jvalue = i->second_;

        if (key == "workerthreads")
            valueMap_["WorkerThreads"] = GetBoolValue(jvalue, true);
        else if (key == "logquiet")
            valueMap_["LogQuiet"] = GetBoolValue(jvalue, false);
        else if (key == "loglevel")
            valueMap_["LogLevel"] = GetIntValue(jvalue, 1);
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
            valueMap_["Headless"] = GetBoolValue(jvalue, false);
        else if (key == "framelimiter")
            valueMap_["FrameLimiter"] = GetBoolValue(jvalue, true);
        else if (key == "flushgpu")
            valueMap_["FlushGPU"] = GetBoolValue(jvalue, false);
        else if (key == "forcegl2")
            valueMap_["ForceGL2"] = GetBoolValue(jvalue, false);
        else if (key == "orientations")
            valueMap_["Orientations"] = GetStringValue(jvalue, "LandscapeLeft LandscapeRight");
        else if (key == "vsync")
            valueMap_["VSync"] = GetBoolValue(jvalue, false);
        else if (key == "triplebuffer")
            valueMap_["TripleBuffer"] = GetBoolValue(jvalue, false);
        else if (key == "multisample")
            valueMap_["Multisample"] = GetIntValue(jvalue, 1);
        else if (key == "renderpath")
        {
            String renderPath = GetStringValue(jvalue, "forward").ToLower();

            if (renderPath == "forward")
                valueMap_["RenderPath"] = "RenderPaths/Forward.xml";
            else if (renderPath == "prepass")
                valueMap_["RenderPath"] = "RenderPaths/Prepass.xml";
            else if (renderPath == "deferred")
                valueMap_["RenderPath"] = "RenderPaths/Deferred.xml";
        }
        else if (key == "shadows")
            valueMap_["Shadows"] = GetBoolValue(jvalue, true);
        else if (key == "lowqualityshadows")
            valueMap_["LowQualityShadows"] = GetBoolValue(jvalue, false);
        else if (key == "materialquality")
        {
            String quality = GetStringValue(jvalue, "high").ToLower();

            if (quality == "high")
                valueMap_["MaterialQuality"] = QUALITY_HIGH;
            else if (quality == "medium")
                valueMap_["MaterialQuality"] = QUALITY_MEDIUM;
            else if (quality == "low")
                valueMap_["MaterialQuality"] = QUALITY_LOW;
        }
        else if (key == "texturequality")
        {
            String quality = GetStringValue(jvalue, "high").ToLower();

            if (quality == "high")
                valueMap_["TextureQuality"] = QUALITY_HIGH;
            else if (quality == "medium")
                valueMap_["TextureQuality"] = QUALITY_MEDIUM;
            else if (quality == "low")
                valueMap_["TextureQuality"] = QUALITY_LOW;
        }
        else if (key == "texturefiltermode")
        {
            String mode = GetStringValue(jvalue, "trilinear").ToLower();

            if (mode == "trilinear")
                valueMap_["TextureFilterMode"] = FILTER_TRILINEAR;
            else if (mode == "bilinear")
                valueMap_["TextureFilterMode"] = FILTER_BILINEAR;
            else if (mode == "nearest")
                valueMap_["TextureFilterMode"] = FILTER_NEAREST;
            else if (mode == "anisotropic")
                valueMap_["TextureFilterMode"] = FILTER_ANISOTROPIC;
        }
        else if (key == "textureanisotropy")
        {
            valueMap_["TextureAnisotropy"] = GetIntValue(jvalue, 4);
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
            valueMap_["WindowTitle"] = GetStringValue(jvalue, "Atomic");
        else if (key == "fullscreen")
            valueMap_["FullScreen"] = GetBoolValue(jvalue, false);
        else if (key == "borderless")
            valueMap_["Borderless"] = GetBoolValue(jvalue, false);
        else if (key == "resizable")
            valueMap_["WindowResizable"] = GetBoolValue(jvalue, false);
        else if (key == "width")
            valueMap_["WindowWidth"] = GetIntValue(jvalue, false);
        else if (key == "height")
            valueMap_["WindowHeight"] = GetIntValue(jvalue, false);
        else if (key == "positionx")
            valueMap_["WindowPositionX"] = GetIntValue(jvalue, false);
        else if (key == "positiony")
            valueMap_["WindowPositionY"] = GetIntValue(jvalue, false);

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
            valueMap_["Sound"] = GetBoolValue(jvalue, true);
        else if (key == "interpolation")
            valueMap_["SoundInterpolation"] = GetBoolValue(jvalue, true);
        else if (key == "stereo")
            valueMap_["SoundStereo"] = GetBoolValue(jvalue, true);
        else if (key == "bufferms")
            valueMap_["SoundBuffer"] = GetIntValue(jvalue, 100);
        else if (key == "mixrate")
            valueMap_["SoundMixRate"] = GetIntValue(jvalue, 44100);

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
            valueMap_["TouchEmulation"] = GetBoolValue(jvalue, false);
    }

    return true;

}

bool EngineConfig::LoadWebViewConfig(const JSONValue& jwebview)
{
    if (!jwebview.IsObject())
        return false;

    for (JSONObject::ConstIterator i = jwebview.Begin(); i != jwebview.End(); ++i)
    {
        String key = i->first_.ToLower();
        const JSONValue& jvalue = i->second_;

        if (key == "useragent")
            valueMap_["WebViewUserAgent"] = GetStringValue(jvalue, String::EMPTY);
        else if (key == "productversion")
            valueMap_["WebViewProductVersion"] = GetStringValue(jvalue, String::EMPTY);
        else if (key == "debugPort")
            valueMap_["WebViewDebugPort"] = GetIntValue(jvalue, 8080);
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

    const JSONValue& jwebview = jdesktop["webview"];
    if (jwebview.IsObject())
        LoadWebViewConfig(jwebview);


    return true;
}

}
