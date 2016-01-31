
#pragma once

#include <Atomic/Graphics/Texture2D.h>
#include <Atomic/Graphics/Material.h>

class CefRenderHandler;

namespace Atomic
{

class WebClient;


/// Base class for various WebRenderHandlers
class ATOMIC_API WebRenderHandler : public Object
{
    OBJECT(WebRenderHandler);

public:

    /// Construct.
    WebRenderHandler(Context* context);
    /// Destruct.
    virtual ~WebRenderHandler();

    /// Get the current renderer width
    virtual int GetWidth() const = 0;
    /// Get the current renderer height
    virtual int GetHeight() const = 0;
    /// Get the WebClient associated with the render handler
    WebClient* GetWebClient() const;

    /// Set the dimensions of the render handler
    virtual void SetSize(int width, int height) = 0;

    /// Set the render handlers WebClient
    void SetWebClient(WebClient* webClient);

    /// Get the (internal) CEFRenderHandler
    virtual CefRenderHandler* GetCEFRenderHandler() = 0;

protected:

    WeakPtr<WebClient> webClient_;


};

}
