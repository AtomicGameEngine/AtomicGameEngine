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

#include <TurboBadger/tb_editfield.h>

namespace Atomic
{

enum UI_EDIT_TYPE {
    UI_EDIT_TYPE_TEXT = tb::EDIT_TYPE_TEXT,
    UI_EDIT_TYPE_SEARCH = tb::EDIT_TYPE_SEARCH,
    UI_EDIT_TYPE_PASSWORD = tb::EDIT_TYPE_PASSWORD,
    UI_EDIT_TYPE_EMAIL = tb::EDIT_TYPE_EMAIL,
    UI_EDIT_TYPE_PHONE = tb::EDIT_TYPE_PHONE,
    UI_EDIT_TYPE_URL = tb::EDIT_TYPE_URL,
    UI_EDIT_TYPE_NUMBER = tb::EDIT_TYPE_NUMBER
};

class UIEditField : public UIWidget
{
    ATOMIC_OBJECT(UIEditField, UIWidget)

public:

    UIEditField(Context* context, bool createWidget = true);
    virtual ~UIEditField();

    void AppendText(const String& text);

    void SetTextAlign(UI_TEXT_ALIGN align);

    void SetAdaptToContentSize(bool adapt);
    bool GetAdaptToContentSize() const;

    void SetEditType(UI_EDIT_TYPE type);

    void SetReadOnly(bool readonly);
    void SetStyling(bool styling);
    void SetMultiline(bool multiline);

    void Reformat(bool update_fragments = false);

    void ScrollTo(int x, int y);

    void SetWrapping(bool wrap);
    bool GetWrapping();

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

    virtual void OnFocusChanged(bool focused);

private:
    // Used to keep track of if we have just been focused for the click select
    bool firstFocusFlag_;
};

}
