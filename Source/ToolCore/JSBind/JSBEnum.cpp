#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/Resource/JSONFile.h>

#include "JSBind.h"
#include "JSBModule.h"
#include "JSBEnum.h"

namespace ToolCore
{

JSBEnum::JSBEnum(Context* context, JSBModule *module, const String& name) : Object(context),
    module_(module), name_(name)
{

}

JSBEnum::~JSBEnum()
{

}

void JSBEnum::Preprocess()
{
    // TextureUnit is a special case as uses preprocessor for values depending on
    // Desktop or Mobile graphics

    JSBind* jsbind = GetSubsystem<JSBind>();

    if (name_ == "TextureUnit")
    {
        values_.Clear();

        values_.Push("TU_DIFFUSE");
        values_.Push("TU_ALBEDOBUFFER");
        values_.Push("TU_NORMAL");
        values_.Push("TU_NORMALBUFFER");
        values_.Push("TU_SPECULAR");
        values_.Push("TU_EMISSIVE");
        values_.Push("TU_ENVIRONMENT");

        String platform = jsbind->GetPlatform();

        bool mobile = platform == "WEB" || platform == "ANDROID" || platform == "IOS";

        if (mobile)
        {
            values_.Push("TU_LIGHTRAMP");
            values_.Push("TU_LIGHTSHAPE");
            values_.Push("TU_SHADOWMAP");
            values_.Push("MAX_MATERIAL_TEXTURE_UNITS");
            values_.Push("MAX_TEXTURE_UNITS");
        }
        else
        {
            values_.Push("TU_VOLUMEMAP");
            values_.Push("TU_CUSTOM1");
            values_.Push("TU_CUSTOM2");
            values_.Push("TU_LIGHTRAMP");
            values_.Push("TU_LIGHTSHAPE");
            values_.Push("TU_SHADOWMAP");
            values_.Push("TU_FACESELECT");
            values_.Push("TU_INDIRECTION");
            values_.Push("TU_DEPTHBUFFER");
            values_.Push("TU_LIGHTBUFFER");
            values_.Push("TU_ZONE");
            values_.Push("MAX_MATERIAL_TEXTURE_UNITS");
            values_.Push("MAX_TEXTURE_UNITS");
        }

    }
}

}
