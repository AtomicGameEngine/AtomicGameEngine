// Copyright (c) 2014-2017, THUNDERBEAST GAMES LLC All rights reserved
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
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/XMLFile.h>
#include <Atomic/Graphics/Material.h>

#include "BakeMaterial.h"

namespace AtomicGlow
{


BakeMaterialCache::BakeMaterialCache(Context* context) : Object(context)
{
}

BakeMaterialCache::~BakeMaterialCache()
{

}

BakeMaterial* BakeMaterialCache::GetBakeMaterial(Material *material)
{
    if (!material)
        return 0;

    SharedPtr<BakeMaterial> bakeMaterial;

    if (bakeCache_.TryGetValue(material->GetName(), bakeMaterial))
    {
        return bakeMaterial;
    }

    bakeMaterial = new BakeMaterial(context_);

    if (!bakeMaterial->LoadMaterial(material))
    {
        return 0;
    }

    bakeCache_[material->GetName()] = bakeMaterial;

    return bakeMaterial;

}

BakeMaterial::BakeMaterial(Context* context) : Object(context),
    occlusionMasked_(false)
{
    uoffset_ = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
    voffset_ = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
}

BakeMaterial::~BakeMaterial()
{

}

Variant BakeMaterial::ParseShaderParameterValue(const String& value)
{
    String valueTrimmed = value.Trimmed();
    if (valueTrimmed.Length() && IsAlpha((unsigned)valueTrimmed[0]))
        return Variant(ToBool(valueTrimmed));
    else
        return ToVectorVariant(valueTrimmed);
}

bool BakeMaterial::LoadMaterial(Material *material)
{
    material_ = material;

    ATOMIC_LOGINFOF("Material: %s", material->GetName().CString());

    ResourceCache* cache = GetSubsystem<ResourceCache>();

    XMLFile* xmlFile = cache->GetResource<XMLFile>(material->GetName());

    if (!xmlFile)
        return false;

    XMLElement rootElem = xmlFile->GetRoot();

    XMLElement techniqueElem = rootElem.GetChild("technique");

    if (techniqueElem)
    {
        String name = techniqueElem.GetAttribute("name").ToLower();

        // TODO: better way of setting/detecting occlusion masked materials
        if (name.Contains("diffalpha") || name.Contains("difflightmapalpha"))
        {
            occlusionMasked_ = true;
        }
    }

    // techniques

    // textures
    XMLElement textureElem = rootElem.GetChild("texture");
    while (textureElem)
    {
        String unit = textureElem.GetAttribute("unit").ToLower();

        if (unit == "diffuse")
        {
            String name = textureElem.GetAttribute("name");

            diffuseTexture_ = cache->GetResource<Image>(name);

            if (diffuseTexture_.Null())
                return false;

            ATOMIC_LOGINFOF("diffuse: %s %ux%u", name.CString(), diffuseTexture_->GetWidth(), diffuseTexture_->GetHeight());
        }

        textureElem = textureElem.GetNext("texture");
    }

    // Shader parameters

    XMLElement parameterElem = rootElem.GetChild("parameter");
    while (parameterElem)
    {
        String name = parameterElem.GetAttribute("name").ToLower();

        Variant value;

        if (!parameterElem.HasAttribute("type"))
            value = ParseShaderParameterValue(parameterElem.GetAttribute("value"));
        else
            value = Variant(parameterElem.GetAttribute("type"), parameterElem.GetAttribute("value"));

        if (name == "uoffset")
            uoffset_ = value.GetVector4();

        else if (name == "voffset")
            voffset_ = value.GetVector4();

        parameterElem = parameterElem.GetNext("parameter");
    }


    return true;
}


}
