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

EVENT(E_FINDTEXT, FindText)
{
    PARAM(P_FLAGS, Flags);    // int
    PARAM(P_TEXT, Text);      // string
}

EVENT(E_FINDTEXTCLOSE, FindTextClose)
{

}

EVENT(E_FINDTEXTOPEN, FindTextOpen)
{

}

EVENT(E_JAVASCRIPTSAVED, JavascriptSaved)
{

}

// editor play request
EVENT(E_EDITORPLAYREQUEST, EditorPlayRequest)
{
    PARAM(P_MODE, Mode);    // uint (AEPlayerMode)

}

// stop play mode
EVENT(E_EDITORPLAYSTOP, EditorPlayStop)
{

}

// stop play mode
EVENT(E_EDITORRESOURCEEDITORCHANGED, EditorResourceEditorChanged)
{
    PARAM(P_RESOURCEEDITOR, ResourceEditor); // ResourceEditor*
}


// emitted once play has started
EVENT(E_EDITORPLAYERSTARTED, EditorPlayerStarted)
{
    PARAM(P_MODE, Mode);    // uint (AEPlayerMode)
}

// emitted once player has paused
EVENT(E_EDITORPLAYERPAUSED, EditorPlayerPaused)
{

}

// emitted once paused player has resumed
EVENT(E_EDITORPLAYERRESUMED, EditorPlayerResumed)
{

}

// emitted once play has stopped
EVENT(E_EDITORPLAYERSTOPPED, EditorPlayerStopped)
{

}

EVENT(E_EDITORBUILD, EditorBuild)
{
    PARAM(P_PLATFORM, Platform);      // string
    PARAM(P_BUILDPATH, BuildPath);    // string
}


// TODO: consolidate all errors to use this message

static const unsigned EDITOR_MODALERROR = 0x1;
static const unsigned EDITOR_MODALINFO = 0x2;


EVENT(E_EDITORMODAL, EditorModal)
{
    PARAM(P_TYPE, Type);      // uint (EDITOR_ERROR_MODAL, etc)
    PARAM(P_TITLE, Title);      // for modal errors, title text
    PARAM(P_MESSAGE, Message);    // for modal errors, error text
}

EVENT(E_EDITORACTIVESCENEEDITORCHANGE, EditorActiveSceneEditorChange)
{
    PARAM(P_SCENEEDITOR, SceneEditor);
}


EVENT(E_PLAYERERROR, PlayerError)
{
    PARAM(P_TEXT, Text);      // string
}

EVENT(E_CONSOLEMESSAGE, ConsoleMessage)
{
    PARAM(P_TEXT, Text);      // string
}

EVENT(E_CONSOLEERRORMESSAGE, ConsoleErrorMessage)
{
    PARAM(P_TEXT, Text);      // string
}

EVENT(E_PLATFORMCHANGE, PlatformChange)
{
    PARAM(P_PLATFORM, Platform);      // uint (AEEditorPlatform)
}

EVENT(E_SUBPROCESSOUTPUT, SubprocessOutput)
{
    PARAM(P_TEXT, Text);      // string
}

EVENT(E_SUBPROCESSCOMPLETE, SubprocessComplete)
{
    PARAM(P_PROCESSKEY, ProcessKey); // unsigned
    PARAM(P_RETCODE, RetCode);      // int (return code of process)
}

EVENT(E_CURLCOMPLETE, CurlComplete)
{
    PARAM(P_CURLREQUEST, Request);      // CurlRequest*
}

EVENT(E_EDITORSHUTDOWN, EditorShutdown)
{

}

}
