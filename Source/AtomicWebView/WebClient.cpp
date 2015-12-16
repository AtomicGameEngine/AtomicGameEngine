
#include <ThirdParty/CEF/include/cef_client.h>


#include "WebBrowserHost.h"
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
        webBrowserHost_ = webClient_->GetSubsystem<WebBrowserHost>();
    }

    CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE
    {

        if (webClient_->renderHandler_.Null())
            return nullptr;

        return webClient_->renderHandler_->GetCEFRenderHandler();

    }

    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE
    {
        return webBrowserHost_->GetCefLifeSpanHandler();
    }

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                  CefProcessId source_process,
                                  CefRefPtr<CefProcessMessage> message) OVERRIDE
    {
        return false;
    }



    IMPLEMENT_REFCOUNTING(WebClientPrivate);

private:

    WeakPtr<WebBrowserHost> webBrowserHost_;
    WeakPtr<WebClient> webClient_;

};


WebClient::WebClient(Context* context) : Object(context)
{
    d_ = new WebClientPrivate(this);
}

WebClient::~WebClient()
{
    renderHandler_ = 0;
    //d_->Release();
}

void WebClient::SetWebRenderHandler(WebRenderHandler* handler)
{
    renderHandler_ = handler;
}


}
