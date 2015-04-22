
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
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }

    UI* ui = GetSubsystem<UI>();

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
}

UIWidget* UIWidget::GetWidget(const String& id)
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

void UIWidget::ConvertEvent(UIWidget *handler, UIWidget* target, const tb::TBWidgetEvent &ev, VariantMap& data)
{
    using namespace WidgetEvent;
    data[P_HANDLER] = handler;
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
    data[P_REFID] = (unsigned) ev.ref_id;
    data[P_TOUCH] = (unsigned) ev.touch;
}

void UIWidget::OnDelete()
{
    if (widget_)
    {
        // if we don't have a UI subsystem, we are exiting
        UI* ui = GetSubsystem<UI>();
        if (ui)
            ui->UnwrapWidget(widget_);
    }

    widget_ = 0;

    VariantMap eventData;
    eventData[WidgetDeleted::P_WIDGET] = this;
    SendEvent(E_WIDGETDELETED, eventData);

    ReleaseRef();
}

void UIWidget::AddChild(UIWidget* child)
{
    if (!widget_ || !child->widget_)
        return;

    widget_->AddChild(child->widget_);

    // this is to get proper padding, for instance when adding to a window
    // takes title into account.  This *may* cause problems here, if so
    // may need to make UIWidget AddChild virtual and inflate only on UIWindow::AddChild, etc
    widget_->OnInflateChild(child->widget_);

}

void UIWidget::SetText(const String& text)
{
    if (!widget_)
        return;

    widget_->SetText(text.CString());
}

void UIWidget::SetPosition(int x, int y)
{
    if (!widget_)
        return;

    widget_->SetPosition(TBPoint(x, y));

}

void UIWidget::SetSize(int width, int height)
{
    if (!widget_)
        return;

    widget_->SetSize(width, height);       
}

void UIWidget::Invalidate()
{
    if (!widget_)
        return;

    widget_->Invalidate();
}

void UIWidget::Center()
{
    if (!widget_)
        return;

    // this should center on parent widget, not root
    UI* ui = GetSubsystem<UI>();
    TBRect rect = widget_->GetRect();
    TBWidget* root = ui->GetRootWidget();
    TBRect bounds(0, 0, root->GetRect().w, root->GetRect().h);
    widget_->SetRect(rect.CenterIn(bounds).MoveIn(bounds).Clip(bounds));

}

UIWidget* UIWidget::GetParent()
{
    if (!widget_)
        return 0;

    TBWidget* parent = widget_->GetParent();

    if (!parent)
        return 0;

    UI* ui = GetSubsystem<UI>();

    return ui->WrapWidget(parent);

}

UIWidget* UIWidget::GetContentRoot()
{
    if (!widget_)
        return 0;

    TBWidget* root = widget_->GetContentRoot();

    if (!root)
        return 0;

    UI* ui = GetSubsystem<UI>();

    return ui->WrapWidget(root);
}

void UIWidget::Destroy()
{
    if (!widget_)
        return;

    if (widget_->GetParent())
        widget_->GetParent()->RemoveChild(widget_);

    delete widget_;
}

void UIWidget::RemoveChild(UIWidget* child, bool cleanup)
{
    if (!widget_ || !child)
        return;

    TBWidget* childw = child->GetInternalWidget();

    if (!childw)
        return;

    widget_->RemoveChild(childw);

    if (cleanup)
        delete childw;
}


const String& UIWidget::GetId()
{
    if (!widget_ || !widget_->GetID())
    {
        if (id_.Length())
            id_.Clear();

        return id_;
    }

    if (id_.Length())
        return id_;

    UI* ui = GetSubsystem<UI>();
    ui->GetTBIDString(widget_->GetID(), id_);

    return id_;

}

bool UIWidget::OnEvent(const tb::TBWidgetEvent &ev)
{
    UI* ui = GetSubsystem<UI>();

    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.target && ev.target->GetID() == TBID("__popup-menu"))
        {
            // popup menu

            if (JSGetHeapPtr())
            {
                VariantMap eventData;
                eventData[PopupMenuSelect::P_BUTTON] = this;
                eventData[PopupMenuSelect::P_REFID] = (unsigned) ev.ref_id;
                SendEvent(E_POPUPMENUSELECT, eventData);
            }

            return true;
        }
        else
        {
            if (!ev.target || ui->IsWidgetWrapped(ev.target))
            {
                VariantMap eventData;
                ConvertEvent(this, ui->WrapWidget(ev.target), ev, eventData);
                SendEvent(E_WIDGETEVENT, eventData);

                if (eventData[WidgetEvent::P_HANDLED].GetBool())
                    return true;

            }
        }

    }

    return false;
}

}
