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

#ifdef ATOMIC_PLATFORM_ANDROID

/* Notes

1) This is based on the Android webview from the LoomSDK: https://github.com/LoomSDK/LoomSDK/blob/master/loom/common/platform/platformWebViewAndroid.cpp

*/

#include "../../IO/Log.h"
#include "../../Graphics/Graphics.h"
#include "UIPlatformWebView.h"
#include "../../Platform/Android/AndroidJNI.h"

namespace Atomic

{

//_________________________________________________________________________
// JNI Helpers
//_________________________________________________________________________
static AtomicJNIMethodInfo gCreateMethodInfo;
static AtomicJNIMethodInfo gDestroyMethodInfo;
static AtomicJNIMethodInfo gDestroyAllMethodInfo;
static AtomicJNIMethodInfo gShowMethodInfo;
static AtomicJNIMethodInfo gHideMethodInfo;
static AtomicJNIMethodInfo gRequestMethodInfo;
static AtomicJNIMethodInfo gGoBackMethodInfo;
static AtomicJNIMethodInfo gGoForwardMethodInfo;
static AtomicJNIMethodInfo gCanGoBackMethodInfo;
static AtomicJNIMethodInfo gCanGoForwardMethodInfo;
static AtomicJNIMethodInfo gSetDimensionsMethodInfo;

static void android_webViewEnsureInitialized()
{
    static bool initialized = false;

    if (!initialized)
    {
        // initialize all of our jni method infos
        AtomicJNI::GetStaticMethodInfo(gCreateMethodInfo,
                                     "com/atomicgameengine/player/AtomicWebView",
                                     "create",
                                     "(IJJ)Z");

        AtomicJNI::GetStaticMethodInfo(gDestroyMethodInfo,
                                     "com/atomicgameengine/player/AtomicWebView",
                                     "destroy",
                                     "(I)V");

        AtomicJNI::GetStaticMethodInfo(gDestroyAllMethodInfo,
                                     "com/atomicgameengine/player/AtomicWebView",
                                     "destroyAll",
                                     "()V");

        AtomicJNI::GetStaticMethodInfo(gShowMethodInfo,
                                     "com/atomicgameengine/player/AtomicWebView",
                                     "show",
                                     "(I)V");

        AtomicJNI::GetStaticMethodInfo(gHideMethodInfo,
                                     "com/atomicgameengine/player/AtomicWebView",
                                     "hide",
                                     "(I)V");

        AtomicJNI::GetStaticMethodInfo(gRequestMethodInfo,
                                     "com/atomicgameengine/player/AtomicWebView",
                                     "request",
                                     "(ILjava/lang/String;)V");

        AtomicJNI::GetStaticMethodInfo(gGoBackMethodInfo,
                                     "com/atomicgameengine/player/AtomicWebView",
                                     "goBack",
                                     "(I)Z");

        AtomicJNI::GetStaticMethodInfo(gGoForwardMethodInfo,
                                     "com/atomicgameengine/player/AtomicWebView",
                                     "goForward",
                                     "(I)Z");

        AtomicJNI::GetStaticMethodInfo(gCanGoBackMethodInfo,
                                     "com/atomicgameengine/player/AtomicWebView",
                                     "canGoBack",
                                     "(I)Z");

        AtomicJNI::GetStaticMethodInfo(gCanGoForwardMethodInfo,
                                     "com/atomicgameengine/player/AtomicWebView",
                                     "canGoForward",
                                     "(I)Z");

        AtomicJNI::GetStaticMethodInfo(gSetDimensionsMethodInfo,
                                     "com/atomicgameengine/player/AtomicWebView",
                                     "setDimensions",
                                     "(IIIII)V");

        initialized = true;
    }
}


bool UIPlatformWebView::PlatformCreateWebView()
{
    if (webViewHandle_ != UI_PLATFORM_WEBVIEW_INVALID_HANDLE)
    {
        ATOMIC_LOGWARNING("UIPlatformWebView::CreatePlatformWebView() - Attempting to create platform webview with valid handle");
        return false;
    }

    webViewHandle_ = webViewHandleCounter_++;

    android_webViewEnsureInitialized();
    return (bool) gCreateMethodInfo.GetEnv()->CallStaticBooleanMethod(gCreateMethodInfo.classID, gCreateMethodInfo.methodID, (jint) webViewHandle_, (jlong)0, (jlong)0);

}

void UIPlatformWebView::PlatformShowWebView(bool visible)
{
    if (webViewHandle_ == UI_PLATFORM_WEBVIEW_INVALID_HANDLE)
    {
        ATOMIC_LOGWARNING("UIPlatformWebView::ShowPlatformWebView() - Invalid webview handle");
        return;
    }

    if (visible)
    {
        gShowMethodInfo.GetEnv()->CallStaticVoidMethod(gShowMethodInfo.classID, gShowMethodInfo.methodID, (jint)webViewHandle_);
    }
    else
    {
        gHideMethodInfo.GetEnv()->CallStaticVoidMethod(gHideMethodInfo.classID, gHideMethodInfo.methodID, (jint)webViewHandle_);
    }
}

void UIPlatformWebView::PlatformDestroyWebView()
{
    if (webViewHandle_ == UI_PLATFORM_WEBVIEW_INVALID_HANDLE)
    {
        return;
    }

    webViewLookup_.Erase(webViewHandle_);

    gDestroyMethodInfo.GetEnv()->CallStaticVoidMethod(gDestroyMethodInfo.classID, gDestroyMethodInfo.methodID, (jint)webViewHandle_);

    webViewHandle_ = UI_PLATFORM_WEBVIEW_INVALID_HANDLE;

}

void UIPlatformWebView::PlatformPositionWebView()
{
    if (webViewHandle_ == UI_PLATFORM_WEBVIEW_INVALID_HANDLE)
    {
        return;
    }

    IntVector2 rootPos = ConvertToRoot(IntVector2( 0, 0));

    int height = GetSubsystem<Graphics>()->GetHeight();
    int posY = height - GetHeight() - rootPos.y_;

    gSetDimensionsMethodInfo.GetEnv()->CallStaticVoidMethod(gSetDimensionsMethodInfo.classID, gSetDimensionsMethodInfo.methodID, (jint)webViewHandle_, (jint)rootPos.x_, (jint)posY, (jint)GetWidth(), (jint)GetHeight());

}

void UIPlatformWebView::DestroyAll()
{
    android_webViewEnsureInitialized();
    gDestroyAllMethodInfo.GetEnv()->CallStaticVoidMethod(gDestroyAllMethodInfo.classID, gDestroyAllMethodInfo.methodID);
}

bool UIPlatformWebView::GoBack()
{

    if (webViewHandle_ == UI_PLATFORM_WEBVIEW_INVALID_HANDLE)
    {
        return false;
    }

    jboolean result = gGoBackMethodInfo.GetEnv()->CallStaticBooleanMethod(gGoBackMethodInfo.classID, gGoBackMethodInfo.methodID, (jint)webViewHandle_);
    return (bool)result;
}

bool UIPlatformWebView::GoForward()
{
    if (webViewHandle_ == UI_PLATFORM_WEBVIEW_INVALID_HANDLE)
    {
        return false;
    }

    jboolean result = gGoForwardMethodInfo.GetEnv()->CallStaticBooleanMethod(gGoForwardMethodInfo.classID, gGoForwardMethodInfo.methodID, (jint)webViewHandle_);
    return (bool)result;
}

bool UIPlatformWebView::CanGoBack() const
{
    if (webViewHandle_ == UI_PLATFORM_WEBVIEW_INVALID_HANDLE)
    {
        return false;
    }

    jboolean result = gCanGoBackMethodInfo.GetEnv()->CallStaticBooleanMethod(gCanGoBackMethodInfo.classID, gCanGoBackMethodInfo.methodID, (jint)webViewHandle_);
    return (bool)result;
}

bool UIPlatformWebView::CanGoForward() const
{
    if (webViewHandle_ == UI_PLATFORM_WEBVIEW_INVALID_HANDLE)
    {
        return false;
    }

    jboolean result = gCanGoForwardMethodInfo.GetEnv()->CallStaticBooleanMethod(gCanGoForwardMethodInfo.classID, gCanGoForwardMethodInfo.methodID, (jint)webViewHandle_);
    return (bool)result;
}

void UIPlatformWebView::PauseAll()
{
}

void UIPlatformWebView::ResumeAll()
{
}

void UIPlatformWebView::LoadURL(const String& url)
{
    if (webViewHandle_ == UI_PLATFORM_WEBVIEW_INVALID_HANDLE)
    {
        return;
    }

    jstring urlString = gRequestMethodInfo.GetEnv()->NewStringUTF(url.CString());
    gRequestMethodInfo.GetEnv()->CallStaticVoidMethod(gRequestMethodInfo.classID, gRequestMethodInfo.methodID, (jint)webViewHandle_, urlString);
    gRequestMethodInfo.GetEnv()->DeleteLocalRef(urlString);
}

void UIPlatformWebView::Reload()
{
    if (webViewHandle_ == UI_PLATFORM_WEBVIEW_INVALID_HANDLE)
    {
        return;
    }

}


}

extern "C"
{
void Java_com_atomicgameengine_player_AtomicWebView_nativeCallback(JNIEnv *env, jobject thiz, jstring data, jlong callback, jlong payload, jint type)
{
    /*
    atomic_webViewCallback cb          = (atomic_webViewCallback)callback;
    const char *dataString = env->GetStringUTFChars(data, 0);

    cb((void *)payload, (atomic_webViewCallbackType)type, dataString);

    env->ReleaseStringUTFChars(data, dataString);
    */
}
}


#endif
