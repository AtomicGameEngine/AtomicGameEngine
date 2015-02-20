// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <TurboBadger/tb_window.h>
#include <TurboBadger/tb_select.h>
#include <TurboBadger/tb_editfield.h>

#include <Atomic/Core/Context.h>
#include <Atomic/IO/File.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Input/InputEvents.h>
#include <Atomic/UI/TBUI.h>

#include "Resources/AEResourceOps.h"
#include "AEPreferences.h"

#include "AEVersion.h"
#include "AEEditor.h"
#include "AEEvents.h"
#include "Project/AEProject.h"
#include "Project/ProjectUtils.h"

#include "License/AELicenseSystem.h"
#include "UIAbout.h"

namespace AtomicEditor
{

UIAbout::UIAbout(Context* context):
    UIModalOpWindow(context)
{
    TBUI* tbui = GetSubsystem<TBUI>();
    window_->SetText("About the Atomic Game Engine");
    tbui->LoadResourceFile(window_->GetContentRoot(), "AtomicEditor/editor/ui/about.tb.txt");

    TBEditField* age_license = window_->GetWidgetByIDAndType<TBEditField>(TBIDC("age_license"));
    assert(age_license);

    TBEditField* thirdparty_license = window_->GetWidgetByIDAndType<TBEditField>(TBIDC("thirdparty_license"));
    assert(thirdparty_license);

    TBEditField* externaltool_license = window_->GetWidgetByIDAndType<TBEditField>(TBIDC("externaltool_license"));
    assert(externaltool_license);

    TBEditField* about_text = window_->GetWidgetByIDAndType<TBEditField>(TBIDC("about_text"));
    assert(about_text);


    ResourceCache* cache = GetSubsystem<ResourceCache>();

    SharedPtr<File> file = cache->GetFile("AtomicEditor/eulas/atomic_game_engine_eula.txt");
    String text;
    file->ReadText(text);
    age_license->SetText(text.CString());

    file = cache->GetFile("AtomicEditor/eulas/atomic_thirdparty_eula.txt");
    file->ReadText(text);
    thirdparty_license->SetText(text.CString());

    file = cache->GetFile("AtomicEditor/eulas/atomic_external_tools_eula.txt");
    file->ReadText(text);
    externaltool_license->SetText(text.CString());

    GenerateAboutText(text);

    about_text->SetText(text.CString());

    window_->ResizeToFitContent();
    Center();
}


UIAbout::~UIAbout()
{

}

void UIAbout::GenerateAboutText(String& text)
{
    LicenseSystem* licenseSystem = GetSubsystem<LicenseSystem>();
    text.Clear();

    text += "<widget TBImageWidget: filename: 'AtomicEditor/editor/images/atomic_logo.png'>\n";
    text.AppendWithFormat("<color #D4FB79>Version  %i.%i.p%i</color>\n", ATOMIC_EDITOR_VERSION_MAJOR, ATOMIC_EDITOR_VERSION_MINOR, ATOMIC_EDITOR_VERSION_PATCH);
    text += "(c) 2014-2015 THUNDERBEAST GAMES LLC\n\n";

    text += "<color #76D6FF>Installed platform licenses:</color>\n\n";

    if (licenseSystem->HasPlatformLicense())
    {
        text += "    <widget TBSkinImage: skin: 'LogoMac-Small'> <widget TBSkinImage: skin: 'LogoWindows-Small'> " \
        "<widget TBSkinImage: skin: 'LogoHTML5-Small'> <widget TBSkinImage: skin: 'LogoAndroid-Small'> " \
        "<widget TBSkinImage: skin: 'LogoIOS-Small'>\n\n";
    }
    else
    {
        text += "<color #FF2600>    No platform licenses installed</color>\n\n";
    }

    text += "<color #76D6FF>Special Thanks:</color>\n\n";
    text += "    The Urho3D Project - http://urho3d.github.io\n\n";
    text += "    Sami Vaarala - http://www.duktape.org";
}

bool UIAbout::OnEvent(const TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.target->GetID() == TBIDC("ok"))
        {
            GetSubsystem<UIModalOps>()->Hide();
            return true;
        }

    }

    return false;
}

}


