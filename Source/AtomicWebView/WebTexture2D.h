
#include <Atomic/Graphics/Texture2D.h>
#include <Atomic/Graphics/Material.h>

#include "WebRenderer.h"

#pragma once

namespace Atomic
{

class ATOMIC_API WebTexture2D : public WebRenderer
{
    OBJECT(WebTexture2D);

public:

    /// Construct.
    WebTexture2D(Context* context, int width = 1024, int height = 1024);
    /// Destruct.
    ~WebTexture2D();

private:

    SharedPtr<Texture2D> texture_;
    SharedPtr<Material> material_;

};

}
