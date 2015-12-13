
#include <ThirdParty/CEF/include/cef_client.h>
#include <Atomic/Core/Object.h>

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

private:

};

}
