#include "EditorComponents.h"

#include "EditorComponent.h"
#include "CubemapGenerator.h"

namespace AtomicEditor
{

void RegisterEditorComponentLibrary(Atomic::Context* context)
{
    EditorComponent::RegisterObject(context);
    CubemapGenerator::RegisterObject(context);
}

}
