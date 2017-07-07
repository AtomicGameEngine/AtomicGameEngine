

#include "../../Core/CoreEvents.h"
#include "../../IO/Log.h"
#include "../UI.h"

#include "UIPlatformWebView.h"


namespace Atomic
{

AtomicWebViewHandle UIPlatformWebView::webViewHandleCounter_ = 1;
HashMap<AtomicWebViewHandle, WeakPtr<UIPlatformWebView>> UIPlatformWebView::webViewLookup_;

UIPlatformWebView::UIPlatformWebView(Context* context, bool createWidget) : UIWidget(context, false),
    webViewHandle_(UI_PLATFORM_WEBVIEW_INVALID_HANDLE)
{
    if (createWidget)
    {
        widget_ = new tb::TBWidget();
        widget_->SetDelegate(this);
        widget_->SetIsFocusable(true);
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }

    SubscribeToEvent(E_ENDFRAME, ATOMIC_HANDLER(UIPlatformWebView, HandleEndFrame));
}

UIPlatformWebView::~UIPlatformWebView()
{
    if (webViewHandle_ != UI_PLATFORM_WEBVIEW_INVALID_HANDLE)
    {
        PlatformDestroyWebView();
    }

}

void UIPlatformWebView::OnFocusChanged(bool focused)
{
    UIWidget::OnFocusChanged(focused);

    // ATOMIC_LOGINFOF("UIPlatformWebView::OnFocusChanged(%s)", focused ? "true" : "false");
}

void UIPlatformWebView::OnRequestSent(const String& request)
{

}

void UIPlatformWebView::OnError(const String& request)
{

}

void UIPlatformWebView::HandleEndFrame(StringHash eventType, VariantMap& eventData)
{
    if (webViewHandle_ == UI_PLATFORM_WEBVIEW_INVALID_HANDLE)
    {
        if (!GetWidth() || !GetHeight())
        {
            return;
        }

        if (!PlatformCreateWebView())
        {
            return;
        }

        PlatformShowWebView();
    }

    if (!widget_->GetVisibilityCombined())
    {
        PlatformHideWebView();
    }
    else
    {
        PlatformShowWebView();
    }

    PlatformPositionWebView();
}

}

