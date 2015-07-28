// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <TurboBadger/tb_layout.h>

#include <Atomic/Core/Context.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/UI/UI.h>

#include "UIListView.h"
#include "UIIssuesWidget.h"
#include "UIMainFrame.h"
#include "UIResourceFrame.h"

#include "AEEvents.h"
#include "AEJavascript.h"

namespace AtomicEditor
{

IssuesWidget::IssuesWidget(Context* context) :
    AEWidget(context),
    issuelayout_(0)
{
    context_->RegisterSubsystem(this);

    UI* tbui = GetSubsystem<UI>();
    tbui->LoadResourceFile(delegate_, "AtomicEditor/editor/ui/issueswidget.tb.txt");

    issuelayout_ = delegate_->GetWidgetByIDAndType<TBLayout>(TBIDC("issuelayout"));
    assert(issuelayout_);

    issueList_ = new ListView(context_, "issuelist");

    TBWidgetDelegate* issueListWD = issueList_->GetWidgetDelegate();

    issueListWD->SetGravity(WIDGET_GRAVITY_ALL);

    issuelayout_->AddChild(issueListWD);

    SubscribeToEvent(E_EDITORSHUTDOWN, HANDLER(IssuesWidget, HandleEditorShutdown));
}

IssuesWidget::~IssuesWidget()
{

}

void IssuesWidget::UpdateIssues()
{
    AEJavascript* aejs = GetSubsystem<AEJavascript>();
    const Vector<JSError>& errors = aejs->GetJSErrors();

    issueList_->DeleteAllItems();

    for (unsigned i = 0; i < errors.Size(); i++)
    {
        const JSError& error = errors[i];

        String errorString;
        String filename = GetFileNameAndExtension(error.fullpath);

        errorString.AppendWithFormat("%s - %s - Line: %i Column: %i",
                                     filename.CString(), error.message.CString(), error.line, error.column);

        issueList_->AddItem(errorString.CString(), NULL, TBID(i));
    }

}

bool IssuesWidget::OnEvent(const TBWidgetEvent &ev)
{
    if (!ev.target)
        return false;

    TBID id = ev.target->GetID();

    if (ev.type == EVENT_TYPE_CLICK)
    {
        // we clicked the folder list
        if (ev.target->GetID() == TBID("issuelist"))
        {
            AEJavascript* aejs = GetSubsystem<AEJavascript>();
            const Vector<JSError>& errors = aejs->GetJSErrors();
            TBSelectList* list = (TBSelectList*) ev.target;
            TBID tbid = list->GetSelectedItemID();

            if (tbid < errors.Size())
            {
                const JSError& error = errors[tbid];
                // make sure we are editing it
                ResourceFrame* rframe = GetSubsystem<MainFrame>()->GetResourceFrame();
                rframe->EditResource(error.fullpath);
                rframe->NavigateToResource(error.fullpath, -1, error.tokenPos);
            }

        }
    }

    return false;
}

void IssuesWidget::HandleEditorShutdown(StringHash eventType, VariantMap& eventData)
{
    context_->RemoveSubsystem(GetType());
}

}

