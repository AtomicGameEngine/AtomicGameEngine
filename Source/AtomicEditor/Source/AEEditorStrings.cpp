
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <Atomic/Core/Context.h>

#include "AEEvents.h"
#include "AEEditorStrings.h"

namespace AtomicEditor
{

EditorStrings::EditorStrings(Context* context) :
    Object(context)
{
    InitializeStrings();
    SubscribeToEvent(E_EDITORSHUTDOWN, HANDLER(EditorStrings, HandleEditorShutdown));
}

EditorStrings::~EditorStrings()
{

}

const String& EditorStrings::Str(EditorString estring)
{
    if (stringLookup_.Contains(estring))
        return stringLookup_[estring];
    return String::EMPTY;
}

void EditorStrings::InitializeStrings()
{

    stringLookup_[RevealInFinder] = "Reveal in Finder";

    // Shortcuts
    stringLookup_[ShortcutUndo] = "⌘Z";
    stringLookup_[ShortcutRedo] ="⇧⌘Z";
    stringLookup_[ShortcutCut] = "⌘X";
    stringLookup_[ShortcutCopy] = "⌘C";
    stringLookup_[ShortcutPaste] = "⌘V";
    stringLookup_[ShortcutFind] = "⌘F";
    stringLookup_[ShortcutBeautify] = "⌘B";

    stringLookup_[ShortcutSaveFile] = "⌘S";
    stringLookup_[ShortcutCloseFile] = "⌘W";

    stringLookup_[ShortcutPlay] = "⌘P";


}

void EditorStrings::HandleEditorShutdown(StringHash eventType, VariantMap& eventData)
{
    context_->RemoveSubsystem(GetType());
}

}
