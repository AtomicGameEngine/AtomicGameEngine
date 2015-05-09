// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <Atomic/UI/UI.h>
#include <Atomic/IO/Log.h>
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

namespace AtomicEditor
{

// == AdvancedItemWidget ======================================================

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

// == AdvancedItemSource ======================================================

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
