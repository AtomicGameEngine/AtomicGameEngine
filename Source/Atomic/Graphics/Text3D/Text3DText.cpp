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
#include "../../Core/CoreEvents.h"
#include "../Texture2D.h"
#include "../../IO/Log.h"
#include "../../Resource/ResourceCache.h"
#include "../../Resource/Localization.h"
#include "../../Resource/ResourceEvents.h"

#include "Text3DFont.h"
#include "Text3DFontFace.h"
#include "Text3DText.h"

#include "../../DebugNew.h"

namespace Atomic
{

const char* textEffects[] =
{
    "None",
    "Shadow",
    "Stroke",
    0
};

const char* horizontalAlignments[] =
{
    "Left",
    "Center",
    "Right",
    0
};

const char* verticalAlignments[] =
{
    "Top",
    "Center",
    "Bottom",
    0
};

static const float MIN_ROW_SPACING = 0.5f;

extern const char* horizontalAlignments[];
extern const char* GEOMETRY_CATEGORY;

Text3DText::Text3DText(Context* context) :
    Animatable(context),
    fontSize_(TEXT3D_DEFAULT_FONT_SIZE),
    textAlignment_(HA_LEFT),
    rowSpacing_(1.0f),
    wordWrap_(false),
    autoLocalizable_(false),
    charLocationsDirty_(true),
    selectionStart_(0),
    selectionLength_(0),
    selectionColor_(Color::TRANSPARENT),
    hoverColor_(Color::TRANSPARENT),
    textEffect_(TE_NONE),
    shadowOffset_(IntVector2(1, 1)),
    strokeThickness_(1),
    roundStroke_(false),
    effectColor_(Color::BLACK),
    effectDepthBias_(0.0f),
    rowHeight_(0),
    size_(IntVector2::ZERO),
    minSize_(IntVector2::ZERO),
    maxSize_(M_MAX_INT, M_MAX_INT),
    indent_(0),
    indentSpacing_(16),
    colorGradient_(false),
    opacity_(1.0f),
    hovering_(false),
    selected_(false)
{
}

Text3DText::~Text3DText()
{
}

void Text3DText::RegisterObject(Context* context)
{
    context->RegisterFactory<Text3DText>(GEOMETRY_CATEGORY);

    ATOMIC_COPY_BASE_ATTRIBUTES(Animatable);
    ATOMIC_UPDATE_ATTRIBUTE_DEFAULT_VALUE("Use Derived Opacity", false);
    ATOMIC_MIXED_ACCESSOR_ATTRIBUTE("Font", GetFontAttr, SetFontAttr, ResourceRef, ResourceRef(Text3DFont::GetTypeStatic()), AM_FILE);
    ATOMIC_ATTRIBUTE("Font Size", int, fontSize_, TEXT3D_DEFAULT_FONT_SIZE, AM_FILE);
    ATOMIC_MIXED_ACCESSOR_ATTRIBUTE("Text", GetTextAttr, SetTextAttr, String, String::EMPTY, AM_FILE);
    ATOMIC_ENUM_ATTRIBUTE("Text Alignment", textAlignment_, horizontalAlignments, HA_LEFT, AM_FILE);
    ATOMIC_ATTRIBUTE("Row Spacing", float, rowSpacing_, 1.0f, AM_FILE);
    ATOMIC_ATTRIBUTE("Word Wrap", bool, wordWrap_, false, AM_FILE);
    ATOMIC_ACCESSOR_ATTRIBUTE("Auto Localizable", GetAutoLocalizable, SetAutoLocalizable, bool, false, AM_FILE);
    ATOMIC_ACCESSOR_ATTRIBUTE("Selection Color", GetSelectionColor, SetSelectionColor, Color, Color::TRANSPARENT, AM_FILE);
    ATOMIC_ACCESSOR_ATTRIBUTE("Hover Color", GetHoverColor, SetHoverColor, Color, Color::TRANSPARENT, AM_FILE);
    ATOMIC_ENUM_ATTRIBUTE("Text Effect", textEffect_, textEffects, TE_NONE, AM_FILE);
    ATOMIC_ATTRIBUTE("Shadow Offset", IntVector2, shadowOffset_, IntVector2(1, 1), AM_FILE);
    ATOMIC_ATTRIBUTE("Stroke Thickness", int, strokeThickness_, 1, AM_FILE);
    ATOMIC_ATTRIBUTE("Round Stroke", bool, roundStroke_, false, AM_FILE);
    ATOMIC_ACCESSOR_ATTRIBUTE("Effect Color", GetEffectColor, SetEffectColor, Color, Color::BLACK, AM_FILE);
}

void Text3DText::ApplyAttributes()
{
    Animatable::ApplyAttributes();

    colorGradient_ = false;

    for (unsigned i = 1; i < MAX_TEXT_CORNERS; ++i)
    {
        if (color_[i] != color_[0])
            colorGradient_ = true;
    }

    // Localize now if attributes were loaded out-of-order
    if (autoLocalizable_ && stringId_.Length())
    {
        Localization* l10n = GetSubsystem<Localization>();
        text_ = l10n->Get(stringId_);
    }

    DecodeToUnicode();

    fontSize_ = Max(fontSize_, 1);
    strokeThickness_ = Abs(strokeThickness_);
    ValidateSelection();
    UpdateText();
}

void Text3DText::OnAttributeAnimationAdded()
{
    if (attributeAnimationInfos_.Size() == 1)
        SubscribeToEvent(E_POSTUPDATE, ATOMIC_HANDLER(Text3DText, HandlePostUpdate));
}

void Text3DText::OnAttributeAnimationRemoved()
{
    if (attributeAnimationInfos_.Empty())
        UnsubscribeFromEvent(E_POSTUPDATE);
}


void Text3DText::SetColor(const Color& color)
{
    for (unsigned i = 0; i < MAX_TEXT_CORNERS; ++i)
        color_[i] = color;

    colorGradient_ = false;
}

void Text3DText::SetColor(Text3DCorner corner, const Color& color)
{
    color_[corner] = color;
    colorGradient_ = false;

    for (unsigned i = 0; i < MAX_TEXT_CORNERS; ++i)
    {
        if (i != corner && color_[i] != color_[corner])
            colorGradient_ = true;
    }
}

void Text3DText::SetOpacity(float opacity)
{
    opacity_ = Clamp(opacity, 0.0f, 1.0f);
    MarkDirty();
}

void Text3DText::MarkDirty()
{

}

void Text3DText::GetBatches(PODVector<Text3DBatch>& batches, PODVector<float>& vertexData, const IntRect& currentScissor)
{    
    Text3DFontFace* face = font_ ? font_->GetFace(fontSize_) : (Text3DFontFace*)0;
    if (!face)
    {
        hovering_ = false;
        return;
    }

    // If face has changed or char locations are not valid anymore, update before rendering
    if (charLocationsDirty_ || !fontFace_ || face != fontFace_)
        UpdateCharLocations();
    // If face uses mutable glyphs mechanism, reacquire glyphs before rendering to make sure they are in the texture
    else if (face->HasMutableGlyphs())
    {
        for (unsigned i = 0; i < printText_.Size(); ++i)
            face->GetGlyph(printText_[i]);
    }

    // Hovering and/or whole selection batch
    if ((hovering_ && hoverColor_.a_ > 0.0) || (selected_ && selectionColor_.a_ > 0.0f))
    {
        bool both = hovering_ && selected_ && hoverColor_.a_ > 0.0 && selectionColor_.a_ > 0.0f;
        Text3DBatch batch(this, BLEND_ALPHA, currentScissor, 0, &vertexData);
        batch.SetColor(both ? selectionColor_.Lerp(hoverColor_, 0.5f) :
            (selected_ && selectionColor_.a_ > 0.0f ? selectionColor_ : hoverColor_));
        batch.AddQuad(0, 0, GetWidth(), GetHeight(), 0, 0);
        Text3DBatch::AddOrMerge(batch, batches);
    }

    // Partial selection batch
    if (!selected_ && selectionLength_ && charLocations_.Size() >= selectionStart_ + selectionLength_ && selectionColor_.a_ > 0.0f)
    {
        Text3DBatch batch(this, BLEND_ALPHA, currentScissor, 0, &vertexData);
        batch.SetColor(selectionColor_);

        IntVector2 currentStart = charLocations_[selectionStart_].position_;
        IntVector2 currentEnd = currentStart;
        for (unsigned i = selectionStart_; i < selectionStart_ + selectionLength_; ++i)
        {
            // Check if row changes, and start a new quad in that case
            if (charLocations_[i].size_ != IntVector2::ZERO)
            {
                if (charLocations_[i].position_.y_ != currentStart.y_)
                {
                    batch.AddQuad(currentStart.x_, currentStart.y_, currentEnd.x_ - currentStart.x_,
                        currentEnd.y_ - currentStart.y_, 0, 0);
                    currentStart = charLocations_[i].position_;
                    currentEnd = currentStart + charLocations_[i].size_;
                }
                else
                {
                    currentEnd.x_ += charLocations_[i].size_.x_;
                    currentEnd.y_ = Max(currentStart.y_ + charLocations_[i].size_.y_, currentEnd.y_);
                }
            }
        }
        if (currentEnd != currentStart)
        {
            batch.AddQuad(currentStart.x_, currentStart.y_, currentEnd.x_ - currentStart.x_, currentEnd.y_ - currentStart.y_, 0, 0);
        }

        Text3DBatch::AddOrMerge(batch, batches);
    }

    // Text batch
    Text3DTextEffect textEffect = font_->IsSDFFont() ? TE_NONE : textEffect_;
    const Vector<SharedPtr<Texture2D> >& textures = face->GetTextures();
    for (unsigned n = 0; n < textures.Size() && n < pageGlyphLocations_.Size(); ++n)
    {
        // One batch per texture/page
        Text3DBatch pageBatch(this, BLEND_ALPHA, currentScissor, textures[n], &vertexData);

        const PODVector<Text3DGlyphLocation>& pageGlyphLocation = pageGlyphLocations_[n];

        switch (textEffect)
        {
        case TE_NONE:
            ConstructBatch(pageBatch, pageGlyphLocation, 0, 0);
            break;

        case TE_SHADOW:
            ConstructBatch(pageBatch, pageGlyphLocation, shadowOffset_.x_, shadowOffset_.y_, &effectColor_, effectDepthBias_);
            ConstructBatch(pageBatch, pageGlyphLocation, 0, 0);
            break;

        case TE_STROKE:
            if (roundStroke_)
            {
                // Samples should be even or glyph may be redrawn in wrong x y pos making stroke corners rough
                // Adding to thickness helps with thickness of 1 not having enought samples for this formula
                // or certain fonts with reflex corners requiring more glyph samples for a smooth stroke when large
                int thickness = Min(strokeThickness_, fontSize_);
                int samples = thickness * thickness + (thickness % 2 == 0 ? 4 : 3);
                float angle = 360.f / samples;
                float floatThickness = (float)thickness;
                for (int i = 0; i < samples; ++i)
                {
                    float x = Cos(angle * i) * floatThickness;
                    float y = Sin(angle * i) * floatThickness;
                    ConstructBatch(pageBatch, pageGlyphLocation, (int)x, (int)y, &effectColor_, effectDepthBias_);
                }
            }
            else
            {
                int thickness = Min(strokeThickness_, fontSize_);
                int x, y;
                for (x = -thickness; x <= thickness; ++x)
                {
                    for (y = -thickness; y <= thickness; ++y)
                    {
                        // Don't draw glyphs that aren't on the edges
                        if (x > -thickness && x < thickness &&
                            y > -thickness && y < thickness)
                            continue;
    
                        ConstructBatch(pageBatch, pageGlyphLocation, x, y, &effectColor_, effectDepthBias_);
                    }
                }
            }
            ConstructBatch(pageBatch, pageGlyphLocation, 0, 0);
            break;
        }

        Text3DBatch::AddOrMerge(pageBatch, batches);
    }

    // Reset hovering for next frame
    hovering_ = false;
}

void Text3DText::OnResize(const IntVector2& newSize, const IntVector2& delta)
{
    if (wordWrap_)
        UpdateText(true);
    else
        charLocationsDirty_ = true;
}

void Text3DText::OnIndentSet()
{
    charLocationsDirty_ = true;
}

bool Text3DText::SetFont(const String& fontName, int size)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    return SetFont(cache->GetResource<Text3DFont>(fontName), size);
}

bool Text3DText::SetFont(Text3DFont* font, int size)
{
    if (!font)
    {
        ATOMIC_LOGERROR("Null font for Text");
        return false;
    }

    if (font != font_ || size != fontSize_)
    {
        font_ = font;
        fontSize_ = Max(size, 1);
        UpdateText();
    }

    return true;
}

bool Text3DText::SetFontSize(int size)
{
    // Initial font must be set
    if (!font_)
        return false;
    else
        return SetFont(font_, size);
}

void Text3DText::DecodeToUnicode()
{
    unicodeText_.Clear();
    for (unsigned i = 0; i < text_.Length();)
        unicodeText_.Push(text_.NextUTF8Char(i));
}

void Text3DText::SetText(const String& text)
{
    if (autoLocalizable_)
    {
        stringId_ = text;
        Localization* l10n = GetSubsystem<Localization>();
        text_ = l10n->Get(stringId_);
    }
    else
    {
        text_ = text;
    }

    DecodeToUnicode();
    ValidateSelection();
    UpdateText();
}

void Text3DText::SetTextAlignment(Text3DHorizontalAlignment align)
{
    if (align != textAlignment_)
    {
        textAlignment_ = align;
        charLocationsDirty_ = true;
    }
}

void Text3DText::SetRowSpacing(float spacing)
{
    if (spacing != rowSpacing_)
    {
        rowSpacing_ = Max(spacing, MIN_ROW_SPACING);
        UpdateText();
    }
}

void Text3DText::SetWordwrap(bool enable)
{
    if (enable != wordWrap_)
    {
        wordWrap_ = enable;
        UpdateText();
    }
}

void Text3DText::SetAutoLocalizable(bool enable)
{
    if (enable != autoLocalizable_)
    {
        autoLocalizable_ = enable;
        if (enable)
        {
            stringId_ = text_;
            Localization* l10n = GetSubsystem<Localization>();
            text_ = l10n->Get(stringId_);
            SubscribeToEvent(E_CHANGELANGUAGE, ATOMIC_HANDLER(Text3DText, HandleChangeLanguage));
        }
        else
        {
            text_ = stringId_;
            stringId_ = "";
            UnsubscribeFromEvent(E_CHANGELANGUAGE);
        }
        DecodeToUnicode();
        ValidateSelection();
        UpdateText();
    }
}

void Text3DText::HandleChangeLanguage(StringHash eventType, VariantMap& eventData)
{
    Localization* l10n = GetSubsystem<Localization>();
    text_ = l10n->Get(stringId_);
    DecodeToUnicode();
    ValidateSelection();
    UpdateText();
}

void Text3DText::SetSelection(unsigned start, unsigned length)
{
    selectionStart_ = start;
    selectionLength_ = length;
    ValidateSelection();
}

void Text3DText::ClearSelection()
{
    selectionStart_ = 0;
    selectionLength_ = 0;
}

void Text3DText::SetSelectionColor(const Color& color)
{
    selectionColor_ = color;
}

void Text3DText::SetHoverColor(const Color& color)
{
    hoverColor_ = color;
}

void Text3DText::SetTextEffect(Text3DTextEffect textEffect)
{
    textEffect_ = textEffect;
}

void Text3DText::SetEffectShadowOffset(const IntVector2& offset)
{
    shadowOffset_ = offset;
}

void Text3DText::SetEffectStrokeThickness(int thickness)
{
    strokeThickness_ = Abs(thickness);
}

void Text3DText::SetEffectRoundStroke(bool roundStroke)
{
    roundStroke_ = roundStroke;
}

void Text3DText::SetEffectColor(const Color& effectColor)
{
    effectColor_ = effectColor;
}

void Text3DText::SetEffectDepthBias(float bias)
{
    effectDepthBias_ = bias;
}

int Text3DText::GetRowWidth(unsigned index) const
{
    return index < rowWidths_.Size() ? rowWidths_[index] : 0;
}

IntVector2 Text3DText::GetCharPosition(unsigned index)
{
    if (charLocationsDirty_)
        UpdateCharLocations();
    if (charLocations_.Empty())
        return IntVector2::ZERO;
    // For convenience, return the position of the text ending if index exceeded
    if (index > charLocations_.Size() - 1)
        index = charLocations_.Size() - 1;
    return charLocations_[index].position_;
}

IntVector2 Text3DText::GetCharSize(unsigned index)
{
    if (charLocationsDirty_)
        UpdateCharLocations();
    if (charLocations_.Size() < 2)
        return IntVector2::ZERO;
    // For convenience, return the size of the last char if index exceeded (last size entry is zero)
    if (index > charLocations_.Size() - 2)
        index = charLocations_.Size() - 2;
    return charLocations_[index].size_;
}

void Text3DText::SetFontAttr(const ResourceRef& value)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    font_ = cache->GetResource<Text3DFont>(value.name_);
}

ResourceRef Text3DText::GetFontAttr() const
{
    return GetResourceRef(font_, Text3DFont::GetTypeStatic());
}

void Text3DText::SetTextAttr(const String& value)
{
    text_ = value;
    if (autoLocalizable_)
        stringId_ = value;
}

String Text3DText::GetTextAttr() const
{
    if (autoLocalizable_ && stringId_.Length())
        return stringId_;
    else
        return text_;
}

void Text3DText::UpdateText(bool onResize)
{
    rowWidths_.Clear();
    printText_.Clear();

    if (font_)
    {
        Text3DFontFace* face = font_->GetFace(fontSize_);
        if (!face)
            return;

        rowHeight_ = face->GetRowHeight();

        int width = 0;
        int height = 0;
        int rowWidth = 0;
        int rowHeight = (int)(rowSpacing_ * rowHeight_);

        // First see if the text must be split up
        if (!wordWrap_)
        {
            printText_ = unicodeText_;
            printToText_.Resize(printText_.Size());
            for (unsigned i = 0; i < printText_.Size(); ++i)
                printToText_[i] = i;
        }
        else
        {
            int maxWidth = GetWidth();
            unsigned nextBreak = 0;
            unsigned lineStart = 0;
            printToText_.Clear();

            for (unsigned i = 0; i < unicodeText_.Size(); ++i)
            {
                unsigned j;
                unsigned c = unicodeText_[i];

                if (c != '\n')
                {
                    bool ok = true;

                    if (nextBreak <= i)
                    {
                        int futureRowWidth = rowWidth;
                        for (j = i; j < unicodeText_.Size(); ++j)
                        {
                            unsigned d = unicodeText_[j];
                            if (d == ' ' || d == '\n')
                            {
                                nextBreak = j;
                                break;
                            }
                            const Text3DFontGlyph* glyph = face->GetGlyph(d);
                            if (glyph)
                            {
                                futureRowWidth += glyph->advanceX_;
                                if (j < unicodeText_.Size() - 1)
                                    futureRowWidth += face->GetKerning(d, unicodeText_[j + 1]);
                            }
                            if (d == '-' && futureRowWidth <= maxWidth)
                            {
                                nextBreak = j + 1;
                                break;
                            }
                            if (futureRowWidth > maxWidth)
                            {
                                ok = false;
                                break;
                            }
                        }
                    }

                    if (!ok)
                    {
                        // If did not find any breaks on the line, copy until j, or at least 1 char, to prevent infinite loop
                        if (nextBreak == lineStart)
                        {
                            while (i < j)
                            {
                                printText_.Push(unicodeText_[i]);
                                printToText_.Push(i);
                                ++i;
                            }
                        }
                        // Eliminate spaces that have been copied before the forced break
                        while (printText_.Size() && printText_.Back() == ' ')
                        {
                            printText_.Pop();
                            printToText_.Pop();
                        }
                        printText_.Push('\n');
                        printToText_.Push(Min(i, unicodeText_.Size() - 1));
                        rowWidth = 0;
                        nextBreak = lineStart = i;
                    }

                    if (i < unicodeText_.Size())
                    {
                        // When copying a space, position is allowed to be over row width
                        c = unicodeText_[i];
                        const Text3DFontGlyph* glyph = face->GetGlyph(c);
                        if (glyph)
                        {
                            rowWidth += glyph->advanceX_;
                            if (i < unicodeText_.Size() - 1)
                                rowWidth += face->GetKerning(c, unicodeText_[i + 1]);
                        }
                        if (rowWidth <= maxWidth)
                        {
                            printText_.Push(c);
                            printToText_.Push(i);
                        }
                    }
                }
                else
                {
                    printText_.Push('\n');
                    printToText_.Push(Min(i, unicodeText_.Size() - 1));
                    rowWidth = 0;
                    nextBreak = lineStart = i;
                }
            }
        }

        rowWidth = 0;

        for (unsigned i = 0; i < printText_.Size(); ++i)
        {
            unsigned c = printText_[i];

            if (c != '\n')
            {
                const Text3DFontGlyph* glyph = face->GetGlyph(c);
                if (glyph)
                {
                    rowWidth += glyph->advanceX_;
                    if (i < printText_.Size() - 1)
                        rowWidth += face->GetKerning(c, printText_[i + 1]);
                }
            }
            else
            {
                width = Max(width, rowWidth);
                height += rowHeight;
                rowWidths_.Push(rowWidth);
                rowWidth = 0;
            }
        }

        if (rowWidth)
        {
            width = Max(width, rowWidth);
            height += rowHeight;
            rowWidths_.Push(rowWidth);
        }

        // Set at least one row height even if text is empty
        if (!height)
            height = rowHeight;

        // Set minimum and current size according to the text size, but respect fixed width if set
        if (!IsFixedWidth())
        {
            SetMinWidth(wordWrap_ ? 0 : width);
            SetWidth(width);
        }

        SetFixedHeight(height);

        charLocationsDirty_ = true;
    }
    else
    {
        // No font, nothing to render
        pageGlyphLocations_.Clear();
    }
}

void Text3DText::UpdateCharLocations()
{
    // Remember the font face to see if it's still valid when it's time to render
    Text3DFontFace* face = font_ ? font_->GetFace(fontSize_) : (Text3DFontFace*)0;
    if (!face)
        return;
    fontFace_ = face;

    int rowHeight = (int)(rowSpacing_ * rowHeight_);

    // Store position & size of each character, and locations per texture page
    unsigned numChars = unicodeText_.Size();
    charLocations_.Resize(numChars + 1);
    pageGlyphLocations_.Resize(face->GetTextures().Size());
    for (unsigned i = 0; i < pageGlyphLocations_.Size(); ++i)
        pageGlyphLocations_[i].Clear();

    IntVector2 offset = font_->GetTotalGlyphOffset(fontSize_);

    unsigned rowIndex = 0;
    unsigned lastFilled = 0;
    int x = GetRowStartPosition(rowIndex) + offset.x_;
    int y = offset.y_;

    for (unsigned i = 0; i < printText_.Size(); ++i)
    {
        Text3DCharLocation loc;
        loc.position_ = IntVector2(x, y);

        unsigned c = printText_[i];
        if (c != '\n')
        {
            const Text3DFontGlyph* glyph = face->GetGlyph(c);
            loc.size_ = IntVector2(glyph ? glyph->advanceX_ : 0, rowHeight_);
            if (glyph)
            {
                // Store glyph's location for rendering. Verify that glyph page is valid
                if (glyph->page_ < pageGlyphLocations_.Size())
                    pageGlyphLocations_[glyph->page_].Push(Text3DGlyphLocation(x, y, glyph));
                x += glyph->advanceX_;
                if (i < printText_.Size() - 1)
                    x += face->GetKerning(c, printText_[i + 1]);
            }
        }
        else
        {
            loc.size_ = IntVector2::ZERO;
            x = GetRowStartPosition(++rowIndex);
            y += rowHeight;
        }

        if (lastFilled > printToText_[i])
            lastFilled = printToText_[i];

        // Fill gaps in case characters were skipped from printing
        for (unsigned j = lastFilled; j <= printToText_[i]; ++j)
            charLocations_[j] = loc;
        lastFilled = printToText_[i] + 1;
    }
    // Store the ending position
    charLocations_[numChars].position_ = IntVector2(x, y);
    charLocations_[numChars].size_ = IntVector2::ZERO;

    charLocationsDirty_ = false;
}

void Text3DText::ValidateSelection()
{
    unsigned textLength = unicodeText_.Size();

    if (textLength)
    {
        if (selectionStart_ >= textLength)
            selectionStart_ = textLength - 1;
        if (selectionStart_ + selectionLength_ > textLength)
            selectionLength_ = textLength - selectionStart_;
    }
    else
    {
        selectionStart_ = 0;
        selectionLength_ = 0;
    }
}

int Text3DText::GetRowStartPosition(unsigned rowIndex) const
{
    int rowWidth = 0;

    if (rowIndex < rowWidths_.Size())
        rowWidth = rowWidths_[rowIndex];

    int ret = GetIndentWidth();

    switch (textAlignment_)
    {
    case HA_LEFT:
        break;
    case HA_CENTER:
        ret += (GetSize().x_ - rowWidth) / 2;
        break;
    case HA_RIGHT:
        ret += GetSize().x_ - rowWidth;
        break;
    }

    return ret;
}

void Text3DText::SetIndent(int indent)
{
    indent_ = indent;
    OnIndentSet();
}

void Text3DText::SetIndentSpacing(int indentSpacing)
{
    indentSpacing_ = Max(indentSpacing, 0);
    OnIndentSet();
}

void Text3DText::ConstructBatch(Text3DBatch& pageBatch, const PODVector<Text3DGlyphLocation>& pageGlyphLocation, int dx, int dy, Color* color,
    float depthBias)
{
    unsigned startDataSize = pageBatch.vertexData_->Size();

    if (!color)
        pageBatch.SetDefaultColor();
    else
        pageBatch.SetColor(*color);

    for (unsigned i = 0; i < pageGlyphLocation.Size(); ++i)
    {
        const Text3DGlyphLocation& glyphLocation = pageGlyphLocation[i];
        const Text3DFontGlyph& glyph = *glyphLocation.glyph_;
        pageBatch.AddQuad(dx + glyphLocation.x_ + glyph.offsetX_, dy + glyphLocation.y_ + glyph.offsetY_, glyph.width_,
            glyph.height_, glyph.x_, glyph.y_);
    }

    if (depthBias != 0.0f)
    {
        unsigned dataSize = pageBatch.vertexData_->Size();
        for (unsigned i = startDataSize; i < dataSize; i += TEXT3D_VERTEX_SIZE)
            pageBatch.vertexData_->At(i + 2) += depthBias;
    }
}

void Text3DText::HandlePostUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace PostUpdate;

    UpdateAttributeAnimations(eventData[P_TIMESTEP].GetFloat());
}

void Text3DText::SetSelected(bool enable)
{
    selected_ = enable;
}

void Text3DText::SetHovering(bool enable)
{
    hovering_ = enable;
}

void Text3DText::SetWidth(int width)
{
    SetSize(IntVector2(width, size_.y_));
}

void Text3DText::SetHeight(int height)
{
    SetSize(IntVector2(size_.x_, height));
}

void Text3DText::SetMinSize(const IntVector2& minSize)
{
    minSize_.x_ = Max(minSize.x_, 0);
    minSize_.y_ = Max(minSize.y_, 0);
    SetSize(size_);
}

void Text3DText::SetMinSize(int width, int height)
{
    SetMinSize(IntVector2(width, height));
}

void Text3DText::SetMinWidth(int width)
{
    SetMinSize(IntVector2(width, minSize_.y_));
}

void Text3DText::SetMinHeight(int height)
{
    SetMinSize(IntVector2(minSize_.x_, height));
}

void Text3DText::SetMaxSize(const IntVector2& maxSize)
{
    maxSize_.x_ = Max(maxSize.x_, 0);
    maxSize_.y_ = Max(maxSize.y_, 0);
    SetSize(size_);
}

void Text3DText::SetMaxSize(int width, int height)
{
    SetMaxSize(IntVector2(width, height));
}

void Text3DText::SetMaxWidth(int width)
{
    SetMaxSize(IntVector2(width, maxSize_.y_));
}

void Text3DText::SetMaxHeight(int height)
{
    SetMaxSize(IntVector2(maxSize_.x_, height));
}

void Text3DText::SetFixedSize(const IntVector2& size)
{
    minSize_ = maxSize_ = IntVector2(Max(size.x_, 0), Max(size.y_, 0));
    SetSize(size);
}

void Text3DText::SetFixedSize(int width, int height)
{
    SetFixedSize(IntVector2(width, height));
}

void Text3DText::SetFixedWidth(int width)
{
    minSize_.x_ = maxSize_.x_ = Max(width, 0);
    SetWidth(width);
}

void Text3DText::SetFixedHeight(int height)
{
    minSize_.y_ = maxSize_.y_ = Max(height, 0);
    SetHeight(height);
}

void Text3DText::SetSize(const IntVector2& size)
{
    IntVector2 oldSize = size_;
    size_ = size;

    IntVector2 delta = size_ - oldSize;
    MarkDirty();
    OnResize(size_, delta);
}


}
