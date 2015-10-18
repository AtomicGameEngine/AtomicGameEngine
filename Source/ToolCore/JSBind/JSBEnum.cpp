//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
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
