
#pragma once

#include "UISelectItem.h"

namespace Atomic
{

class UIMenuItemSource;

class UIMenuItem : public UISelectItem
{
    OBJECT(UIMenuItem)

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
    OBJECT(UIMenuItemSource)

public:

    UIMenuItemSource(Context* context);
    virtual ~UIMenuItemSource();

    // caller's responsibility to clean up
    tb::TBSelectItemSource *GetTBItemSource();

private:

};

}
