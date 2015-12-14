
#include <Atomic/Core/Object.h>

#pragma once

namespace Atomic
{

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

private:

};

}
