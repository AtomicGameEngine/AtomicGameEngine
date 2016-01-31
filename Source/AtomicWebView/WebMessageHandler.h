
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

    /// Success callback
    void Success(const String& response = String::EMPTY);
    /// Failure callback
    void Failure(int errorCode, const String& errorMessage);

    /// Get the WebClient associated with this WebMessageHandler
    WebClient* GetWebClient() { return client_; }

    /// Set the WebClient associated with this WebMessageHandler
    void SetWebClient(WebClient* client) { client_ = client; }

    /// Get the CefMessageRouterBrowserSide::Handler* as a opaque void*
    void* GetCefHandler() { return (void*) d_; }

private:

    WeakPtr<WebClient> client_;
    WebMessageHandlerPrivate* d_;

};

}
