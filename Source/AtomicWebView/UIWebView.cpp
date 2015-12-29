//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
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
#include <Atomic/Input/InputEvents.h>
#include <Atomic/Input/Input.h>
#include <Atomic/UI/UIRenderer.h>

#include "WebClient.h"
#include "WebTexture2D.h"
#include "WebBrowserHost.h"

#include "UIWebView.h"

using namespace tb;

namespace Atomic
{

class WebViewWidget : public tb::TBWidget
{
    friend class UIWebView;

public:
    // For safe typecasting
    TBOBJECT_SUBCLASS(WebViewWidget, tb::TBWidget);

    WebViewWidget() : browserCreated_(false)
    {
        vertexData_.Resize(6 * UI_VERTEX_SIZE);
        float color;
        ((unsigned&)color) = 0xFFFFFFFF;

        float* data = &vertexData_[0];

        data[2] = 0; data[3] = color; data[4] = 0; data[5] = 0;
        data[8] = 0; data[9] = color; data[10] = 1; data[11] = 0;
        data[14] = 0; data[15] = color; data[16] = 1; data[17] = 1;
        data[20] = 0; data[21] = color; data[22] = 0; data[23] = 0;
        data[26] = 0; data[27] = color; data[28] = 1; data[29] = 1;
        data[32] = 0; data[33] = color; data[34] = 0; data[35] = 1;
    }

    void OnPaint(const PaintProps &paint_props)
    {
        if (webView_.Null())
            return;

        TBRect rect = GetRect();
        rect.x = rect.y = 0;
        ConvertToRoot(rect.x, rect.y);

        IntRect size = webView_->GetRect();

        float* data = &vertexData_[0];

        UI* ui = webView_->GetSubsystem<UI>();

        if (!browserCreated_)
        {
            browserCreated_ = true;
            webView_->webClient_->CreateBrowser(webView_->initialURL_, rect.w, rect.h);
            // start focused
            webView_->webClient_->SendFocusEvent(true);
        }

        webView_->webClient_->SetSize(rect.w, rect.h);

        float color;
        float fopacity = GetOpacity() * ui->GetRenderer()->GetOpacity();
        unsigned char opacity = (unsigned char) (fopacity* 255.0f);
        ((unsigned&)color) = (0x00FFFFFF + (((uint32)opacity) << 24));

        data[3] = color;
        data[9] = color;
        data[15] = color;
        data[21] = color;
        data[27] = color;
        data[33] = color;

        data[0] = rect.x;
        data[1] = rect.y;

        data[6] = rect.x + rect.w;
        data[7] =  rect.y;

        data[12] = rect.x + rect.w;
        data[13] = rect.y + rect.h;

        data[18] = rect.x;
        data[19] = rect.y;

        data[24] = rect.x + rect.w;
        data[25] = rect.y + rect.h;

        data[30] = rect.x;
        data[31] = rect.y + rect.h;

        ui->SubmitBatchVertexData(webView_->GetWebTexture2D()->GetTexture2D(), vertexData_);

    }

private:

    bool browserCreated_;

    WeakPtr<UIWebView> webView_;
    PODVector<float> vertexData_;

};

UIWebView::UIWebView(Context* context, const String &initialURL) : UIWidget(context, false)
{
    widget_ = new WebViewWidget();
    widget_->SetDelegate(this);
    widget_->SetGravity(WIDGET_GRAVITY_ALL);
    widget_->SetIsFocusable(true);
    ((WebViewWidget*)widget_)->webView_ = this;

    UI* ui = GetSubsystem<UI>();
    ui->WrapWidget(this, widget_);

    webClient_ = new WebClient(context);
    webTexture_ = new WebTexture2D(context);
    webClient_->SetWebRenderHandler(webTexture_);

    initialURL_ = initialURL;

    SubscribeToEvent(E_KEYDOWN, HANDLER(UIWebView, HandleKeyDown));
    SubscribeToEvent(E_KEYUP, HANDLER(UIWebView, HandleKeyUp));
    SubscribeToEvent(E_TEXTINPUT, HANDLER(UIWebView, HandleTextInput));
}

UIWebView::~UIWebView()
{

}

void UIWebView::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
    if (!widget_ || !widget_->GetIsFocused())
        return;

    webClient_->SendKeyEvent(eventType, eventData);

}

void UIWebView::HandleKeyUp(StringHash eventType, VariantMap& eventData)
{
    if (!widget_ || !widget_->GetIsFocused())
        return;

    webClient_->SendKeyEvent(eventType, eventData);

}

void UIWebView::HandleTextInput(StringHash eventType, VariantMap& eventData)
{
    if (!widget_ || !widget_->GetIsFocused())
        return;

    webClient_->SendTextInputEvent(eventType, eventData);

}

bool UIWebView::HandleKeyEvent(const TBWidgetEvent &ev, bool keyDown)
{

#ifdef ATOMIC_PLATFORM_OSX
    if (!keyDown)
        return true;
#endif

    if (ev.special_key == TB_KEY_UNDEFINED)
        return true;

    int qual = 0;

    if (ev.modifierkeys & TB_CTRL)
        qual |= QUAL_CTRL;
    if (ev.modifierkeys & TB_SHIFT)
        qual |= QUAL_SHIFT;
    if (ev.modifierkeys & TB_ALT)
        qual |= QUAL_ALT;

    int scanCode = SDL_SCANCODE_UNKNOWN;

    switch (ev.special_key)
    {
    case TB_KEY_UP:
        scanCode = SDL_SCANCODE_UP;
        break;
    case TB_KEY_DOWN:
        scanCode = SDL_SCANCODE_DOWN;
        break;
    case TB_KEY_RIGHT:
        scanCode = SDL_SCANCODE_RIGHT;
        break;
    case TB_KEY_LEFT:
        scanCode = SDL_SCANCODE_LEFT;
        break;
    case TB_KEY_ENTER:
        scanCode = SDL_SCANCODE_RETURN;
        break;
    case TB_KEY_DELETE:
    case TB_KEY_BACKSPACE:
        scanCode = SDL_SCANCODE_BACKSPACE;
        break;
    default:
        break;
    }

    if (scanCode == SDL_SCANCODE_UNKNOWN)
        return true;

    //webClient_->SendKeyEvent(scanCode, qual, !keyDown);

    return true;

    /*
    TB_KEY_UNDEFINED = 0,
    TB_KEY_UP, TB_KEY_DOWN, TB_KEY_LEFT, TB_KEY_RIGHT,
    TB_KEY_PAGE_UP, TB_KEY_PAGE_DOWN, TB_KEY_HOME, TB_KEY_END,
    TB_KEY_TAB, TB_KEY_BACKSPACE, TB_KEY_INSERT, TB_KEY_DELETE,
    TB_KEY_ENTER, TB_KEY_ESC,
    TB_KEY_F1, TB_KEY_F2, TB_KEY_F3, TB_KEY_F4, TB_KEY_F5, TB_KEY_F6,
    TB_KEY_F7, TB_KEY_F8, TB_KEY_F9, TB_KEY_F10, TB_KEY_F11, TB_KEY_F12
    */

}

bool UIWebView::OnEvent(const TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_POINTER_DOWN || ev.type == EVENT_TYPE_POINTER_UP)
    {
        webClient_->SendMouseClickEvent(ev.target_x, ev.target_y, 0, ev.type == EVENT_TYPE_POINTER_UP, 0);
        return true;
    }
    else if (ev.type == EVENT_TYPE_POINTER_MOVE)
    {
        webClient_->SendMouseMoveEvent(ev.target_x, ev.target_y, 0);
        return true;
    }
    else if (ev.type == EVENT_TYPE_WHEEL)
    {
        webClient_->SendMouseWheelEvent(ev.target_x, ev.target_y, 0, ev.delta_x, ev.delta_y);
        return true;
    }
    else if (ev.type == EVENT_TYPE_KEY_DOWN)
    {
        return HandleKeyEvent(ev, true);
    }
    else if (ev.type == EVENT_TYPE_KEY_UP)
    {
        return HandleKeyEvent(ev, false);
    }
    else if (ev.type == EVENT_TYPE_SHORTCUT)
    {
        if (ev.ref_id == TBIDC("copy"))
        {
            webClient_->ShortcutCopy();
            return true;
        }
        else if (ev.ref_id == TBIDC("paste"))
        {
            webClient_->ShortcutPaste();
            return true;
        }
        if (ev.ref_id == TBIDC("cut"))
        {
            webClient_->ShortcutCut();
            return true;
        }
        else if (ev.ref_id == TBIDC("selectall"))
        {
            webClient_->ShortcutSelectAll();
            return true;
        }
        else if (ev.ref_id == TBIDC("undo"))
        {
            webClient_->ShortcutUndo();
            return true;
        }
        else if (ev.ref_id == TBIDC("redo"))
        {
            webClient_->ShortcutRedo();
            return true;
        }

    }

    return UIWidget::OnEvent(ev);
}

WebTexture2D* UIWebView::GetWebTexture2D() const
{
    return webTexture_;
}


}
