

#pragma once

#include "../Scene/Component.h"

namespace Atomic
{

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
    void UndoPrefab();
    void BreakPrefab();

    Node* GetPrefabNode() { return prefabNode_; }

protected:

    /// Handle scene node being assigned at creation.
    virtual void OnNodeSet(Node* node);


private:

    void HandlePrefabChanged(StringHash eventType, VariantMap& eventData);

    void LoadPrefabNode();

    SharedPtr<Node> prefabNode_;
    String prefabGUID_;

};

}
