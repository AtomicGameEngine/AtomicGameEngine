//
// Copyright (c) 2014-2017, THUNDERBEAST GAMES LLC All rights reserved
// Copyright (c) 2011-2017, The Game Engine Company LLC (Apache License 2.0)
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

#include "../UIWidget.h"

namespace Atomic
{

typedef int AtomicWebViewHandle;

static const int UI_PLATFORM_WEBVIEW_INVALID_HANDLE = 0;

// A platform WebView which always displays on top of other UIWidgets as it is, by necessity, a platform window
class ATOMIC_API UIPlatformWebView : public UIWidget
{
    ATOMIC_OBJECT(UIPlatformWebView, UIWidget)

public:

    UIPlatformWebView(Context* context, bool createWidget = true);
    virtual ~UIPlatformWebView();

    /// Load the specified url into the main frame of the browser
    void LoadURL(const String& url);

    /// Reload the current page
    void Reload();

    /// Attempt to go back in the WebView associated with the specified handle. Returns false if there is nothing to go back to.
    bool GoBack();

    /// Attempt to go forward in the WebView associated with the specified handle. Returns false if there is nothing to go forward to.
    bool GoForward();

    /// Returns false if there is nothing to go back to. True otherwise.
    bool CanGoBack() const;

    /// Returns true if there is nothing to go back to. True otherwise.
    bool CanGoForward() const;

    /// Removes all listeners and stops interacting with WebViews on platforms that require it (iOS).
    static void PauseAll();

    /// Resumes normal operation of WebViews after pausing.
    static void ResumeAll();

    void OnRequestSent(const String& request);

    void OnError(const String& request);

    void OnFocusChanged(bool focused);

private:

    void HandleEndFrame(StringHash eventType, VariantMap& eventData);

    void PlatformShowWebView(bool visible = true);    
    void PlatformHideWebView() { PlatformShowWebView(false); }

    bool PlatformCreateWebView();
    void PlatformDestroyWebView();
    void PlatformPositionWebView();

    AtomicWebViewHandle webViewHandle_;

    static HashMap<AtomicWebViewHandle, WeakPtr<UIPlatformWebView>> webViewLookup_;
    static AtomicWebViewHandle webViewHandleCounter_;

    static void DestroyAll();

    String queuedRequest_;

};

}
