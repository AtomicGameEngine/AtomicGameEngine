#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_widgets_common.h>
#include <TurboBadger/tb_layout.h>

#include "UI.h"
#include "UIEvents.h"
#include "UIWidget.h"
#include "UIFontDescription.h"

using namespace tb;

namespace Atomic
{

UIFontDescription::UIFontDescription(Context* context) : Object(context)
{
}

UIFontDescription::~UIFontDescription()
{

}

void UIFontDescription::SetId(const String& id)
{
    desc_.SetID(TBIDC(id.CString()));
}

void UIFontDescription::SetSize(int size)
{
    desc_.SetSize(size);
}

}
