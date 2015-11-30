
#include <ThirdParty/CEF/include/cef_app.h>
#include <ThirdParty/CEF/include/cef_client.h>
#include <ThirdParty/CEF/include/cef_render_handler.h>
#include <ThirdParty/CEF/include/wrapper/cef_helpers.h>


#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/IO/Log.h>

#include "WebBrowserHost.h"

class SimpleApp : public CefApp,
                  public CefBrowserProcessHandler {
 public:
  SimpleApp();

  // CefApp methods:
  virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()
      OVERRIDE { return this; }

  // CefBrowserProcessHandler methods:
  virtual void OnContextInitialized() OVERRIDE;

 private:
  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(SimpleApp);
};

SimpleApp::SimpleApp() {
}

void SimpleApp::OnContextInitialized() {

  CEF_REQUIRE_UI_THREAD();

}

namespace Atomic
{

WebBrowserHost::WebBrowserHost(Context* context) : Object (context)
{
    const Vector<String>& arguments = GetArguments();

    const char** _argv { 0 };

    PODVector<const char*> argv;
    for (unsigned i = 0; i < arguments.Size(); i++)
        argv.Push(arguments[i].CString());

    CefMainArgs args(arguments.Size(), arguments.Size() ? (char**) &argv[0] : (char **) _argv);

    int result = CefExecuteProcess(args, nullptr, nullptr);

    if (result >= 0)
    {
        LOGERROR("CEFExecuteProcess - Error");
    }

    CefSettings settings;

    settings.windowless_rendering_enabled = true;

    // If losing OSX system menu, it means we're calling this
    // before initializing graphics subsystem
    if (!CefInitialize(args, settings, nullptr, nullptr))
    {
        LOGERROR("CefInitialize - Error");
    }

}

WebBrowserHost::~WebBrowserHost()
{

}

}
