// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <TurboBadger/tb_window.h>
#include <TurboBadger/tb_select.h>
#include <TurboBadger/tb_editfield.h>

#include <Atomic/Core/Context.h>
#include <Atomic/UI/UI.h>

#include "Resources/AEResourceOps.h"
#include "AEPreferences.h"

#include "AEEditor.h"
#include "AEEvents.h"
#include "Project/AEProject.h"
#include "Project/ProjectUtils.h"
#include "License/AELicenseSystem.h"

#include "UIInfoModule3D.h"

namespace AtomicEditor
{

// UIBuildSettings------------------------------------------------

InfoModule3D::InfoModule3D(Context* context,const String &exampleFolder, const String &exampleScreenshot):
    UIModalOpWindow(context)
    , exampleFolder_(exampleFolder)
    , exampleScreenshot_(exampleScreenshot)
{
    Editor* editor = GetSubsystem<Editor>();
    Project* project = editor->GetProject();

    UI* tbui = GetSubsystem<UI>();
    window_->SetSettings(WINDOW_SETTINGS_DEFAULT & ~WINDOW_SETTINGS_CLOSE_BUTTON);
    window_->SetText("Atomic Game Engine Pro Required");

    tbui->LoadResourceFile(window_->GetContentRoot(), "AtomicEditor/editor/ui/infomodule3d.tb.txt");

    window_->ResizeToFitContent();
    Center();

}

InfoModule3D::~InfoModule3D()
{
}

bool InfoModule3D::OnEvent(const TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_CLICK)
    {
        UIModalOps* ops = GetSubsystem<UIModalOps>();
        if (ev.target->GetID() == TBIDC("ok"))
        {
            SharedPtr<InfoModule3D> keepAlive(this);
            ops->Hide();

            if (exampleFolder_.Length())
            {
                ops->ShowCreateProject(exampleFolder_, exampleScreenshot_);
            }

            return true;
        }
        if (ev.target->GetID() == TBIDC("purchase"))
        {

            //Editor* editor = GetSubsystem<Editor>();
            FileSystem* fs = GetSubsystem<FileSystem>();
            fs->SystemOpen("https://store.atomicgameengine.com/site");
            ops->Hide();

            return true;
        }

    }

    return false;
}

}
