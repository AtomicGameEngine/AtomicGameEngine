

#include "../Core/Object.h"

namespace Atomic
{

EVENT(E_PREFABSAVE, PrefabSave)
{
    PARAM(P_PREFABCOMPONENT, PrefabComponent); // PrefabComponent Pointer
}

/// Instead of using resource reload system which can be delayed due to file watchers, we explicitly send prefab changes
EVENT(E_PREFABCHANGED, PrefabChanged)
{
    PARAM(P_GUID, GUID); // String
}

}
