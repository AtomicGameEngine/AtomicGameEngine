// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <TurboBadger/tb_window.h>
#include <TurboBadger/tb_select.h>
#include <TurboBadger/tb_editfield.h>

#include <Atomic/Core/Context.h>
#include <Atomic/Core/CoreEvents.h>
#include <Atomic/IO/Log.h>
#include <Atomic/UI/UI.h>

#include <Atomic/Graphics/Graphics.h>
#include <Atomic/Graphics/Viewport.h>
#include <Atomic/Graphics/Camera.h>

#include "UI/Modal/UIModalOps.h"
#include "License/AELicenseSystem.h"

#include "AEEditor.h"
#include "AEEvents.h"

#include "UIPlayer.h"

namespace Atomic
{
    void FixMeSetLight2DGroupViewport(Viewport *viewport);
}

namespace AtomicEditor
{

// UIBuildSettings------------------------------------------------

UIPlayer::UIPlayer(Context* context):
    UIModalOpWindow(context)
{

    aePlayer_ = GetSubsystem<AEPlayer>();
    aePlayer_->SetUIPlayer(this);

    UI* tbui = GetSubsystem<UI>();

    window_->SetSettings(WINDOW_SETTINGS_DEFAULT & ~WINDOW_SETTINGS_CLOSE_BUTTON);

    window_->SetText("Atomic Player");
    tbui->LoadResourceFile(window_->GetContentRoot(), "AtomicEditor/editor/ui/playerwidget.tb.txt");

    Graphics* graphics = GetSubsystem<Graphics>();

    float gwidth = graphics->GetWidth();
    float aspect = float(graphics->GetHeight())/ gwidth;
    gwidth -= 150;

    playerSize_.x_ = gwidth;
    playerSize_.y_ = gwidth * aspect;

    TBLayout* playercontainer = window_->GetWidgetByIDAndType<TBLayout>(TBIDC("playerlayout"));
    assert(playercontainer);

    window_->ResizeToFitContent();

    Center();    

    SubscribeToEvent(E_UPDATE, HANDLER(UIPlayer, HandleUpdate));

}


UIPlayer::~UIPlayer()
{

}

void UIPlayer::HandleUpdate(StringHash eventType, VariantMap& eventData)
{

}


bool UIPlayer::OnEvent(const TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.target->GetID() == TBIDC("play"))
        {
            if (GetSubsystem<Editor>()->IsPlayingProject())
            {
                SendEvent(E_EDITORPLAYSTOP);                               
                return true;
            }
        }
    }

    return true;
}



}


