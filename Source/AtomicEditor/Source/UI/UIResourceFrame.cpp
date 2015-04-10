// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <Atomic/Container/ArrayPtr.h>
#include <Atomic/UI/UI.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Core/CoreEvents.h>
#include <AtomicJS/Javascript/JSEvents.h>

#include "UIResourceFrame.h"

#include "../Editors/JSResourceEditor.h"
#include "../Editors/SceneEditor3D/SceneEditor3D.h"
#include "../Editors/ModelResourceEditor.h"
#include "../Editors/TextResourceEditor.h"
#include "../AEEvents.h"
#include "../AEEditor.h"

#include "UIIssuesWidget.h"
#include "UIErrorsWidget.h"
#include "UIConsoleWidget.h"

#include "License/AELicenseSystem.h"

#include "Modal/UIModalOps.h"

#include "../Tools/External/AEExternalTooling.h"

using namespace tb;

namespace AtomicEditor
{

ResourceFrame::ResourceFrame(Context* context) :
    AEWidget(context),
    tabcontainer_(0),
    resourceLayout_(0)
{
    UI* tbui = GetSubsystem<UI>();
    tbui->LoadResourceFile(delegate_, "AtomicEditor/editor/ui/resourceframe.tb.txt");

    tabcontainer_  = delegate_->GetWidgetByIDAndType<TBTabContainer>(TBIDC("tabcontainer"));
    assert(tabcontainer_);

    resourceLayout_  = delegate_->GetWidgetByIDAndType<TBLayout>(TBIDC("resourcelayout"));
    assert(resourceLayout_);

    delegate_->SetGravity(WIDGET_GRAVITY_ALL);

    issueswidget_ = new IssuesWidget(context_);
    errorswidget_ = new ErrorsWidget(context_);
    consolewidget_ = new ConsoleWidget(context_);

    SubscribeToEvent(E_FINDTEXT, HANDLER(ResourceFrame, HandleFindText));
    SubscribeToEvent(E_FINDTEXTCLOSE, HANDLER(ResourceFrame, HandleFindTextClose));

    SubscribeToEvent(E_EDITORPLAYSTARTED, HANDLER(ResourceFrame, HandlePlayStarted));
    SubscribeToEvent(E_EDITORPLAYSTOPPED, HANDLER(ResourceFrame, HandlePlayStopped));

}

ResourceFrame::~ResourceFrame()
{
}

void ResourceFrame::HandleFindText(StringHash eventType, VariantMap& eventData)
{
    using namespace FindText;

    if (!editors_.Size())
        return;

    int page = tabcontainer_->GetCurrentPage();

    TBWidget* widget = tabcontainer_->GetCurrentPageWidget();
    if (editorLookup_.Contains(widget))
        editorLookup_[widget]->FindText(eventData[P_TEXT].ToString(),
                                        (unsigned) eventData[P_FLAGS].GetInt());
}

void ResourceFrame::HandleFindTextClose(StringHash eventType, VariantMap& eventData)
{
    if (!editors_.Size())
        return;

    TBWidget* widget = tabcontainer_->GetCurrentPageWidget();
    if (editorLookup_.Contains(widget))
        return editorLookup_[widget]->FindTextClose();

}

void ResourceFrame::EditResource(const String& fullpath)
{
    if (editors_.Contains(fullpath))
    {
        NavigateToResource(fullpath);

        if (GetSubsystem<Editor>()->IsPlayingProject())
        {
            SendEvent(E_EDITORPLAYSTOP);
        }

        return;
    }

    delegate_->SetVisibilility(WIDGET_VISIBILITY_VISIBLE);

    String ext = GetExtension(fullpath);

    ResourceEditor* editor = NULL;

    if (ext == ".js")
    {
        JSResourceEditor* jse = new JSResourceEditor(context_, fullpath, tabcontainer_);
        editor = jse;
    }
    else if (ext == ".scene")
    {
        SceneEditor3D* sre = new SceneEditor3D(context_, fullpath, tabcontainer_);
        editor = sre;
    }
    else if (ext == ".xml" || ext == ".txt")
    {
        //SceneResourceEditor* sre = new SceneResourceEditor(context_, fullpath, tabcontainer_);
        TextResourceEditor* tre = new TextResourceEditor(context_, fullpath, tabcontainer_);
        editor = tre;
    }
    else if (ext == ".mdl")
    {
        //ModelResourceEditor* mre = new ModelResourceEditor(context_, fullpath, tabcontainer_);
        //editor = mre;
    }
    else if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".gif" || ext == ".ogg")
    {
        FileSystem* fs = GetSubsystem<FileSystem>();
        fs->SystemOpen(fullpath);
    }
    else if (ext == ".tmx")
    {
        ExternalTooling* tooling = GetSubsystem<ExternalTooling>();
        tooling->LaunchOrOpen("AtomicTiled", fullpath);
    }

    if (editor)
    {
        // We have a new editor so send a stop playing if we are playing
        if (GetSubsystem<Editor>()->IsPlayingProject())
        {
            SendEvent(E_EDITORPLAYSTOP);
        }

        editors_[fullpath] = editor;
        editorLookup_[editor->GetRootContentWidget()] = editor;
        tabcontainer_->SetCurrentPage(tabcontainer_->GetNumPages()-1);

        editor->SetFocus();

// BEGIN LICENSE MANAGEMENT
        LicenseSystem* licenseSystem = GetSubsystem<LicenseSystem>();
        if(licenseSystem->IsStandardLicense())
        {
            if (ext == ".scene" )
            {
                UIModalOps* ops = GetSubsystem<UIModalOps>();
                ops->ShowInfoModule3D();
            }
        }
// END LICENSE MANAGEMENT

    }

}

void ResourceFrame::FocusCurrentTab()
{
    TBWidget* widget = tabcontainer_->GetCurrentPageWidget();
    if (widget && editorLookup_.Contains(widget))
        return editorLookup_[widget]->SetFocus();
}

bool ResourceFrame::OnEvent(const TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_TAB_CHANGED && ev.target == tabcontainer_)
    {
        TBWidget* widget = tabcontainer_->GetCurrentPageWidget();
        if (widget)
        {
            if (editorLookup_.Contains(widget))
            {
                ResourceEditor* editor = editorLookup_[widget];
                if (editor != currentResourceEditor_)
                {
                    VariantMap eventData;
                    eventData[EditorResourceEditorChanged::P_RESOURCEEDITOR] = editor;
                    SendEvent(E_EDITORRESOURCEEDITORCHANGED, eventData);
                    currentResourceEditor_ = editor;
                }
            }
        }

        return true;
    }

    if (ev.type == EVENT_TYPE_KEY_DOWN || ev.type == EVENT_TYPE_SHORTCUT
            || ev.type == EVENT_TYPE_CLICK || ev.type == EVENT_TYPE_RIGHT_POINTER_UP)
    {
        if (!editors_.Size())
            return false;

        TBWidget* widget = tabcontainer_->GetCurrentPageWidget();
        if (editorLookup_.Contains(widget))
            return editorLookup_[widget]->OnEvent(ev);
    }

    return false;

}

bool ResourceFrame::IssuesWidgetVisible()
{
    TBWidget *child;
    for (child = resourceLayout_->GetFirstChild(); child; child = child->GetNext())
    {
        if (child == issueswidget_->GetWidgetDelegate())
            break;
    }

    return child != NULL;

}

void ResourceFrame::ShowIssuesWidget(bool show)
{    
    if (show && ErrorsWidgetVisible())
        ShowErrorsWidget(false);
    if (show && ConsoleWidgetVisible())
        ShowConsoleWidget(false);

    TBWidget *child;
    for (child = resourceLayout_->GetFirstChild(); child; child = child->GetNext())
    {
        if (child == issueswidget_->GetWidgetDelegate())
            break;
    }

    if (show)
    {
        if (!child)
        {
            resourceLayout_->AddChild(issueswidget_->GetWidgetDelegate());
        }

        issueswidget_->UpdateIssues();
    }
    else
    {
        if (child)
            resourceLayout_->RemoveChild(child);
    }
}

bool ResourceFrame::ErrorsWidgetVisible()
{
    TBWidget *child;
    for (child = resourceLayout_->GetFirstChild(); child; child = child->GetNext())
    {
        if (child == errorswidget_->GetWidgetDelegate())
            break;
    }

    return child != NULL;
}

void ResourceFrame::ShowErrorsWidget(bool show)
{
    if (show && ConsoleWidgetVisible())
        ShowConsoleWidget(false);
    if (show && IssuesWidgetVisible())
        ShowIssuesWidget(false);

    TBWidget *child;
    for (child = resourceLayout_->GetFirstChild(); child; child = child->GetNext())
    {
        if (child == errorswidget_->GetWidgetDelegate())
            break;
    }

    if (show)
    {
        if (!child)
        {
            resourceLayout_->AddChild(errorswidget_->GetWidgetDelegate());
        }

        errorswidget_->UpdateErrors();

    }
    else
    {
        if (child)
            resourceLayout_->RemoveChild(child);
    }
}

bool ResourceFrame::ConsoleWidgetVisible()
{

    TBWidget *child;
    for (child = resourceLayout_->GetFirstChild(); child; child = child->GetNext())
    {
        if (child == consolewidget_->GetWidgetDelegate())
            break;
    }

    return child != NULL;

}

void ResourceFrame::ShowConsoleWidget(bool show)
{
    if (show && ErrorsWidgetVisible())
        ShowErrorsWidget(false);
    if (show && IssuesWidgetVisible())
        ShowIssuesWidget(false);

    TBWidget *child;
    for (child = resourceLayout_->GetFirstChild(); child; child = child->GetNext())
    {
        if (child == consolewidget_->GetWidgetDelegate())
            break;
    }

    if (show)
    {
        if (!child)
        {
            resourceLayout_->AddChild(consolewidget_->GetWidgetDelegate());
        }

    }
    else
    {
        if (child)
            resourceLayout_->RemoveChild(child);
    }

}

void ResourceFrame::SendCurrentEditorEvent(const TBWidgetEvent &ev)
{
    TBWidget* widget = tabcontainer_->GetCurrentPageWidget();
    if (!widget)
        return;

    if (editorLookup_.Contains(widget))
        editorLookup_[widget]->OnEvent(ev);

}

void ResourceFrame::NavigateToResource(const String& fullpath, int lineNumber, int tokenPos)
{
    if (!editors_.Contains(fullpath))
        return;

    ResourceEditor* editor = editors_[fullpath];

    TBWidget* root = tabcontainer_->GetContentRoot();

    int i = 0;
    for (TBWidget *child = root->GetFirstChild(); child; child = child->GetNext(), i++)
    {
        if (editorLookup_.Contains(child))
        {
            if (editorLookup_[child] == editor)
            {
                break;
            }
        }
    }

    if (i < tabcontainer_->GetNumPages())
    {
        tabcontainer_->SetCurrentPage(i);

        editor->SetFocus();

        // this cast could be better
        String ext = GetExtension(fullpath);

        if (ext == ".js" && lineNumber != -1)
        {
            JSResourceEditor* jse = (JSResourceEditor*) editor;
            jse->GotoLineNumber(lineNumber);
        }
        else if (ext == ".js" && tokenPos != -1)
        {
            JSResourceEditor* jse = (JSResourceEditor*) editor;
            jse->GotoTokenPos(tokenPos);
        }


    }

}

bool ResourceFrame::HasUnsavedModifications()
{
    HashMap<String, SharedPtr<ResourceEditor> >::ConstIterator itr;
    for (itr = editors_.Begin(); itr != editors_.End(); itr++)
    {
        if (itr->second_->HasUnsavedModifications())
            return true;
    }

    return false;
}

void ResourceFrame::CloseResourceEditor(ResourceEditor* editor, bool navigateToAvailableResource)
{
    assert(editors_.Contains(editor->GetFullPath()));

    editors_.Erase(editor->GetFullPath());

    TBWidget* root = tabcontainer_->GetContentRoot();

    bool found = false;
    for (TBWidget *child = root->GetFirstChild(); child; child = child->GetNext())
    {
        if (editorLookup_.Contains(child))
        {
            if (editorLookup_[child] == editor)
            {
                found = true;
                root->RemoveChild(child);
                editorLookup_.Erase(child);

                break;
            }
        }
    }

    assert(found);

    tabcontainer_->SetCurrentPage(-1);

    if (navigateToAvailableResource)
    {
        if (editors_.Size())
        {
            HashMap<String, SharedPtr<ResourceEditor> >::ConstIterator itr = editors_.End();
            itr--;
            NavigateToResource(itr->second_->GetFullPath());
        }
    }

}

void ResourceFrame::HandlePlayStarted(StringHash eventType, VariantMap& eventData)
{
    //delegate_->SetVisibilility(WIDGET_VISIBILITY_INVISIBLE);
    //delegate_->SetIgnoreInput(true);
    //delegate_->SetState(WIDGET_STATE_DISABLED, true);
}


void ResourceFrame::HandlePlayStopped(StringHash eventType, VariantMap& eventData)
{
    //delegate_->SetVisibilility(WIDGET_VISIBILITY_VISIBLE);
    //delegate_->SetIgnoreInput(false);
    //delegate_->SetState(WIDGET_STATE_DISABLED, false);
}

void ResourceFrame::CloseAllResourceEditors()
{
    Vector<SharedPtr<ResourceEditor> > editors = editors_.Values();

    for (unsigned i = 0; i < editors.Size(); i++)
        editors[i]->Close(false);

}

}
