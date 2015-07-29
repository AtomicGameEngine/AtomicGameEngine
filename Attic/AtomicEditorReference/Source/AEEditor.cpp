// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <Atomic/Engine/Engine.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Input/Input.h>
#include <Atomic/UI/UI.h>
#include <Atomic/Core/CoreEvents.h>

#include <AtomicJS/Javascript/Javascript.h>

#include "AEEditor.h"

#include "AEPreferences.h"
#include "Project/AEProject.h"
#include "AEJavascript.h"
#include "AEEvents.h"
#include "Player/AEPlayer.h"

#ifdef USE_SPIDERMONKEY
#include "Javascript/JSSpiderMonkeyVM.h"
#endif

#include "UI/UIMainFrame.h"
#include "UI/UIProjectFrame.h"
#include "UI/UIResourceFrame.h"
#include "UI/Modal/UIMessageModal.h"

#include "License/AELicenseSystem.h"

#include "Resources/AEResourceOps.h"

#include <TurboBadger/tb_message_window.h>
#include <TurboBadger/animation/tb_animation.h>

namespace AtomicEditor
{

Editor::Editor(Context* context) :
    Object(context),
    currentPlatform_(AE_PLATFORM_UNDEFINED),
    requestExit_(false)

{
    RegisterEditorLibrary(context_);

    aejavascript_ = new AEJavascript(context_);
    aepreferences_ = new AEPreferences(context_);

#ifdef USE_SPIDERMONKEY
    spidermonkey_ = new JSSpiderMonkeyVM(context_);
#endif

    resourceCreator_ = new ResourceOps(context_);

    // Create the Main Editor Frame
    mainframe_ = new MainFrame(context_);

    SubscribeToEvent(E_EXITREQUESTED, HANDLER(Editor, HandleExitRequested));

    SubscribeToEvent(E_PLAYERERROR, HANDLER(Editor, HandlePlayerError));
    SubscribeToEvent(E_POSTUPDATE, HANDLER(Editor, HandlePostUpdate));

    // the player handling might move
    SubscribeToEvent(E_EDITORPLAYREQUEST, HANDLER(Editor, HandlePlayRequest));
    SubscribeToEvent(E_EDITORPLAYSTOP, HANDLER(Editor, HandlePlayStop));
    SubscribeToEvent(E_EDITORPLAYSTARTED, HANDLER(Editor, HandlePlayStarted));
    SubscribeToEvent(E_EDITORPLAYSTOPPED, HANDLER(Editor, HandlePlayStopped));    

// BEGIN LICENSE MANAGEMENT
    GetSubsystem<LicenseSystem>()->Initialize();
// END LICENSE MANAGEMENT


}

Editor::~Editor()
{

}

MainFrame* Editor::GetMainFrame()
{
    return mainframe_;
}

Project* Editor::GetProject()
{
    return project_;
}

AEPreferences* Editor::GetPreferences()
{
    return aepreferences_;
}

void Editor::EditResource(const String& fullpath)
{
    mainframe_->GetResourceFrame()->EditResource(fullpath);
}

void Editor::LoadProject(const String& fullpath)
{
    aepreferences_->RegisterRecentProject(fullpath);

    String path = GetPath(fullpath);

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    cache->AddResourceDir(path, 0);

    String resourcePath = path;
    resourcePath += "Resources";
    cache->AddResourceDir(resourcePath, 0);

    project_ = new Project(context_);
    project_->SetResourcePath(resourcePath);
    project_->Load(fullpath);

    mainframe_->ShowResourceFrame();
    mainframe_->GetProjectFrame()->RefreshFolders();
    mainframe_->UpdateJavascriptErrors();
}

void Editor::CloseProject()
{
    if (project_.Null())
        return;

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    String projectPath = project_->GetProjectFilePath();
    String resourcePath = project_->GetResourcePath();

    mainframe_->GetProjectFrame()->Clear();
    mainframe_->GetResourceFrame()->CloseAllResourceEditors();

    project_ = 0;

    cache->RemoveResourceDir(resourcePath);
    cache->RemoveResourceDir(projectPath);
    cache->ReleaseAllResources(true);

    mainframe_->ShowWelcomeFrame();

}

void Editor::SaveProject()
{
    if (project_.Null())
        return;

    project_->Save(project_->GetProjectFilePath());

}

// Project Playing

void Editor::HandlePlayStarted(StringHash eventType, VariantMap& eventData)
{
}

void Editor::HandlePlayStop(StringHash eventType, VariantMap& eventData)
{    
    SendEvent(E_EDITORPLAYSTOPPED);

    if (!player_)
        return;

    //UI* tbui = GetSubsystem<UI>();
    //tbui->SetKeyboardDisabled(false);
    if (player_->GetMode() != AE_PLAYERMODE_WIDGET)
    {
        //tbui->SetInputDisabled(false);
    }

    Input* input = GetSubsystem<Input>();
    input->SetMouseVisible(true);

    mainframe_->UpdateJavascriptErrors();

    player_->Invalidate();
    player_ = NULL;

}

void Editor::HandlePlayStopped(StringHash eventType, VariantMap& eventData)
{

}

void Editor::HandlePlayerError(StringHash eventType, VariantMap& eventData)
{    

}

void Editor::PostModalError(const String& title, const String& message)
{
    using namespace EditorModal;
    VariantMap eventData;
    eventData[P_TYPE] = EDITOR_MODALERROR;
    eventData[P_TITLE] = title;
    eventData[P_MESSAGE] = message;
    SendEvent(E_EDITORMODAL, eventData);

}

void Editor::PostModalInfo(const String& title, const String& message)
{
    using namespace EditorModal;
    VariantMap eventData;
    eventData[P_TYPE] = EDITOR_MODALINFO;
    eventData[P_TITLE] = title;
    eventData[P_MESSAGE] = message;
    SendEvent(E_EDITORMODAL, eventData);

}

void Editor::RequestPlatformChange(AEEditorPlatform platform)
{
// BEGIN LICENSE MANAGEMENT
    LicenseSystem* licenseSystem = GetSubsystem<LicenseSystem>();

    if (!licenseSystem->RequestPlatformChange(platform))
    {
        PostModalInfo("Platform License Required", "Platform License is required to switch to this deployment");
        return;
    }

    if (currentPlatform_ == platform)
        return;

    // if we can switch platforms via some other event, may want to do this in a handler
    currentPlatform_ = platform;

    if (!project_.Null())
        project_->Save(project_->GetProjectFilePath());

    using namespace PlatformChange;
    VariantMap eventData;
    eventData[P_PLATFORM] = (unsigned) platform;
    SendEvent(E_PLATFORMCHANGE, eventData);

// END LICENSE MANAGEMENT

}

void Editor::HandlePlayRequest(StringHash eventType, VariantMap& eventData)
{
    if (player_ || project_.Null())
        return;

    ResourceFrame* resourceFrame = mainframe_->GetResourceFrame();

    if (resourceFrame->HasUnsavedModifications())
    {
        PostModalError("Unsaved Modifications", "There are unsaved modications.\nPlease save before entering play mode");
        return;

    }
    else if (mainframe_->UpdateJavascriptErrors())
    {
        return;
    }

    assert(!player_);

    AEPlayerMode mode = (AEPlayerMode) eventData[EditorPlayStarted::P_MODE].GetUInt();

    if (mode != AE_PLAYERMODE_WIDGET)
    {
        //tbui->SetInputDisabled(true);
    }

    player_ = new AEPlayer(context_);

    TBWidgetDelegate* tb = mainframe_->GetResourceFrame()->GetWidgetDelegate();
    TBRect rect = tb->GetRect();
    tb->ConvertToRoot(rect.x, rect.y);

    if (!player_->Play(mode, IntRect(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h)))
    {
        player_->Invalidate();
        player_ = 0;
        return;
    }

    SendEvent(E_EDITORPLAYSTARTED, eventData);

}

void Editor::HandlePostUpdate(StringHash eventType, VariantMap& eventData)
{
    if (player_ && player_->HasErrors())
    {
        SendEvent(E_EDITORPLAYSTOP);
    }

    if (requestExit_)
    {
        requestExit_ = false;
        SendEvent(E_EXITREQUESTED);
    }

}

void Editor::HandleExitRequested(StringHash eventType, VariantMap& eventData)
{
    if (aepreferences_.NotNull())
    {
        aepreferences_->Write();
    }

    mainframe_ = 0;
    player_ = 0;
    project_ = 0;
    javascript_ = 0;
    aejavascript_ = 0;
    aepreferences_ = 0;

    TBAnimationManager::BeginBlockAnimations();

    UI* tbui = GetSubsystem<UI>();
    tbui->Shutdown();

    context_->RemoveSubsystem(Javascript::GetBaseTypeStatic());

    SendEvent(E_EDITORSHUTDOWN);

    SharedPtr<Editor> keepAlive(this);
    context_->RemoveSubsystem(GetType());

    Engine* engine = GetSubsystem<Engine>();
    engine->Exit();
}


void RegisterEditorLibrary(Context* context)
{

}

}
