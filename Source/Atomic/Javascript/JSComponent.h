
#pragma once

#include "../Scene/Component.h"

namespace Atomic
{

class Javascript;
class JSFile;
class JSVM;

/// Inbuilt scripted component methods.
enum JSScriptMethod
{
    JSMETHOD_START = 0,
    JSMETHOD_STOP,
    JSMETHOD_DELAYEDSTART,
    JSMETHOD_UPDATE,
    JSMETHOD_POSTUPDATE,
    JSMETHOD_FIXEDUPDATE,
    JSMETHOD_FIXEDPOSTUPDATE,
    JSMETHOD_LOAD,
    JSMETHOD_SAVE,
    JSMETHOD_READNETWORKUPDATE,
    JSMETHOD_WRITENETWORKUPDATE,
    JSMETHOD_APPLYATTRIBUTES,
    JSMETHOD_TRANSFORMCHANGED,
    MAX_JSSCRIPT_METHODS
};

class ATOMIC_API JSComponent : public Component
{
    OBJECT(JSComponent);

public:
    /// Construct.
    JSComponent(Context* context);
    /// Destruct.
    virtual ~JSComponent();
    /// Register object factory.
    static void RegisterObject(Context* context);

    /// Return class name.
    const String& GetClassName() const { return className_; }
    /// Return script file attribute.
    ResourceRef GetScriptFileAttr() const;

    void SetScriptFileAttr(ResourceRef value);
    void SetClassName(const String& className);

    /// Handle enabled/disabled state change.
    virtual void OnSetEnabled();
    void OnNodeSet(Node *node);

    void SetDestroyed() { destroyed_ = true; }

    /// Create object of certain class from the script file. Return true if successful.
    bool CreateObject(JSFile* scriptFile, const String& className);
    /// Set script file only. Recreate object if necessary.
    void SetScriptFile(JSFile* scriptFile);

    void ListenToEvent(Object* sender, StringHash eventType, JS_HEAP_PTR __duk_function);

private:

    /// (Re)create the script object and check for supported methods if successfully created.
    void CreateObject();
    void ReleaseObject();

    void ClearScriptMethods();
    void UpdateEventSubscription();

    void GetScriptMethods();

    /// Handle scene update event.
    void HandleSceneUpdate(StringHash eventType, VariantMap& eventData);
    /// Handle scene post-update event.
    void HandleScenePostUpdate(StringHash eventType, VariantMap& eventData);
#ifdef ATOMIC_PHYSICS
    /// Handle physics pre-step event.
    void HandlePhysicsPreStep(StringHash eventType, VariantMap& eventData);
    /// Handle physics post-step event.
    void HandlePhysicsPostStep(StringHash eventType, VariantMap& eventData);
#endif
    /// Handle an event in script.
    void HandleScriptEvent(StringHash eventType, VariantMap& eventData);
    /// Handle script file reload start.
    void HandleScriptFileReload(StringHash eventType, VariantMap& eventData);
    /// Handle script file reload finished.
    void HandleScriptFileReloadFinished(StringHash eventType, VariantMap& eventData);

    /// Class name.
    String className_;

    /// Script subsystem.
    SharedPtr<Javascript> script_;

    WeakPtr<JSVM> vm_;

    /// Script file.
    WeakPtr<JSFile> scriptFile_;

    HashMap<StringHash, JS_HEAP_PTR> scriptEventFunctions_;

    /// Script object.
    void* scriptObject_;

    /// Pointers to supported inbuilt methods.
    void* methods_[MAX_JSSCRIPT_METHODS];

    /// Subscribed to scene update events flag.
    bool subscribed_;

    /// Subscribed to scene post and fixed update events flag.
    bool subscribedPostFixed_;

    bool started_;
    bool destroyed_;

};

}
