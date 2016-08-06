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

#pragma once

#include <Atomic/Container/List.h>
#include <Atomic/Core/Object.h>
#include "WebRenderHandler.h"

class CefClient;

namespace Atomic
{

class WebClientPrivate;
class WebMessageHandler;

/// WebClient is the main interface for communicating with a browser instance
class ATOMIC_API WebClient : public Object
{
    friend class WebBrowserHost;
    friend class WebClientPrivate;

    ATOMIC_OBJECT(WebClient, Object)

public:

    /// Construct.
    WebClient(Context* context);

    /// Destruct.
    virtual ~WebClient();

    /// Create the browser, call only once initialized with handlers
    bool CreateBrowser(const String& initialURL, int width, int height);

    /// Set the browser's width and height
    void SetSize(int width, int height);

    /// Set browser zoom level, specify 0.0 to reset the zoom level
    void SetZoomLevel(float zoomLevel);

    /// Send a mouse click event to the browser
    void SendMouseClickEvent(int x, int y, unsigned button, bool mouseUp, unsigned modifier, int clickCount = 1) const;
    /// Send a mouse press event to the browser
    void SendMousePressEvent(int x, int y, unsigned button = 0, unsigned modifier = 0, int clickCount = 1) const;
    /// Send a mouse move event to the browser
    void SendMouseMoveEvent(int x, int y, unsigned modifier, bool mouseLeave = false) const;
    /// Send a mouse wheel event to the browser
    void SendMouseWheelEvent(int x, int y, unsigned modifier, int deltaX, int deltaY) const;

    /// Send a focus event to the browser
    void SendFocusEvent(bool focus = true);

    /// Send a TextInput event to the browser
    void SendTextInputEvent(const StringHash eventType, VariantMap& eventData);
    /// Send a key event to the browser
    void SendKeyEvent(const StringHash eventType, VariantMap& eventData);

    // Shortcuts, note that some web pages (notably some text editors)
    // only work with key events and not all shorcuts

    /// Invoke the Cut shortcut on the browser's main frame
    void ShortcutCut();
    /// Invoke the Copy shortcut on the browser's main frame
    void ShortcutCopy();
    /// Invoke the Paste shortcut on the browser's main frame
    void ShortcutPaste();
    /// Invoke the SelectAll shortcut on the browser's main frame
    void ShortcutSelectAll();
    /// Invoke the Undo shortcut on the browser's main frame
    void ShortcutUndo();
    /// Invoke the Redo shortcut on the browser's main frame
    void ShortcutRedo();
    /// Invoke the Delete shortcut on the browser's main frame
    void ShortcutDelete();

    /// Add a message handler to the WebClient
    void AddMessageHandler(WebMessageHandler* handler, bool first = false);
    /// Remove a message handler to the WebClient
    void RemoveMessageHandler(WebMessageHandler* handler);

    /// Execute some JavaScript in the browser
    void ExecuteJavaScript(const String& script);

    /// Eval some JavaScript in the browser (async return value referenced by evalID)
    void EvalJavaScript(unsigned evalID, const String& script);

    // Navigation

    /// Returns true if the page is currently loading
    bool IsLoading();

    /// Load the specified url into the main frame of the browser
    void LoadURL(const String& url);

    /// Load html source into main frame of browser
    void LoadString(const String& source, const String &url = "http://localcontent/");

    /// Go back in page history
    void GoBack();
    /// Go forward in page history
    void GoForward();

    /// Reload the current page
    void Reload();

    /// Set the render handler for this client
    void SetWebRenderHandler(WebRenderHandler* handler);

    /// Get the (internal) CefClient for this WebClient
    CefClient* GetCefClient();

private:

    void HandleWebViewGlobalPropertiesChanged(StringHash eventType, VariantMap& eventData);

    void UpdateGlobalProperties();

    void WasResized();

    void EvalJavaScriptResult(unsigned evalID, bool result, const String& value);

    SharedPtr<WebRenderHandler> renderHandler_;

    List<SharedPtr<WebMessageHandler>> messageHandlers_;

    WebClientPrivate* d_;

};

}
