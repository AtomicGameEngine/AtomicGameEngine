// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Core/Object.h>
#include <Atomic/IO/FileSystem.h>

using namespace Atomic;

#define EDITOR_STRING(x) (GetSubsystem<EditorStrings>()->Str(EditorStrings::x))

namespace AtomicEditor
{

class EditorStrings : public Object
{

    OBJECT(EditorStrings);

public:

    enum EditorString
    {
        RevealInFinder,
        ShortcutUndo,
        ShortcutRedo,
        ShortcutCut,
        ShortcutCopy,
        ShortcutPaste,
        ShortcutSelectAll,
        ShortcutFind,
        ShortcutFindNext,
        ShortcutFindPrev,
        ShortcutBeautify,
        ShortcutCloseFile,
        ShortcutSaveFile,
        ShortcutPlay,
        ShortcutBuild,
        ShortcutBuildSettings,
    };


    /// Construct.
    EditorStrings(Context* context);
    /// Destruct.
    ~EditorStrings();

    const String& Str(EditorString estring);

private:

    void InitializeStrings();

    void HandleEditorShutdown(StringHash eventType, VariantMap& eventData);

    HashMap<unsigned, String> stringLookup_;


};


}
