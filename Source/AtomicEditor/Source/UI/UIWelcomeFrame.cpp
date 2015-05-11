// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include <rapidjson/document.h>
#include "rapidjson/prettywriter.h"
#include "rapidjson/filestream.h"

using namespace rapidjson;

#include "AtomicEditor.h"

#include <TurboBadger/tb_select.h>
#include <TurboBadger/tb_editfield.h>
#include <TurboBadger/image/tb_image_widget.h>


#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/Log.h>

#include <Atomic/UI/UI.h>

#include "License/AELicenseSystem.h"

#include "../Project/ProjectUtils.h"
#include "../AEPreferences.h"
#include "../AEEditor.h"

#include "UI/Modal/UIModalOps.h"
#include "UIWelcomeFrame.h"

using namespace tb;

namespace AtomicEditor
{

WelcomeFrame::WelcomeFrame(Context* context) :
    AEWidget(context), currentExampleLayout_(0), exampleCount_(0)
{

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

#ifdef ATOMIC_PLATFORM_OSX
    exampleSourceDir_ = fileSystem->GetAppBundleResourceFolder();
#else
    exampleSourceDir_ = fileSystem->GetProgramDir();
#endif

    exampleInfoDir_ = exampleSourceDir_ + "ExampleInfo";
    exampleSourceDir_ += "Examples";

    UI* tbui = GetSubsystem<UI>();
    tbui->LoadResourceFile(delegate_, "AtomicEditor/editor/ui/welcomeframe.tb.txt");

    delegate_->SetGravity(WIDGET_GRAVITY_ALL);

    UpdateRecentProjects();

    FillExamples();
}

WelcomeFrame::~WelcomeFrame()
{

}

void WelcomeFrame::AddExample(const String& name, const String& desc, const String& screenshot, const String& folder, const String& module)
{
    TBLayout* exlayout = delegate_->GetWidgetByIDAndType<TBLayout>(TBIDC("examples_layout"));
    assert(exlayout);

    if (!currentExampleLayout_)
    {
        currentExampleLayout_ = new TBLayout();
        currentExampleLayout_->SetSpacing(8);
        exlayout->AddChild(currentExampleLayout_);
    }

    // 200x150

    TBLayout* example = new TBLayout();
    example->SetSkinBg(TBIDC("StarCondition"));
    example->SetAxis(AXIS_Y);
    example->SetLayoutDistribution(LAYOUT_DISTRIBUTION_GRAVITY);
    example->SetLayoutSize(LAYOUT_SIZE_AVAILABLE);

    // IMAGE BUTTON

    TBID id = TBIDC(name.CString());

    TBButton* button = new TBButton();
    button->SetSkinBg("StarButton");
    button->SetID(id);
    TBImageWidget* image = new TBImageWidget();

    image->SetImage((exampleInfoDir_ + "/" + screenshot).CString());
    image->SetSkinBg(TBIDC("ImageFrame"));
    TBRect rect(0, 0, image->GetImageWidth()/2, image->GetImageHeight()/2);
    image->SetRect(rect);

    // NAME FIELD
    TBTextField* nameField = new TBTextField();
    nameField->SetSkinBg(TBIDC("ImageCaption"));
    nameField->SetText(name.CString());

    TBRect nameRect(0, image->GetImageHeight()/2 - 16,  image->GetImageWidth()/2, 16);
    nameField->SetRect(nameRect);

    nameField->SetGravity(WIDGET_GRAVITY_BOTTOM);

    image->AddChild(nameField);

    button->AddChild(image);

    LayoutParams lp;
    lp.min_w = image->GetImageWidth()/2;
    lp.min_h = image->GetImageHeight()/2;

    button->SetLayoutParams(lp);

    button->SetGravity(WIDGET_GRAVITY_LEFT);

    example->AddChild(button);

    // DESC TEXT

    TBEditField* descField = new TBEditField();
    descField->SetStyling(true);
    descField->SetMultiline(true);
    descField->SetReadOnly(true);
    descField->SetWrapping(true);

    String styleDesc = "<color #A9A9A9>" + desc + "</color>";

    descField->SetText(styleDesc.CString());

    descField->SetAdaptToContentSize(true);

    lp.min_h /= 2;
    lp.max_h = lp.min_h;
    lp.max_w = lp.min_w;
    descField->SetLayoutParams(lp);

    example->AddChild(descField);

    currentExampleLayout_->AddChild(example);

    exampleCount_++;

    // three across, todo, be smarter about this
    if (! (exampleCount_ % 3))
    {
        currentExampleLayout_ = NULL;
    }

    ExampleInfo info;
    info.name = name;
    info.folder = folder;
    info.id = id;
    info.module = module;
    info.screenshot = exampleInfoDir_ + "/" + screenshot;

    exampleInfo_.Push(info);


}

void WelcomeFrame::FillExamples()
{

    rapidjson::Document document;

    String exampleJsonFile = exampleInfoDir_ + "/Examples.json";

    File jsonFile(context_, exampleJsonFile);

    if (!jsonFile.IsOpen())
        return;

    String json;
    jsonFile.ReadText(json);

    if (!json.Length())
        return;

    if (document.Parse<0>(json.CString()).HasParseError())
    {
        LOGERRORF("Could not parse JSON data from %s", exampleJsonFile.CString());
        return;
    }

    const Value::Member* examples = document.FindMember("Examples");;
    if (!examples || !examples->value.IsArray())
        return;

    for (Value::ConstValueIterator itr = examples->value.Begin(); itr != examples->value.End(); itr++)
    {
        if (!(*itr).IsObject())
            continue;

        const Value::Member* name = (*itr).FindMember("name");
        const Value::Member* desc = (*itr).FindMember("desc");
        const Value::Member* screenshot = (*itr).FindMember("screenshot");
        const Value::Member* folder = (*itr).FindMember("folder");
        const Value::Member* module = (*itr).FindMember("module");

        if (!name || !desc || !screenshot || !folder || !module)
            continue;

        if (!name->value.IsString() || !desc->value.IsString() || !screenshot->value.IsString()
                                    || !folder->value.IsString() || !module->value.IsString())
        {
            continue;
        }

        AddExample(name->value.GetString(), desc->value.GetString(),
                   screenshot->value.GetString(), folder->value.GetString(), module->value.GetString());
    }

}

void WelcomeFrame::UpdateRecentProjects()
{
    TBSelectList* select = delegate_->GetWidgetByIDAndType<TBSelectList>(TBIDC("recentprojects"));
    if (!select)
        return;

    TBGenericStringItemSource* source = select->GetDefaultSource();
    source->DeleteAllItems();

    const Vector<String>& recent = GetSubsystem<AEPreferences>()->GetRecentProjects();

    for (unsigned i = 0; i < recent.Size(); i++)
    {
        source->AddItem(new TBGenericStringItem(recent[i].CString()));
    }

}

bool WelcomeFrame::OnEvent(const TBWidgetEvent &ev)
{
    UIModalOps* ops = GetSubsystem<UIModalOps>();
    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.target)
        {
            if (ev.target->GetID() == TBIDC("getting_started"))
            {
                fileSystem->SystemOpen("http://atomicgameengine.com/videos");
            }
            else if (ev.target->GetID() ==  TBIDC("forum"))
            {
                fileSystem->SystemOpen("http://atomicgameengine.com/forum");
            }
            else if (ev.target->GetID() == TBIDC("atomic_github"))
            {
                fileSystem->SystemOpen("https://github.com/AtomicGameEngine/AtomicGameEngine");
            }
            else if (ev.target->GetID() == TBIDC("examples_github"))
            {
                fileSystem->SystemOpen("https://github.com/AtomicGameEngine/AtomicExamples");
            }
            else if (ev.target->GetID() == TBIDC("open project"))
            {
                GetSubsystem<ProjectUtils>()->OpenProjectFileDialog();
                return true;
            }
            else if (ev.target->GetID() == TBIDC("new project"))
            {

                GetSubsystem<UIModalOps>()->ShowNewProject();
                return true;
            }
            else if (ev.target->GetID() == TBID("recentprojects"))
            {
                // we clicked the recent projects list
                TBSelectList* list = (TBSelectList*) ev.target;
                int value = list->GetValue();

                if (value >= 0)
                {
                    TBGenericStringItemSource* source = list->GetDefaultSource();
                    if (source->GetNumItems())
                    {
                        String projectpath = String(source->GetItemString(value));
                        GetSubsystem<Editor>()->LoadProject(projectpath);
                    }
                }

                return true;
            }
            else
            {
                for (unsigned i = 0; i < exampleInfo_.Size(); i++)
                {
                    List<ExampleInfo>::Iterator itr = exampleInfo_.Begin();
                    while (itr != exampleInfo_.End())
                    {
                        if (ev.target->GetID() == (*itr).id)
                        {
                            String exampleSourceDir = exampleSourceDir_ + "/" + (*itr).folder;

// BEGIN LICENSE MANAGEMENT
                            LicenseSystem* licenseSystem = GetSubsystem<LicenseSystem>();
                            if (licenseSystem->IsStandardLicense())
                            {
                                if ((*itr).module == "3D")
                                {

                                    ops->ShowInfoModule3D(exampleSourceDir, (*itr).screenshot);
                                    return true;
                                }
                            }
// END LICENSE MANAGEMENT                            
                            ops->ShowCreateProject(exampleSourceDir, (*itr).screenshot);
                            return true;
                        }

                        itr++;
                    }

                }
            }
        }
    }

    return false;
}


}
