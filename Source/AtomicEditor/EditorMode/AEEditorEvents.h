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


// emitted when the play button has been pressed in the editor, but the player has not yet started
ATOMIC_EVENT(E_EDITORPLAYREQUEST, EditorPlayRequest)
{
    ATOMIC_PARAM(P_MODE, Mode);    // uint (AEPlayerMode)
}

// Called when the active resource editor has changed
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

// NOT CURRENTLY WIRED
ATOMIC_EVENT(E_EDITORBUILD, EditorBuild)
{
    ATOMIC_PARAM(P_PLATFORM, Platform);      // string
    ATOMIC_PARAM(P_BUILDPATH, BuildPath);    // string
}


// TODO: consolidate all errors to use this message

static const unsigned EDITOR_MODALERROR = 0x1;
static const unsigned EDITOR_MODALINFO = 0x2;

// emitted to display a modal message in the editor
ATOMIC_EVENT(E_EDITORMODAL, EditorModal)
{
    ATOMIC_PARAM(P_TYPE, Type);      // uint (Type can be EDITOR_MODALERROR, EDITOR_MODAL_INFOT)
    ATOMIC_PARAM(P_TITLE, Title);      // string (for modal errors, title text)
    ATOMIC_PARAM(P_MESSAGE, Message);    // string (for modal errors, error text)
}

// emitted when a Scene editor gains focus.  Could be null.
ATOMIC_EVENT(E_EDITORACTIVESCENEEDITORCHANGE, EditorActiveSceneEditorChange)
{
    ATOMIC_PARAM(P_SCENEEDITOR, SceneEditor);  // SceneEditor3D pointer
}

// NOTE: This is not triggered by anything
ATOMIC_EVENT(E_EDITORSCENECLOSED, EditorSceneClosed)
{
    ATOMIC_PARAM(P_SCENE, Scene); // Scene pointer
}

// emitted when a subprocess logs information to the console
ATOMIC_EVENT(E_SUBPROCESSOUTPUT, SubprocessOutput)
{
    ATOMIC_PARAM(P_TEXT, Text);      // string
}

// emitted when a subprocess has completed
ATOMIC_EVENT(E_SUBPROCESSCOMPLETE, SubprocessComplete)
{
    ATOMIC_PARAM(P_PROCESSKEY, ProcessKey); // unsigned
    ATOMIC_PARAM(P_RETCODE, RetCode);      // int (return code of process)
}

// Command called to load a new project
ATOMIC_EVENT(E_EDITORLOADPROJECT, EditorLoadProject)
{
    ATOMIC_PARAM(P_PATH, Path);     // string
}

// emitted once a content folder has been refreshed
ATOMIC_EVENT(E_CONTENTFOLDERCHANGED, ContentFolderChanged)
{
    ATOMIC_PARAM(P_PATH, Path);     // string
}

// emitted when the editor has been requested to close the current project.
ATOMIC_EVENT(E_EDITORCLOSEPROJECT, EditorCloseProject)
{

}

// emitted once a project has completed closing
ATOMIC_EVENT(E_EDITORPROJECTCLOSED, EditorProjectClosed)
{

}

// command to save all the open editors
ATOMIC_EVENT(E_EDITORSAVEALLRESOURCES, EditorSaveAllResources)
{

}

// command to save a specific resource
ATOMIC_EVENT(E_EDITORSAVERESOURCE, EditorSaveResource)
{
    ATOMIC_PARAM(P_PATH, Path);     // string full path of the current resource
}

// emitted once the resource has been saved
ATOMIC_EVENT(E_EDITORSAVERESOURCENOTIFICATION, EditorSaveResourceNotification)
{
    ATOMIC_PARAM(P_PATH, Path);     // string
}

// emitted when a resource gains focus
ATOMIC_EVENT(E_EDITOREDITRESOURCE, EditorEditResource)
{
    ATOMIC_PARAM(P_PATH, Path);     // string
    ATOMIC_PARAM(P_LINENUMBER, LineNumber); // int optional
}

// command to delete a resource
ATOMIC_EVENT(E_EDITORDELETERESOURCE, EditorDeleteResource)
{
    ATOMIC_PARAM(P_PATH, Path);     // string
}

// emitted once the resource has been deleted
ATOMIC_EVENT(E_EDITORDELETERESOURCENOTIFICATION, EditorDeleteResourceNotification)
{
    ATOMIC_PARAM(P_PATH, Path);     // string
}

// emitted when a resource has been renamed
ATOMIC_EVENT(E_EDITORRENAMERESOURCENOTIFICATION, EditorRenameResourceNotification)
{
    ATOMIC_PARAM(P_PATH, Path);     // string (full path of the old resource)
    ATOMIC_PARAM(P_NEWPATH, NewPath);   // string (full path of the new resource)
    ATOMIC_PARAM(P_NEWNAME, NewName);    // string (new name of resource)
    ATOMIC_PARAM(P_ASSET, Asset);       // ToolCore.Asset pointer
}

// emitted when user preferences have been changed
ATOMIC_EVENT(E_USERPREFERENCESCHANGEDNOTIFICATION, UserPreferencesChangedNotification)
{
   ATOMIC_PARAM(P_PROJECTPREFERENCES, ProjectPreferences);  // string (JSON object)
   ATOMIC_PARAM(P_APPLICATIONPREFERENCES, ApplicationPreferences);        // string (JSON object)
}

// emitted when an item is selected in a scene
ATOMIC_EVENT(E_INSPECTORPROJECTREFERENCE, InspectorProjectReference)
{
    ATOMIC_PARAM(P_PATH, Path);     // string (Full path of the resource to edit)
}

// emitted when a component attribute has been changed
ATOMIC_EVENT(E_ATTRIBUTEEDITRESOURCECHANGED, AttributeEditResourceChanged)
{
    ATOMIC_PARAM(P_ATTRINFOEDIT, AttrInfoEdit); // AttrbuteInfoEdit window
    ATOMIC_PARAM(P_RESOURCE, Resource); // Resource pointer
}

// emitted when the built in player logs something
ATOMIC_EVENT(E_EDITORPLAYERLOG, EditorPlayerLog)
{
    ATOMIC_PARAM(P_MESSAGE, Message);   // string
    ATOMIC_PARAM(P_LEVEL, Level);       // int
}

// emitted right before a project is unloaded
ATOMIC_EVENT(E_PROJECTUNLOADEDNOTIFICATION, ProjectUnloadedNotification)
{

}

// command to load a new project
ATOMIC_EVENT(E_REQUESTPROJECTLOAD, RequestProjectLoad)
{
    ATOMIC_PARAM(P_PATH, Path);     // string (Full path to the .atomic file)
}

// emitted once a project has been loaded
ATOMIC_EVENT(E_LOADPROJECTNOTIFICATION,LoadProjectNotification)
{
    ATOMIC_PARAM(P_PATH, Path);     // string (Full path to the .atomic file)
}

}
