// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <Atomic/Core/Context.h>
#include <Atomic/IO/Log.h>
#include <Atomic/UI/UI.h>

#include <TurboBadger/tb_editfield.h>

#include "../AEEvents.h"
#include "UIFindTextWidget.h"


namespace AtomicEditor
{

FindTextWidget::FindTextWidget(Context* context) :
    AEWidget(context),
    findtextedit_(0)
{
    context_->RegisterSubsystem(this);

    UI* tbui = GetSubsystem<UI>();
    tbui->LoadResourceFile(delegate_, "AtomicEditor/editor/ui/findtextwidget.tb.txt");
    tbui->GetRootWidget()->AddChild(delegate_);

    findtextedit_ = delegate_->GetWidgetByIDAndType<TBEditField>(TBIDC("findtextedit"));
    assert(findtextedit_);

    delegate_->SetVisibilility(WIDGET_VISIBILITY_INVISIBLE);

    SubscribeToEvent(E_FINDTEXTOPEN, HANDLER(FindTextWidget, HandleFindTextOpen));
    SubscribeToEvent(E_FINDTEXTCLOSE, HANDLER(FindTextWidget, HandleFindTextClose));
    SubscribeToEvent(E_EDITORSHUTDOWN, HANDLER(FindTextWidget, HandleEditorShutdown));
}

FindTextWidget::~FindTextWidget()
{

}

void FindTextWidget::SetUISize(int uiWidth, int uiHeight)
{
    TBContainer* findcontainer = delegate_->GetWidgetByIDAndType<TBContainer>(TBIDC("findcontainer"));
    assert(findcontainer);

    PreferredSize psize = delegate_->GetPreferredSize();
    int height = psize.min_h + 16;
    delegate_->SetSize(uiWidth, height);
    findcontainer->SetSize(uiWidth, height);
    delegate_->SetPosition(TBPoint(0, uiHeight - height));
}

void FindTextWidget::HandleFindTextOpen(StringHash eventType, VariantMap& eventData)
{
    if (delegate_->GetVisibility() == WIDGET_VISIBILITY_VISIBLE)
    {
        findtextedit_->SetFocus(WIDGET_FOCUS_REASON_UNKNOWN);
        return;
    }

    delegate_->SetVisibilility(WIDGET_VISIBILITY_VISIBLE);

    findtextedit_->SetFocus(WIDGET_FOCUS_REASON_UNKNOWN);

    TBStyleEdit* styleEdit = findtextedit_->GetStyleEdit();
    styleEdit->selection.SelectAll();
}

void FindTextWidget::HandleFindTextClose(StringHash eventType, VariantMap& eventData)
{
    delegate_->SetVisibilility(WIDGET_VISIBILITY_INVISIBLE);
}

void FindTextWidget::GetFindText(String& text)
{
    TBStr _text;
    findtextedit_->GetText(_text);
    text = _text.CStr();
}
void FindTextWidget::Find(const String& findText, unsigned flags)
{
    using namespace FindText;
    VariantMap eventData;
    eventData[P_TEXT] = findText;
    eventData[P_FLAGS] = flags;
    SendEvent(E_FINDTEXT, eventData);
}

bool FindTextWidget::OnEvent(const TBWidgetEvent &ev)
{

    bool findnext = false;

    if (ev.type == EVENT_TYPE_KEY_DOWN)
    {
        if (ev.special_key == TB_KEY_ESC)
        {
            SendEvent(E_FINDTEXTCLOSE);
        }
        else if (ev.special_key == TB_KEY_ENTER)
        {
            findnext = true;
        }
    }

    if (ev.type == EVENT_TYPE_CHANGED)
    {
        TBStr _text;
        findtextedit_->GetText(_text);

        using namespace FindText;

        VariantMap eventData;
        int flags = FINDTEXT_FLAG_NONE;

        flags |= FINDTEXT_FLAG_WRAP;

        eventData[P_TEXT] = _text.CStr();
        eventData[P_FLAGS] = flags;
        SendEvent(E_FINDTEXT, eventData);

    }

    if (findnext || ev.type == EVENT_TYPE_SHORTCUT)
    {
        if (findnext || ev.ref_id == TBIDC("findnext") || ev.ref_id == TBIDC("findprev"))
        {
            TBStr _text;
            findtextedit_->GetText(_text);

            unsigned flags = FINDTEXT_FLAG_NONE;
            if (findnext || ev.ref_id == TBIDC("findnext"))
                flags |= FINDTEXT_FLAG_NEXT;
            else if (ev.ref_id == TBIDC("findprev"))
                flags |= FINDTEXT_FLAG_PREV;

            flags |= FINDTEXT_FLAG_WRAP;

            Find(_text.CStr(), flags);

        }
    }


    return false;
}

void FindTextWidget::HandleEditorShutdown(StringHash eventType, VariantMap& eventData)
{
    context_->RemoveSubsystem(GetType());
}

}

