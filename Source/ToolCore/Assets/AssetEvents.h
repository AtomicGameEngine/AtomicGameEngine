

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


}
