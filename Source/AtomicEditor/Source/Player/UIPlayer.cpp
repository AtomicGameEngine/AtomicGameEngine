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

// BEGIN LICENSE MANAGEMENT
    LicenseSystem* license = GetSubsystem<LicenseSystem>();
    starterLicense_ = license->IsStarterLicense();
// END LICENSE MANAGEMENT

    aePlayer_ = GetSubsystem<AEPlayer>();
    aePlayer_->SetUIPlayer(this);

    TBUI* tbui = GetSubsystem<TBUI>();

    // FIXME: disabling close button as having the widget die is currently bad
    window_->DisableCloseButton();

    window_->SetText("Atomic Player");
    tbui->LoadResourceFile(window_->GetContentRoot(), "AtomicEditor/editor/ui/playerwidget.tb.txt");

    Graphics* graphics = GetSubsystem<Graphics>();

    view3D_ = new UIView3D(context_);

    // horrible hack
    Viewport* viewport = view3D_->GetViewport();
    FixMeSetLight2DGroupViewport(viewport);

    TBWidgetDelegate* view3DDelegate = view3D_->GetWidgetDelegate();

    float gwidth = graphics->GetWidth();
    float aspect = float(graphics->GetHeight())/ gwidth;
    gwidth -= 150;

    playerSize_.x_ = gwidth;
    playerSize_.y_ = gwidth * aspect;

    LayoutParams lp;
    lp.SetWidth(playerSize_.x_);
    lp.SetHeight(playerSize_.y_);
    view3DDelegate->SetLayoutParams(lp);

    TBLayout* playercontainer = window_->GetWidgetByIDAndType<TBLayout>(TBIDC("playerlayout"));
    assert(playercontainer);

    playercontainer->AddChild(view3DDelegate);

    window_->ResizeToFitContent();

    Center();    

    SubscribeToEvent(E_UPDATE, HANDLER(UIPlayer, HandleUpdate));

}

Viewport *UIPlayer::SetView(Scene* scene, Camera* camera)
{
    view3D_->SetView(scene, camera);
    return view3D_->GetViewport();
}

UIPlayer::~UIPlayer()
{

}

void UIPlayer::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    view3D_->QueueUpdate();
    UI* ui = GetSubsystem<UI>();

    // BEGIN LICENSE MANAGEMENT
    if (starterLicense_)
    {
        Camera* camera = view3D_->GetViewport()->GetCamera();
        if (camera && !camera->IsOrthographic())
        {
            SendEvent(E_EDITORPLAYSTOP);
            UIModalOps* ops = GetSubsystem<UIModalOps>();
            ops->ShowInfoModule3D();
            return;
        }
    }
    // END LICENSE MANAGEMENT

    TBWidgetDelegate* view3DDelegate = view3D_->GetWidgetDelegate();
    TBRect rect = view3DDelegate->GetRect();
    view3DDelegate->ConvertToRoot(rect.x, rect.y);

    ui->GetRoot()->SetPosition(rect.x, rect.y);
    ui->GetRoot()->SetSize(rect.w, rect.h);
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


