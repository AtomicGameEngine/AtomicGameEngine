// Copyright (c) 2014-2017, THUNDERBEAST GAMES LLC All rights reserved
// Copyright (c) 2011-2017, The Game Engine Company LLC (Apache License 2.0)

package com.atomicgameengine.player;

import java.util.Hashtable;
import java.util.Set;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.view.ViewGroup;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.webkit.WebChromeClient;
import android.widget.RelativeLayout;

/**
 *    Java class that manages WebView instances. This maps directly to the platformWebView C API
 */
public class AtomicWebView {

    private static native void nativeCallback(String data, long callback, long payload, int type);

    private static void deferNativeCallback(String data, long callback, long payload, int type)
    {
        final String fData = data;
        final long fCallback = callback;
        final long fPayload = payload;
        final int fType = type;

        // TODO: does this require queueEvent?
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                nativeCallback(fData, fCallback, fPayload, fType);
            }
        });
    }

    public static boolean create(final int handle, final long callback, final long payload)
    {
        final Context context = activity;

        activity.runOnUiThread(new Runnable() {

            @Override
            public void run() {
                WebView webView = new WebView(context);

                AtomicWebView.webViews.put(handle, webView);

                webView.setWebChromeClient(new WebChromeClient());
                webView.setWebViewClient(new WebViewClient() {
                    @Override
                    public void onReceivedError(WebView view, int errorCode,
                            String description, String failingUrl) {
                        super.onReceivedError(view, errorCode, description, failingUrl);
                        deferNativeCallback(description, callback, payload, 1);
                    }

                    @Override
                    public boolean shouldOverrideUrlLoading(WebView view, String url) {
                        return false;
                    }

                    @Override
                    public void onPageStarted(WebView view, String url,
                            Bitmap favicon) {
                        super.onPageStarted(view, url, favicon);
                        deferNativeCallback(url, callback, payload, 0);
                    }

                });

                webView.getSettings().setJavaScriptEnabled(true);
                webView.getSettings().setJavaScriptCanOpenWindowsAutomatically(true);
            }

        });

        return true;

    }

    public static void show(final int handle)
    {
        final ViewGroup layout = rootLayout;

        activity.runOnUiThread(new Runnable() {

            @Override
            public void run() {
                WebView webView = AtomicWebView.webViews.get(handle);

                // Important: this is currently being run per frame
                // and hide/show should be cached native side
                if(webView.getParent() == layout)
                {
                    return;
                }

                layout.addView(webView);
                RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(webView.getLayoutParams());
                params.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM);
                webView.setLayoutParams(params);
            }
        });
    }

    public static void hide(final int handle)
    {
        final ViewGroup layout = rootLayout;

        activity.runOnUiThread(new Runnable() {

            @Override
            public void run() {
                WebView webView = AtomicWebView.webViews.get(handle);

                if(webView.getParent() == layout)
                    layout.removeView(webView);
            }
        });
    }

    public static void destroy(final int handle)
    {
        final ViewGroup layout = rootLayout;

        activity.runOnUiThread(new Runnable() {

            @Override
            public void run() {
                WebView webView = webViews.get(handle);
                AtomicWebView.webViews.remove(handle);

                if(webView != null && webView.getParent() == layout)
                    layout.removeView(webView);
            }
        });
    }

    public static void destroyAll()
    {
        final ViewGroup layout = rootLayout;

        activity.runOnUiThread(new Runnable() {

            @Override
            public void run() {
                Set<Integer> keys = webViews.keySet();
                for(Integer key: keys){
                    WebView webView = AtomicWebView.webViews.remove(key);

                    if(webView != null && webView.getParent() == layout)
                        layout.removeView(webView);
                }
            }
        });
    }

    public static void request(final int handle, final String url)
    {
        activity.runOnUiThread(new Runnable() {

            @Override
            public void run() {
                WebView webView = AtomicWebView.webViews.get(handle);
                webView.loadUrl(url);
            }
        });
    }

    public static boolean goBack(final int handle)
    {
        final AtomicWebViewPayload payload = new AtomicWebViewPayload();

        activity.runOnUiThread(new Runnable() {

            @Override
            public void run() {
                WebView webView = AtomicWebView.webViews.get(handle);

                if(webView.canGoBack()) {
                    payload.boolValue = true;
                    webView.goBack();
                }
                else {
                    payload.boolValue = false;
                }

                synchronized (payload) {
                    payload.notify();
                }
            }
        });

        try {
            synchronized (payload) {
                payload.wait();
            }
            return payload.boolValue;
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        return false;
    }

    public static boolean goForward(final int handle)
    {
        final AtomicWebViewPayload payload = new AtomicWebViewPayload();

        activity.runOnUiThread(new Runnable() {

            @Override
            public void run() {
                WebView webView = AtomicWebView.webViews.get(handle);

                if(webView.canGoForward()) {
                    payload.boolValue = true;
                    webView.goForward();
                }
                else {
                    payload.boolValue = false;
                }

                synchronized (payload) {
                    payload.notify();
                }
            }
        });

        try {
            synchronized (payload) {
                payload.wait();
            }
            return payload.boolValue;
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        return false;
    }

    public static boolean canGoBack(final int handle)
    {
        final AtomicWebViewPayload payload = new AtomicWebViewPayload();

        activity.runOnUiThread(new Runnable() {

            @Override
            public void run() {
                WebView webView = AtomicWebView.webViews.get(handle);

                payload.boolValue = webView.canGoBack();

                synchronized (payload) {
                    payload.notify();
                }
            }
        });

        try {
            synchronized (payload) {
                payload.wait();
            }
            return payload.boolValue;
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        return false;
    }

    public static boolean canGoForward(final int handle)
    {
        final AtomicWebViewPayload payload = new AtomicWebViewPayload();

        activity.runOnUiThread(new Runnable() {

            @Override
            public void run() {
                WebView webView = AtomicWebView.webViews.get(handle);

                payload.boolValue = webView.canGoForward();

                synchronized (payload) {
                    payload.notify();
                }
            }
        });

        try {
            synchronized (payload) {
                payload.wait();
            }
            return payload.boolValue;
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        return false;
    }

    public static void setDimensions(final int handle, final int x, final int y, final int width, final int height)
    {
        activity.runOnUiThread(new Runnable() {

            @Override
            public void run() {
                WebView webView = AtomicWebView.webViews.get(handle);
                RelativeLayout.LayoutParams params = (RelativeLayout.LayoutParams)webView.getLayoutParams();
                if(params != null)
                {
                    params.bottomMargin = y;
                    params.leftMargin = x;
                    params.width = width;
                    params.height = height;
                    webView.setLayoutParams(params);
                }

            }
        });
    }

    public static void setRootLayout(ViewGroup value)
    {
        rootLayout = value;
        activity = (Activity)rootLayout.getContext();
    }

    protected static ViewGroup rootLayout;
    protected static Activity activity;
    protected static Hashtable<Integer, WebView> webViews = new Hashtable<Integer, WebView>();
}

class AtomicWebViewPayload
{
    public Boolean boolValue;
    public int intValue;
}
