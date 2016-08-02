

#include "../Core/Object.h"

namespace Atomic
{

ATOMIC_EVENT(E_PREFABSAVE, PrefabSave)
{
    ATOMIC_PARAM(P_PREFABCOMPONENT, PrefabComponent); // PrefabComponent Pointer
}

/// Instead of using resource reload system which can be delayed due to file watchers, we explicitly send prefab changes
ATOMIC_EVENT(E_PREFABCHANGED, PrefabChanged)
{
    ATOMIC_PARAM(P_GUID, GUID); // String
}

}
