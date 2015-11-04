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

#include <TurboBadger/tb_menu_window.h>
#include <TurboBadger/tb_select.h>

#include "UIMenubar.h"

using namespace tb;

namespace tb {


// THIS MUST MATCH TBSimpleLayoutItemWidget in tb_select_item.cpp
class TBSimpleLayoutItemWidget : public TBLayout, private TBWidgetListener
{
public:
    TBSimpleLayoutItemWidget(TBID image, TBSelectItemSource *source, const char *str);
    ~TBSimpleLayoutItemWidget();
    virtual bool OnEvent(const TBWidgetEvent &ev);
private:
    TBSelectItemSource *m_source;
    TBTextField m_textfield;
    TBSkinImage m_image;
    TBSkinImage m_image_arrow;
    TBMenuWindow *m_menu; ///< Points to the submenu window if opened
    virtual void OnWidgetDelete(TBWidget *widget);
    void OpenSubMenu();
    void CloseSubMenu();
};

}

namespace Atomic
{


class MenubarItem : public TBGenericStringItem
{
public:
    MenubarItem(const char *str, const TBID &id = TBID((uint32)0), const String& shortcut = String::EMPTY)
        : TBGenericStringItem(str, id), shortcut_(shortcut)
    {

    }

    MenubarItem(const char *str, TBSelectItemSource *sub_source) : TBGenericStringItem(str, sub_source) {}

    String shortcut_;

};

class MenubarItemSource : public TBSelectItemSourceList<MenubarItem>
{
public:
    virtual bool Filter(int index, const char *filter);
    virtual TBWidget *CreateItemWidget(int index, TBSelectItemViewer *viewer);
};

class MenubarItemWidget : public TBLayout
{
public:
    MenubarItemWidget(MenubarItem *item, MenubarItemSource *source, TBSelectItemViewer *source_viewer, int index);
    virtual bool OnEvent(const TBWidgetEvent &ev);
private:
    MenubarItemSource *m_source;
    TBSelectItemViewer *m_source_viewer;
    int m_index;
};

// UI IMPLEMENTATION

UIMenuItem::UIMenuItem(Context* context, const String& str, const String& id, const String& shortcut, const String& skinBg):
    UISelectItem(context, str, id),
    shortcut_(shortcut),
    skinBg_(skinBg)
{

}

UIMenuItem::~UIMenuItem()
{

}

tb::TBGenericStringItem* UIMenuItem::GetTBItem()
{
    MenubarItem* item;

    if (!subSource_)
    {
        item = new MenubarItem(str_.CString(), id_, shortcut_);
    }
    else
    {
        item = new MenubarItem(str_.CString(), subSource_->GetTBItemSource());
    }

    if (skinBg_.Length())
    {
        item->SetSkinImage(TBIDC(skinBg_.CString()));
    }

    return item;

}

tb::TBSelectItemSource* UIMenuItemSource::GetTBItemSource()
{
    // caller's responsibility to clean up
    MenubarItemSource* src = new MenubarItemSource();

    for (List<SharedPtr<UISelectItem> >::Iterator itr = items_.Begin(); itr != items_.End(); itr++)
    {
        MenubarItem* tbitem = (MenubarItem*) (*itr)->GetTBItem();
        src->AddItem(tbitem);
    }

    return src;
}

UIMenuItemSource::UIMenuItemSource(Context* context) : UISelectItemSource(context)
{

}

UIMenuItemSource::~UIMenuItemSource()
{

}

// WIDGET IMPLEMENTATION

MenubarItemWidget::MenubarItemWidget(MenubarItem *item, MenubarItemSource *source,
                                     TBSelectItemViewer *source_viewer, int index)
    : m_source(source)
    , m_source_viewer(source_viewer)
    , m_index(index)
{
    SetSkinBg(TBIDC("TBSelectItem"));
    SetLayoutDistribution(LAYOUT_DISTRIBUTION_GRAVITY);
    SetLayoutDistributionPosition(LAYOUT_DISTRIBUTION_POSITION_LEFT_TOP);
    SetPaintOverflowFadeout(false);

    TBWidget* root = GetContentRoot();

    TBFontDescription fd;
    fd.SetID(TBIDC("Vera"));
    fd.SetSize(12);

    TBTextField* text = new TBTextField();
    text->SetIgnoreInput(true);
    text->SetText(item->str);
    text->SetFontDescription(fd);
    root->AddChild(text);

    if (item->shortcut_.Length())
    {
        TBWidget* spacer = new TBWidget();
        spacer->SetIgnoreInput(true);
        spacer->SetGravity(WIDGET_GRAVITY_LEFT_RIGHT);
        root->AddChild(spacer);

        TBTextField* shortcut = new TBTextField();
        shortcut->SetIgnoreInput(true);
        shortcut->SetText(item->shortcut_.CString());
        shortcut->SetFontDescription(fd);
        shortcut->SetGravity(WIDGET_GRAVITY_RIGHT);
        root->AddChild(shortcut);
    }

}

bool MenubarItemWidget::OnEvent(const TBWidgetEvent &ev)
{
    if (m_source && ev.type == EVENT_TYPE_CLICK && ev.target == this)
    {
        return false;
    }

    return false;
}

bool MenubarItemSource::Filter(int index, const char *filter)
{
    return true;
}

TBWidget *MenubarItemSource::CreateItemWidget(int index, TBSelectItemViewer *viewer)
{
    const char *string = GetItemString(index);

    TBSelectItemSource *sub_source = GetItemSubSource(index);
    TBID image = GetItemImage(index);

    if (sub_source || image)
    {
        if (TBSimpleLayoutItemWidget *itemwidget = new TBSimpleLayoutItemWidget(image, sub_source, string))
        {
            itemwidget->SetID(GetItem(index)->id);
            return itemwidget;
        }
    }
    else if (string && *string == '-')
    {
        if (TBSeparator *separator = new TBSeparator)
        {
            separator->SetGravity(WIDGET_GRAVITY_ALL);
            separator->SetSkinBg(TBIDC("AESeparator"));
            return separator;
        }
    }
    else if (TBLayout *layout = new MenubarItemWidget(GetItem(index), this, viewer, index))
    {
        layout->SetID(GetItem(index)->id);
        return layout;
    }

    return NULL;
}

}
