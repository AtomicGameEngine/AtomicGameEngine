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

#include <TurboBadger/tb_widgets.h>

#include <Atomic/UI/UI.h>
#include <Atomic/UI/UIWidget.h>

#include <Atomic/Graphics/Texture2D.h>

namespace Atomic
{

class WebClient;
class WebTexture2D;

/// UI widget for WebViews
class UIWebView : public UIWidget
{
    friend class WebViewWidget;

    ATOMIC_OBJECT(UIWebView, UIWidget)

public:

    UIWebView(Context* context, const String& initialURL = String::EMPTY);
    virtual ~UIWebView();

    /// Get the widget's WebClient
    WebClient* GetWebClient() { return webClient_; }

    /// Get the WebTexture in use by the WebView
    WebTexture2D* GetWebTexture2D() const;

protected:

    bool OnEvent(const tb::TBWidgetEvent &ev);
    void OnFocusChanged(bool focused);

private:

    void HandleKeyDown(StringHash eventType, VariantMap& eventData);
    void HandleKeyUp(StringHash eventType, VariantMap& eventData);
    void HandleTextInput(StringHash eventType, VariantMap& eventData);

    SharedPtr<WebClient> webClient_;
    SharedPtr<WebTexture2D> webTexture_;

    String initialURL_;

};

}
