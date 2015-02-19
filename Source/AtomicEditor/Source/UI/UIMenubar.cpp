// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <Atomic/UI/TBUI.h>
#include <Atomic/IO/Log.h>
#include "UIMenubar.h"

using namespace tb;

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

    TBTextField* text = new TBTextField();
    text->SetText(item->str);
    root->AddChild(text);

    if (item->shortcut_.Length())
    {
        TBWidget* spacer = new TBWidget();
        spacer->SetGravity(WIDGET_GRAVITY_LEFT_RIGHT);
        root->AddChild(spacer);

        TBTextField* shortcut = new TBTextField();
        shortcut->SetText(item->shortcut_.CString());
        shortcut->SetGravity(WIDGET_GRAVITY_RIGHT);
        root->AddChild(shortcut);

    }

}

bool MenubarItemWidget::OnEvent(const TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_CLICK && ev.target->GetID() == TBIDC("check"))
    {
        // MenubarItem *item = m_source->GetItem(m_index);
        m_source->InvokeItemChanged(m_index, m_source_viewer);
        return true;
    }
    else if (ev.type == EVENT_TYPE_CLICK && ev.target->GetID() == TBIDC("delete"))
    {
        m_source->DeleteItem(m_index);
        return true;
    }
    return TBLayout::OnEvent(ev);
}

// == AdvancedItemSource ======================================================

bool MenubarItemSource::Filter(int index, const char *filter)
{
    return true;
}

TBWidget *MenubarItemSource::CreateItemWidget(int index, TBSelectItemViewer *viewer)
{
    const char *string = GetItemString(index);
    if (string && *string == '-')
    {
        if (TBSeparator *separator = new TBSeparator)
        {
            separator->SetGravity(WIDGET_GRAVITY_ALL);
            separator->SetSkinBg(TBIDC("TBSelectItem.separator"));
            return separator;
        }
    }

    if (TBLayout *layout = new MenubarItemWidget(GetItem(index), this, viewer, index))
        return layout;
    return NULL;
}



}
