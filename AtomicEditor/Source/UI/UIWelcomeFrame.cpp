// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicEditor

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

#include <Atomic/UI/TBUI.h>

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
    String exampleSourceDir_ = fileSystem->GetProgramDir();
#endif

    exampleInfoDir_ = exampleSourceDir_ + "ExampleInfo";
    exampleSourceDir_ += "Examples";

    TBUI* tbui = GetSubsystem<TBUI>();
    tbui->LoadResourceFile(delegate_, "AtomicEditor/editor/ui/welcomeframe.tb.txt");

    delegate_->SetGravity(WIDGET_GRAVITY_ALL);

    UpdateRecentProjects();

    FillExamples();
}

WelcomeFrame::~WelcomeFrame()
{

}

void WelcomeFrame::AddExample(const String& name, const String& desc, const String& screenshot, const String& folder)
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

         if (!name || !desc || !screenshot || !folder)
             continue;

         if (!name->value.IsString() || !desc->value.IsString() || !screenshot->value.IsString() || !folder->value.IsString())
             continue;

         AddExample(name->value.GetString(), desc->value.GetString(), screenshot->value.GetString(), folder->value.GetString());
    }

}

bool WelcomeFrame::HandleExampleCopy(const String& name, const String& exampleFolder, String& atomicProjectFile)\
{
    Editor* editor = GetSubsystem<Editor>();
    String projectPath = GetSubsystem<ProjectUtils>()->NewProjectFileDialog();

    if (!projectPath.Length())
        return false;

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

    if (!fileSystem->DirExists(projectPath))
    {
        editor->PostModalInfo("New Project Editor Error", "Project folder does not exist");
        return false;
    }

    Vector<String> results;
    fileSystem->ScanDir(results, projectPath, "*", SCAN_DIRS | SCAN_FILES, false);
    while (results.Remove(".")) {}
    while (results.Remove("..")) {}

    if (results.Size())
    {
        editor->PostModalInfo("New Project Editor Error", "Project folder must be empty.\nPlease create a new folder or select an empty one");
        return false;
    }

    String exampleSourceDir = exampleSourceDir_ + "/" + exampleFolder;

    bool result = fileSystem->CopyDir(exampleSourceDir + "/" + "Resources", projectPath + "/" + "Resources");

    if (!result)
    {
        editor->PostModalInfo("New Project Editor Error", "Error copying example");
        return false;
    }

    // example folder name and .atomic must match
    atomicProjectFile = projectPath + "/" + exampleFolder + ".atomic";
    result = fileSystem->Copy(exampleSourceDir + "/" + exampleFolder + ".atomic", atomicProjectFile);

    if (!result)
    {
        editor->PostModalInfo("New Project Editor Error", "Error copying example project file");
        return false;
    }

    return true;

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
    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.target)
        {
            if (ev.target->GetID() == TBIDC("open project"))
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
                TBGenericStringItemSource* source = list->GetDefaultSource();
                if (source->GetNumItems())
                {
                    String projectpath = String(source->GetItemString(value));
                    GetSubsystem<Editor>()->LoadProject(projectpath);
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
                            String projectPath;
                            if (HandleExampleCopy((*itr).name, (*itr).folder, projectPath))
                            {
                                GetSubsystem<Editor>()->LoadProject(projectPath);

                            }

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
