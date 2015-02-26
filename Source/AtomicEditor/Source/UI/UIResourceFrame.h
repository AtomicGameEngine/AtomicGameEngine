// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Container/HashMap.h>
#include "AEWidget.h"
#include <TurboBadger/tb_tab_container.h>
#include <TurboBadger/tb_style_edit.h>

using namespace Atomic;

namespace tb
{
class TBLayout;
}

namespace AtomicEditor
{

class ResourceEditor;
class IssuesWidget;
class ErrorsWidget;
class ConsoleWidget;

class ResourceFrame : public AEWidget
{
    OBJECT(ResourceFrame);

public:
    /// Construct.
    ResourceFrame(Context* context);
    /// Destruct.
    virtual ~ResourceFrame();

    void EditResource(const String& fullpath);
    void NavigateToResource(const String& fullpath, int lineNumber = -1, int tokenPos = -1);

    void CloseResourceEditor(ResourceEditor* editor, bool navigateToAvailableResource = true);
    void CloseAllResourceEditors();

    void FocusCurrentTab();

    bool HasUnsavedModifications();

    bool IssuesWidgetVisible();
    void ShowIssuesWidget(bool show = true);

    bool ErrorsWidgetVisible();
    void ShowErrorsWidget(bool show = true);

    bool ConsoleWidgetVisible();
    void ShowConsoleWidget(bool show = true);

    bool OnEvent(const TBWidgetEvent &ev);

    void SendCurrentEditorEvent(const TBWidgetEvent &ev);

private:

    void HandleFindText(StringHash eventType, VariantMap& eventData);
    void HandleFindTextClose(StringHash eventType, VariantMap& eventData);
    void HandlePlayStarted(StringHash eventType, VariantMap& eventData);
    void HandlePlayStopped(StringHash eventType, VariantMap& eventData);

    HashMap<String, SharedPtr<ResourceEditor> > editors_;
    HashMap<TBWidget*, SharedPtr<ResourceEditor> > editorLookup_;

    WeakPtr<ResourceEditor> currentResourceEditor_;

    SharedPtr<IssuesWidget> issueswidget_;
    SharedPtr<ErrorsWidget> errorswidget_;
    SharedPtr<ConsoleWidget> consolewidget_;    

    TBTabContainer* tabcontainer_;
    TBLayout* resourceLayout_;

};

}
