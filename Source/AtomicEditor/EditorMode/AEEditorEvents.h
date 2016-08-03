//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace AtomicEditor
{

static const unsigned FINDTEXT_FLAG_NONE = 0x0;
static const unsigned FINDTEXT_FLAG_CASESENSITIVE = 0x1;
static const unsigned FINDTEXT_FLAG_WHOLEWORD = 0x2;
static const unsigned FINDTEXT_FLAG_WRAP = 0x4;
static const unsigned FINDTEXT_FLAG_NEXT = 0x8;
static const unsigned FINDTEXT_FLAG_PREV = 0x10;

ATOMIC_EVENT(E_FINDTEXT, FindText)
{
    ATOMIC_PARAM(P_FLAGS, Flags);    // int
    ATOMIC_PARAM(P_TEXT, Text);      // string
}

ATOMIC_EVENT(E_FINDTEXTCLOSE, FindTextClose)
{

}

ATOMIC_EVENT(E_FINDTEXTOPEN, FindTextOpen)
{

}

ATOMIC_EVENT(E_JAVASCRIPTSAVED, JavascriptSaved)
{

}

// editor play request
ATOMIC_EVENT(E_EDITORPLAYREQUEST, EditorPlayRequest)
{
    ATOMIC_PARAM(P_MODE, Mode);    // uint (AEPlayerMode)

}

// stop play mode
ATOMIC_EVENT(E_EDITORPLAYSTOP, EditorPlayStop)
{

}

// stop play mode
ATOMIC_EVENT(E_EDITORRESOURCEEDITORCHANGED, EditorResourceEditorChanged)
{
    ATOMIC_PARAM(P_RESOURCEEDITOR, ResourceEditor); // ResourceEditor*
}


// emitted once play has started
ATOMIC_EVENT(E_EDITORPLAYERSTARTED, EditorPlayerStarted)
{
    ATOMIC_PARAM(P_MODE, Mode);    // uint (AEPlayerMode)
}

// emitted once player has paused
ATOMIC_EVENT(E_EDITORPLAYERPAUSED, EditorPlayerPaused)
{

}

// emitted once paused player has resumed
ATOMIC_EVENT(E_EDITORPLAYERRESUMED, EditorPlayerResumed)
{

}

// emitted once play has stopped
ATOMIC_EVENT(E_EDITORPLAYERSTOPPED, EditorPlayerStopped)
{

}

ATOMIC_EVENT(E_EDITORBUILD, EditorBuild)
{
    ATOMIC_PARAM(P_PLATFORM, Platform);      // string
    ATOMIC_PARAM(P_BUILDPATH, BuildPath);    // string
}


// TODO: consolidate all errors to use this message

static const unsigned EDITOR_MODALERROR = 0x1;
static const unsigned EDITOR_MODALINFO = 0x2;


ATOMIC_EVENT(E_EDITORMODAL, EditorModal)
{
    ATOMIC_PARAM(P_TYPE, Type);      // uint (EDITOR_ERROR_MODAL, etc)
    ATOMIC_PARAM(P_TITLE, Title);      // for modal errors, title text
    ATOMIC_PARAM(P_MESSAGE, Message);    // for modal errors, error text
}

ATOMIC_EVENT(E_EDITORACTIVESCENEEDITORCHANGE, EditorActiveSceneEditorChange)
{
    ATOMIC_PARAM(P_SCENEEDITOR, SceneEditor);
}


ATOMIC_EVENT(E_PLAYERERROR, PlayerError)
{
    ATOMIC_PARAM(P_TEXT, Text);      // string
}

ATOMIC_EVENT(E_CONSOLEMESSAGE, ConsoleMessage)
{
    ATOMIC_PARAM(P_TEXT, Text);      // string
}

ATOMIC_EVENT(E_CONSOLEERRORMESSAGE, ConsoleErrorMessage)
{
    ATOMIC_PARAM(P_TEXT, Text);      // string
}

ATOMIC_EVENT(E_PLATFORMCHANGE, PlatformChange)
{
    ATOMIC_PARAM(P_PLATFORM, Platform);      // uint (AEEditorPlatform)
}

ATOMIC_EVENT(E_SUBPROCESSOUTPUT, SubprocessOutput)
{
    ATOMIC_PARAM(P_TEXT, Text);      // string
}

ATOMIC_EVENT(E_SUBPROCESSCOMPLETE, SubprocessComplete)
{
    ATOMIC_PARAM(P_PROCESSKEY, ProcessKey); // unsigned
    ATOMIC_PARAM(P_RETCODE, RetCode);      // int (return code of process)
}

ATOMIC_EVENT(E_CURLCOMPLETE, CurlComplete)
{
    ATOMIC_PARAM(P_CURLREQUEST, Request);      // CurlRequest*
}

ATOMIC_EVENT(E_EDITORSHUTDOWN, EditorShutdown)
{

}

}
