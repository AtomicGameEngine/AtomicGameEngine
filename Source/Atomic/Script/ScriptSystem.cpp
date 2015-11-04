#include "ScriptSystem.h"
#include "ScriptComponent.h"
#include "ScriptComponentFile.h"

namespace Atomic
{

void RegisterScriptLibrary(Context* context);

ScriptSystem::ScriptSystem(Context* context) : Object(context)
{
    RegisterScriptLibrary(context);
}

ScriptSystem::~ScriptSystem()
{

}

void RegisterScriptLibrary(Context* context)
{
    ScriptComponentFile::RegisterObject(context);
    ScriptComponent::RegisterObject(context);
}

}
