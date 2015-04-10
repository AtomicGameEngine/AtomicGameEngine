// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <Atomic/UI/UI.h>
#include <Atomic/IO/Log.h>

#include "UIPlayerWidget.h"

using namespace tb;

namespace AtomicEditor
{

PlayerWidget::PlayerWidget(Context* context) :
    AEWidget(context)
{
    UI* tbui = GetSubsystem<UI>();
    tbui->LoadResourceFile(delegate_, "AtomicEditor/editor/ui/playerwidget.tb.txt");
}

PlayerWidget::~PlayerWidget()
{

}

bool PlayerWidget::OnEvent(const TBWidgetEvent &ev)
{
    return false;
}

}
