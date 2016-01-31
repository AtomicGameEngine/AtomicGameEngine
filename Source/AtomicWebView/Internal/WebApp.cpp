
#include <include/cef_client.h>

#include "WebApp.h"
#include "../WebString.h"

namespace Atomic
{

// These flags must match the Chromium values.
static const char kProcessType[] = "type";
static const char kRendererProcess[] = "renderer";
#ifdef ATOMIC_PLATFORM_LINUX
static const char kZygoteProcess[] = "zygote";
#endif

WebApp::WebApp()
{

}

WebApp::ProcessType WebApp::GetProcessType(CefRefPtr<CefCommandLine> command_line)
{
    // The command-line flag won't be specified for the browser process.
    if (!command_line->HasSwitch(kProcessType))
        return BrowserProcess;

    String processType;
    ConvertCEFString(command_line->GetSwitchValue(kProcessType), processType);

    if (processType == kRendererProcess)
        return RendererProcess;

#ifdef ATOMIC_PLATFORM_LINUX
    else if (processType == kZygoteProcess)
        return ZygoteProcess;
#endif

    return OtherProcess;
}




}
