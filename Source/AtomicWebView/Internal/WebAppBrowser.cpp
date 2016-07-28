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

#include <Atomic/IO/Log.h>

#include "WebAppBrowser.h"
#include "../WebBrowserHost.h"

namespace Atomic
{

WebAppBrowser::WebAppBrowser()
{

}

void WebAppBrowser::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
{

    // media stream support
    command_line->AppendSwitch("--enable-media-stream");
    command_line->AppendSwitch("--enable-usermedia-screen-capturing");

    // transparency support
    command_line->AppendSwitch("--off-screen-rendering-enabled");
    command_line->AppendSwitch("--transparent-painting-enabled");

#ifdef ATOMIC_PLATFORM_LINUX
    // Issues with GPU acceleration (and possibly offscreen rendering)
    // https://github.com/AtomicGameEngine/AtomicGameEngine/issues/924
    command_line->AppendSwitch("--disable-gpu");
#endif

    CefApp::OnBeforeCommandLineProcessing(process_type, command_line);
}

// CefBrowserProcessHandler methods.
void WebAppBrowser::OnContextInitialized()
{
    CefBrowserProcessHandler::OnContextInitialized();
}

bool WebAppBrowser::CreateGlobalProperties(CefRefPtr<CefDictionaryValue>& globalProps)
{

    // Get a copy global properties
    GlobalPropertyMap props = WebBrowserHost::GetGlobalProperties();

    if (props.Empty())
        return false;

    GlobalPropertyMap::ConstIterator itr = props.Begin();

    // populate with globals args
    globalProps = CefDictionaryValue::Create();

    while (itr != props.End())
    {
        HashMap<String, Variant>::ConstIterator pitr = itr->second_.Begin();

        const String& globalVar = itr->first_;

        CefRefPtr<CefDictionaryValue> kprops = CefDictionaryValue::Create();

        while (pitr != itr->second_.End())
        {
            const String& propertyName = pitr->first_;
            const Variant& value = pitr->second_;

            if (value.GetType() == VAR_INT || value.GetType() == VAR_FLOAT || value.GetType() == VAR_DOUBLE)
            {
                kprops->SetDouble(propertyName.CString(), value.GetDouble());
            }
            else if (value.GetType() == VAR_BOOL)
            {
                kprops->SetBool(propertyName.CString(), value.GetBool());
            }
            else if (value.GetType() == VAR_STRING)
            {
                kprops->SetString(propertyName.CString(), value.GetString().CString());
            }


            pitr++;
        }

        globalProps->SetDictionary(globalVar.CString(), kprops);

        itr++;
    }

    return true;

}

void WebAppBrowser::OnRenderProcessThreadCreated(CefRefPtr<CefListValue> extra_info)
{
    // We're not on main thread here, we're on IO thread
    CefRefPtr<CefDictionaryValue> globalProps;

    if (CreateGlobalProperties(globalProps))
    {
        extra_info->SetDictionary(0, globalProps);
    }
    else
    {
        extra_info->SetNull(0);
    }

    extra_info->SetString(1, WebBrowserHost::GetJSMessageQueryFunctionName().CString());
    extra_info->SetString(2, WebBrowserHost::GetJSMessageQueryCancelFunctionName().CString());


}


}
