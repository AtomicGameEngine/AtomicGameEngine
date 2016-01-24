
#pragma once

#include <Atomic/Core/Object.h>

namespace Atomic
{

class WebClient;
class WebMessageHandlerPrivate;

class WebMessageHandler : public Object
{

    OBJECT(WebMessageHandler);

public:

    /// Construct.
    WebMessageHandler(Context* context);
    /// Destruct.
    virtual ~WebMessageHandler();

    void Success(const String& response = String::EMPTY);
    void Failure(int errorCode, const String& errorMessage);

    WebClient* GetWebClient() { return client_; }
    void SetWebClient(WebClient* client) { client_ = client; }

    /// Get the CefMessageRouterBrowserSide::Handler* as a opaque void*
    void* GetCefHandler() { return (void*) d_; }

private:

    WeakPtr<WebClient> client_;
    WebMessageHandlerPrivate* d_;

};

}
