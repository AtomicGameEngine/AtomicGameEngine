//
// Copyright (c) 2014-2016, THUNDERBEAST GAMES LLC All rights reserved
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

#include <Atomic/Core/Variant.h>
#include <Atomic/Resource/ResourceCache.h>

#include "ScriptSystem.h"

namespace Atomic
{
    /// Wraps a Variant as a RefCounted so we can easily send it to script code
    /// For performance sensitive code, specialized marshaling should be used instead
    class ScriptVariant : public RefCounted
    {
        ATOMIC_REFCOUNTED(ScriptVariant)

    public:

        ScriptVariant() : RefCounted()
        {

        }

        virtual ~ScriptVariant()
        {

        }

        const Variant& GetVariant() { return variant_; }

        void SetVariant(const Variant& value) { variant_ = value; }

        Vector2 GetVector2() { return variant_.GetVector2(); }

        void SetVector2(const Vector2& value) { variant_ = value; }

        Vector3 GetVector3() { return variant_.GetVector3(); }

        void SetVector3(const Vector3& value) { variant_ = value; }

        Color GetColor() { return variant_.GetColor(); }

        void SetColor(const Color& value) { variant_ = value; }

    private:

        Variant variant_;

    };

}