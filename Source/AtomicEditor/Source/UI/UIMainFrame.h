// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include "AEWidget.h"
#include "UIMenubar.h"

#include <TurboBadger/tb_menu_window.h>

using namespace Atomic;

namespace tb
{

class TBEditField;
class TBLayout;
class TBMenuWindow;

}

namespace AtomicEditor
{

class ProjectFrame;
class HierarchyFrame;
class InspectorFrame;
class ResourceFrame;
class FindTextWidget;
class MainToolbar;
class PlayerWidget;
class WelcomeFrame;
class MessageModal;
class UIModalOps;

class MainFrame : public AEWidget
{
    OBJECT(MainFrame);

    MenubarItemSource menuAtomicEditorSource;
    MenubarItemSource menuFileSource;
    MenubarItemSource menuEditSource;
    MenubarItemSource menuBuildSource;

    MenubarItemSource menuHelpSource;
    MenubarItemSource menuDeveloperSource;

    MenubarItemSource menuToolsSource;

public:
    /// Construct.
    MainFrame(Context* context);
    /// Destruct.
    virtual ~MainFrame();

    bool OnEvent(const TBWidgetEvent &ev);

    ProjectFrame* GetProjectFrame();
    HierarchyFrame* GetHierarchyFrame();
    InspectorFrame* GetInspectorFrame();
    ResourceFrame* GetResourceFrame();
    WelcomeFrame* GetWelcomeFrame();
    FindTextWidget* GetFindTextWidget();
    MainToolbar* GetMainToolbar();
    MessageModal* GetMessageModal();
    UIModalOps* GetUIModalOps();

    // content views
    void ShowResourceFrame(bool show = true);
    void ShowWelcomeFrame(bool show = true);    

    bool ResourceFrameVisible();
    bool WelcomeFrameVisible();
    bool InspectorFrameVisible();


    void ShowInspectorFrame(bool show);

    // reveals the current project folder in finder/explorer
    void RevealInFinder();

    PlayerWidget* GetPlayerWidget();

    bool UpdateJavascriptErrors();

    bool IsProjectLoaded();

private:

    bool HandleMenubarEvent(const TBWidgetEvent &ev);
    bool HandlePopupMenuEvent(const TBWidgetEvent &ev);
    void InitializeMenuSources();

    void UpdateFindTextWidget();

    void HandleScreenMode(StringHash eventType, VariantMap& eventData);
    void HandleJavascriptSaved(StringHash eventType, VariantMap& eventData);
    void HandlePlatformChange(StringHash eventType, VariantMap& eventData);
    void HandleEditorShutdown(StringHash eventType, VariantMap& eventData);
    void HandleResourceEditorChanged(StringHash eventType, VariantMap& eventData);

    TBEditField* consoletext_;
    TBLayout* resourceviewcontainer_;
    TBLayout* inspectorlayout_;
    TBSkinImage* platformIndicator_;

    SharedPtr<ProjectFrame> projectframe_;
    SharedPtr<HierarchyFrame> hierarchyframe_;
    SharedPtr<InspectorFrame> inspectorframe_;
    SharedPtr<ResourceFrame> resourceframe_;
    SharedPtr<FindTextWidget> findtextwidget_;
    SharedPtr<MainToolbar> maintoolbar_;
    SharedPtr<WelcomeFrame> welcomeframe_;
    SharedPtr<PlayerWidget> playerwidget_;

    SharedPtr<MessageModal> messageModal_;
    SharedPtr<UIModalOps> uiModalOps_;



};

}
