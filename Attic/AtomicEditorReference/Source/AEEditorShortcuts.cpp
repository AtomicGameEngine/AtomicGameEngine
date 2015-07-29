// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <rapidjson/document.h>
#include "rapidjson/prettywriter.h"
#include "rapidjson/filestream.h"

#include <Atomic/Core/Context.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/Input/Input.h>
#include <Atomic/Input/InputEvents.h>

#include "UI/UIMainFrame.h"
#include "UI/UIResourceFrame.h"
#include "UI/Modal/UIModalOps.h"

#include "AEEditor.h"
#include "AEEvents.h"
#include "AEEditorShortcuts.h"

using namespace rapidjson;

namespace AtomicEditor
{

EditorShortcuts::EditorShortcuts(Context* context) :
    Object(context)
{
    SubscribeToEvent(E_KEYDOWN, HANDLER(EditorShortcuts, HandleKeyDown));
    SubscribeToEvent(E_KEYUP, HANDLER(EditorShortcuts, HandleKeyUp));
    SubscribeToEvent(E_EDITORSHUTDOWN, HANDLER(EditorShortcuts, HandleEditorShutdown));
}

EditorShortcuts::~EditorShortcuts()
{

}

void EditorShortcuts::HandleEditorShutdown(StringHash eventType, VariantMap& eventData)
{
    context_->RemoveSubsystem(GetType());
}

bool EditorShortcuts::IsProjectLoaded()
{
    Editor* editor = context_->GetSubsystem<Editor>();
    return editor->GetProject() != NULL;
}

void EditorShortcuts::InvokePlayStop()
{
    if (!IsProjectLoaded())
        return;

    Editor* editor = context_->GetSubsystem<Editor>();

    if (!editor->IsPlayingProject())
    {
        UIModalOps* ops = GetSubsystem<UIModalOps>();
        if (ops->ModalActive())
            return;

        VariantMap eventData;
        eventData[EditorPlayRequest::P_MODE] = (unsigned) AE_PLAYERMODE_WIDGET;
        SendEvent(E_EDITORPLAYREQUEST, eventData);
    }
    else
    {
        SendEvent(E_EDITORPLAYSTOP);
    }
}

void EditorShortcuts::InvokeResourceFrameShortcut(const String& id)
{
    if (!IsProjectLoaded())
        return;

    MainFrame* frame = GetSubsystem<MainFrame>();
    ResourceFrame* resourceFrame = frame->GetResourceFrame();
    TBWidgetEvent ev(EVENT_TYPE_SHORTCUT);
    ev.ref_id = TBIDC(id.CString());
    resourceFrame->OnEvent(ev);

}

void EditorShortcuts::InvokeFileSave()
{
    InvokeResourceFrameShortcut("save");
}

void EditorShortcuts::InvokeFileClose()
{
    InvokeResourceFrameShortcut("close");
}

void EditorShortcuts::InvokeEditCut()
{
    InvokeResourceFrameShortcut("cut");
}

void EditorShortcuts::InvokeEditCopy()
{
    InvokeResourceFrameShortcut("copy");
}

void EditorShortcuts::InvokeEditPaste()
{
    InvokeResourceFrameShortcut("paste");
}

void EditorShortcuts::InvokeEditSelectAll()
{
    InvokeResourceFrameShortcut("selectall");
}

void EditorShortcuts::InvokeEditUndo()
{
    InvokeResourceFrameShortcut("undo");
}

void EditorShortcuts::InvokeEditRedo()
{
    InvokeResourceFrameShortcut("redo");
}

void EditorShortcuts::InvokeEditFind()
{
    InvokeResourceFrameShortcut("find");
}

void EditorShortcuts::InvokeEditFindNext()
{
    InvokeResourceFrameShortcut("findnext");
}

void EditorShortcuts::InvokeEditFindPrev()
{
    InvokeResourceFrameShortcut("findprev");
}

void EditorShortcuts::InvokeEditFormatCode()
{
    InvokeResourceFrameShortcut("beautify");
}

void EditorShortcuts::InvokeBuild()
{
    if (!IsProjectLoaded())
        return;

    UIModalOps* ops = GetSubsystem<UIModalOps>();

    if (!ops->ModalActive())
        ops->ShowBuild();

}

void EditorShortcuts::InvokeBuildSettings()
{
    if (!IsProjectLoaded())
        return;

    UIModalOps* ops = GetSubsystem<UIModalOps>();

    if (!ops->ModalActive())
        ops->ShowBuildSettings();

}

void EditorShortcuts::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
    using namespace KeyDown;

    Input* input = context_->GetSubsystem<Input>();

#ifdef ATOMIC_PLATFORM_WINDOWS
    bool cmdKey = (input->GetKeyDown(KEY_LCTRL) || input->GetKeyDown(KEY_RCTRL));
#else
    bool cmdKey = (input->GetKeyDown(KEY_LGUI) || input->GetKeyDown(KEY_RGUI));
#endif

    int keycode = eventData[P_KEY].GetInt();
    int qualifiers = eventData[P_QUALIFIERS].GetInt();


    if (cmdKey && keycode == KEY_P)
    {
        InvokePlayStop();
    }
    else if (cmdKey && (qualifiers & QUAL_SHIFT) && keycode == KEY_B)
    {
        InvokeBuildSettings();
    }
    else if (cmdKey && keycode == KEY_B)
    {
        InvokeBuild();
    }


}

void EditorShortcuts::HandleKeyUp(StringHash eventType, VariantMap& eventData)
{
    using namespace KeyUp;


}


}
