

#include <include/wrapper/cef_message_router.h>

#include "WebAppRenderer.h"

namespace Atomic
{

// Must match the value in client_handler.cc.
static const char kFocusedNodeChangedMessage[] = "ClientRenderer.FocusedNodeChanged";

class WebRenderDelegate : public WebAppRenderer::Delegate
{

public:

    WebRenderDelegate()
        : last_node_is_editable_(false)
    {

    }

    virtual void OnWebKitInitialized(CefRefPtr<WebAppRenderer> app) OVERRIDE
    {
        // Create the renderer-side router for query handling.
        CefMessageRouterConfig config;
        config.js_query_function = "atomicQuery";
        config.js_cancel_function = "atomicQueryCancel";

        message_router_ = CefMessageRouterRendererSide::Create(config);
    }

    virtual void OnContextCreated(CefRefPtr<WebAppRenderer> app,
                                  CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefRefPtr<CefV8Context> context) OVERRIDE
    {
        message_router_->OnContextCreated(browser,  frame, context);
    }

    virtual void OnContextReleased(CefRefPtr<WebAppRenderer> app,
                                   CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefV8Context> context) OVERRIDE
    {
        message_router_->OnContextReleased(browser,  frame, context);
    }

    virtual bool OnProcessMessageReceived(
            CefRefPtr<WebAppRenderer> app,
            CefRefPtr<CefBrowser> browser,
            CefProcessId source_process,
            CefRefPtr<CefProcessMessage> message) OVERRIDE
    {
        return message_router_->OnProcessMessageReceived(
                    browser, source_process, message);
    }

private:
    bool last_node_is_editable_;

    // Handles the renderer side of query routing.
    CefRefPtr<CefMessageRouterRendererSide> message_router_;

    IMPLEMENT_REFCOUNTING(WebRenderDelegate);
};

WebAppRenderer::WebAppRenderer() {
}

void WebAppRenderer::OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info)
{
    delegates_.Push(new WebRenderDelegate());

    DelegateSet::Iterator it = delegates_.Begin();
    for (; it != delegates_.End(); ++it)
        (*it)->OnRenderThreadCreated(this, extra_info);
}

void WebAppRenderer::OnWebKitInitialized()
{
    DelegateSet::Iterator it = delegates_.Begin();
    for (; it != delegates_.End(); ++it)
        (*it)->OnWebKitInitialized(this);
}

void WebAppRenderer::OnBrowserCreated(CefRefPtr<CefBrowser> browser)
{
    DelegateSet::Iterator it = delegates_.Begin();
    for (; it != delegates_.End(); ++it)
        (*it)->OnBrowserCreated(this, browser);
}

void WebAppRenderer::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser)
{
    DelegateSet::Iterator it = delegates_.Begin();
    for (; it != delegates_.End(); ++it)
        (*it)->OnBrowserDestroyed(this, browser);
}

CefRefPtr<CefLoadHandler> WebAppRenderer::GetLoadHandler()
{
    CefRefPtr<CefLoadHandler> load_handler;
    DelegateSet::Iterator it = delegates_.Begin();
    for (; it != delegates_.End() && !load_handler.get(); ++it)
        load_handler = (*it)->GetLoadHandler(this);

    return load_handler;
}

bool WebAppRenderer::OnBeforeNavigation(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefFrame> frame,
                                        CefRefPtr<CefRequest> request,
                                        NavigationType navigation_type,
                                        bool is_redirect)
{
    DelegateSet::Iterator it = delegates_.Begin();
    for (; it != delegates_.End(); ++it) {
        if ((*it)->OnBeforeNavigation(this, browser, frame, request, navigation_type, is_redirect))
        {
            return true;
        }
    }

    return false;
}

void WebAppRenderer::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefRefPtr<CefV8Context> context)
{
    DelegateSet::Iterator it = delegates_.Begin();
    for (; it != delegates_.End(); ++it)
        (*it)->OnContextCreated(this, browser, frame, context);
}

void WebAppRenderer::OnContextReleased(CefRefPtr<CefBrowser> browser,
                                       CefRefPtr<CefFrame> frame,
                                       CefRefPtr<CefV8Context> context)
{
    DelegateSet::Iterator it = delegates_.Begin();
    for (; it != delegates_.End(); ++it)
        (*it)->OnContextReleased(this, browser, frame, context);
}

void WebAppRenderer::OnUncaughtException(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefV8Context> context,
        CefRefPtr<CefV8Exception> exception,
        CefRefPtr<CefV8StackTrace> stackTrace)
{
    DelegateSet::Iterator it = delegates_.Begin();
    for (; it != delegates_.End(); ++it)
    {
        (*it)->OnUncaughtException(this, browser, frame, context, exception, stackTrace);
    }
}

void WebAppRenderer::OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
                                          CefRefPtr<CefFrame> frame,
                                          CefRefPtr<CefDOMNode> node)
{
    DelegateSet::Iterator it = delegates_.Begin();
    for (; it != delegates_.End(); ++it)
        (*it)->OnFocusedNodeChanged(this, browser, frame, node);
}

bool WebAppRenderer::OnProcessMessageReceived(
        CefRefPtr<CefBrowser> browser,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message)
{
    assert(source_process == PID_BROWSER);

    bool handled = false;

    DelegateSet::Iterator it = delegates_.Begin();
    for (; it != delegates_.End() && !handled; ++it) {
        handled = (*it)->OnProcessMessageReceived(this, browser, source_process, message);
    }

    return handled;
}
}
