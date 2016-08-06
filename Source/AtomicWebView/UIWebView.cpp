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

    void OnProcess()
    {
        TBWidget::OnProcess();

        if (webView_.Null())
            return;

        if (!browserCreated_)
        {
            browserCreated_ = true;
            TBRect rect = GetRect();
            webView_->webClient_->CreateBrowser(webView_->initialURL_, rect.w, rect.h);
        }

    }

    void OnResized(int oldW, int oldH)
    {
        if (webView_.Null())
            return;

        TBRect rect = GetRect();

        if (rect.w <= 0 || rect.h <= 0)
            return;

        webView_->webClient_->SetSize(rect.w, rect.h);
    }

    void OnPaint(const PaintProps &paint_props)
    {
        if (webView_.Null())
            return;

        TBRect rect = GetRect();
        rect.x = rect.y = 0;
        ConvertToRoot(rect.x, rect.y);

        float* data = &vertexData_[0];

        UI* ui = webView_->GetSubsystem<UI>();

        float color;
        float fopacity = GetOpacity() * ui->GetRenderer()->GetOpacity();
        unsigned char opacity = (unsigned char) (fopacity* 255.0f);
        ((unsigned&)color) = (0x00FFFFFF + (((uint32)opacity) << 24));

        float x = (float) rect.x;
        float y = (float) rect.y;
        float w = (float) rect.w;
        float h = (float) rect.h;

#ifdef ATOMIC_PLATFORM_WINDOWS

#ifndef ATOMIC_D3D11
        //Direct3D9 Adjustment
        x += 0.5f;
        y += 0.5f;
#endif

#endif

        data[3] = color;
        data[9] = color;
        data[15] = color;
        data[21] = color;
        data[27] = color;
        data[33] = color;

        data[0] = x;
        data[1] = y;

        data[6] = x + w;
        data[7] =  y;

        data[12] = x + w;
        data[13] = y + h;

        data[18] = x;
        data[19] = y;

        data[24] = x + w;
        data[25] = y + h;

        data[30] = x;
        data[31] = y + h;

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

    SubscribeToEvent(E_KEYDOWN, ATOMIC_HANDLER(UIWebView, HandleKeyDown));
    SubscribeToEvent(E_KEYUP, ATOMIC_HANDLER(UIWebView, HandleKeyUp));
    SubscribeToEvent(E_TEXTINPUT, ATOMIC_HANDLER(UIWebView, HandleTextInput));
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

bool UIWebView::OnEvent(const TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_POINTER_DOWN || ev.type == EVENT_TYPE_POINTER_UP)
    {
        webClient_->SendMouseClickEvent(ev.target_x, ev.target_y, 0, ev.type == EVENT_TYPE_POINTER_UP, 0, ev.count);
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
    else if (ev.type == EVENT_TYPE_SHORTCUT)
    {

#ifdef ATOMIC_PLATFORM_OSX
        // On OSX, these shortcuts are not being invoked with key combinations (Handle KeyDown/KeyUp)
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
        else if (ev.ref_id == TBIDC("cut"))
        {
            webClient_->ShortcutCut();
            return true;
        }
#endif


        // Handled by key combinations in Handle KeyDown/KeyUp
        /*
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
        */
    }
    else if (ev.type == EVENT_TYPE_KEY_DOWN || ev.type == EVENT_TYPE_KEY_UP)
    {
        // We consume all key events as they are handled by the UI event system directly
        return true;
    }

    return UIWidget::OnEvent(ev);
}

void UIWebView::OnFocusChanged(bool focused)
{
    webClient_->SendFocusEvent(focused);
}

WebTexture2D* UIWebView::GetWebTexture2D() const
{
    return webTexture_;
}


}
