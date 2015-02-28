// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <TurboBadger/tb_window.h>
#include <TurboBadger/tb_select.h>
#include <TurboBadger/tb_editfield.h>

#include <Atomic/Core/Context.h>
#include <Atomic/UI/TBUI.h>

#include "License/AELicenseSystem.h"

#include "Resources/AEResourceOps.h"
#include "AEPreferences.h"

#include "AEEditor.h"
#include "AEEvents.h"
#include "Project/AEProject.h"
#include "Project/ProjectUtils.h"

#include "UINewProject.h"

namespace AtomicEditor
{

UINewProject::UINewProject(Context* context):
    UIModalOpWindow(context)
{
    Editor* editor = GetSubsystem<Editor>();
    Project* project = editor->GetProject();

    TBUI* tbui = GetSubsystem<TBUI>();
    window_->SetText("New Project");
    tbui->LoadResourceFile(window_->GetContentRoot(), "AtomicEditor/editor/ui/newproject.tb.txt");

    window_->ResizeToFitContent();
    Center();
}


UINewProject::~UINewProject()
{

}

bool UINewProject::Create2DProject(const String& projectPath, const String& filename)
{
    FileSystem* fileSystem = GetSubsystem<FileSystem>();

#ifdef ATOMIC_PLATFORM_OSX
    String templateSourceDir = fileSystem->GetAppBundleResourceFolder();
#else
    String templateSourceDir = fileSystem->GetProgramDir();
#endif

    templateSourceDir += "/ProjectTemplates/Project2D";
    fileSystem->CopyDir(templateSourceDir + "/Resources", projectPath + "/Resources");

    File file(context_, projectPath + "/" + filename + ".atomic", FILE_WRITE);
    file.Close();

    return true;
}

bool UINewProject::CreateEmptyProject(const String& projectPath, const String &filename)
{
    FileSystem* fileSystem = GetSubsystem<FileSystem>();

#ifdef ATOMIC_PLATFORM_OSX
    String templateSourceDir = fileSystem->GetAppBundleResourceFolder();
#else
    String templateSourceDir = fileSystem->GetProgramDir();
#endif

    templateSourceDir += "/ProjectTemplates/EmptyProject";

    fileSystem->CopyDir(templateSourceDir + "/Resources", projectPath + "/Resources");

    File file(context_, projectPath + "/" + filename + ".atomic", FILE_WRITE);
    file.Close();

    return true;
}

bool UINewProject::Create3DProject(const String& projectPath, const String &filename)
{

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

#ifdef ATOMIC_PLATFORM_OSX
    String templateSourceDir = fileSystem->GetAppBundleResourceFolder();
#else
    String templateSourceDir = fileSystem->GetProgramDir();
#endif

    templateSourceDir += "/ProjectTemplates/Project3D";
    fileSystem->CopyDir(templateSourceDir + "/Resources", projectPath + "/Resources");

    File file(context_, projectPath + "/" + filename + ".atomic", FILE_WRITE);
    file.Close();

    return true;
}


bool UINewProject::OnEvent(const TBWidgetEvent &ev)
{
    Editor* editor = GetSubsystem<Editor>();
    UIModalOps* ops = GetSubsystem<UIModalOps>();

    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.target->GetID() == TBIDC("cancel"))
        {
            ops->Hide();
            return true;
        }

        int projectType = -1;

        if (ev.target->GetID() == TBIDC("project_empty"))
        {
            projectType = 0;
        }
        else if (ev.target->GetID() == TBIDC("project_2d"))
        {
            projectType = 1;
        }
        else if (ev.target->GetID() == TBIDC("project_3d"))
        {
// BEGIN LICENSE MANAGEMENT
            LicenseSystem* licenseSystem = GetSubsystem<LicenseSystem>();

            if (licenseSystem->IsStarterLicense())
            {
                SharedPtr<UINewProject>(this);
                UIModalOps* ops = GetSubsystem<UIModalOps>();
                ops->Hide();
                ops->ShowInfoModule3D();
                return true;
            }
// END LICENSE MANAGEMENT

            projectType = 2;
        }

        if (projectType != -1)
        {
            String fullProjectPath = GetSubsystem<ProjectUtils>()->NewProjectFileDialog();

            if (!fullProjectPath.Length())
                return true;

            String projectPath;
            String fileName;
            String ext;

            SplitPath(fullProjectPath, projectPath, fileName, ext);

            FileSystem* fileSystem = GetSubsystem<FileSystem>();
            if (!fileSystem->DirExists(projectPath))
            {
                editor->PostModalInfo("New Project Editor Error", "Project folder does not exist");
                return true;
            }
            Vector<String> results;
            fileSystem->ScanDir(results, projectPath, "*", SCAN_DIRS | SCAN_FILES, false);
            while (results.Remove(".")) {}
            while (results.Remove("..")) {}

            if (results.Size())
            {
                editor->PostModalInfo("New Project Editor Error", "Project folder must be empty.\nPlease create a new folder or select an empty one");
                return true;
            }

            bool result = false;

            if (projectType == 0)
            {
                result = CreateEmptyProject(projectPath, fileName);
            }
            else if (projectType == 1)
            {
                result = Create2DProject(projectPath, fileName);
            }
            else if (projectType == 2)
            {
                result = Create3DProject(projectPath, fileName);
            }

            if (!result)
            {
                editor->PostModalInfo("New Project Editor Error", "Error Creating Project");
                return true;
            }

            Vector<String> elements = projectPath.Split('/');
            String projectName = elements.Back();

            editor->LoadProject(projectPath + "/" + projectName + ".atomic");

            ops->Hide();
            return true;
        }

    }

    return false;
}

}
