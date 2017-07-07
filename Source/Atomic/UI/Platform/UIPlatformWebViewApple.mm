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

#import <Foundation/Foundation.h>

/*

Notes:

1) This is based on the Cocoa/iOS webview from the LoomSDK: https://github.com/LoomSDK/LoomSDK/blob/master/loom/common/platform/platformWebViewCocoa.mm
2) macOS mouse tracking complicates the platform's webview and causes issues with the Atomic UI controls beneath it

*/

#ifdef ATOMIC_PLATFORM_OSX

#import <WebKit/WebKit.h>
typedef WebView CocoaWebView;
typedef NSRect CocoaRect;
typedef NSView CocoaView;
#define CocoaMakeRect NSMakeRect

#else

#import <UIKit/UIKit.h>
typedef UIWebView CocoaWebView;
typedef CGRect CocoaRect;
typedef UIView CocoaView;
#define CocoaMakeRect CGRectMake

#endif

#include "../../IO/Log.h"
#include "UIPlatformWebView.h"

using namespace Atomic;

//_________________________________________________________________________
// Helpers
//_________________________________________________________________________

#ifdef ATOMIC_PLATFORM_IOS
static float pixelsToPoints(float pixels)
{
    float scale = [UIScreen mainScreen].scale;
    return pixels / scale;
}
#endif


static CocoaView* GetMainView()
{
#ifdef ATOMIC_PLATFORM_OSX
    return [[[[NSApplication sharedApplication] windows] objectAtIndex:0] contentView];
#else
    return [[[[UIApplication sharedApplication] keyWindow] rootViewController] view];
#endif
}

#ifdef ATOMIC_PLATFORM_OSX

static NSWindow* GetMainWindow()
{
    return [[[NSApplication sharedApplication] windows] objectAtIndex:0];
}

#endif

@interface AtomicWebViewDelegate : NSObject
{
    UIPlatformWebView* uiWebView;
}

-(id)initWithWebView:(UIPlatformWebView*)webview;

@end

@interface WebViewRef : NSObject {
    CocoaRect _rect;
    CocoaWebView* _view;
    AtomicWebViewDelegate* _delegate;
}
@property CocoaRect rect;
@property (retain) CocoaWebView* view;
@property (retain) AtomicWebViewDelegate* delegate;
@end


@implementation WebViewRef

@synthesize view = _view;
@synthesize delegate = _delegate;

- (CocoaRect)rect
{
    return _rect;
}
- (void)setRect:(CocoaRect)newRect;
{
    _rect = newRect;
    CocoaRect frame;
#ifdef ATOMIC_PLATFORM_OSX
    frame = newRect;
#else
    frame.size.width = pixelsToPoints(newRect.size.width);
    frame.size.height = pixelsToPoints(newRect.size.height);
    frame.origin.x = pixelsToPoints(newRect.origin.x);
    frame.origin.y = getMainView().frame.size.height - frame.size.height - pixelsToPoints(newRect.origin.y);
#endif
    self.view.frame = frame;
}

@end

static NSMutableDictionary* gWebViews;
NSMutableDictionary* webViews()
{
    if(gWebViews == NULL)
        gWebViews = [[NSMutableDictionary dictionary] retain];

    return gWebViews;
}

WebViewRef* GetWebViewRef(AtomicWebViewHandle handle)
{
    return [webViews() objectForKey:[NSNumber numberWithInt:handle]];
}

//_________________________________________________________________________
// WebView Delegate
//_________________________________________________________________________
@implementation AtomicWebViewDelegate

-(id)initWithWebView:(UIPlatformWebView*)webview
{
    self = [self init];
    uiWebView = webview;
    return self;
}


#ifdef ATOMIC_PLATFORM_OSX

- (void)webView:(CocoaWebView *)sender didStartProvisionalLoadForFrame:(WebFrame *)frame
{
    if(frame == [sender mainFrame])
    {
        NSURLRequest* request = frame.provisionalDataSource.request;
        NSString *urlString = [[request URL] absoluteString];
        uiWebView->OnRequestSent([urlString cStringUsingEncoding:1]);
    }
}

- (void)webView:(CocoaWebView *)sender didFailLoadWithError:(NSError *)error forFrame:(WebFrame *)frame
{
    if(frame == [sender mainFrame])
    {
        NSInteger code = [error code];
        NSString *codeString = [NSString stringWithFormat:@"WebKit Error code: %ld",(long)code];
        uiWebView->OnError([codeString cStringUsingEncoding:1]);
    }
}

- (void)webView:(CocoaWebView *)sender didFailProvisionalLoadWithError:(NSError *)error forFrame:(WebFrame *)frame
{
    if(frame == [sender mainFrame])
    {
        NSInteger code = [error code];
        NSString *codeString = [NSString stringWithFormat:@"WebKit Error code: %ld",(long)code];
        uiWebView->OnError([codeString cStringUsingEncoding:1]);
    }
}

#else

- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType
{
    NSString *urlString = [[request URL] absoluteString];
    callback(payload, WEBVIEW_REQUEST_SENT, [urlString cStringUsingEncoding:1]);

    return YES;
}

- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error
{
    NSInteger code = [error code];
    NSString *codeString = [NSString stringWithFormat:@"WebKit Error code: %ld",(long)code];
    callback(payload, WEBVIEW_REQUEST_ERROR, [codeString cStringUsingEncoding:1]);
}

#endif


@end

namespace Atomic
{

bool UIPlatformWebView::PlatformCreateWebView()
{
    if (webViewHandle_ != UI_PLATFORM_WEBVIEW_INVALID_HANDLE)
    {
        ATOMIC_LOGWARNING("UIPlatformWebView::CreatePlatformWebView() - Attempting to create platform webview with valid handle");
        return false;
    }

    webViewHandle_ = webViewHandleCounter_++;

    webViewLookup_[webViewHandle_]  = this;

    IntVector2 rootPos = ConvertToRoot();

    CocoaRect bounds;
    bounds.size.width = (float) GetWidth();
    bounds.size.height = (float) GetHeight();

    float mainHeight = GetMainView().frame.size.height;

    bounds.origin.x = (float) rootPos.x_;
    bounds.origin.y = mainHeight - bounds.size.height - rootPos.y_;

    CocoaWebView* webView = [[[CocoaWebView alloc] initWithFrame:bounds] retain];
    AtomicWebViewDelegate* delegate = [[[AtomicWebViewDelegate alloc] initWithWebView:this] retain];

#ifdef ATOMIC_PLATFORM_OSX
    [webView setFrameLoadDelegate:(id)delegate];
    [webView setWantsLayer:YES];
#else
    [webView setDelegate:delegate];
#endif

    WebViewRef *ref = [WebViewRef alloc];
    ref.view = webView;
    ref.rect = webView.frame;
    ref.delegate = delegate;

    [webViews() setObject:ref forKey:[NSNumber numberWithInt:webViewHandle_]];

    if (!queuedRequest_.Empty())
    {
        LoadURL(queuedRequest_);
        queuedRequest_.Clear();
    }

    return true;

}

void UIPlatformWebView::PlatformShowWebView(bool visible)
{
    if (webViewHandle_ == UI_PLATFORM_WEBVIEW_INVALID_HANDLE)
    {
        ATOMIC_LOGWARNING("UIPlatformWebView::ShowPlatformWebView() - Invalid webview handle");
        return;
    }

    CocoaWebView* webView = GetWebViewRef(webViewHandle_).view;

    if (!webView)
    {
        ATOMIC_LOGERROR("UIPlatformWebView::ShowPlatformWebView() - Unable to retrieve webview from handle");
        return;
    }

    if (visible)
    {

#ifdef ATOMIC_PLATFORM_OSX

        if (webView.superview != GetMainView())
        {
            [GetMainView() addSubview:webView];
        }

#endif

    }
    else
    {
#ifdef ATOMIC_PLATFORM_OSX

        if (webView.superview == GetMainView())
        {
            [webView removeFromSuperview];
        }

#endif
    }
}

void UIPlatformWebView::PlatformDestroyWebView()
{
    if (webViewHandle_ == UI_PLATFORM_WEBVIEW_INVALID_HANDLE)
    {
        return;
    }

    webViewLookup_.Erase(webViewHandle_);

    WebViewRef* ref = GetWebViewRef(webViewHandle_);

    CocoaWebView* webView = ref.view;
    AtomicWebViewDelegate* delegate = ref.delegate;

    [webView removeFromSuperview];
    [webView release];
    [delegate release];
    [ref release];

    [webViews() removeObjectForKey:[NSNumber numberWithInt:webViewHandle_]];

    webViewHandle_ = UI_PLATFORM_WEBVIEW_INVALID_HANDLE;

}

void UIPlatformWebView::PlatformPositionWebView()
{
    if (webViewHandle_ == UI_PLATFORM_WEBVIEW_INVALID_HANDLE)
    {
        return;
    }

    WebViewRef* ref = GetWebViewRef(webViewHandle_);

    IntVector2 rootPos = ConvertToRoot(IntVector2( 0, 0));

    CocoaRect frame;
    frame.size.width = (float) GetWidth();
    frame.size.height = (float) GetHeight();

    float mainHeight = GetMainView().frame.size.height;

    frame.origin.x = (float) rootPos.x_;
    frame.origin.y = mainHeight - frame.size.height - rootPos.y_;

    if (frame.origin.x == ref.rect.origin.x &&
        frame.origin.y == ref.rect.origin.y &&
        frame.size.width == ref.rect.size.width &&
        frame.size.height == ref.rect.size.height )
    {
        return;
    }


    ref.rect = frame;
}

void UIPlatformWebView::DestroyAll()
{
    NSArray* keys = [webViews() allKeys];
    for (int i=0; i<[keys count]; i++)
    {
        NSNumber* num = [keys objectAtIndex:i];
        const WeakPtr<UIPlatformWebView>& ptr = webViewLookup_[[num intValue]];
        if (ptr.NotNull())
        {
            ptr->PlatformDestroyWebView();
        }
    }
}

bool UIPlatformWebView::GoBack()
{

    if (webViewHandle_ == UI_PLATFORM_WEBVIEW_INVALID_HANDLE)
    {
        return false;
    }

    CocoaWebView* webView = GetWebViewRef(webViewHandle_).view;

    if (![webView canGoBack]) return false;

    [webView goBack];
    return true;
}

bool UIPlatformWebView::GoForward()
{
    if (webViewHandle_ == UI_PLATFORM_WEBVIEW_INVALID_HANDLE)
    {
        return false;
    }

    CocoaWebView* webView = GetWebViewRef(webViewHandle_).view;

    if (![webView canGoForward]) return false;

    [webView goForward];

    return true;
}

bool UIPlatformWebView::CanGoBack() const
{
    if (webViewHandle_ == UI_PLATFORM_WEBVIEW_INVALID_HANDLE)
    {
        return false;
    }

    CocoaWebView* webView = GetWebViewRef(webViewHandle_).view;

    return [webView canGoBack];
}

bool UIPlatformWebView::CanGoForward() const
{
    if (webViewHandle_ == UI_PLATFORM_WEBVIEW_INVALID_HANDLE)
    {
        return false;
    }

    CocoaWebView* webView = GetWebViewRef(webViewHandle_).view;

    return [webView canGoForward];
}

void UIPlatformWebView::PauseAll()
{
#ifdef ATOMIC_PLATFORM_IOS
    NSArray* keys = [webViews() allKeys];
    for (int i=0; i<[keys count]; i++)
    {
        NSNumber* num = [keys objectAtIndex:i];
        WebViewRef* ref = getWebViewRef([num intValue]);
        CocoaWebView* webView = ref.view;
        [webView setDelegate:nil];
    }
#endif
}

void UIPlatformWebView::ResumeAll()
{
#ifdef ATOMIC_PLATFORM_IOS
    NSArray* keys = [webViews() allKeys];
    for (int i=0; i<[keys count]; i++)
    {
        NSNumber* num = [keys objectAtIndex:i];
        WebViewRef* ref = getWebViewRef([num intValue]);
        CocoaWebView* webView = ref.view;
        [webView setDelegate:(id)ref.delegate];
    }
#endif
}

void UIPlatformWebView::Reload()
{
    if (webViewHandle_ == UI_PLATFORM_WEBVIEW_INVALID_HANDLE)
    {
        return;
    }

    CocoaWebView* webView = GetWebViewRef(webViewHandle_).view;

#ifdef ATOMIC_PLATFORM_OSX
    [[webView mainFrame] reload];
#else
    [webView reload];
#endif

}

void UIPlatformWebView::LoadURL(const String& url)
{
    if (webViewHandle_ == UI_PLATFORM_WEBVIEW_INVALID_HANDLE)
    {
        queuedRequest_ = url;
        return;
    }

    CocoaWebView* webView = GetWebViewRef(webViewHandle_).view;
    NSURL* urlObj = [NSURL URLWithString:[NSString stringWithUTF8String:url.CString()]];
    NSURLRequest* request = [NSURLRequest requestWithURL:urlObj];

#ifdef ATOMIC_PLATFORM_OSX
    [[webView mainFrame] loadRequest:request];
#else
    [webView loadRequest:request];
#endif
}

}

