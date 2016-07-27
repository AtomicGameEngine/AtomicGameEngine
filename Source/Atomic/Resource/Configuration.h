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
#include "JSONValue.h"

namespace Atomic
{

class Context;

/// Configuration base class for mapping JSON setting files to VariantMap configuration settings
class Configuration
{
public:

    Configuration();

    bool LoadFromFile(Context* context, const String& filename);
    bool LoadFromJSON(const String& json);
    void Clear();

    /// Apply the configuration to a setting variant map, values that exist will not be overriden
    void ApplyConfig(VariantMap& settings, bool overwrite = false);

    const VariantMap& GetConfig() { return valueMap_; }
    bool IsLoaded() const { return isLoaded_; }

protected:
    static bool GetBoolValue(const JSONValue& jvalue, bool defaultValue);
    static int GetIntValue(const JSONValue& jvalue, int defaultValue);
    static String GetStringValue(const JSONValue& jvalue, const String& defaultValue);
    static void GetArrayValue(const JSONArray& jarray, StringVector& value);

    virtual bool LoadDesktopConfig(JSONValue root) { return true; };

    VariantMap valueMap_;

private:
    String filename_;
    bool isLoaded_;
};
}
