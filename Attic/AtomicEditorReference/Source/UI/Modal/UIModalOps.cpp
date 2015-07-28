// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <TurboBadger/tb_widgets_common.h>
#include <TurboBadger/tb_window.h>
#include <TurboBadger/tb_message_window.h>
#include <TurboBadger/tb_editfield.h>

#include <Atomic/Core/Context.h>
#include <Atomic/Graphics/Graphics.h>
#include <Atomic/UI/UI.h>

#include "AEEvents.h"

#include "Resources/AEResourceOps.h"

#include "UI/UIMainFrame.h"
#include "UI/UIProjectFrame.h"

#include "UIModalOps.h"
#include "UIResourceOps.h"
#include "UIBuild.h"
#include "UIBuildSettings.h"
#include "UIProgramOutput.h"
#include "UINewProject.h"
#include "UICreateProject.h"
#include "UIAbout.h"
#include "UIPlatformsInfo.h"
#include "UIInfoModule3D.h"

#include "License/UIActivation.h"
#include "License/UIActivationSuccess.h"
#include "License/UIManageLicense.h"
#include "License/AELicenseSystem.h"
#include "License/UIEulaAgreement.h"

#include "Player/UIPlayer.h"

namespace AtomicEditor
{

// UIModalOpWindow------------------------------------------------

UIModalOpWindow::UIModalOpWindow(Context* context):
    AEWidget(context)
{
    window_ = new TBWindow();
    UI* tbui = GetSubsystem<UI>();
    TBWidget* root = tbui->GetRootWidget();
    root->AddChild(delegate_);
    // start with full screen as size
    delegate_->SetRect(TBRect(0, 0, root->GetRect().w, root->GetRect().h));
    delegate_->AddChild(window_);

    delegate_->SetFocus(WIDGET_FOCUS_REASON_UNKNOWN);
}

UIModalOpWindow::~UIModalOpWindow()
{
}

bool UIModalOpWindow::OnEvent(const TBWidgetEvent &ev)
{
    return false;
}

void UIModalOpWindow::Center()
{
    UI* tbui = GetSubsystem<UI>();
    TBRect rect = window_->GetRect();
    TBWidget* root = tbui->GetRootWidget();
    TBRect bounds(0, 0, root->GetRect().w, root->GetRect().h);
    window_->SetRect(rect.CenterIn(bounds).MoveIn(bounds).Clip(bounds));
    delegate_->SetRect(bounds);
}

// UIModalOps------------------------------------------------

UIModalOps::UIModalOps(Context* context) :
    AEWidget(context),
    dimmer_(0),
    isHiding_(false)
{
    TBWidgetListener::AddGlobalListener(this);
    context->RegisterSubsystem(this);
    dimmer_ = new TBDimmer();

    SubscribeToEvent(E_EDITORSHUTDOWN, HANDLER(UIModalOps, HandleEditorShutdown));
}

void UIModalOps::Show()
{
    assert(!dimmer_->GetParent());
    UI* tbui = GetSubsystem<UI>();
    TBWidget* root = tbui->GetRootWidget();
    root->AddChild(dimmer_);
}

void UIModalOps::Hide()
{
    isHiding_ = true;

    if (dimmer_->GetParent())
        dimmer_->GetParent()->RemoveChild(dimmer_);

    opWindow_ = NULL;

    UI* tbui = GetSubsystem<UI>();
    tbui->GetRootWidget()->SetFocusRecursive(WIDGET_FOCUS_REASON_UNKNOWN);

    isHiding_ = false;

}

void UIModalOps::ShowCreateComponent(const String& resourcePath)
{
    assert(opWindow_.Null());

    resourcePath_ = resourcePath;
    Show();
    opWindow_ = new UICreateComponent(context_);
}

void UIModalOps::ShowCreateModule(const String& resourcePath)
{
    assert(opWindow_.Null());

    resourcePath_ = resourcePath;
    Show();
    opWindow_ = new UICreateModule(context_);
}

void UIModalOps::ShowCreate2DLevel(const String& resourcePath)
{
    assert(opWindow_.Null());

    resourcePath_ = resourcePath;
    Show();
    opWindow_ = new UICreate2DLevel(context_);
}

void UIModalOps::ShowCreateScript(const String& resourcePath)
{
    assert(opWindow_.Null());

    resourcePath_ = resourcePath;
    Show();
    opWindow_ = new UICreateScript(context_);
}

void UIModalOps::ShowResourceDelete(const String& resourcePath)
{
    assert(opWindow_.Null());

    resourcePath_ = resourcePath;
    Show();
    opWindow_ = new UIResourceDelete(context_);
}

void UIModalOps::ShowNewFolder(const String& resourcePath)
{
    assert(opWindow_.Null());

    resourcePath_ = resourcePath;
    Show();
    opWindow_ = new UINewFolder(context_);
}

void UIModalOps::ShowBuildSettings()
{
    assert(opWindow_.Null());

    Show();
    opWindow_ = new UIBuildSettings(context_);
}

void UIModalOps::ShowBuild()
{
    assert(opWindow_.Null());

    Show();
// BEGIN LICENSE MANAGEMENT
    LicenseSystem* licenseSystem = GetSubsystem<LicenseSystem>();
    /*
    if (licenseSystem->HasPlatformLicense())
    */
        opWindow_ = new UIBuild(context_);
    //else
    //    opWindow_ = new PlatformsInfo(context_);
// END LICENSE MANAGEMENT
}


void UIModalOps::ShowNewProject()
{
    assert(opWindow_.Null());

    Show();
    opWindow_ = new UINewProject(context_);
}

void UIModalOps::ShowCreateProject(const String &templateFolder, const String &imagePath)
{
    assert(opWindow_.Null());

    Show();
    opWindow_ = new UICreateProject(context_, templateFolder, imagePath);
}


void UIModalOps::SetProgramOutputSubprocess(Object* subprocess)
{
    if (opWindow_.Null())
        return;

    if (opWindow_->GetType() != UIProgramOutput::GetTypeStatic())
        return;

    UIProgramOutput* output = (UIProgramOutput*)(opWindow_.Get());
    output->SetSubprocess(subprocess);
}

void UIModalOps::PrintToProgramOutput(const String& text)
{
    if (opWindow_.Null())
        return;

    if (opWindow_->GetType() != UIProgramOutput::GetTypeStatic())
        return;

    UIProgramOutput* output = (UIProgramOutput*)(opWindow_.Get());
    output->OutputText(text);
}

void UIModalOps::ShowProgramOutput(Object *subprocess)
{
    assert(opWindow_.Null());

    Show();
    UIProgramOutput* output = new UIProgramOutput(context_);
    output->SetSubprocess(subprocess);
    opWindow_ = output;

}

void UIModalOps::ShowActivation()
{
    assert(opWindow_.Null());

    Show();
    opWindow_ = new UIActivation(context_);

}

void UIModalOps::ShowActivationSuccess()
{
    assert(opWindow_.Null());

    Show();
    opWindow_ = new UIActivationSuccess(context_);

}

void UIModalOps::ShowAbout()
{
    assert(opWindow_.Null());

    Show();
    opWindow_ = new UIAbout(context_);

}

void UIModalOps::ShowManageLicense()
{
    assert(opWindow_.Null());

    Show();
    opWindow_ = new UIManageLicense(context_);

}

void UIModalOps::ShowPlatformsInfo()
{
    assert(opWindow_.Null());

    Show();
    opWindow_ = new PlatformsInfo(context_);
}

void UIModalOps::ShowEulaAgreement()
{
    assert(opWindow_.Null());

    Show();
    opWindow_ = new UIEulaAgreement(context_);

}

void UIModalOps::ShowPlayer()
{
    assert(opWindow_.Null());

    Show();
    opWindow_ = new UIPlayer(context_);
}

void UIModalOps::ShowInfoModule3D(const String &exampleFolder, const String &exampleScreenshot)
{
    assert(opWindow_.Null());
    Show();
    opWindow_ = new InfoModule3D(context_, exampleFolder, exampleScreenshot);
}

void UIModalOps::OnWidgetDelete(TBWidget *widget)
{
    if (isHiding_)
        return;

    if (opWindow_)
    {
        if (widget == opWindow_->GetWindow())
            Hide();
    }

}

bool UIModalOps::OnWidgetDying(TBWidget *widget)
{
    return false;
}

UIModalOps::~UIModalOps()
{
    TBWidgetListener::RemoveGlobalListener(this);

}

bool UIModalOps::OnEvent(const TBWidgetEvent &ev)
{
    return false;
}

void UIModalOps::HandleEditorShutdown(StringHash eventType, VariantMap& eventData)
{
    context_->RemoveSubsystem(GetType());
}

}
