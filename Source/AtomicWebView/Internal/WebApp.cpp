//
// Copyright (c) 2014-2016, THUNDERBEAST GAMES LLC All rights reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

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
