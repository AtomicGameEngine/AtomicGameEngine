
#pragma once

#include <Atomic/Core/Object.h>
#include <Atomic/Core/Context.h>

namespace Atomic
{

class ATOMIC_API JSEventDispatcher : public Object, public GlobalEventListener
{
    OBJECT(JSEventDispatcher);

public:
    /// Construct.
    JSEventDispatcher(Context* context);
    /// Destruct.
    virtual ~JSEventDispatcher();

    void RegisterJSEvent(StringHash hash) { jsEvents_[hash] = true; }

private:

    void BeginSendEvent(Context* context, Object* sender, StringHash eventType, VariantMap& eventData);
    void EndSendEvent(Context* context, Object* sender, StringHash eventType, VariantMap& eventData);

    HashMap<StringHash, bool> jsEvents_;

};

class ATOMIC_API JSEventHelper : public Object
{
    OBJECT(JSEventHelper);

public:
    /// Construct.
    JSEventHelper(Context* context);
    /// Destruct.
    virtual ~JSEventHelper();

    void AddEventHandler(StringHash eventType);
    void AddEventHandler(Object* sender, StringHash eventType);

private:

    void HandleEvent(StringHash eventType, VariantMap& eventData);

};


}
