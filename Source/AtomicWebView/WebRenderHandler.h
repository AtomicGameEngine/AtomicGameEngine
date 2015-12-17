
#include <Atomic/Graphics/Texture2D.h>
#include <Atomic/Graphics/Material.h>

#pragma once

class CefRenderHandler;

namespace Atomic
{

class WebClient;

class ATOMIC_API WebRenderHandler : public Object
{
    OBJECT(WebRenderHandler);

public:

    /// Construct.
    WebRenderHandler(Context* context);
    /// Destruct.
    virtual ~WebRenderHandler();

    unsigned GetCurrentWidth() const { return currentWidth_; }
    unsigned GetCurrentHeight() const { return currentHeight_; }

    unsigned GetMaxWidth() const { return maxWidth_; }
    unsigned GetMaxHeight() const { return maxHeight_; }

    void SetWebClient(WebClient* webClient);
    WebClient* GetWebClient() const;

    virtual CefRenderHandler* GetCEFRenderHandler() = 0;

protected:

    unsigned currentWidth_;
    unsigned currentHeight_;

    unsigned maxHeight_;
    unsigned maxWidth_;

    WeakPtr<WebClient> webClient_;


};

}
