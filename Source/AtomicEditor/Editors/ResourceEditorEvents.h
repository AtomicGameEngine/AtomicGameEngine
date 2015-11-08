//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Core/Object.h>

namespace AtomicEditor
{

EVENT(E_EDITORRESOURCECLOSE, EditorResourceClose)
{
PARAM(P_EDITOR, Editor);            // ResourceEditor
PARAM(P_NAVIGATE, NavigateToAvailableResource); // bool
}

EVENT(E_EDITORRESOURCECLOSECANCELED, EditorResourceCloseCanceled)
{

}

}
