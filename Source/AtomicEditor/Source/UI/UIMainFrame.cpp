// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/UI/TBUI.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Core/Context.h>
#include <Atomic/Input/Input.h>
#include <Atomic/Input/InputEvents.h>
#include <Atomic/Graphics/Graphics.h>
#include <Atomic/Graphics/GraphicsEvents.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>
#include "UIMainFrame.h"
#include "UIMainToolbar.h"
#include "UIProjectFrame.h"
#include "UIPlayerWidget.h"
#include "UIResourceFrame.h"
#include "UIWelcomeFrame.h"
#include "UI/Modal/UIModalOps.h"
#include "UI/Modal/UIMessageModal.h"

#include "License/AEVersionCheck.h"

#include "UIFindTextWidget.h"
#include "../AEEvents.h"
#include "../AEEditor.h"
#include "../AEEditorStrings.h"
#include "../AEPreferences.h"
#include "../AEJavascript.h"
#include "../Player/AEPlayer.h"
#include "../Project/AEProject.h"
#include "../Project/ProjectUtils.h"

#include <TurboBadger/tb_message_window.h>
#include <TurboBadger/tb_editfield.h>

using namespace tb;

namespace AtomicEditor
{

MainFrame::MainFrame(Context* context) :
    AEWidget(context),
    consoletext_(0),
    resourceviewcontainer_(0),
    platformIndicator_(0)
{
    context->RegisterSubsystem(this);

    // Instead of TBGenericStringItem here, we need custom item so can have shortcuts be right aligned

    menuAtomicEditorSource.AddItem(new MenubarItem("About Atomic Editor", TBIDC("about atomic editor")));
    menuAtomicEditorSource.AddItem(new MenubarItem("-"));
    menuAtomicEditorSource.AddItem(new MenubarItem("Manage License", TBIDC("manage license")));
    menuAtomicEditorSource.AddItem(new MenubarItem("-"));
    menuAtomicEditorSource.AddItem(new MenubarItem("Check for Updates", TBIDC("check update")));
    menuAtomicEditorSource.AddItem(new MenubarItem("Quit", TBIDC("quit")));

    menuFileSource.AddItem(new MenubarItem("New Project", TBIDC("new project")));
    menuFileSource.AddItem(new MenubarItem("Open Project", TBIDC("open project")));
    menuFileSource.AddItem(new MenubarItem("Save Project", TBIDC("save project")));
    menuFileSource.AddItem(new MenubarItem("-"));
    menuFileSource.AddItem(new MenubarItem("Close Project", TBIDC("close project")));
    menuFileSource.AddItem(new MenubarItem("-"));
    menuFileSource.AddItem(new MenubarItem("Save File", TBIDC("save file"), EDITOR_STRING(ShortcutSaveFile)));
    menuFileSource.AddItem(new MenubarItem("Close File", TBIDC("close file"), EDITOR_STRING(ShortcutCloseFile)));

    menuBuildSource.AddItem(new MenubarItem("Build", TBIDC("project_build"), EDITOR_STRING(ShortcutBuild)));
    menuBuildSource.AddItem(new MenubarItem("-"));
    menuBuildSource.AddItem(new MenubarItem("Build Settings", TBIDC("project_build_settings"), EDITOR_STRING(ShortcutBuildSettings)));

    menuToolsSource.AddItem(new MenubarItem("Tiled Map Editor", TBIDC("tools tiled")));

    menuEditSource.AddItem(new MenubarItem("Undo", TBIDC("edit undo"),  EDITOR_STRING(ShortcutUndo)));
    menuEditSource.AddItem(new MenubarItem("Redo", TBIDC("edit redo"),  EDITOR_STRING(ShortcutRedo)));
    menuEditSource.AddItem(new MenubarItem("-"));
    menuEditSource.AddItem(new MenubarItem("Cut", TBIDC("edit cut"),  EDITOR_STRING(ShortcutCut)));
    menuEditSource.AddItem(new MenubarItem("Copy", TBIDC("edit copy"),  EDITOR_STRING(ShortcutCopy)));
    menuEditSource.AddItem(new MenubarItem("Paste", TBIDC("edit paste"),  EDITOR_STRING(ShortcutPaste)));
    menuEditSource.AddItem(new MenubarItem("-"));
    menuEditSource.AddItem(new MenubarItem("Find", TBIDC("edit find"),  EDITOR_STRING(ShortcutFind)));
    menuEditSource.AddItem(new MenubarItem("Format Code", TBIDC("format code"),  EDITOR_STRING(ShortcutBeautify)));
    menuEditSource.AddItem(new MenubarItem("-"));
    menuEditSource.AddItem(new MenubarItem("Play", TBIDC("play"),  EDITOR_STRING(ShortcutPlay)));

    menuResourcesSource.AddItem(new MenubarItem("Create", &menuResourcesCreateSource));
    menuResourcesSource.AddItem(new MenubarItem("-"));
    menuResourcesSource.AddItem(new MenubarItem("Reveal in Finder", TBIDC("reveal")));

    MenubarItem* item;
    item = new MenubarItem("Folder", TBIDC("create_folder"));
    item->SetSkinImage(TBIDC("Folder.icon"));
    menuResourcesCreateSource.AddItem(item);

    menuResourcesCreateSource.AddItem(new MenubarItem("-"));

    item = new MenubarItem("Component", TBIDC("create_component"));
    item->SetSkinImage(TBIDC("JavascriptBitmap"));
    menuResourcesCreateSource.AddItem(item);
    item = new MenubarItem("Script", TBIDC("create_script"));
    item->SetSkinImage(TBIDC("JavascriptBitmap"));
    menuResourcesCreateSource.AddItem(item);
    item = new MenubarItem("Module", TBIDC("create_module"));
    item->SetSkinImage(TBIDC("JavascriptBitmap"));
    menuResourcesCreateSource.AddItem(item);

    menuResourcesCreateSource.AddItem(new MenubarItem("-"));

    item = new MenubarItem("2D Level", TBIDC("create_2d_level"));
    item->SetSkinImage(TBIDC("2DLevelBitmap"));
    menuResourcesCreateSource.AddItem(item);

    menuHelpSource.AddItem(new MenubarItem("API Documentation", TBIDC("help_api")));
    menuHelpSource.AddItem(new MenubarItem("-"));
    menuHelpSource.AddItem(new MenubarItem("Forums", TBIDC("help_forums")));
    menuHelpSource.AddItem(new MenubarItem("-"));
    menuHelpSource.AddItem(new MenubarItem("Atomic Game Engine on GitHub", TBIDC("help_github")));

    menuDeveloperSource.AddItem(new MenubarItem("Set 1920x1080 Resolution", TBIDC("developer_resolution")));


    TBUI* tbui = GetSubsystem<TBUI>();
    tbui->LoadResourceFile(delegate_, "AtomicEditor/editor/ui/mainframe.tb.txt");

    Graphics* graphics = GetSubsystem<Graphics>();

    delegate_->SetSize(graphics->GetWidth(), graphics->GetHeight());
    tbui->GetRootWidget()->AddChild(delegate_);

    maintoolbar_ = new MainToolbar(context_);
    TBLayout* maintoolbarcontainer = delegate_->GetWidgetByIDAndType<TBLayout>(TBIDC("maintoolbarcontainer"));
    assert(maintoolbarcontainer);

    TBWidget* wd = maintoolbar_->GetWidgetDelegate();
    TBRect rect = maintoolbarcontainer->GetRect();
    wd->SetSize(rect.w, rect.h);
    maintoolbarcontainer->AddChild(wd);

    projectframe_ = new ProjectFrame(context_);
    TBLayout* projectviewcontainer = delegate_->GetWidgetByIDAndType<TBLayout>(TBIDC("projectviewcontainer"));
    assert(projectviewcontainer);

    // better way to do this? projectviewcontainer isn't a layout
    wd = projectframe_->GetWidgetDelegate();
    rect = projectviewcontainer->GetRect();
    wd->SetSize(rect.w, rect.h);
    projectviewcontainer->AddChild(wd);

    resourceviewcontainer_ = delegate_->GetWidgetByIDAndType<TBLayout>(TBIDC("resourceviewcontainer"));
    assert(resourceviewcontainer_);
    rect = resourceviewcontainer_->GetRect();

    // player widget
    playerwidget_ = new PlayerWidget(context_);
    wd = playerwidget_->GetWidgetDelegate();
    wd->SetSize(rect.w, rect.h);

    // welcome frame
    welcomeframe_ = new WelcomeFrame(context_);
    wd = welcomeframe_->GetWidgetDelegate();
    wd->SetSize(rect.w, rect.h);

    resourceframe_ = new ResourceFrame(context_);

    // better way to do this? projectviewcontainer isn't a layout
    wd = resourceframe_->GetWidgetDelegate();
    wd->SetSize(rect.w, rect.h);
    //resourceviewcontainer->AddChild(wd);

    platformIndicator_ = delegate_->GetWidgetByIDAndType<TBSkinImage>(TBIDC("current_platform_indicator"));
    assert(platformIndicator_);

    consoletext_ = delegate_->GetWidgetByIDAndType<TBEditField>(TBIDC("consoletext"));
    consoletext_->SetText("Atomic Editor Initialized");

    TBContainer* consolecontainer = delegate_->GetWidgetByIDAndType<TBContainer>(TBIDC("consolecontainer"));
    assert(consolecontainer);
    consolecontainer->SetVisibilility(WIDGET_VISIBILITY_INVISIBLE);
    consolecontainer->GetParent()->RemoveChild(consolecontainer);

    findtextwidget_ = new FindTextWidget(context_);
    UpdateFindTextWidget();

    SubscribeToEvent(E_KEYDOWN, HANDLER(MainFrame, HandleKeyDown));
    SubscribeToEvent(E_KEYUP, HANDLER(MainFrame, HandleKeyUp));
    SubscribeToEvent(E_SCREENMODE, HANDLER(MainFrame, HandleScreenMode));
    SubscribeToEvent(E_JAVASCRIPTSAVED, HANDLER(MainFrame, HandleJavascriptSaved));
    SubscribeToEvent(E_PLATFORMCHANGE, HANDLER(MainFrame, HandlePlatformChange));
    SubscribeToEvent(E_EDITORSHUTDOWN, HANDLER(MainFrame, HandleEditorShutdown));

    messageModal_ = new MessageModal(context_);
    uiModalOps_ = new UIModalOps(context_);

    // show welcome frame to start
    ShowWelcomeFrame();

}

MainFrame::~MainFrame()
{
}

ProjectFrame* MainFrame::GetProjectFrame()
{
    return projectframe_;
}

ResourceFrame* MainFrame::GetResourceFrame()
{
    return resourceframe_;
}

FindTextWidget* MainFrame::GetFindTextWidget()
{
    return findtextwidget_;
}

MainToolbar* MainFrame::GetMainToolbar()
{
    return maintoolbar_;
}

PlayerWidget* MainFrame::GetPlayerWidget()
{
    return playerwidget_;
}

MessageModal* MainFrame::GetMessageModal()
{
    return messageModal_;
}

UIModalOps* MainFrame::GetUIModalOps()
{
    return uiModalOps_;
}

void MainFrame::HandleJavascriptSaved(StringHash eventType, VariantMap& eventData)
{
    UpdateJavascriptErrors();
}

void MainFrame::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
    using namespace KeyDown;

    int keycode = eventData[P_KEY].GetInt();
    int scancode = eventData[P_SCANCODE].GetInt();

}

bool MainFrame::UpdateJavascriptErrors()
{
    // parse errors
    bool hasErrors = false;
    AEJavascript* aejs = GetSubsystem<AEJavascript>();
    aejs->CheckJSErrors();
    const Vector<JSError>& errors = aejs->GetJSErrors();
    if (errors.Size())
    {
        hasErrors = true;
        resourceframe_->ShowIssuesWidget();
    }
    else
    {
        resourceframe_->ShowIssuesWidget(false);
    }

    AEPlayer* player = GetSubsystem<AEPlayer>();
    if (player)
    {
        const Vector<AEPlayerError>& playererrors = player->GetErrors();
        if (playererrors.Size())
        {
            hasErrors = true;
            resourceframe_->ShowErrorsWidget();
        }
        else
            resourceframe_->ShowErrorsWidget(false);
    }


    return hasErrors;

}

void MainFrame::HandleKeyUp(StringHash eventType, VariantMap& eventData)
{
    using namespace KeyUp;

    Input* input = context_->GetSubsystem<Input>();
    Editor* editor = context_->GetSubsystem<Editor>();

#ifdef ATOMIC_PLATFORM_WINDOWS
    bool cmdKey = (input->GetKeyDown(KEY_LCTRL) || input->GetKeyDown(KEY_RCTRL));
#else
    bool cmdKey = (input->GetKeyDown(KEY_LGUI) || input->GetKeyDown(KEY_RGUI));
#endif

    int keycode = eventData[P_KEY].GetInt();

    if (cmdKey && keycode == KEY_P)
    {
        if (!editor->IsPlayingProject())
        {
            VariantMap eventData;
            eventData[EditorPlayRequest::P_MODE] = (unsigned) AE_PLAYERMODE_WIDGET;
            SendEvent(E_EDITORPLAYREQUEST, eventData);
        }
        else
        {
            SendEvent(E_EDITORPLAYSTOP);
        }
    }

}

bool MainFrame::ResourceFrameVisible()
{
    TBWidget *child;
    for (child = resourceviewcontainer_->GetFirstChild(); child; child = child->GetNext())
    {
        if (child == resourceframe_->GetWidgetDelegate())
            break;
    }

    return child != NULL;
}

bool MainFrame::WelcomeFrameVisible()
{
    TBWidget *child;
    for (child = resourceviewcontainer_->GetFirstChild(); child; child = child->GetNext())
    {
        if (child == welcomeframe_->GetWidgetDelegate())
            break;
    }

    return child != NULL;
}

void MainFrame::ShowResourceFrame(bool show)
{
    if (show && ResourceFrameVisible())
        return;

    if (show && WelcomeFrameVisible())
        ShowWelcomeFrame(false);

    TBWidget *child;
    for (child = resourceviewcontainer_->GetFirstChild(); child; child = child->GetNext())
    {
        if (child == resourceframe_->GetWidgetDelegate())
            break;
    }

    if (show)
    {
        if (!child)
        {
            resourceviewcontainer_->AddChild(resourceframe_->GetWidgetDelegate());
        }

    }
    else
    {
        if (child)
            resourceviewcontainer_->RemoveChild(child);
    }


}

void MainFrame::ShowWelcomeFrame(bool show)
{
    if (show && WelcomeFrameVisible())
        return;

    if (show && ResourceFrameVisible())
        ShowResourceFrame(false);

    TBWidget *child;
    for (child = resourceviewcontainer_->GetFirstChild(); child; child = child->GetNext())
    {
        if (child == welcomeframe_->GetWidgetDelegate())
            break;
    }

    if (show)
    {
        welcomeframe_->UpdateRecentProjects();
        if (!child)
        {
            resourceviewcontainer_->AddChild(welcomeframe_->GetWidgetDelegate());
        }

    }
    else
    {
        if (child)
            resourceviewcontainer_->RemoveChild(child);
    }

}

bool MainFrame::IsProjectLoaded()
{
    Editor* editor = GetSubsystem<Editor>();
    Project* project = editor->GetProject();

    if (!project)
        return false;

    return true;

}


bool MainFrame::OnEvent(const TBWidgetEvent &ev)
{
    Editor* editor = GetSubsystem<Editor>();

    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.target->GetID() == TBIDC("file popup"))
        {
            if (ev.ref_id == TBIDC("new project"))
            {
                if (editor->IsProjectLoaded())
                {
                    editor->PostModalError("Close Project", "Please close the current project before creating a new one");
                }
                else
                {
                    uiModalOps_->ShowNewProject();
                }
            }
            else if (ev.ref_id == TBIDC("open project"))
            {
                if (editor->IsProjectLoaded())
                {
                    editor->PostModalError("Close Project", "Please close the current project before opening a new one");
                }
                else
                {
                    GetSubsystem<ProjectUtils>()->OpenProjectFileDialog();
                    return true;
                }
            }

            else if (ev.ref_id == TBIDC("close project"))
            {
                //TODO: Confirmation
                editor->CloseProject();
            }

            return true;
        }

        if (ev.target->GetID() == TBIDC("atomic editor popup"))
        {
            if (ev.ref_id == TBIDC("about atomic editor"))
            {
                uiModalOps_->ShowAbout();
                return true;
            }
            else if (ev.ref_id == TBIDC("manage license"))
            {
                uiModalOps_->ShowManageLicense();
                return true;
            }
            else if (ev.ref_id == TBIDC("check update"))
            {
                GetSubsystem<VersionCheck>()->DoVersionCheck();
                return true;
            }
            else if (ev.ref_id == TBIDC("quit"))
            {
                // TODO: confirmation

                // shenanigens, need to delete the popup on quit
                // otherwise it is still live with a source at exit
                ev.target->GetParent()->RemoveChild(ev.target);
                delete ev.target;
                TBWidgetEvent* eptr = (TBWidgetEvent*) &ev;
                eptr->target = NULL;
                editor->RequestExit();
                return true;
            }

        }

        if (ev.target->GetID() == TBIDC("build popup"))
        {
            if (!IsProjectLoaded())
                return true;

            if (ev.ref_id == TBIDC("project_build_settings"))
            {
                uiModalOps_->ShowBuildSettings();
            }
            else if (ev.ref_id == TBIDC("project_build"))
            {
                uiModalOps_->ShowBuild();
            }

            return true;
        }


        if (ev.target->GetID() == TBIDC("edit popup"))
        {
            if (ev.ref_id == TBIDC("format code"))
            {

            }
        }

        if (ev.target->GetID() == TBIDC("resources popup"))
        {
            UIModalOps* ops = GetSubsystem<UIModalOps>();

            String resourcePath = projectframe_->GetCurrentContentFolder();

            if (resourcePath.Length())
            {
                if (ev.ref_id == TBIDC("create_component"))
                {
                    ops->ShowCreateComponent(resourcePath);
                }
                else if (ev.ref_id == TBIDC("create_script"))
                {
                    ops->ShowCreateScript(resourcePath);
                }
                else if (ev.ref_id == TBIDC("create_module"))
                {
                    ops->ShowCreateModule(resourcePath);
                }
                else if (ev.ref_id == TBIDC("create_2d_level"))
                {
                    ops->ShowCreate2DLevel(resourcePath);
                }
                else if (ev.ref_id == TBIDC("create_folder"))
                {
                    ops->ShowNewFolder(resourcePath);
                }
                else if (ev.ref_id == TBIDC("reveal"))
                {
                    RevealInFinder();
                }
            }

            return true;
        }

        if (ev.target->GetID() == TBIDC("help popup"))
        {
            FileSystem* fileSystem = GetSubsystem<FileSystem>();

            if (ev.ref_id == TBIDC("help_forums"))
            {

                fileSystem->SystemOpen("http://www.atomicgameengine.com/forum");

            }
            if (ev.ref_id == TBIDC("help_github"))
            {

                fileSystem->SystemOpen("https://github.com/AtomicGameEngine/AtomicGameEngine");

            }
            else if (ev.ref_id == TBIDC("help_api"))
            {
#ifdef ATOMIC_PLATFORM_OSX
                String docSourceDir = fileSystem->GetAppBundleResourceFolder();
#else
                String docSourceDir = fileSystem->GetProgramDir();
#endif

                docSourceDir += "Docs/index.html";

                fileSystem->SystemOpen("file://" + docSourceDir);
            }

            return true;

        }

        if (ev.target->GetID() == TBIDC("developer popup"))
        {
            if (ev.ref_id == TBIDC("developer_resolution"))
            {
                Graphics* graphics = GetSubsystem<Graphics>();
                graphics->SetWindowSize(1920, 1080);
                graphics->CenterWindow();
            }
        }


        if (ev.target->GetID() == TBIDC("menu atomic editor"))
        {
            if (TBMenuWindow *menu = new TBMenuWindow(ev.target, TBIDC("atomic editor popup")))
            {
                menu->Show(&menuAtomicEditorSource, TBPopupAlignment());
            }

            return true;
        }
        else if (ev.target->GetID() == TBIDC("menu edit"))
        {
            if (TBMenuWindow *menu = new TBMenuWindow(ev.target, TBIDC("edit popup")))
                menu->Show(&menuEditSource, TBPopupAlignment());

            return true;
        }
        else if (ev.target->GetID() == TBIDC("menu file"))
        {
            if (TBMenuWindow *menu = new TBMenuWindow(ev.target, TBIDC("file popup")))
                menu->Show(&menuFileSource, TBPopupAlignment());

            return true;
        }
        else if (ev.target->GetID() == TBIDC("menu resources"))
        {
            if (TBMenuWindow *menu = new TBMenuWindow(ev.target, TBIDC("resources popup")))
                menu->Show(&menuResourcesSource, TBPopupAlignment());

            return true;
        }
        else if (ev.target->GetID() == TBIDC("menu build"))
        {
            if (TBMenuWindow *menu = new TBMenuWindow(ev.target, TBIDC("build popup")))
                menu->Show(&menuBuildSource, TBPopupAlignment());

            return true;
        }
        else if (ev.target->GetID() == TBIDC("menu tools"))
        {
            if (TBMenuWindow *menu = new TBMenuWindow(ev.target, TBIDC("tools popup")))
                menu->Show(&menuToolsSource, TBPopupAlignment());

            return true;
        }
        else if (ev.target->GetID() == TBIDC("menu issues"))
        {
            resourceframe_->ShowIssuesWidget(!resourceframe_->IssuesWidgetVisible());
            return true;
        }
        else if (ev.target->GetID() == TBIDC("menu console"))
        {
            resourceframe_->ShowConsoleWidget(!resourceframe_->ConsoleWidgetVisible());
            return true;
        }
        else if (ev.target->GetID() == TBIDC("menu errors"))
        {
            resourceframe_->ShowErrorsWidget(!resourceframe_->ErrorsWidgetVisible());
            return true;
        }
        else if (ev.target->GetID() == TBIDC("unsaved_modifications_dialog"))
        {
            resourceframe_->FocusCurrentTab();
            return true;
        }
        else if (ev.target->GetID() == TBIDC("menu help"))
        {
            if (TBMenuWindow *menu = new TBMenuWindow(ev.target, TBIDC("help popup")))
                menu->Show(&menuHelpSource, TBPopupAlignment());

            return true;
        }
        else if (ev.target->GetID() == TBIDC("menu developer"))
        {
            if (TBMenuWindow *menu = new TBMenuWindow(ev.target, TBIDC("developer popup")))
                menu->Show(&menuDeveloperSource, TBPopupAlignment());

            return true;
        }


    }

    return false;

}

void MainFrame::UpdateFindTextWidget()
{
    TBRect rect = delegate_->GetRect();
    findtextwidget_->SetUISize(rect.w, rect.h);
}

void MainFrame::HandleScreenMode(StringHash eventType, VariantMap& eventData)
{
    using namespace ScreenMode;
    int width = eventData[P_WIDTH].GetInt();
    int height = eventData[P_HEIGHT].GetInt();

    delegate_->SetSize(width, height);
    UpdateFindTextWidget();
}

void MainFrame::RevealInFinder()
{
    Editor* editor = context_->GetSubsystem<Editor>();
    Project* project = editor->GetProject();

    if (!project)
        return;

    String resourcePath = projectframe_->GetCurrentContentFolder();

    if (!resourcePath.Length())
        resourcePath = project->GetResourcePath();

    ProjectUtils* utils = context_->GetSubsystem<ProjectUtils>();
    utils->RevealInFinder(resourcePath);

}

void MainFrame::HandlePlatformChange(StringHash eventType, VariantMap& eventData)
{
    using namespace PlatformChange;
    AEEditorPlatform platform = (AEEditorPlatform) eventData[P_PLATFORM].GetUInt();
    if (platform == AE_PLATFORM_MAC)
        platformIndicator_->SetSkinBg(TBIDC("LogoMac"));
    else if (platform == AE_PLATFORM_WINDOWS)
        platformIndicator_->SetSkinBg(TBIDC("LogoWindows"));
    else if (platform == AE_PLATFORM_ANDROID)
        platformIndicator_->SetSkinBg(TBIDC("LogoAndroid"));
    else if (platform == AE_PLATFORM_HTML5)
        platformIndicator_->SetSkinBg(TBIDC("LogoHTML5"));
    else if (platform == AE_PLATFORM_IOS)
        platformIndicator_->SetSkinBg(TBIDC("LogoIOS"));

}

void MainFrame::HandleEditorShutdown(StringHash eventType, VariantMap& eventData)
{
    context_->RemoveSubsystem(GetType());
}

}
