//
// Copyright (c) 2008-2016 the Urho3D project.
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

#include "../Core/Object.h"

namespace Atomic
{

/// Resource reloading started.
ATOMIC_EVENT(E_RELOADSTARTED, ReloadStarted)
{
}

/// Resource reloading finished successfully.
ATOMIC_EVENT(E_RELOADFINISHED, ReloadFinished)
{
}

/// Resource reloading failed.
ATOMIC_EVENT(E_RELOADFAILED, ReloadFailed)
{
}

/// Tracked file changed in the resource directories.
ATOMIC_EVENT(E_FILECHANGED, FileChanged)
{
    ATOMIC_PARAM(P_FILENAME, FileName);                    // String
    ATOMIC_PARAM(P_RESOURCENAME, ResourceName);            // String
}

/// Resource loading failed.
ATOMIC_EVENT(E_LOADFAILED, LoadFailed)
{
    ATOMIC_PARAM(P_RESOURCENAME, ResourceName);            // String
}

/// Resource not found.
ATOMIC_EVENT(E_RESOURCENOTFOUND, ResourceNotFound)
{
    ATOMIC_PARAM(P_RESOURCENAME, ResourceName);            // String
}

/// Unknown resource type.
ATOMIC_EVENT(E_UNKNOWNRESOURCETYPE, UnknownResourceType)
{
    ATOMIC_PARAM(P_RESOURCETYPE, ResourceType);            // StringHash
}

/// Resource background loading finished.
ATOMIC_EVENT(E_RESOURCEBACKGROUNDLOADED, ResourceBackgroundLoaded)
{
    ATOMIC_PARAM(P_RESOURCENAME, ResourceName);            // String
    ATOMIC_PARAM(P_SUCCESS, Success);                      // bool
    ATOMIC_PARAM(P_RESOURCE, Resource);                    // Resource pointer
}

/// Language changed.
ATOMIC_EVENT(E_CHANGELANGUAGE, ChangeLanguage)
{
}

// ATOMIC BEGIN

/// Resource was renamed
ATOMIC_EVENT(E_RENAMERESOURCENOTIFICATION, RenameResourceNotification)
{
    ATOMIC_PARAM(P_RESOURCEPATH, Path);                    // String
    ATOMIC_PARAM(P_NEWRESOURCEPATH, NewPath);              // String
    ATOMIC_PARAM(P_NEWNAME, NewName);                      // String
    ATOMIC_PARAM(P_RESOURCE, Asset);                       // Resource pointer
}

// ATOMIC END

}
