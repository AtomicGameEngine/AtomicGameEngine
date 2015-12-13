#include <ThirdParty/CEF/include/cef_client.h>

#include <Atomic/Graphics/Texture2D.h>
#include <Atomic/Graphics/Material.h>

#pragma once

namespace Atomic
{

class ATOMIC_API WebRenderer : public Object, public CefRenderHandler
{
    OBJECT(WebRenderer);

public:

    /// Construct.
    WebRenderer(Context* context);
    /// Destruct.
    ~WebRenderer();

private:


};

}
