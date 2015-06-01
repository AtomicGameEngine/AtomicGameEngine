
#pragma once

#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_select_item.h>

#include "../Core/Object.h"
#include "../Container/List.h"

namespace Atomic
{

class UISelectItemSource;

class UISelectItem : public Object
{
    OBJECT(UISelectItem)

public:

    UISelectItem(Context* context, const String& str = String::EMPTY, const String& id = String::EMPTY, const String& skinImage = String::EMPTY);
    virtual ~UISelectItem();

    void SetString(const String& str) { str_ = str; }
    void SetID(const String& id);
    void SetSkinImage(const String& skinImage);
    void SetSubSource(UISelectItemSource *subSource);

    virtual tb::TBGenericStringItem* GetTBItem();

protected:

    String str_;

    // TBID
    tb::TBID id_;
    // TBID
    tb::TBID skinImage_;

    SharedPtr<UISelectItemSource> subSource_;

};

class UISelectItemSource : public Object
{
    OBJECT(UISelectItemSource)

public:

    UISelectItemSource(Context* context);
    virtual ~UISelectItemSource();

    void AddItem(UISelectItem* item) { items_.Push(SharedPtr<UISelectItem>(item)); }

    // caller's responsibility to clean up
    virtual tb::TBSelectItemSource* GetTBItemSource();

protected:

    List<SharedPtr<UISelectItem>> items_;

};

}
