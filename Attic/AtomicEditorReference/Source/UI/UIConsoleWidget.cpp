// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <TurboBadger/tb_layout.h>

#include <Atomic/Core/Context.h>
#include <Atomic/IO/Log.h>
#include <Atomic/UI/UI.h>

#include "UIListView.h"
#include "UIConsoleWidget.h"
#include "UIMainFrame.h"
#include "UIResourceFrame.h"

#include <Atomic/IO/FileSystem.h>

#include <AtomicJS/Javascript/JSEvents.h>

#include "../AEJavascript.h"
#include "../AEEvents.h"

namespace AtomicEditor
{

ConsoleWidget::ConsoleWidget(Context* context) :
    AEWidget(context),
    consolelayout_(0)
{
    context_->RegisterSubsystem(this);
    SubscribeToEvent(E_EDITORSHUTDOWN, HANDLER(ConsoleWidget, HandleEditorShutdown));

    UI* tbui = GetSubsystem<UI>();
    tbui->LoadResourceFile(delegate_, "AtomicEditor/editor/ui/consolewidget.tb.txt");

    consolelayout_ = delegate_->GetWidgetByIDAndType<TBLayout>(TBIDC("consolelayout"));
    assert(consolelayout_);

    consoleList_ = new ListView(context_, "consolelist");

    TBWidgetDelegate* consoleListWD = consoleList_->GetWidgetDelegate();

    consoleListWD->SetGravity(WIDGET_GRAVITY_ALL);

    consolelayout_->AddChild(consoleListWD);

    SubscribeToEvent(E_JSPRINT, HANDLER(ConsoleWidget, HandleMessage));
    SubscribeToEvent(E_CONSOLEMESSAGE, HANDLER(ConsoleWidget, HandleMessage));
    SubscribeToEvent(E_CONSOLEERRORMESSAGE, HANDLER(ConsoleWidget, HandleMessage));

}

ConsoleWidget::~ConsoleWidget()
{

}

void ConsoleWidget::HandleMessage(StringHash eventType, VariantMap& eventData)
{
    if (eventType == E_CONSOLEMESSAGE)
    {
        using namespace ConsoleMessage;
        const String& message = eventData[P_TEXT].GetString();
    }
    else if (eventType == E_JSPRINT)
    {
        using namespace JSPrint;
        const String& message = eventData[P_TEXT].GetString();
        consoleList_->AddItem(message.CString(), NULL, TBID(consoleList_->GetNumItems()));
    }
    else if (eventType == E_CONSOLEERRORMESSAGE)
    {
        using namespace ConsoleErrorMessage;
        const String& message = eventData[P_TEXT].GetString();
        consoleList_->AddItem(message.CString(), NULL, TBID(consoleList_->GetNumItems()));
    }
}

bool ConsoleWidget::OnEvent(const TBWidgetEvent &ev)
{

    return false;
}

void ConsoleWidget::HandleEditorShutdown(StringHash eventType, VariantMap& eventData)
{
    context_->RemoveSubsystem(GetType());
}

}

