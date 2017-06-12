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

#include "../../Precompiled.h"

#include "../../Core/Context.h"
#include "../../Core/Profiler.h"
#include "../Graphics.h"
#include "../../IO/Deserializer.h"
#include "../../IO/FileSystem.h"
#include "Text3DFont.h"
#include "Text3DBitmap.h"
#include "Text3DFreeType.h"
#include "../../Resource/ResourceCache.h"
#include "../../Resource/XMLElement.h"
#include "../../Resource/XMLFile.h"
#include "../../DebugNew.h"

namespace Atomic
{

static const int MIN_POINT_SIZE = 1;
static const int MAX_POINT_SIZE = 96;

Text3DFont::Text3DFont(Context* context) :
    Resource(context),
    fontDataSize_(0),
    absoluteOffset_(IntVector2::ZERO),
    scaledOffset_(Vector2::ZERO),
    fontType_(FONT_NONE),
    sdfFont_(false)
{
}

Text3DFont::~Text3DFont()
{
    // To ensure FreeType deallocates properly, first clear all faces, then release the raw font data
    ReleaseFaces();
    fontData_.Reset();
}

void Text3DFont::RegisterObject(Context* context)
{
    context->RegisterFactory<Text3DFont>();
}

bool Text3DFont::BeginLoad(Deserializer& source)
{
    // In headless mode, do not actually load, just return success
    Graphics* graphics = GetSubsystem<Graphics>();
    if (!graphics)
        return true;

    fontType_ = FONT_NONE;
    faces_.Clear();

    fontDataSize_ = source.GetSize();
    if (fontDataSize_)
    {
        fontData_ = new unsigned char[fontDataSize_];
        if (source.Read(&fontData_[0], fontDataSize_) != fontDataSize_)
            return false;
    }
    else
    {
        fontData_.Reset();
        return false;
    }

    String ext = GetExtension(GetName());
    if (ext == ".ttf" || ext == ".otf" || ext == ".woff")
    {
        fontType_ = FONT_FREETYPE;
        LoadParameters();
    }
    else if (ext == ".xml" || ext == ".fnt" || ext == ".sdf")
        fontType_ = FONT_BITMAP;

    sdfFont_ = ext == ".sdf";

    SetMemoryUse(fontDataSize_);
    return true;
}

bool Text3DFont::SaveXML(Serializer& dest, int pointSize, bool usedGlyphs, const String& indentation)
{
    Text3DFontFace* fontFace = GetFace(pointSize);
    if (!fontFace)
        return false;

    ATOMIC_PROFILE(FontSaveXML);

    SharedPtr<Text3DBitmap> packedFontFace(new Text3DBitmap(this));
    if (!packedFontFace->Load(fontFace, usedGlyphs))
        return false;

    return packedFontFace->Save(dest, pointSize, indentation);
}

void Text3DFont::SetAbsoluteGlyphOffset(const IntVector2& offset)
{
    absoluteOffset_ = offset;
}

void Text3DFont::SetScaledGlyphOffset(const Vector2& offset)
{
    scaledOffset_ = offset;
}

Text3DFontFace* Text3DFont::GetFace(int pointSize)
{
    // In headless mode, always return null
    Graphics* graphics = GetSubsystem<Graphics>();
    if (!graphics)
        return 0;

    // For bitmap font type, always return the same font face provided by the font's bitmap file regardless of the actual requested point size
    if (fontType_ == FONT_BITMAP)
        pointSize = 0;
    else
        pointSize = Clamp(pointSize, MIN_POINT_SIZE, MAX_POINT_SIZE);

    HashMap<int, SharedPtr<Text3DFontFace> >::Iterator i = faces_.Find(pointSize);
    if (i != faces_.End())
    {
        if (!i->second_->IsDataLost())
            return i->second_;
        else
        {
            // Erase and reload face if texture data lost (OpenGL mode only)
            faces_.Erase(i);
        }
    }

    ATOMIC_PROFILE(GetFontFace);

    switch (fontType_)
    {
    case FONT_FREETYPE:
        return GetFaceFreeType(pointSize);

    case FONT_BITMAP:
        return GetFaceBitmap(pointSize);

    default:
        return 0;
    }
}

IntVector2 Text3DFont::GetTotalGlyphOffset(int pointSize) const
{
    Vector2 multipliedOffset = (float)pointSize * scaledOffset_;
    return absoluteOffset_ + IntVector2((int)multipliedOffset.x_, (int)multipliedOffset.y_);
}

void Text3DFont::ReleaseFaces()
{
    faces_.Clear();
}

void Text3DFont::LoadParameters()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    String xmlName = ReplaceExtension(GetName(), ".xml");
    SharedPtr<XMLFile> xml = cache->GetTempResource<XMLFile>(xmlName, false);
    if (!xml)
        return;

    XMLElement rootElem = xml->GetRoot();

    XMLElement absoluteElem = rootElem.GetChild("absoluteoffset");
    if (!absoluteElem)
        absoluteElem = rootElem.GetChild("absolute");

    if (absoluteElem)
    {
        absoluteOffset_.x_ = absoluteElem.GetInt("x");
        absoluteOffset_.y_ = absoluteElem.GetInt("y");
    }

    XMLElement scaledElem = rootElem.GetChild("scaledoffset");
    if (!scaledElem)
        scaledElem = rootElem.GetChild("scaled");

    if (scaledElem)
    {
        scaledOffset_.x_ = scaledElem.GetFloat("x");
        scaledOffset_.y_ = scaledElem.GetFloat("y");
    }
}

Text3DFontFace* Text3DFont::GetFaceFreeType(int pointSize)
{
    SharedPtr<Text3DFontFace> newFace(new Text3DFreeType(this));
    if (!newFace->Load(&fontData_[0], fontDataSize_, pointSize))
        return 0;

    faces_[pointSize] = newFace;
    return newFace;
}

Text3DFontFace* Text3DFont::GetFaceBitmap(int pointSize)
{
    SharedPtr<Text3DFontFace> newFace(new Text3DBitmap(this));
    if (!newFace->Load(&fontData_[0], fontDataSize_, pointSize))
        return 0;

    faces_[pointSize] = newFace;
    return newFace;
}

}
