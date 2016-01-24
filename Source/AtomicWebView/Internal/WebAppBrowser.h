
#pragma once

#include "WebApp.h"

namespace Atomic
{

class WebAppBrowser : public WebApp, public CefBrowserProcessHandler
{

public:

    WebAppBrowser();

    // CefApp methods.
    void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) OVERRIDE;

    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE
    {
        return this;
    }

    // CefBrowserProcessHandler methods.
    void OnContextInitialized() OVERRIDE;


private:

    DISALLOW_COPY_AND_ASSIGN(WebAppBrowser);
    IMPLEMENT_REFCOUNTING(WebAppBrowser);

};

}

