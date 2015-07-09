

#pragma once

#include "../Scene/Component.h"

namespace Atomic
{

/// Helper base class for user-defined game logic components that hooks up to update events and forwards them to virtual functions similar to ScriptInstance class.
class PrefabComponent : public Component
{
    OBJECT(PrefabComponent);

public:

    /// Construct.
    PrefabComponent(Context* context);
    /// Destruct.
    virtual ~PrefabComponent();

    /// Register object factory.
    static void RegisterObject(Context* context);

    void SetPrefabGUID(const String& guid);
    const String& GetPrefabGUID() const { return prefabGUID_; }

    bool SavePrefab();

    Node* GetPrefabNode() { return prefabNode_; }

protected:

    /// Handle scene node being assigned at creation.
    virtual void OnNodeSet(Node* node);


private:

    void HandleReloadFinished(StringHash eventType, VariantMap& eventData);

    SharedPtr<Node> prefabNode_;
    String prefabGUID_;

};

}
