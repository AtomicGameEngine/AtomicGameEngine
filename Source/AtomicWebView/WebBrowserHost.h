
#include <Atomic/Core/Object.h>

#pragma once

namespace Atomic
{

class ATOMIC_API WebBrowserHost : public Object
{
    OBJECT(WebBrowserHost);

public:
    /// Construct.
    WebBrowserHost(Context* context);
    /// Destruct.
    ~WebBrowserHost();

private:

};

}
