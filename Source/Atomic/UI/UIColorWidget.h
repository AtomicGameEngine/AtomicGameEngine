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

#pragma once


#include "UIWidget.h"


namespace Atomic
{

/// A widget that can render a Texture2D, so the image data
/// doesn't need to be loaded 2x (once for Texture2D and once for say a UIImageWidget)
class UIColorWidget : public UIWidget
{
    ATOMIC_OBJECT(UIColorWidget, UIWidget)

public:

    UIColorWidget(Context* context, bool createWidget = true);
    virtual ~UIColorWidget();

    /// Set color from a Color value
    void SetColor(const Color& color);

    /// Set color from hex string
    void SetColorString(const String &name);

    Color GetColor() const;

    void SetAlpha ( float value );

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}
