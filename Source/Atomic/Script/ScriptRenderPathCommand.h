//
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

#include "../Core/Variant.h"
#include "../Graphics/RenderPath.h"

namespace Atomic
{

    /// Wrapper around RenderPathCommand as that is setup as a struct and usage a significant delta in order to change to RefCounted
    class ATOMIC_API ScriptRenderPathCommand : public RefCounted
    {
        ATOMIC_REFCOUNTED(ScriptRenderPathCommand)

    public:

        /// Construct.
        ScriptRenderPathCommand()
        {

        }

        virtual ~ScriptRenderPathCommand()
        {

        }

        /// Set a shader parameter.
        void SetShaderParameter(const String& name, const Variant& value) { renderPathCommand_.SetShaderParameter(name, value); }

        /// Gets tag associated with command
        const String& GetTag() const { return renderPathCommand_.tag_; }

        RenderPathCommand renderPathCommand_;

    };

}
