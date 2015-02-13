// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicEditor

#include "AtomicEditor.h"
#include <TurboBadger/tb_layout.h>
#include <Atomic/Core/Context.h>
#include <Atomic/IO/Log.h>
#include <Atomic/UI/TBUI.h>

#include "UIBuildSettingsWeb.h"

namespace AtomicEditor
{

UIBuildSettingsWeb::UIBuildSettingsWeb(Context* context) :
    AEWidget(context)
{
    TBUI* tbui = GetSubsystem<TBUI>();
    tbui->LoadResourceFile(delegate_, "AtomicEditor/editor/ui/buildsettings_windows.tb.txt");
}

UIBuildSettingsWeb::~UIBuildSettingsWeb()
{

}

bool UIBuildSettingsWeb::OnEvent(const TBWidgetEvent &ev)
{
    return false;
}



}

