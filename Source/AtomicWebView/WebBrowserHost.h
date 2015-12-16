
#include <Atomic/Core/Object.h>

#pragma once

class CefLifeSpanHandler;

namespace Atomic
{

class WebBrowserHostPrivate;
class WebClient;

class ATOMIC_API WebBrowserHost : public Object
{
    OBJECT(WebBrowserHost);

public:
    /// Construct.
    WebBrowserHost(Context* context);
    /// Destruct.
    virtual ~WebBrowserHost();

    bool CreateBrowser(WebClient* webClient);

    CefLifeSpanHandler* GetCefLifeSpanHandler();

private:

    void HandleBeginFrame(StringHash eventType, VariantMap& eventData);

    WebBrowserHostPrivate* d_;

};

}
