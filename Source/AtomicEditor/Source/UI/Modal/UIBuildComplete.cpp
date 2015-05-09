// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <Atomic/Core/Context.h>
#include "AEEvents.h"

#include "UI/UIMainFrame.h"
#include "UI/Modal/UIBuildComplete.h"

#include <TurboBadger/tb_widgets_common.h>
#include <TurboBadger/tb_window.h>
#include <TurboBadger/tb_message_window.h>
#include <TurboBadger/tb_editfield.h>

#include <Atomic/UI/UI.h>

#include <Build/BuildSystem.h>
#include <Project/ProjectUtils.h>

using namespace tb;

namespace AtomicEditor
{

/// Construct.
UIBuildComplete::UIBuildComplete(Context* context, const String &title, const String &message, const String &buildFolder, bool success) :
    AEWidget(context)
  , window_(0)
  , dimmer_(0)
{

    buildFolder_ = buildFolder;
    success_ = success;

    UI* tbui = GetSubsystem<UI>();
    dimmer_ = new TBDimmer();

    window_ = new TBWindow();
    tbui->LoadResourceFile(window_->GetContentRoot(), "AtomicEditor/editor/ui/buildcomplete.tb.txt");

    window_->ResizeToFitContent();
    delegate_->AddChild(window_);

    message_ = delegate_->GetWidgetByIDAndType<TBTextField>(TBIDC("message"));
    assert(message_);

    TBButton* reveal = delegate_->GetWidgetByIDAndType<TBButton>(TBIDC("reveal"));
    assert(reveal);

    if (!success)
        reveal->SetState(WIDGET_STATE_DISABLED, true);

    message_->SetText(message.CString());
    window_->SetText(title.CString());

    Center();

    TBWidgetListener::AddGlobalListener(this);
}


/// Destruct.
UIBuildComplete::~UIBuildComplete()
{
    TBWidgetListener::RemoveGlobalListener(this);
}

bool UIBuildComplete::OnEvent(const TBWidgetEvent &ev)
{

    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();

    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.target->GetID() == TBIDC("ok"))
        {
            buildSystem->ClearBuildCompleteUI();
            return true;
        }
        else if (ev.target->GetID() == TBIDC("reveal"))
        {
            ProjectUtils* utils = GetSubsystem<ProjectUtils>();
            utils->RevealInFinder(buildFolder_);
            return true;
        }

    }

    return false;
}

void UIBuildComplete::SetMessage(const String& message)
{
    message_->SetText(message.CString());
}

void UIBuildComplete::Center()
{
    UI* tbui = GetSubsystem<UI>();
    TBRect rect = window_->GetRect();
    TBWidget* root = tbui->GetRootWidget();
    TBRect bounds(0, 0, root->GetRect().w, root->GetRect().h);
    window_->SetRect(rect.CenterIn(bounds).MoveIn(bounds).Clip(bounds));
    delegate_->SetRect(bounds);

}

void UIBuildComplete::Show()
{
    assert(!dimmer_->GetParent());
    UI* tbui = GetSubsystem<UI>();
    TBWidget* root = tbui->GetRootWidget();
    root->AddChild(dimmer_);
    root->AddChild(delegate_);

}

void UIBuildComplete::Hide()
{
    UI* tbui = GetSubsystem<UI>();
    tbui->GetRootWidget()->SetFocusRecursive(WIDGET_FOCUS_REASON_UNKNOWN);

    if (dimmer_->GetParent())
        dimmer_->GetParent()->RemoveChild(dimmer_);

    if (delegate_->GetParent())
        delegate_->GetParent()->RemoveChild(delegate_);
}

void UIBuildComplete::OnWidgetDelete(TBWidget *widget)
{
    if (widget == window_)
    {
        BuildSystem* buildSystem = GetSubsystem<BuildSystem>();
        buildSystem->ClearBuildCompleteUI();
    }
}

bool UIBuildComplete::OnWidgetDying(TBWidget *widget)
{
    return false;
}



}
