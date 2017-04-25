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

#pragma once

#include <Atomic/Resource/Image.h>
#include <Atomic/Graphics/Material.h>

using namespace Atomic;

namespace AtomicGlow
{

class BakeMaterial : public Object
{
    ATOMIC_OBJECT(BakeMaterial, Object)

    public:

    BakeMaterial(Context* context);
    virtual ~BakeMaterial();

    bool LoadMaterial(Material* material);

    Material* GetMaterial() { return material_; }

    Image* GetDiffuseTexture() const { return diffuseTexture_; }

    bool GetOcclusionMasked() const { return occlusionMasked_; }

    const Vector4& GetUOffset() const { return uoffset_; }
    const Vector4& GetVOffset() const { return voffset_; }

private:

    Variant ParseShaderParameterValue(const String& value);

    SharedPtr<Material> material_;

    // parameters which are loaded out of material xml
    // as we don't load material in headless graphics mode

    SharedPtr<Image> diffuseTexture_;

    bool occlusionMasked_;

    Vector4 uoffset_;
    Vector4 voffset_;

};

class BakeMaterialCache : public Object
{
    friend class BakeMaterial;

    ATOMIC_OBJECT(BakeMaterialCache, Object)

    public:

    BakeMaterialCache(Context* context);
    virtual ~BakeMaterialCache();

    BakeMaterial* GetBakeMaterial(Material* material);

private:

    /// Material->GetName -> BakeMaterial
    HashMap<StringHash, SharedPtr<BakeMaterial>> bakeCache_;

};



}
