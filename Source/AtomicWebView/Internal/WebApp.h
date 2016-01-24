
#pragma once

#include <include/cef_app.h>
#include "WebApp.h"

namespace Atomic
{

// Base class for customizing process-type-based behavior.
class WebApp : public CefApp
{

public:

    enum ProcessType
    {
        BrowserProcess,
        RendererProcess,
        ZygoteProcess,
        OtherProcess,
    };

    WebApp();

    // Determine the process type based on command-line arguments.
    static ProcessType GetProcessType(CefRefPtr<CefCommandLine> command_line);

private:

    DISALLOW_COPY_AND_ASSIGN(WebApp);


};


}

