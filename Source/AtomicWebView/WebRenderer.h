
#include <Atomic/Graphics/Texture2D.h>
#include <Atomic/Graphics/Material.h>

#include "WebClient.h"

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
