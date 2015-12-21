
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

    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;

    virtual void SetSize(int width, int height) = 0;

    void SetWebClient(WebClient* webClient);
    WebClient* GetWebClient() const;

    virtual CefRenderHandler* GetCEFRenderHandler() = 0;

protected:

    WeakPtr<WebClient> webClient_;


};

}
