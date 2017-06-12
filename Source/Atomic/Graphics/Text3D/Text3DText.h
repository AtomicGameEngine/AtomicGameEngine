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

#include "../../Scene/Animatable.h"
#include "Text3DBatch.h"

namespace Atomic
{

static const int TEXT3D_DEFAULT_FONT_SIZE = 12;

class Text3DFont;
class Text3DFontFace;
struct Text3DFontGlyph;

/// Text effect.
enum Text3DTextEffect
{
    TE_NONE = 0,
    TE_SHADOW,
    TE_STROKE
};

/// Cached character location and size within text. Used for queries related to text editing.
struct Text3DCharLocation
{
    /// Position.
    IntVector2 position_;
    /// Size.
    IntVector2 size_;
};

/// Glyph and its location within the text. Used when preparing text rendering.
struct Text3DGlyphLocation
{
    /// Construct.
    Text3DGlyphLocation(int x, int y, const Text3DFontGlyph* glyph) :
        x_(x),
        y_(y),
        glyph_(glyph)
    {
    }

    /// X coordinate.
    int x_;
    /// Y coordinate.
    int y_;
    /// Glyph.
    const Text3DFontGlyph* glyph_;
};


enum Text3DHorizontalAlignment
{
    HA_LEFT = 0,
    HA_CENTER,
    HA_RIGHT,
    HA_CUSTOM
};


enum Text3DVerticalAlignment
{
    VA_TOP = 0,
    VA_CENTER,
    VA_BOTTOM,
    VA_CUSTOM
};

enum Text3DCorner
{
    C_TOPLEFT = 0,
    C_TOPRIGHT,
    C_BOTTOMLEFT,
    C_BOTTOMRIGHT,
    MAX_TEXT_CORNERS
};

class ATOMIC_API Text3DText : public Animatable
{
    ATOMIC_OBJECT(Text3DText, Animatable)

    friend class Text3D;

public:
    /// Construct.
    Text3DText(Context* context);
    /// Destruct.
    virtual ~Text3DText();
    /// Register object factory.
    static void RegisterObject(Context* context);

    /// Apply attribute changes that can not be applied immediately.
    virtual void ApplyAttributes();
    /// Return UI rendering batches.
    virtual void GetBatches(PODVector<Text3DBatch>& batches, PODVector<float>& vertexData, const IntRect& currentScissor);
    /// React to resize.
    virtual void OnResize(const IntVector2& newSize, const IntVector2& delta);
    /// React to indent change.
    virtual void OnIndentSet();

    /// Set font by looking from resource cache by name and font size. Return true if successful.
    bool SetFont(const String& fontName, int size = TEXT3D_DEFAULT_FONT_SIZE);
    /// Set font and font size. Return true if successful.
    bool SetFont(Text3DFont* font, int size = TEXT3D_DEFAULT_FONT_SIZE);
    /// Set font size only while retaining the existing font. Return true if successful.
    bool SetFontSize(int size);
    /// Set text. Text is assumed to be either ASCII or UTF8-encoded.
    void SetText(const String& text);
    /// Set row alignment.
    void SetTextAlignment(Text3DHorizontalAlignment align);
    /// Set row spacing, 1.0 for original font spacing.
    void SetRowSpacing(float spacing);
    /// Set wordwrap. In wordwrap mode the text element will respect its current width. Otherwise it resizes itself freely.
    void SetWordwrap(bool enable);
    /// The text will be automatically translated. The text value used as string identifier.
    void SetAutoLocalizable(bool enable);
    /// Set selection. When length is not provided, select until the text ends.
    void SetSelection(unsigned start, unsigned length = M_MAX_UNSIGNED);
    /// Clear selection.
    void ClearSelection();
    /// Set selection background color. Color with 0 alpha (default) disables.
    void SetSelectionColor(const Color& color);
    /// Set hover background color. Color with 0 alpha (default) disables.
    void SetHoverColor(const Color& color);
    /// Set text effect.
    void SetTextEffect(Text3DTextEffect textEffect);
    /// Set shadow offset.
    void SetEffectShadowOffset(const IntVector2& offset);
    /// Set stroke thickness.
    void SetEffectStrokeThickness(int thickness);
    /// Set stroke rounding. Corners of the font will be rounded off in the stroke so the stroke won't have corners.
    void SetEffectRoundStroke(bool roundStroke);
    /// Set effect color.
    void SetEffectColor(const Color& effectColor);

    /// Return font.
    Text3DFont* GetFont() const { return font_; }

    /// Return font size.
    int GetFontSize() const { return fontSize_; }

    /// Return text.
    const String& GetText() const { return text_; }

    /// Return row alignment.
    Text3DHorizontalAlignment GetTextAlignment() const { return textAlignment_; }

    /// Return row spacing.
    float GetRowSpacing() const { return rowSpacing_; }

    /// Return wordwrap mode.
    bool GetWordwrap() const { return wordWrap_; }

    /// Return auto localizable mode.
    bool GetAutoLocalizable() const { return autoLocalizable_; }

    /// Return selection start.
    unsigned GetSelectionStart() const { return selectionStart_; }

    /// Return selection length.
    unsigned GetSelectionLength() const { return selectionLength_; }

    /// Return selection background color.
    const Color& GetSelectionColor() const { return selectionColor_; }

    /// Return hover background color.
    const Color& GetHoverColor() const { return hoverColor_; }

    /// Return text effect.
    Text3DTextEffect GetTextEffect() const { return textEffect_; }

    /// Return effect shadow offset.
    const IntVector2& GetEffectShadowOffset() const { return shadowOffset_; }

    /// Return effect stroke thickness.
    int GetEffectStrokeThickness() const { return strokeThickness_; }

    /// Return effect round stroke.
    bool GetEffectRoundStroke() const { return roundStroke_; }

    /// Return effect color.
    const Color& GetEffectColor() const { return effectColor_; }

    /// Return row height.
    int GetRowHeight() const { return rowHeight_; }

    /// Return number of rows.
    unsigned GetNumRows() const { return rowWidths_.Size(); }

    /// Return number of characters.
    unsigned GetNumChars() const { return unicodeText_.Size(); }

    /// Return width of row by index.
    int GetRowWidth(unsigned index) const;
    /// Return position of character by index relative to the text element origin.
    IntVector2 GetCharPosition(unsigned index);
    /// Return size of character by index.
    IntVector2 GetCharSize(unsigned index);

    /// Set text effect Z bias. Zero by default, adjusted only in 3D mode.
    void SetEffectDepthBias(float bias);

    /// Return effect Z bias.
    float GetEffectDepthBias() const { return effectDepthBias_; }

    /// Set size.
    void SetSize(const IntVector2& size);
    /// Set size.
    void SetSize(int width, int height);
    /// Set width only.
    void SetWidth(int width);
    /// Set height only.
    void SetHeight(int height);
    /// Set minimum size.
    void SetMinSize(const IntVector2& minSize);
    /// Set minimum size.
    void SetMinSize(int width, int height);
    /// Set minimum width.
    void SetMinWidth(int width);
    /// Set minimum height.
    void SetMinHeight(int height);
    /// Set maximum size.
    void SetMaxSize(const IntVector2& maxSize);
    /// Set maximum size.
    void SetMaxSize(int width, int height);
    /// Set maximum width.
    void SetMaxWidth(int width);
    /// Set maximum height.
    void SetMaxHeight(int height);
    /// Set fixed size.
    void SetFixedSize(const IntVector2& size);
    /// Set fixed size.
    void SetFixedSize(int width, int height);
    /// Set fixed width.
    void SetFixedWidth(int width);
    /// Set fixed height.
    void SetFixedHeight(int height);

    /// Return size.
    const IntVector2& GetSize() const { return size_; }

    /// Return width.
    int GetWidth() const { return size_.x_; }

    /// Return height.
    int GetHeight() const { return size_.y_; }

    /// Return minimum size.
    const IntVector2& GetMinSize() const { return minSize_; }

    /// Return minimum width.
    int GetMinWidth() const { return minSize_.x_; }

    /// Return minimum height.
    int GetMinHeight() const { return minSize_.y_; }

    /// Return maximum size.
    const IntVector2& GetMaxSize() const { return maxSize_; }

    /// Return minimum width.
    int GetMaxWidth() const { return maxSize_.x_; }

    /// Return minimum height.
    int GetMaxHeight() const { return maxSize_.y_; }

    /// Return true if size is fixed.
    bool IsFixedSize() const { return minSize_ == maxSize_; }

    /// Return true if width is fixed.
    bool IsFixedWidth() const { return minSize_.x_ == maxSize_.x_; }

    /// Return true if height is fixed.
    bool IsFixedHeight() const { return minSize_.y_ == maxSize_.y_; }

    /// Set horizontal indentation.
    void SetIndent(int indent);
    /// Set indent spacing (number of pixels per indentation level).
    void SetIndentSpacing(int indentSpacing);

    /// Return horizontal indentation.
    int GetIndent() const { return indent_; }

    /// Return indent spacing (number of pixels per indentation level).
    int GetIndentSpacing() const { return indentSpacing_; }

    /// Return indent width in pixels.
    int GetIndentWidth() const { return indent_ * indentSpacing_; }


    /// Return corner color.
    const Color& GetColor(Text3DCorner corner) const { return color_[corner]; }

    const Color& GetColor() const { return color_[C_TOPLEFT]; }

    /// Set color on all corners.
    void SetColor(const Color& color);

    /// Set color on one corner.
    void SetColor(Text3DCorner corner, const Color& color);

    /// Return whether has different color in at least one corner.
    bool HasColorGradient() const { return colorGradient_; }

    /// Set opacity.
    void SetOpacity(float opacity);

    /// Return opacity.
    float GetOpacity() const { return opacity_; }


    /// Return whether is selected.
    bool IsSelected() const { return selected_; }

    /// Set selected mode.
    void SetSelected(bool enable);

    /// Return whether the cursor is hovering on this element.
    bool IsHovering() const { return hovering_; }

    /// Set hovering state.
    void SetHovering(bool enable);

    /// Set font attribute.
    void SetFontAttr(const ResourceRef& value);
    /// Return font attribute.
    ResourceRef GetFontAttr() const;
    /// Set text attribute.
    void SetTextAttr(const String& value);
    /// Return text attribute.
    String GetTextAttr() const;

protected:

    /// Handle attribute animation added.
    virtual void OnAttributeAnimationAdded();
    /// Handle attribute animation removed.
    virtual void OnAttributeAnimationRemoved();

    virtual void MarkDirty();

    /// Update text when text, font or spacing changed.
    void UpdateText(bool onResize = false);
    /// Update cached character locations after text update, or when text alignment or indent has changed.
    void UpdateCharLocations();
    /// Validate text selection to be within the text.
    void ValidateSelection();
    /// Return row start X position.
    int GetRowStartPosition(unsigned rowIndex) const;
    /// Contruct batch.
    void ConstructBatch
        (Text3DBatch& pageBatch, const PODVector<Text3DGlyphLocation>& pageGlyphLocation, int dx = 0, int dy = 0, Color* color = 0,
            float depthBias = 0.0f);

    /// Font.
    SharedPtr<Text3DFont> font_;
    /// Current face.
    WeakPtr<Text3DFontFace> fontFace_;
    /// Font size.
    int fontSize_;
    /// UTF-8 encoded text.
    String text_;
    /// Row alignment.
    Text3DHorizontalAlignment textAlignment_;
    /// Row spacing.
    float rowSpacing_;
    /// Wordwrap mode.
    bool wordWrap_;
    /// Char positions dirty flag.
    bool charLocationsDirty_;
    /// Selection start.
    unsigned selectionStart_;
    /// Selection length.
    unsigned selectionLength_;
    /// Selection background color.
    Color selectionColor_;
    /// Hover background color.
    Color hoverColor_;
    /// Text effect.
    Text3DTextEffect textEffect_;
    /// Text effect shadow offset.
    IntVector2 shadowOffset_;
    /// Text effect stroke thickness.
    int strokeThickness_;
    /// Text effect stroke rounding flag.
    bool roundStroke_;
    /// Effect color.
    Color effectColor_;
    /// Text effect Z bias.
    float effectDepthBias_;
    /// Row height.
    int rowHeight_;
    /// Text as Unicode characters.
    PODVector<unsigned> unicodeText_;
    /// Text modified into printed form.
    PODVector<unsigned> printText_;
    /// Mapping of printed form back to original char indices.
    PODVector<unsigned> printToText_;
    /// Row widths.
    PODVector<int> rowWidths_;
    /// Glyph locations per each texture in the font.
    Vector<PODVector<Text3DGlyphLocation> > pageGlyphLocations_;
    /// Cached locations of each character in the text.
    PODVector<Text3DCharLocation> charLocations_;
    /// The text will be automatically translated.
    bool autoLocalizable_;
    /// Localization string id storage. Used when autoLocalizable flag is set.
    String stringId_;
    /// Handle change Language.
    void HandleChangeLanguage(StringHash eventType, VariantMap& eventData);
    /// UTF8 to Unicode.
    void DecodeToUnicode();

    /// Size.
    IntVector2 size_;
    /// Minimum size.
    IntVector2 minSize_;
    /// Maximum size.
    IntVector2 maxSize_;

    /// Horizontal indentation.
    int indent_;
    /// Indent spacing (number of pixels per indentation level).
    int indentSpacing_;

    /// Colors.
    Color color_[MAX_TEXT_CORNERS];
    /// Has color gradient flag.
    bool colorGradient_;

    /// Opacity.
    float opacity_;

    /// Selected flag.
    bool selected_;

    /// Hovering flag.
    bool hovering_;

private:

    /// Handle logic post-update event.
    void HandlePostUpdate(StringHash eventType, VariantMap& eventData);

};

}
