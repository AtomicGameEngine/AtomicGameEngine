
#pragma once

#include <Atomic/Scene/Component.h>

namespace Atomic
{

class ScriptComponentFile;

class ATOMIC_API ScriptComponent : public Component
{
    ATOMIC_OBJECT(ScriptComponent, Component);

public:

    /// Construct.
    ScriptComponent(Context* context);
    /// Destruct.
    virtual ~ScriptComponent();

    static void RegisterObject(Context* context);

    virtual const String& GetComponentClassName() const = 0;

    virtual ScriptComponentFile* GetComponentFile() = 0;

    VariantMap& GetFieldValues() { return fieldValues_; }


protected:

    VariantMap fieldValues_;

};

}
