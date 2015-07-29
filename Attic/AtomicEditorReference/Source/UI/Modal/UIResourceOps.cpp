// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <TurboBadger/tb_window.h>
#include <TurboBadger/tb_editfield.h>

#include <Atomic/Core/Context.h>
#include <Atomic/UI/UI.h>

#include "Resources/AEResourceOps.h"

#include "UIResourceOps.h"

namespace AtomicEditor
{

// UINewFolder------------------------------------------------

UINewFolder::UINewFolder(Context* context):
    UIModalOpWindow(context)
{
    UI* tbui = GetSubsystem<UI>();
    window_->SetText("New Folder");
    tbui->LoadResourceFile(window_->GetContentRoot(), "AtomicEditor/editor/ui/resourcenewfolder.tb.txt");
    nameField_ = window_->GetContentRoot()->GetWidgetByIDAndType<TBEditField>(TBIDC("folder_name"));
    assert(nameField_);
    window_->ResizeToFitContent();
    Center();
}

UINewFolder::~UINewFolder()
{

}

bool UINewFolder::OnEvent(const TBWidgetEvent &ev)
{
    UIModalOps* ops = GetSubsystem<UIModalOps>();

    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.target->GetID() == TBIDC("create"))
        {
            TBStr text;
            nameField_->GetText(text);
            ResourceOps* rops = GetSubsystem<ResourceOps>();
            rops->HandleNewFolder(ops->GetResourcePath() + text.CStr(), true);
            ops->Hide();
            return true;
        }

        if (ev.target->GetID() == TBIDC("cancel"))
        {
            ops->Hide();
            return true;
        }
    }

    return false;
}

// UIResourceDelete------------------------------------------------

UIResourceDelete::UIResourceDelete(Context* context):
    UIModalOpWindow(context)
{
    UIModalOps* ops = GetSubsystem<UIModalOps>();

    UI* tbui = GetSubsystem<UI>();
    window_->SetText("Delete Resource");
    tbui->LoadResourceFile(window_->GetContentRoot(), "AtomicEditor/editor/ui/resourcedelete.tb.txt");

    TBEditField* message = window_->GetContentRoot()->GetWidgetByIDAndType<TBEditField>(TBIDC("message"));

    String msg;
    message->SetText(msg.AppendWithFormat("Are you sure you want to delete resource:\n\n%s\n\nThis operation cannot be undone"
                     ,ops->GetResourcePath().CString()).CString());

    window_->ResizeToFitContent();
    Center();
}

UIResourceDelete::~UIResourceDelete()
{

}

bool UIResourceDelete::OnEvent(const TBWidgetEvent &ev)
{
    UIModalOps* ops = GetSubsystem<UIModalOps>();

    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.target->GetID() == TBIDC("delete"))
        {
            ResourceOps* rops = GetSubsystem<ResourceOps>();
            rops->HandleResourceDelete(ops->GetResourcePath());
            ops->Hide();
            return true;
        }

        if (ev.target->GetID() == TBIDC("cancel"))
        {
            ops->Hide();
            return true;
        }
    }

    return false;
}

// UICreateComponent------------------------------------------------

UICreateComponent::UICreateComponent(Context* context):
    UIModalOpWindow(context)
{
    UI* tbui = GetSubsystem<UI>();
    window_->SetText("Create Javascript Component");
    tbui->LoadResourceFile(window_->GetContentRoot(), "AtomicEditor/editor/ui/resourcecreatecomponent.tb.txt");
    nameField_ = window_->GetContentRoot()->GetWidgetByIDAndType<TBEditField>(TBIDC("component_name"));
    assert(nameField_);
    window_->ResizeToFitContent();
    Center();
}

UICreateComponent::~UICreateComponent()
{

}

bool UICreateComponent::OnEvent(const TBWidgetEvent &ev)
{
    UIModalOps* ops = GetSubsystem<UIModalOps>();

    if (ev.type == EVENT_TYPE_POINTER_MOVE)
        return false;

    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.target->GetID() == TBIDC("create"))
        {
            TBStr text;
            nameField_->GetText(text);
            ResourceOps* rops = GetSubsystem<ResourceOps>();
            rops->HandleCreateComponent(ops->GetResourcePath(), text.CStr(), true, true);
            ops->Hide();
            return true;
        }

        if (ev.target->GetID() == TBIDC("cancel"))
        {
            ops->Hide();
            return true;
        }
    }

    return false;
}

// UICreateScript------------------------------------------------

UICreateScript::UICreateScript(Context* context):
    UIModalOpWindow(context)
{
    UI* tbui = GetSubsystem<UI>();
    window_->SetText("Create Script");
    tbui->LoadResourceFile(window_->GetContentRoot(), "AtomicEditor/editor/ui/resourcecreatescript.tb.txt");
    nameField_ = window_->GetContentRoot()->GetWidgetByIDAndType<TBEditField>(TBIDC("script_name"));
    assert(nameField_);
    window_->ResizeToFitContent();
    Center();
}

UICreateScript::~UICreateScript()
{

}

bool UICreateScript::OnEvent(const TBWidgetEvent &ev)
{
    UIModalOps* ops = GetSubsystem<UIModalOps>();

    if (ev.type == EVENT_TYPE_POINTER_MOVE)
        return false;

    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.target->GetID() == TBIDC("create"))
        {
            TBStr text;
            nameField_->GetText(text);
            ResourceOps* rops = GetSubsystem<ResourceOps>();
            rops->HandleCreateScript(ops->GetResourcePath(), text.CStr(), true, true);
            ops->Hide();
            return true;
        }

        if (ev.target->GetID() == TBIDC("cancel"))
        {
            ops->Hide();
            return true;
        }
    }

    return false;
}

// UICreateModule------------------------------------------------

UICreateModule::UICreateModule(Context* context):
    UIModalOpWindow(context)
{
    UI* tbui = GetSubsystem<UI>();
    window_->SetText("Create Module");
    tbui->LoadResourceFile(window_->GetContentRoot(), "AtomicEditor/editor/ui/resourcecreatemodule.tb.txt");
    nameField_ = window_->GetContentRoot()->GetWidgetByIDAndType<TBEditField>(TBIDC("module_name"));
    assert(nameField_);
    window_->ResizeToFitContent();
    Center();
}

UICreateModule::~UICreateModule()
{

}

bool UICreateModule::OnEvent(const TBWidgetEvent &ev)
{
    UIModalOps* ops = GetSubsystem<UIModalOps>();

    if (ev.type == EVENT_TYPE_POINTER_MOVE)
        return false;

    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.target->GetID() == TBIDC("create"))
        {
            TBStr text;
            nameField_->GetText(text);
            ResourceOps* rops = GetSubsystem<ResourceOps>();
            rops->HandleCreateModule(ops->GetResourcePath(), text.CStr(), true, true);
            ops->Hide();
            return true;
        }

        if (ev.target->GetID() == TBIDC("cancel"))
        {
            ops->Hide();
            return true;
        }
    }

    return false;
}

// UICreateModule------------------------------------------------

UICreate2DLevel::UICreate2DLevel(Context* context):
    UIModalOpWindow(context)
{
    UI* tbui = GetSubsystem<UI>();
    window_->SetText("Create 2D Level");
    tbui->LoadResourceFile(window_->GetContentRoot(), "AtomicEditor/editor/ui/resourcecreate2dlevel.tb.txt");
    nameField_ = window_->GetContentRoot()->GetWidgetByIDAndType<TBEditField>(TBIDC("level_name"));
    assert(nameField_);
    window_->ResizeToFitContent();
    Center();
}

UICreate2DLevel::~UICreate2DLevel()
{

}

bool UICreate2DLevel::OnEvent(const TBWidgetEvent &ev)
{
    UIModalOps* ops = GetSubsystem<UIModalOps>();

    if (ev.type == EVENT_TYPE_POINTER_MOVE)
        return false;

    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.target->GetID() == TBIDC("create"))
        {
            TBStr text;
            nameField_->GetText(text);
            ResourceOps* rops = GetSubsystem<ResourceOps>();
            rops->HandleCreate2DLevel(ops->GetResourcePath(), text.CStr(), true, true);
            ops->Hide();
            return true;
        }

        if (ev.target->GetID() == TBIDC("cancel"))
        {
            ops->Hide();
            return true;
        }
    }

    return false;
}

}
