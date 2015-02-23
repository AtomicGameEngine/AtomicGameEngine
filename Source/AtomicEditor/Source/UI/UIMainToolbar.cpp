// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <Atomic/UI/TBUI.h>
#include <Atomic/IO/Log.h>
#include "UIMainToolbar.h"
#include "../AEEvents.h"
#include "../AETypes.h"
#include "../AEEditor.h"

#include "UIMainFrame.h"
#include "UIResourceFrame.h"

using namespace tb;

namespace AtomicEditor
{

MainToolbar::MainToolbar(Context* context) :
    AEWidget(context)
{
    TBUI* tbui = GetSubsystem<TBUI>();
    tbui->LoadResourceFile(delegate_, "AtomicEditor/editor/ui/maintoolbar.tb.txt");
}

MainToolbar::~MainToolbar()
{

}

bool MainToolbar::OnEvent(const TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.target->GetID() == TBIDC("3d_translate") ||
                ev.target->GetID() == TBIDC("3d_rotate") ||
                ev.target->GetID() == TBIDC("3d_scale") )
        {
            ResourceFrame* rframe = GetSubsystem<MainFrame>()->GetResourceFrame();
            rframe->SendCurrentEditorEvent(ev);
        }

        if (ev.target && ev.target->GetID() == TBIDC("maintoolbar_play"))
        {
            if (GetSubsystem<Editor>()->IsPlayingProject())
            {
                SendEvent(E_EDITORPLAYSTOP);
                return true;
            }
            else
            {
                VariantMap eventData;
                eventData[EditorPlayRequest::P_MODE] = (unsigned) AE_PLAYERMODE_WIDGET;
                SendEvent(E_EDITORPLAYREQUEST, eventData);
                return true;
            }
        }
    }

    return false;
}

}
