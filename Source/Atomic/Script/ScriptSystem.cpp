#include "ScriptSystem.h"
#include "ScriptComponent.h"
#include "ScriptComponentFile.h"

namespace Atomic
{

WeakPtr<Context> ScriptSystem::scriptContext_;

void RegisterScriptLibrary(Context* context);

ScriptSystem::ScriptSystem(Context* context) : Object(context)
{
    RegisterScriptLibrary(context);
    scriptContext_ = context;
}

ScriptSystem::~ScriptSystem()
{
    scriptContext_ = nullptr;
}

void RegisterScriptLibrary(Context* context)
{
    ScriptComponentFile::RegisterObject(context);
    ScriptComponent::RegisterObject(context);
}

}
