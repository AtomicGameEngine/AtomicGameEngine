// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <TurboBadger/tb_window.h>
#include <TurboBadger/tb_select.h>
#include <TurboBadger/tb_editfield.h>

#include <Atomic/Core/Context.h>
#include <Atomic/UI/UI.h>

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

    UI* tbui = GetSubsystem<UI>();
    window_->SetText("Project Type");
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

            if (licenseSystem->IsStandardLicense())
            {
                SharedPtr<UINewProject> keepAlive(this);
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
            FileSystem* fileSystem = GetSubsystem<FileSystem>();

        #ifdef ATOMIC_PLATFORM_OSX
            String templateSourceDir = fileSystem->GetAppBundleResourceFolder();
        #else
            String templateSourceDir = fileSystem->GetProgramDir();
        #endif

            if (projectType == 0)
                templateSourceDir += "/ProjectTemplates/EmptyProject";
            else if (projectType == 1)
                templateSourceDir += "/ProjectTemplates/Project2D";
            else
                templateSourceDir += "/ProjectTemplates/Project3D";

            SharedPtr<UINewProject> keepAlive(this);
            UIModalOps* ops = GetSubsystem<UIModalOps>();
            ops->Hide();
            ops->ShowCreateProject(templateSourceDir);

            return true;

        }

    }

    return false;
}

}
