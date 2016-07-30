//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
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

#include <AtomicJS/Javascript/JSVM.h>

#include "AEEditorPrefs.h"
#include "AEPlayerApp.h"

namespace Atomic
{
    void jsapi_init_webview(JSVM* vm, const VariantMap& engineParameters);
}

namespace AtomicEditor
{
    AEPlayerApplication::AEPlayerApplication(Context* context) :
        IPCPlayerApp(context)
    {

    }

    AEPlayerApplication::~AEPlayerApplication()
    {

    }

    void AEPlayerApplication::Setup()
    {
        IPCPlayerApp::Setup();

        AEEditorPrefs* prefs = new AEEditorPrefs(context_);
        context_->RegisterSubsystem(prefs);

#ifdef ATOMIC_WEBVIEW
        JSVM::RegisterPackage(jsapi_init_webview, engineParameters_);
#endif

    }

    void AEPlayerApplication::Start()
    {
        IPCPlayerApp::Start();

        GetSubsystem<AEEditorPrefs>()->ValidateWindow();

    }

    void AEPlayerApplication::Stop()
    {
        IPCPlayerApp::Stop();
    }

    void AEPlayerApplication::ProcessArguments()
    {
        IPCPlayerApp::ProcessArguments();
    }

}
