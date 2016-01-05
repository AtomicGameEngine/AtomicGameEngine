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

#pragma once

#include "../Core/Variant.h"
#include "../Resource/JSONValue.h"

namespace Atomic
{

class Context;

/// Atomic engine configuration
class EngineConfig
{

public:

    static bool LoadFromFile(Context* context, const String& filename);
    static bool LoadFromJSON(const String& json);

    /// Apply the configuration to a setting variant map, values that exist will not be overriden
    static void ApplyConfig(VariantMap& settings);

    static const VariantMap& GetConfig() { return engineConfig_; }

private:

    static bool LoadDesktopConfig(JSONValue root);
    static bool LoadGraphicsConfig(const JSONValue& jgraphics);
    static bool LoadWindowConfig(const JSONValue& jwindow);
    static bool LoadSoundConfig(const JSONValue& jsound);
    static bool LoadInputConfig(const JSONValue& jinput);

    static bool GetBoolValue(const JSONValue& jvalue, bool defaultValue);
    static int GetIntValue(const JSONValue& jvalue, int defaultValue);
    static String GetStringValue(const JSONValue& jvalue, const String& defaultValue);

    static bool LoadEngineConfig(const JSONValue& jengine);

    static VariantMap engineConfig_;
    static String engineConfigFilename_;

};

}
