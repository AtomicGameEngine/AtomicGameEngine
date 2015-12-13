
#include <ThirdParty/CEF/include/cef_client.h>
#include <Atomic/Core/Object.h>

#include "WebRenderer.h"

#pragma once

namespace Atomic
{

class ATOMIC_API WebClient : public Object, public CefClient
{
    OBJECT(WebClient);

public:
    /// Construct.
    WebClient(Context* context);
    /// Destruct.
    ~WebClient();

    // CEF3
    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() {
      return renderer_.Get();
    }


private:

    SharedPtr<WebRenderer> renderer_;

};

}
