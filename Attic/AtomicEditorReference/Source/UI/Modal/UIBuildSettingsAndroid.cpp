// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"
#include <TurboBadger/tb_layout.h>
#include <TurboBadger/tb_editfield.h>
#include <TurboBadger/tb_select.h>
#include <Atomic/Core/Context.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/MemoryBuffer.h>
#include <Atomic/UI/UI.h>

#include "AEEditor.h"
#include "AEEvents.h"
#include "AEPreferences.h"
#include "Project/ProjectUtils.h"
#include "Subprocess/AESubprocessSystem.h"

#include "Build/BuildSystem.h"

#include "UIBuildSettingsAndroid.h"

namespace AtomicEditor
{

UIBuildSettingsAndroid::UIBuildSettingsAndroid(Context* context) :
    AEWidget(context)
{
    UI* tbui = GetSubsystem<UI>();
    tbui->LoadResourceFile(delegate_, "AtomicEditor/editor/ui/buildsettings_android.tb.txt");

    appNameEdit_ = delegate_->GetWidgetByIDAndType<TBEditField>(TBIDC("app_name"));
    assert(appNameEdit_);

    appPackageEdit_ = delegate_->GetWidgetByIDAndType<TBEditField>(TBIDC("app_package"));
    assert(appPackageEdit_);

    productNameEdit_ = delegate_->GetWidgetByIDAndType<TBEditField>(TBIDC("product_name"));
    assert(productNameEdit_);

    companyNameEdit_ = delegate_->GetWidgetByIDAndType<TBEditField>(TBIDC("company_name"));
    assert(companyNameEdit_);

    sdkTargetSelect_ = delegate_->GetWidgetByIDAndType<TBSelectDropdown>(TBIDC("sdk_target_select"));
    assert(sdkTargetSelect_);

    TBTextField* jdk_root_text = delegate_->GetWidgetByIDAndType<TBTextField>(TBIDC("jdk_root_text"));
    assert(jdk_root_text);

    TBTextField* ant_path_text = delegate_->GetWidgetByIDAndType<TBTextField>(TBIDC("ant_path_text"));
    assert(ant_path_text);

#ifdef ATOMIC_PLATFORM_WINDOWS
    jdk_root_text->SetText("JDK Root: (Ex. C:\\Program Files\\Java\\jdk1.8.0_31)");
    ant_path_text->SetText("Ant Path: (The folder that contains ant.bat)");
#else

    TBButton* choose_jdk_root = delegate_->GetWidgetByIDAndType<TBButton>(TBIDC("choose_jdk_root"));
    assert(choose_jdk_root);

    TBButton* choose_ant_path = delegate_->GetWidgetByIDAndType<TBButton>(TBIDC("choose_ant_path"));
    assert(choose_ant_path);

    choose_jdk_root->SetVisibilility(WIDGET_VISIBILITY_GONE);
    choose_ant_path->SetVisibilility(WIDGET_VISIBILITY_GONE);

    TBEditField* ant_path = delegate_->GetWidgetByIDAndType<TBEditField>(TBIDC("ant_path"));
    assert(ant_path);

    ant_path->SetVisibilility(WIDGET_VISIBILITY_GONE);
    ant_path_text->SetVisibilility(WIDGET_VISIBILITY_GONE);

    TBEditField* jdk_root = delegate_->GetWidgetByIDAndType<TBEditField>(TBIDC("jdk_root"));
    assert(jdk_root);

    jdk_root->SetVisibilility(WIDGET_VISIBILITY_GONE);
    jdk_root_text->SetVisibilility(WIDGET_VISIBILITY_GONE);

#endif
}

UIBuildSettingsAndroid::~UIBuildSettingsAndroid()
{

}

void UIBuildSettingsAndroid::HandleEvent(StringHash eventType, VariantMap& eventData)
{
    if (eventType == E_SUBPROCESSOUTPUT)
    {
        targetOutput_ += eventData[SubprocessOutput::P_TEXT].GetString();
    }
    else if (eventType == E_SUBPROCESSCOMPLETE)
    {
        TBGenericStringItemSource* source = sdkTargetSelect_->GetDefaultSource();

        MemoryBuffer reader(targetOutput_.CString(), targetOutput_.Length() + 1);

        String id;
        bool parsed = false;

        while (!reader.IsEof())
        {
            String line = reader.ReadLine();
            if (line.StartsWith("id:"))
            {
                //id: 33 or "Google Inc.:Google APIs (x86 System Image):19"
                parsed = false;
                Vector<String> elements = line.Split('\"');
                if (elements.Size() == 2)
                {
                    String api = elements[1];
                    TBGenericStringItem* item = new TBGenericStringItem(api.CString());
                    item->tag = TBValue(id.CString());
                    source->AddItem(item);
                }
            }
        }

        // force update
        sdkTargetSelect_->SetValue(-1);
        sdkTargetSelect_->SetValue(0);

        //LOGINFOF("%s", targetOutput_.CString());
    }

}

void UIBuildSettingsAndroid::RefreshAndroidTargets()
{
    Editor* editor = context_->GetSubsystem<Editor>();
    SubprocessSystem* subs = context_->GetSubsystem<SubprocessSystem>();

    TBGenericStringItemSource* source = sdkTargetSelect_->GetDefaultSource();
    source->DeleteAllItems();

    String androidCommand = GetNativePath(editor->GetPreferences()->GetAndroidSDKPath());

    if (!androidCommand.Length())
        return;



#ifdef ATOMIC_PLATFORM_OSX
    Vector<String> args = String("list targets").Split(' ');
    androidCommand += "tools/android";
#endif
#ifdef ATOMIC_PLATFORM_WINDOWS
    Vector<String> args;
    // android is a batch file on windows, so have to run with cmd /c
    args.Push("/c");
    args.Push("\"" + androidCommand + "\\tools\\android.bat\"");
    args.Push("list");
    args.Push("targets");
    androidCommand = "cmd";
#endif
#ifdef ATOMIC_PLATFORM_LINUX
    Vector<String> args = String("list targets").Split(' ');
    androidCommand += "tools/android";
#endif

    targetOutput_.Clear();
    targetLookup_.Clear();

    Subprocess* subprocess = subs->Launch(androidCommand, args);
    if (!subprocess)
    {
        //ERROR
        assert(0);
    }

    SubscribeToEvent(subprocess, E_SUBPROCESSCOMPLETE, HANDLER(UIBuildSettingsAndroid, HandleEvent));
    SubscribeToEvent(subprocess, E_SUBPROCESSOUTPUT, HANDLER(UIBuildSettingsAndroid, HandleEvent));


}

void UIBuildSettingsAndroid::StoreSettings()
{
    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();

    AndroidBuildSettings settings;

    TBStr text;
    appNameEdit_->GetText(text);
    settings.appName = text.CStr();
    text.Clear();

    appPackageEdit_->GetText(text);
    settings.package = text.CStr();
    text.Clear();

    productNameEdit_->GetText(text);
    settings.productName = text.CStr();
    text.Clear();

    companyNameEdit_->GetText(text);
    settings.companyName = text.CStr();
    text.Clear();

    sdkTargetSelect_->GetText(text);
    settings.targetSDKVersion = text.CStr();
    text.Clear();

    buildSystem->GetBuildSettings()->SetAndroidSettings(settings);

}

void UIBuildSettingsAndroid::Refresh()
{
    Editor* editor = context_->GetSubsystem<Editor>();

    AEPreferences* prefs = editor->GetPreferences();

    TBEditField* sdk_path = delegate_->GetWidgetByIDAndType<TBEditField>(TBIDC("sdk_path"));
    sdk_path->SetText(prefs->GetAndroidSDKPath().CString());

    TBEditField* ant_path = delegate_->GetWidgetByIDAndType<TBEditField>(TBIDC("ant_path"));
    ant_path->SetText(prefs->GetAntPath().CString());

    TBEditField* jdk_root = delegate_->GetWidgetByIDAndType<TBEditField>(TBIDC("jdk_root"));
    jdk_root->SetText(prefs->GetJDKRootPath().CString());

    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();
    const AndroidBuildSettings& settings = buildSystem->GetBuildSettings()->GetAndroidSettings();

    appNameEdit_->SetText(settings.appName.CString());
    appPackageEdit_->SetText(settings.package.CString());
    productNameEdit_->SetText(settings.productName.CString());
    companyNameEdit_->SetText(settings.companyName.CString());
    sdkTargetSelect_->SetText(settings.targetSDKVersion.CString());

}

bool UIBuildSettingsAndroid::OnEvent(const TBWidgetEvent &ev)
{
    Editor* editor = context_->GetSubsystem<Editor>();
    ProjectUtils* utils = context_->GetSubsystem<ProjectUtils>();

    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.target->GetID() == TBIDC("choose_sdk_path"))
        {
            String path = utils->GetAndroidSDKPath("");
            if (path.Length())
            {
                editor->GetPreferences()->SetAndroidSDKPath(path);
                Refresh();
            }
            return true;
        }
        else if (ev.target->GetID() == TBIDC("choose_ant_path"))
        {
            String path = utils->GetAntPath("");
            if (path.Length())
            {
                editor->GetPreferences()->SetAntPath(path);
                Refresh();
            }
            return true;
        }
        else if (ev.target->GetID() == TBIDC("choose_jdk_root"))
        {
            String path = utils->GetJDKRootPath("");
            if (path.Length())
            {
                editor->GetPreferences()->SetJDKRootPath(path);
                Refresh();
            }
            return true;
        }
        else if (ev.target->GetID() == TBIDC("refresh_sdk_targets"))
        {
            RefreshAndroidTargets();
        }

    }

    return false;

}

}

