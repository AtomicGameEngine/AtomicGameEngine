//
// Copyright (c) 2008-2017 the Urho3D project.
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

#include "Text3DFontFace.h"

namespace Atomic
{

class FreeTypeLibrary;
class Texture2D;

/// Free type font face description.
class ATOMIC_API Text3DFreeType : public Text3DFontFace
{
    ATOMIC_REFCOUNTED(Text3DFreeType)

public:
    /// Construct.
    Text3DFreeType(Text3DFont* font);
    /// Destruct.
    ~Text3DFreeType();

    /// Load font face.
    virtual bool Load(const unsigned char* fontData, unsigned fontDataSize, int pointSize);
    /// Return pointer to the glyph structure corresponding to a character. Return null if glyph not found.
    virtual const Text3DFontGlyph* GetGlyph(unsigned c);

    /// Return if font face uses mutable glyphs.
    virtual bool HasMutableGlyphs() const { return hasMutableGlyph_; }

    /// Set whether to force font autohinting instead of using FreeType's TTF bytecode interpreter.
    void SetForceAutoHint(bool enable) { forceAutoHint_ = enable; }

private:
    /// Setup next texture.
    bool SetupNextTexture(int textureWidth, int textureHeight);
    /// Load char glyph.
    bool LoadCharGlyph(unsigned charCode, Image* image = 0);

    /// FreeType library.
    SharedPtr<FreeTypeLibrary> freeType_;
    /// FreeType face. Non-null after creation only in dynamic mode.
    void* face_;
    /// Load mode.
    int loadMode_;
    /// Ascender.
    int ascender_;
    /// Has mutable glyph.
    bool hasMutableGlyph_;
    /// Glyph area allocator.
    AreaAllocator allocator_;

    bool forceAutoHint_;
};

}
