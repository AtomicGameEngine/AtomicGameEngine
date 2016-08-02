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

#include "../Input/Input.h"

namespace Atomic
{


class UIButton : public UIWidget
{
    ATOMIC_OBJECT(UIButton, UIWidget)

public:

    UIButton(Context* context, bool createWidget = true);
    virtual ~UIButton();

    /// Set if the button's text field should be allowed to squeeze below its preferred size
    /// If squeezable it may shrink to width 0
    void SetSqueezable(bool value);

    void SetEmulationButton(int button);

    /// A button link may be set via the text attribute, if enabled
    /// a FileSystem open will be called when the button is clicked
    /// for example a http:// link will open the default browser
    void SetURLEnabled(bool enabled) { urlEnabled_ = enabled; }

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

    int emulationButton_;
    bool urlEnabled_;
};

}
