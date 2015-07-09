

#include "../Core/Object.h"

namespace Atomic
{

EVENT(E_PREFABSAVE, PrefabSave)
{
    PARAM(P_PREFABCOMPONENT, PrefabComponent); // PrefabComponent Pointer
}

EVENT(E_PREFABCHANGED, PrefabChanged)
{
    PARAM(P_PREFABCOMPONENT, PrefabComponent); // PrefabComponent Pointer
    PARAM(P_NODE, Node); // Node Pointer
}

}
