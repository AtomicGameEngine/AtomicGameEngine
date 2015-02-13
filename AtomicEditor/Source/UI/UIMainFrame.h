// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicEditor

#pragma once

#include "AEWidget.h"

#include <TurboBadger/tb_menu_window.h>

using namespace Atomic;

namespace tb
{

class TBEditField;
class TBLayout;

}

namespace AtomicEditor
{

class ProjectFrame;
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

    TBGenericStringItemSource menuAtomicEditorSource;
    TBGenericStringItemSource menuFileSource;
    TBGenericStringItemSource menuEditSource;
    TBGenericStringItemSource menuProjectSource;
    TBGenericStringItemSource menuResourcesSource;
    TBGenericStringItemSource menuResourcesCreateSource;
    TBGenericStringItemSource menuHelpSource;

public:
    /// Construct.
    MainFrame(Context* context);
    /// Destruct.
    virtual ~MainFrame();

    bool OnEvent(const TBWidgetEvent &ev);

    ProjectFrame* GetProjectFrame();
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

    // reveals the current project folder in finder/explorer
    void RevealInFinder();
    void ShowResourceCreateWindow(const String& resourceType);

    PlayerWidget* GetPlayerWidget();

    bool UpdateJavascriptErrors();

    bool IsProjectLoaded();

private:

    void UpdateFindTextWidget();

    void HandleKeyDown(StringHash eventType, VariantMap& eventData);
    void HandleKeyUp(StringHash eventType, VariantMap& eventData);
    void HandleScreenMode(StringHash eventType, VariantMap& eventData);
    void HandleJavascriptSaved(StringHash eventType, VariantMap& eventData);
    void HandlePlatformChange(StringHash eventType, VariantMap& eventData);

    TBEditField* consoletext_;
    TBLayout* resourceviewcontainer_;
    TBSkinImage* platformIndicator_;

    SharedPtr<ProjectFrame> projectframe_;
    SharedPtr<ResourceFrame> resourceframe_;
    SharedPtr<FindTextWidget> findtextwidget_;
    SharedPtr<MainToolbar> maintoolbar_;
    SharedPtr<WelcomeFrame> welcomeframe_;
    SharedPtr<PlayerWidget> playerwidget_;

    SharedPtr<MessageModal> messageModal_;
    SharedPtr<UIModalOps> uiModalOps_;



};

}
