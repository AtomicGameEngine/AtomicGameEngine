
#pragma once

#include <Duktape/duktape.h>
#include <Atomic/Core/Object.h>

namespace Atomic
{

class JSUI : public Object
{
    OBJECT(JSUI)

public:

    JSUI(Context* context);
    virtual ~JSUI();

private:

    duk_context* ctx_;

    void HandleWidgetEvent(StringHash eventType, VariantMap& eventData);


};

}
