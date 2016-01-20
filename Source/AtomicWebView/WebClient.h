
#include <Atomic/Core/Object.h>
#include "WebRenderHandler.h"

#pragma once

class CefClient;

namespace Atomic
{

class WebClientPrivate;

class ATOMIC_API WebClient : public Object
{
    friend class WebBrowserHost;
    friend class WebClientPrivate;

    OBJECT(WebClient)

public:
    /// Construct.
    WebClient(Context* context);

    /// Destruct.
    virtual ~WebClient();

    /// call once initialized with handlers
    bool CreateBrowser(const String& initialURL, int width, int height);

    void SetSize(int width, int height);

    void SetWebRenderHandler(WebRenderHandler* handler);

    CefClient* GetCefClient();

    void SendMouseClickEvent(int x, int y, unsigned button, bool mouseUp, unsigned modifier) const;
    void SendMousePressEvent(int x, int y, unsigned button = 0, unsigned modifier = 0) const;
    void SendMouseMoveEvent(int x, int y, unsigned modifier, bool mouseLeave = false) const;
    void SendMouseWheelEvent(int x, int y, unsigned modifier, int deltaX, int deltaY) const;

    void SendFocusEvent(bool focus = true);

    void SendTextInputEvent(const StringHash eventType, VariantMap& eventData);
    void SendKeyEvent(const StringHash eventType, VariantMap& eventData);

    void ShortcutCut();
    void ShortcutCopy();
    void ShortcutPaste();
    void ShortcutSelectAll();
    void ShortcutUndo();
    void ShortcutRedo();
    void ShortcutDelete();

    void ExecuteJavaScript(const String& script);

    // Navigation

    bool IsLoading();

    /// Load the specified url into the main frame of the browser
    void LoadURL(const String& url);
    void GoBack();
    void GoForward();

    void Reload();

private:

    void WasResized();

    SharedPtr<WebRenderHandler> renderHandler_;

    WebClientPrivate* d_;

};

}
