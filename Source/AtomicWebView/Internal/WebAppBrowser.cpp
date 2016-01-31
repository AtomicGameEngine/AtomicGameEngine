

#include "WebAppBrowser.h"

namespace Atomic
{

WebAppBrowser::WebAppBrowser()
{

}

void WebAppBrowser::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
{
    CefApp::OnBeforeCommandLineProcessing(process_type, command_line);
}

// CefBrowserProcessHandler methods.
void WebAppBrowser::OnContextInitialized()
{
    CefBrowserProcessHandler::OnContextInitialized();
}


}
