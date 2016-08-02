
#pragma once

#include <Atomic/Core/Variant.h>
#include <Atomic/Resource/ResourceCache.h>

#include "ScriptSystem.h"

namespace Atomic
{

/// Wraps a VariantMap as a RefCounted so we can easily send it to script code
/// For performance sensitive code, specialized marshaling should be used instead (for example physics event data)
class ScriptVariantMap : public RefCounted
{
    ATOMIC_REFCOUNTED(ScriptVariantMap)

public:

    ScriptVariantMap() : RefCounted()
    {

    }

    virtual ~ScriptVariantMap()
    {

    }

    bool GetBool(StringHash key) const
    {
        Variant* variant = vmap_[key];

        if (!variant)
            return 0;

        return variant->GetBool();

    }

    int GetInt(StringHash key) const
    {
        Variant* variant = vmap_[key];

        if (!variant)
            return 0;

        return variant->GetInt();

    }

    unsigned GetUInt(StringHash key) const
    {
        Variant* variant = vmap_[key];

        if (!variant)
            return 0;

        return variant->GetUInt();

    }

    float GetFloat(StringHash key) const
    {
        Variant* variant = vmap_[key];

        if (!variant)
            return 0;

        return variant->GetFloat();

    }

    const Vector3& GetVector3(StringHash key) const
    {
        Variant* variant = vmap_[key];

        if (!variant)
            return Vector3::ZERO;

        return variant->GetVector3();

    }

    const Quaternion& GetQuaternion(StringHash key) const
    {
        Variant* variant = vmap_[key];

        if (!variant)
            return Quaternion::IDENTITY;

        return variant->GetQuaternion();

    }

    void* GetVoidPtr(StringHash key) const
    {
        Variant* variant = vmap_[key];

        if (!variant)
            return nullptr;

        return variant->GetVoidPtr();

    }

    RefCounted* GetPtr(StringHash key) const
    {
        Variant* variant = vmap_[key];

        if (!variant)
            return 0;

        return variant->GetPtr();

    }

    const String& GetString(StringHash key) const
    {
        Variant* variant = vmap_[key];

        if (!variant)
            return String::EMPTY;

        return variant->GetString();

    }

    VariantType GetVariantType(StringHash key) const
    {
        Variant* variant = vmap_[key];

        if (!variant)
            return VAR_NONE;

        return variant->GetType();
    }

    Resource* GetResourceFromRef(StringHash key) const
    {
        Variant* variant = vmap_[key];

        if (variant && variant->GetType() == VAR_RESOURCEREF)
        {
            const ResourceRef& ref = variant->GetResourceRef();

            if (!ref.name_.Length())
                return 0;

            return ScriptSystem::GetContext()->GetSubsystem<ResourceCache>()->GetResource(ref.type_, ref.name_);

        }

        return 0;

    }

    void SetUInt(StringHash key, unsigned value)
    {
        vmap_[key] = value;
    }

    void SetString(StringHash key, String value)
    {
        vmap_[key] = value;
    }

    void Clear()
    {
        vmap_.Clear();
    }

    bool Contains(StringHash key) const
    {
        return vmap_.Contains(key);
    }

    void CopySourceVariantMap(const VariantMap& src) { vmap_ = src; }

    VariantMap& GetVariantMap() { return vmap_; }

private:

    VariantMap vmap_;

};

}
