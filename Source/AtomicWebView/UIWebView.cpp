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

#include <Atomic/UI/UIRenderer.h>

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

    WebViewWidget()
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
        IntVector2 size; /* = webView_->GetSize();*/

        if (size.x_ != rect.w || size.y_ != rect.h)
        {
            size.x_ = rect.w;
            size.y_ = rect.h;

            webView_->SetResizeRequired();
            // early out here, responsible for flicker
            // https://github.com/AtomicGameEngine/AtomicGameEngine/issues/115
            return;
        }

        float* data = &vertexData_[0];

        UI* ui = webView_->GetSubsystem<UI>();

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

        /*
        webView_->GetSubsystem<UI>()->SubmitBatchVertexData(webView_->GetTexture(), vertexData_);
        */


    }

private:

    WeakPtr<UIWebView> webView_;
    PODVector<float> vertexData_;

};

UIWebView::UIWebView(Context* context) : UIWidget(context, false),
    resizeRequired_(false)
{

    widget_ = new WebViewWidget();
    widget_->SetDelegate(this);
    widget_->SetGravity(WIDGET_GRAVITY_ALL);
    ((WebViewWidget*)widget_)->webView_ = this;

    UI* ui = GetSubsystem<UI>();
    ui->WrapWidget(this, widget_);
}

UIWebView::~UIWebView()
{

}


}
