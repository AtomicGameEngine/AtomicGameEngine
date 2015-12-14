
#include <Atomic/Graphics/Texture2D.h>
#include <Atomic/Graphics/Material.h>

#pragma once

namespace Atomic
{

class ATOMIC_API WebRenderHandler : public Object
{
    OBJECT(WebRenderHandler);

public:

    /// Construct.
    WebRenderHandler(Context* context);
    /// Destruct.
    virtual ~WebRenderHandler();

    virtual void SetCurrentWidth(unsigned width) = 0;
    virtual void SetCurrentHeight(unsigned height) = 0;

    virtual void SetMaxWidth(unsigned width) = 0;
    virtual void SetMaxHeight(unsigned height) = 0;

    unsigned GetCurrentWidth() const { return currentWidth_; }
    unsigned GetCurrentHeight() const { return currentHeight_; }

    unsigned GetMaxWidth() const { return maxWidth_; }
    unsigned GetMaxHeight() const { return maxHeight_; }

private:

    unsigned currentWidth_;
    unsigned currentHeight_;

    unsigned maxHeight_;
    unsigned maxWidth_;


};

}
