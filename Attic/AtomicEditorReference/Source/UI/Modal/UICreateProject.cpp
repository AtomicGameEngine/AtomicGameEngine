// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include <Poco/File.h>

#include "AtomicEditor.h"

#include <TurboBadger/tb_window.h>
#include <TurboBadger/tb_select.h>
#include <TurboBadger/tb_editfield.h>
#include <TurboBadger/image/tb_image_widget.h>

#include <Atomic/Core/Context.h>
#include <Atomic/Core/StringUtils.h>
#include <Atomic/UI/UI.h>

#include "Resources/AEResourceOps.h"
#include "AEPreferences.h"

#include "AEEditor.h"
#include "AEEvents.h"
#include "Project/AEProject.h"
#include "Project/ProjectUtils.h"

#include "UICreateProject.h"

namespace AtomicEditor
{

// UIBuildSettings------------------------------------------------

UICreateProject::UICreateProject(Context* context, const String &templateFolder, const String &imagePath):
    UIModalOpWindow(context)
  , projectPathField_(0)
  , templateFolder_(templateFolder)
{
    Editor* editor = GetSubsystem<Editor>();
    Project* project = editor->GetProject();

    UI* tbui = GetSubsystem<UI>();
    window_->SetText("Create Project");
    tbui->LoadResourceFile(window_->GetContentRoot(), "AtomicEditor/editor/ui/createproject.tb.txt");

    projectPathField_ = delegate_->GetWidgetByIDAndType<TBEditField>(TBIDC("project_path"));
    assert(projectPathField_);

    projectNameField_ = delegate_->GetWidgetByIDAndType<TBEditField>(TBIDC("project_name"));
    assert(projectNameField_);

    TBImageWidget* image = delegate_->GetWidgetByIDAndType<TBImageWidget>(TBIDC("project_image"));
    assert(image);

    if (!imagePath.Length())
        image->SetVisibilility(WIDGET_VISIBILITY_GONE);
    else
        image->SetImage(imagePath.CString());

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

    String userDocuments = fileSystem->GetUserDocumentsDir();

#if defined(ATOMIC_PLATFORM_OSX)
    userDocuments += "Documents/AtomicProjects";
#else
    userDocuments += "AtomicProjects";
#endif

    projectPathField_->SetText(userDocuments.CString());


    window_->ResizeToFitContent();
    Center();
}


UICreateProject::~UICreateProject()
{

}

bool UICreateProject::DoCreate(const String& folder, const String& projectName)
{
    FileSystem* fileSystem = GetSubsystem<FileSystem>();

    fileSystem->CopyDir(templateFolder_ + "/Resources", folder + "/Resources");

    File file(context_, folder + "/" + projectName + ".atomic", FILE_WRITE);
    file.Close();

    return true;
}

bool UICreateProject::OnEvent(const TBWidgetEvent &ev)
{
    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    Editor* editor = GetSubsystem<Editor>();
    UIModalOps* ops = GetSubsystem<UIModalOps>();
    ProjectUtils* utils = context_->GetSubsystem<ProjectUtils>();

    if (ev.type == EVENT_TYPE_CLICK)
    {

        if (ev.target->GetID() == TBIDC("choose_path"))
        {
            String buildPath = utils->NewProjectFileDialog();

            if (buildPath.Length())
                projectPathField_->SetText(buildPath.CString());

            return true;
        }
        else if (ev.target->GetID() == TBIDC("create"))
        {
            String name = projectNameField_->GetText().CStr();
            name = name.Trimmed();
            if (!name.Length())
            {
                editor->PostModalInfo("New Project Editor Error", "Please enter a project name");
                return true;
            }
            String folder = projectPathField_->GetText().CStr();
            folder = folder.Trimmed();
            if (!folder.Length())
            {
                editor->PostModalInfo("New Project Editor Error", "Please choose a root project folder");
                return true;
            }

            folder += "/" + name;

            if (fileSystem->DirExists(folder) || fileSystem->FileExists(folder))
            {
                editor->PostModalInfo("New Project Editor Error",
                                      ToString("%s exists\n\nPlease choose a different root folder or project name", folder.CString()));
                return true;

            }

            if (!fileSystem->DirExists(folder))
            {
                Poco::File dirs(folder.CString());
                dirs.createDirectories();

                if (!fileSystem->DirExists(folder))
                {
                    editor->PostModalInfo("New Project Editor Error",
                                          ToString("Unable to create project folder:\n%s", folder.CString()));
                    return true;
                }

                if (DoCreate(folder, name))
                {
                    SharedPtr<UICreateProject> keepAlive(this);
                    UIModalOps* ops = GetSubsystem<UIModalOps>();
                    ops->Hide();
                    editor->LoadProject(folder + "/" + name + ".atomic");
                    return true;
                }
            }

        }
        else if (ev.target->GetID() == TBIDC("cancel"))
        {
            ops->Hide();
            return true;
        }
    }

    return false;
}

}
