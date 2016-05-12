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

#include "../../Atomic/Core/Variant.h"
#include "../../Atomic/Resource/JSONValue.h"
#include "../../Atomic/Resource/Configuration.h"

namespace Atomic
{
    class Context;

    class AssetBuildConfig :
        Configuration
    {

    public:

        static bool LoadFromFile(Context* context, const String& filename) { return assetBuildConfig_.Configuration::LoadFromFile(context, filename); }
        static bool LoadFromJSON(const String& json) { return assetBuildConfig_.Configuration::LoadFromJSON(json); }

        /// Apply the configuration to a setting variant map, values that exist will not be overriden
        static void ApplyConfig(VariantMap& settings, bool overwrite = false) { return assetBuildConfig_.Configuration::ApplyConfig(settings, overwrite); }

        static const bool IsLoaded() { return assetBuildConfig_.Configuration::IsLoaded(); }

    private:

        virtual bool LoadDesktopConfig(JSONValue root);
        bool LoadAssetBuildTagArray(const JSONArray& tags);

        static AssetBuildConfig assetBuildConfig_;
    };
}
