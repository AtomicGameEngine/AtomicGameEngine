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
#include "UISelectItem.h"

namespace tb
{
    class TBSelectList;
}

namespace Atomic
{

class UISelectList : public UIWidget
{
    ATOMIC_OBJECT(UISelectList, UIWidget)

public:

    UISelectList(Context* context, bool createWidget = true);
    virtual ~UISelectList();

    void SetFilter(const String& filter);

    void SetSource(UISelectItemSource* source);
    void InvalidateList();

    void SetValue(int value);
    double GetValue();

    String GetHoverItemID();

    String GetSelectedItemID();

    void ScrollToSelectedItem();

    String GetItemID(int index);
    bool GetItemSelected(int index);

    int GetNumItems() const;

    void SelectItem(int index, bool selected = true);

    tb::TBSelectList* GetTBSelectList();

    void SelectNextItem();
    void SelectPreviousItem();

    void SetUIListView(bool value);

protected:

    void HandleUIUpdate(StringHash eventType, VariantMap& eventData);

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};


}
