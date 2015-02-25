
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

#ifdef ATOMIC_PLATFORM_WINDOWS
    String shortcutKey = "Ctrl-";
    stringLookup_[ShortcutRedo] ="Ctrl-Y";
    stringLookup_[ShortcutFindNext] = "F3";
    stringLookup_[ShortcutFindPrev] = "Shift-F3";
    stringLookup_[ShortcutBuildSettings] = "Ctrl-Shift-B";
#else
    String shortcutKey = "⌘";
    stringLookup_[ShortcutRedo] ="⇧⌘Z";
    stringLookup_[ShortcutFindNext] = "⌘G";
    stringLookup_[ShortcutFindPrev] = "⇧⌘G";
    stringLookup_[ShortcutBuildSettings] = "⇧⌘B";

#endif
    // Shortcuts
    stringLookup_[ShortcutUndo] = shortcutKey + "Z";

    stringLookup_[ShortcutCut] = shortcutKey + "X";
    stringLookup_[ShortcutCopy] = shortcutKey + "C";
    stringLookup_[ShortcutPaste] = shortcutKey + "V";
    stringLookup_[ShortcutSelectAll] = shortcutKey + "A";
    stringLookup_[ShortcutFind] = shortcutKey + "F";

    stringLookup_[ShortcutBeautify] = shortcutKey + "I";

    stringLookup_[ShortcutSaveFile] = shortcutKey + "S";
    stringLookup_[ShortcutCloseFile] = shortcutKey + "W";

    stringLookup_[ShortcutPlay] = shortcutKey + "P";

    stringLookup_[ShortcutBuild] = shortcutKey +"B";



}

void EditorStrings::HandleEditorShutdown(StringHash eventType, VariantMap& eventData)
{
    context_->RemoveSubsystem(GetType());
}

}
