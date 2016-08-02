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

#include "UISelectItem.h"

namespace Atomic
{

class UIMenuItemSource;

class UIMenuItem : public UISelectItem
{
    ATOMIC_OBJECT(UIMenuItem, UISelectItem)

public:

    UIMenuItem(Context* context, const String& str = String::EMPTY, const String& id = String::EMPTY, const String& shortcut = String::EMPTY, const String& skinBg = String::EMPTY);
    virtual ~UIMenuItem();

    tb::TBGenericStringItem* GetTBItem();

private:

    String shortcut_;
    String skinBg_;

};

class UIMenuItemSource : public UISelectItemSource
{
    ATOMIC_OBJECT(UIMenuItemSource, UISelectItemSource)

public:

    UIMenuItemSource(Context* context);
    virtual ~UIMenuItemSource();

    // caller's responsibility to clean up
    tb::TBSelectItemSource *GetTBItemSource();

private:

};

}
