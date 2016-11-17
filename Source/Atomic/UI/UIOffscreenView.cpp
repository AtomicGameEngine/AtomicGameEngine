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

#include <TurboBadger/tb_widgets.h>

#include "../Graphics/Graphics.h"
#include "../Input/Input.h"
#include "../Input/InputEvents.h"
#include "UI.h"
#include "UIOffscreenView.h"

using namespace tb;

namespace Atomic
{

UIOffscreenView::UIOffscreenView(Context* context) : UIWidget(context, false)
{
    widget_ = new TBWidget();
    widget_->SetDelegate(this);
    widget_->SetVisibilility(tb::WIDGET_VISIBILITY_VISIBLE);

    UI* ui = GetSubsystem<UI>();
    ui->WrapWidget(this, widget_);

    texture_ = new Texture2D(context_);

    clearRenderTargetEachFrame_ = true;

    // Set initial size for view
    SetSize(512, 512);

    // Set initial filtering mode to bilinear.
    GetTexture2D()->SetFilterMode(Atomic::FILTER_BILINEAR);

    // Add our self to the ui subsystem.
    ui->GetOffscreenViews()->Insert(this);
}

UIOffscreenView::~UIOffscreenView()
{
    UI* ui = GetSubsystem<UI>();

    // Remove our self from the ui subsystem, if it's still around.
    if (ui)
        ui->GetOffscreenViews()->Erase(this);
}

void UIOffscreenView::SetSize(int width, int height)
{
    Graphics* graphics = GetSubsystem<Graphics>();
    widget_->SetSize(width, height);
    GetTexture2D()->SetSize(width, height, graphics->GetRGBAFormat(), Atomic::TEXTURE_RENDERTARGET);
}

Material* UIOffscreenView::GetMaterial()
{
    if (material_.Null())
    {
        // If the convenience material doesn't exist, then create it.
        ResourceCache* cache = GetSubsystem<ResourceCache>();
        material_ = new Material(context_);
        material_->SetTechnique(0, cache->GetResource<Technique>("Techniques/Diff.xml"));
        material_->SetTexture(Atomic::TU_DIFFUSE, texture_);
    }
    return material_;
}

void UIOffscreenView::RegisterInput(Camera* camera, Octree* octree, Drawable* drawable, const IntRect& rect)
{
    inputCamera_ = camera;
    inputOctree_ = octree;
    inputDrawable_ = drawable;
    inputRect_ = rect;
}

void UIOffscreenView::UnregisterInput()
{
    inputCamera_.Reset();
    inputOctree_.Reset();
    inputDrawable_.Reset();
    inputRect_ = IntRect::ZERO;
}

bool UIOffscreenView::SavePNGTo(Serializer& serializer) const
{
    Image image(context_);
    if (!image.SetSize(GetWidth(), GetHeight(), 4))
        return false;
    if (!GetTexture2D()->GetData(0, image.GetData()))
        return false;
    if (!image.Save(serializer))
        return false;
    return true;
}

bool UIOffscreenView::SavePNG(const String& fileName) const
{
    File outFile(context_, fileName, FILE_WRITE);
    if (!outFile.IsOpen())
        return false;
    return SavePNGTo(outFile);
}

static inline MODIFIER_KEYS GetModifierKeys(int qualifiers, bool superKey)
{
    MODIFIER_KEYS code = TB_MODIFIER_NONE;
    if (qualifiers & QUAL_ALT)   code |= TB_ALT;
    if (qualifiers & QUAL_CTRL)  code |= TB_CTRL;
    if (qualifiers & QUAL_SHIFT) code |= TB_SHIFT;
    if (superKey)                code |= TB_SUPER;
    return code;
}

static inline SPECIAL_KEY GetSpecialKey(int keycode)
{
    SPECIAL_KEY specialKey;

    switch (keycode)
    {
    case KEY_RETURN:
    case KEY_RETURN2:
    case KEY_KP_ENTER:
        specialKey = TB_KEY_ENTER;
        break;
    case KEY_F1:
        specialKey = TB_KEY_F1;
        break;
    case KEY_F2:
        specialKey = TB_KEY_F2;
        break;
    case KEY_F3:
        specialKey = TB_KEY_F3;
        break;
    case KEY_F4:
        specialKey = TB_KEY_F4;
        break;
    case KEY_F5:
        specialKey = TB_KEY_F5;
        break;
    case KEY_F6:
        specialKey = TB_KEY_F6;
        break;
    case KEY_F7:
        specialKey = TB_KEY_F7;
        break;
    case KEY_F8:
        specialKey = TB_KEY_F8;
        break;
    case KEY_F9:
        specialKey = TB_KEY_F9;
        break;
    case KEY_F10:
        specialKey = TB_KEY_F10;
        break;
    case KEY_F11:
        specialKey = TB_KEY_F11;
        break;
    case KEY_F12:
        specialKey = TB_KEY_F12;
        break;
    case KEY_LEFT:
        specialKey = TB_KEY_LEFT;
        break;
    case KEY_UP:
        specialKey = TB_KEY_UP;
        break;
    case KEY_RIGHT:
        specialKey = TB_KEY_RIGHT;
        break;
    case KEY_DOWN:
        specialKey = TB_KEY_DOWN;
        break;
    case KEY_PAGEUP:
        specialKey = TB_KEY_PAGE_UP;
        break;
    case KEY_PAGEDOWN:
        specialKey = TB_KEY_PAGE_DOWN;
        break;
    case KEY_HOME:
        specialKey = TB_KEY_HOME;
        break;
    case KEY_END:
        specialKey = TB_KEY_END;
        break;
    case KEY_INSERT:
        specialKey = TB_KEY_INSERT;
        break;
    case KEY_TAB:
        specialKey = TB_KEY_TAB;
        break;
    case KEY_DELETE:
        specialKey = TB_KEY_DELETE;
        break;
    case KEY_BACKSPACE:
        specialKey = TB_KEY_BACKSPACE;
        break;
    case KEY_ESCAPE:
        specialKey = TB_KEY_ESC;
        break;
    default:
        specialKey = TB_KEY_UNDEFINED;
        break;
    }

    return specialKey;
}

void UIOffscreenView::InvokeRightPointerDown(int x, int y, int click_count, int qualifiers, bool superDown)
{
    widget_->InvokeRightPointerDown(x, y, click_count, GetModifierKeys(qualifiers, superDown));
}

void UIOffscreenView::InvokeRightPointerUp(int x, int y, int qualifiers, bool superDown)
{
    widget_->InvokeRightPointerUp(x, y, GetModifierKeys(qualifiers, superDown));
}

void UIOffscreenView::InvokePointerDown(int x, int y, int click_count, int qualifiers, bool touch, int touchId, bool superDown)
{
    widget_->InvokePointerDown(x, y, click_count, GetModifierKeys(qualifiers, superDown), touch, touchId);
}

void UIOffscreenView::InvokePointerUp(int x, int y, int qualifiers, bool touch, int touchId, bool superDown)
{
    widget_->InvokePointerUp(x, y, GetModifierKeys(qualifiers, superDown), touch, touchId);
}

void UIOffscreenView::InvokePointerMove(int x, int y, int qualifiers, bool touch, int touchId, bool superDown)
{
    widget_->InvokePointerMove(x, y, GetModifierKeys(qualifiers, superDown), touch, touchId);
}

void UIOffscreenView::InvokeWheel(int x, int y, int delta_x, int delta_y, int qualifiers, bool superDown)
{
    widget_->InvokeWheel(x, y, delta_x, delta_y, GetModifierKeys(qualifiers, superDown));
}

bool UIOffscreenView::InvokeKey(int key, int keycode, bool down, int qualifiers, bool superDown)
{
    return widget_->InvokeKey(key, GetSpecialKey(keycode), GetModifierKeys(qualifiers, superDown), down);
}

}
