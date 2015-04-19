
//--player --editor-resource-paths "/Users/josh/Dev/atomic/AtomicGameEngine/Data/AtomicPlayer/Resources/CoreData!/Users/josh/Dev/atomic/AtomicGameEngine/Data/AtomicPlayer/Resources/PlayerData!/Users/josh/Dev/atomic/AtomicExamples/UIExample/Resources"

#include "../IO/Log.h"

#include "UIEvents.h"
#include "UI.h"
#include "UIWidget.h"

using namespace tb;

namespace Atomic
{

UIWidget::UIWidget(Context* context, bool createWidget) : Object(context),
    widget_(0)
{    
    AddRef();

    if (createWidget)
    {
        widget_ = new TBWidget();
        widget_->SetDelegate(this);
    }
}

UIWidget::~UIWidget()
{

}

bool UIWidget::Load(const String& filename)
{
    UI* ui = GetSubsystem<UI>();

    if  (!ui->LoadResourceFile(widget_ , filename))
        return false;    

    VariantMap eventData;
    eventData[WidgetLoaded::P_WIDGET] = this;
    SendEvent(E_WIDGETLOADED, eventData);
    return true;

    return true;
}

UIWidget* UIWidget::GetWidgetByID(const String& id)
{
    if (!widget_)
        return 0;

    TBWidget* child = widget_->GetWidgetByID(TBID(id.CString()));

    if (!child)
        return 0;

    UI* ui = GetSubsystem<UI>();
    return ui->WrapWidget(child);
}

void UIWidget::SetWidget(tb::TBWidget* widget)
{
    widget_ = widget;
    widget_->SetDelegate(this);
}

void UIWidget::ConvertEvent(UIWidget* target, const tb::TBWidgetEvent &ev, VariantMap& data)
{
    using namespace WidgetEvent;
    data[P_TARGET] = target;
    data[P_TYPE] = (unsigned) ev.type;
    data[P_X] = ev.target_x;
    data[P_Y] = ev.target_y;
    data[P_DELTAX] = ev.delta_x;
    data[P_DELTAY] = ev.delta_y;
    data[P_COUNT] = ev.count;
    data[P_KEY] = ev.key;
    data[P_SPECIALKEY] = (unsigned) ev.special_key;
    data[P_MODIFIERKEYS] = (unsigned) ev.modifierkeys;
    data[P_ID] = (unsigned) ev.ref_id;
    data[P_TOUCH] = (unsigned) ev.touch;
}

void UIWidget::OnDelete()
{
    widget_ = 0;
    ReleaseRef();
}

void UIWidget::AddChild(UIWidget* child)
{
    if (!widget_ || !child->widget_)
        return;

    widget_->AddChild(child->widget_);
}

bool UIWidget::SetText(const String& text)
{
    if (!widget_)
        return false;

    return widget_->SetText(text.CString());
}

void UIWidget::SetSize(int width, int height)
{
    if (!widget_)
        return;

    widget_->SetSize(width, height);
}

void UIWidget::Center()
{
    if (!widget_)
        return;

    UI* ui = GetSubsystem<UI>();
    TBRect rect = widget_->GetRect();
    TBWidget* root = ui->GetRootWidget();
    TBRect bounds(0, 0, root->GetRect().w, root->GetRect().h);
    widget_->SetRect(rect.CenterIn(bounds).MoveIn(bounds).Clip(bounds));

}

}
