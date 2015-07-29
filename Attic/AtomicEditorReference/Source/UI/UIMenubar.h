// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include "AEWidget.h"

#include <TurboBadger/tb_menu_window.h>
#include <TurboBadger/tb_select.h>

using namespace Atomic;

namespace AtomicEditor
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


}
