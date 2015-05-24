
#include "UISelectItem.h"

using namespace tb;

namespace Atomic
{

UISelectItem::UISelectItem(Context* context, const String &str, const String &id, const String &skinImage) : Object(context)
    , subSource_(0)
{
    SetID(id);
    SetString(str);
    SetSkinImage(skinImage);
}

UISelectItem::~UISelectItem()
{

}

void UISelectItem::SetID(const String& id)
{
    id_ = TBID(id.CString());
}

void UISelectItem::SetSkinImage(const String& skinImage)
{
    skinImage_ = TBID(skinImage.CString());
}

void UISelectItem::SetSubSource(UISelectItemSource *subSource)
{
    subSource_ = subSource;
}

tb::TBGenericStringItem* UISelectItem::GetTBItem()
{
    tb::TBGenericStringItem* item;
    if (!subSource_)
    {
         item = new tb::TBGenericStringItem(str_.CString(), id_);
         if (skinImage_)
             item->SetSkinImage(skinImage_);
    }
    else
    {
        item = new tb::TBGenericStringItem(str_.CString(), subSource_->GetTBItemSource());
    }

    return item;
}

// Source

UISelectItemSource::UISelectItemSource(Context* context) : Object(context)
{

}

UISelectItemSource::~UISelectItemSource()
{

}

TBSelectItemSource *UISelectItemSource::GetTBItemSource()
{
    // caller's responsibility to clean up
    TBGenericStringItemSource* src = new TBGenericStringItemSource();

    for (List<SharedPtr<UISelectItem> >::Iterator itr = items_.Begin(); itr != items_.End(); itr++)
    {
        tb::TBGenericStringItem* tbitem = (*itr)->GetTBItem();
        src->AddItem(tbitem);
    }

    return src;
}

}
