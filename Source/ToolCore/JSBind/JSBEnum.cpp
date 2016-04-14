//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
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

#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/Resource/JSONFile.h>

#include "JSBind.h"
#include "JSBModule.h"
#include "JSBEnum.h"

namespace ToolCore
{

JSBEnum::JSBEnum(Context* context, JSBModule *module, const String& name) : Object(context),
    module_(module), name_(name), header_(0)
{

}

JSBEnum::~JSBEnum()
{

}

JSBPackage* JSBEnum::GetPackage()
{
    return module_->GetPackage();
}

void JSBEnum::Preprocess()
{
    // TextureUnit is a special case as uses preprocessor for values depending on
    // Desktop or Mobile graphics

    JSBind* jsbind = GetSubsystem<JSBind>();

    if (name_ == "TextureUnit")
    {
        values_.Clear();

        values_["TU_DIFFUSE"] = "0";
        values_["TU_ALBEDOBUFFER"] = "0";
        values_["TU_NORMAL"] = "1";
        values_["TU_NORMALBUFFER"] = "1";
        values_["TU_SPECULAR"] = "2";
        values_["TU_EMISSIVE"] = "3";
        values_["TU_ENVIRONMENT"] = "4";

        String platform = jsbind->GetPlatform();

        bool mobile = platform == "WEB" || platform == "ANDROID" || platform == "IOS";

        if (mobile)
        {
            values_["TU_LIGHTRAMP"] = "5";
            values_["TU_LIGHTSHAPE"] = "6";
            values_["TU_SHADOWMAP"] = "7";
            values_["MAX_MATERIAL_TEXTURE_UNITS"] = "5";
            values_["MAX_TEXTURE_UNITS"] = "8";
        }
        else
        {
            values_["TU_VOLUMEMAP"] = "5";
            values_["TU_CUSTOM1"] = "6";
            values_["TU_CUSTOM2"] = "7";
            values_["TU_LIGHTRAMP"] = "8";
            values_["TU_LIGHTSHAPE"] = "9";
            values_["TU_SHADOWMAP"] = "10";
            values_["TU_FACESELECT"] = "11";
            values_["TU_INDIRECTION"] = "12";
            values_["TU_DEPTHBUFFER"] = "13";
            values_["TU_LIGHTBUFFER"] = "14";
            values_["TU_ZONE"] = "15";
            values_["MAX_MATERIAL_TEXTURE_UNITS"] = "8";
            values_["MAX_TEXTURE_UNITS"] = "16";

        }

    }
}

}
