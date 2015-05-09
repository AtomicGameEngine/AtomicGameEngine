// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <Atomic/UI/UI.h>
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
    UI* tbui = GetSubsystem<UI>();
    tbui->LoadResourceFile(delegate_, "AtomicEditor/editor/ui/maintoolbar.tb.txt");

    Show3DWidgets(false);
}

MainToolbar::~MainToolbar()
{

}

void MainToolbar::Show3DWidgets(bool value)
{
    TBButton* button = delegate_->GetWidgetByIDAndType<TBButton>(TBIDC("3d_translate"));
    if (button)
        button->SetVisibilility( value ? WIDGET_VISIBILITY_VISIBLE : WIDGET_VISIBILITY_GONE);

    button = delegate_->GetWidgetByIDAndType<TBButton>(TBIDC("3d_rotate"));
    if (button)
        button->SetVisibilility( value ? WIDGET_VISIBILITY_VISIBLE : WIDGET_VISIBILITY_GONE);

    button = delegate_->GetWidgetByIDAndType<TBButton>(TBIDC("3d_scale"));
    if (button)
        button->SetVisibilility( value ? WIDGET_VISIBILITY_VISIBLE : WIDGET_VISIBILITY_GONE);
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
