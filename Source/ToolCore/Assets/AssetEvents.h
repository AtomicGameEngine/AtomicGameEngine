//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/Core/Object.h>

namespace ToolCore
{


EVENT(E_RESOURCEADDED, ResourceAdded)
{
    PARAM(P_GUID, GUID);                  // string
}

EVENT(E_RESOURCEREMOVED, ResourceRemoved)
{
    PARAM(P_GUID, GUID);                  // string
}

EVENT(E_ASSETIMPORTERROR, AssetImportError)
{
    PARAM(P_PATH, Path);                  // string
    PARAM(P_GUID, GUID);                  // string
    PARAM(P_ERROR, Error);                  // string
}

EVENT(E_ASSETRENAMED, AssetRenamed)
{
    PARAM(P_ASSET, Asset);                  // asset ptr
}

EVENT(E_ASSETMOVED, AssetMoved)
{
    PARAM(P_ASSET, Asset);                  // asset ptr
    PARAM(P_OLDPATH, OldPath);                  // string

}

}
