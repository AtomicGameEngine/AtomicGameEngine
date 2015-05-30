

#pragma once

#include "UIWidget.h"

namespace Atomic
{

class UIFontDescription : public Object
{
    OBJECT(UIFontDescription)

public:

    UIFontDescription(Context* context);
    virtual ~UIFontDescription();

    void SetId(const String& id);
    void SetSize(int size);

    tb::TBFontDescription* GetTBFontDescription() { return &desc_; }

private:

    tb::TBFontDescription desc_;

};


}
