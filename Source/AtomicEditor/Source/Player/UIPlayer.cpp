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
#include <Atomic/UI/TBUI.h>

#include <Atomic/Graphics/Graphics.h>
#include <Atomic/Graphics/Viewport.h>

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

    TBUI* tbui = GetSubsystem<TBUI>();
    window_->SetText("Atomic Player");
    tbui->LoadResourceFile(window_->GetContentRoot(), "AtomicEditor/editor/ui/playerwidget.tb.txt");

    Graphics* graphics = GetSubsystem<Graphics>();

    view3D_ = new View3D(context_);

    // horrible hack
    Viewport* viewport = view3D_->GetViewport();
    FixMeSetLight2DGroupViewport(viewport);

    TBWidgetDelegate* view3DDelegate = view3D_->GetWidgetDelegate();

    float gwidth = graphics->GetWidth();
    float aspect = float(graphics->GetHeight())/ gwidth;
    gwidth -= 150;

    LayoutParams lp;
    lp.SetWidth(gwidth);
    lp.SetHeight(gwidth * aspect);
    view3DDelegate->SetLayoutParams(lp);

    TBLayout* playercontainer = window_->GetWidgetByIDAndType<TBLayout>(TBIDC("playerlayout"));
    assert(playercontainer);

    playercontainer->AddChild(view3DDelegate);

    window_->ResizeToFitContent();

    Center();

    SubscribeToEvent(E_UPDATE, HANDLER(UIPlayer, HandleUpdate));
}

void UIPlayer::SetView(Scene* scene, Camera* camera)
{
    view3D_->SetView(scene, camera);
}

UIPlayer::~UIPlayer()
{

}

void UIPlayer::HandleUpdate(StringHash eventType, VariantMap& eventData)
{

    view3D_->QueueUpdate();
    UI* ui = GetSubsystem<UI>();

    TBWidgetDelegate* view3DDelegate = view3D_->GetWidgetDelegate();
    TBRect rect = view3DDelegate->GetRect();
    view3DDelegate->ConvertToRoot(rect.x, rect.y);

    ui->GetRoot()->SetPosition(rect.x, rect.y);
    ui->GetRoot()->SetSize(rect.w, rect.h);
}


bool UIPlayer::OnEvent(const TBWidgetEvent &ev)
{
    return false;
}



}


