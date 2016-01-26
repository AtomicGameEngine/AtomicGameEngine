
#pragma once

#include <Atomic/Core/Object.h>

namespace Atomic
{

class WebBrowserHostPrivate;
class WebClient;


/// Browser host subsystem, responsible for initializing CEF
class ATOMIC_API WebBrowserHost : public Object
{
    OBJECT(WebBrowserHost);

public:
    /// Construct.
    WebBrowserHost(Context* context);
    /// Destruct.
    virtual ~WebBrowserHost();

private:

    void HandleBeginFrame(StringHash eventType, VariantMap& eventData);

    WebBrowserHostPrivate* d_;

};

}
