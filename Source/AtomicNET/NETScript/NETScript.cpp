

#include <Atomic/Core/Context.h>

#include "CSComponent.h"
#include "CSComponentAssembly.h"
#include "NETScript.h"

namespace Atomic
{

void RegisterNETScriptLibrary(Context* context)
{
    CSComponent::RegisterObject(context);
    CSComponentAssembly::RegisterObject(context);
}

}