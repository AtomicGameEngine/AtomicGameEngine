
#include <ThirdParty/CEF/include/cef_client.h>

#include "WebClient.h"

namespace Atomic
{

class WebClientPrivate : public CefClient
{
    friend class WebClient;

public:

    WebClientPrivate(WebClient* client)
    {

        webClient_ = client;
    }

    IMPLEMENT_REFCOUNTING(WebClientPrivate)

private:

    WeakPtr<WebClient> webClient_;

};


WebClient::WebClient(Context* context) : Object(context)
{
    d_ = new WebClientPrivate(this);
}

WebClient::~WebClient()
{
    d_->Release();
}

void WebClient::SetWebRenderHandler(WebRenderHandler* handler)
{
    renderHandler_ = handler;
}


}
