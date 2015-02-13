// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <TurboBadger/tb_window.h>
#include <TurboBadger/tb_select.h>
#include <TurboBadger/tb_editfield.h>

#include <Atomic/Core/Context.h>
#include <Atomic/UI/TBUI.h>

#include "Resources/AEResourceOps.h"
#include "AEPreferences.h"

#include "AEEditor.h"
#include "AEEvents.h"
#include "Project/AEProject.h"
#include "Project/ProjectUtils.h"

#include "UIAbout.h"

namespace AtomicEditor
{

// UIBuildSettings------------------------------------------------

UIAbout::UIAbout(Context* context):
    UIModalOpWindow(context)
{
    Editor* editor = GetSubsystem<Editor>();
    Project* project = editor->GetProject();

    TBUI* tbui = GetSubsystem<TBUI>();
    window_->SetText("About the Atomic Game Engine");
    tbui->LoadResourceFile(window_->GetContentRoot(), "AtomicEditor/editor/ui/about.tb.txt");

    window_->ResizeToFitContent();
    Center();
}


UIAbout::~UIAbout()
{
}

bool UIAbout::OnEvent(const TBWidgetEvent &ev)
{


    return false;
}

}
