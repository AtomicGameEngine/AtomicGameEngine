
#include <Atomic/Core/Object.h>
#include "WebRenderHandler.h"

#pragma once

namespace Atomic
{

class WebClientPrivate;

class ATOMIC_API WebClient : public Object
{
    friend class WebBrowserHost;
    friend class WebClientPrivate;

    OBJECT(WebClient)

public:
    /// Construct.
    WebClient(Context* context);

    /// Destruct.
    virtual ~WebClient();

    void SetWebRenderHandler(WebRenderHandler* handler);

private:

    SharedPtr<WebRenderHandler> renderHandler_;

    WebClientPrivate* d_;

};

}
