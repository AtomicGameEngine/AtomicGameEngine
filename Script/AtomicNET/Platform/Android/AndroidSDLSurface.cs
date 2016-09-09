/*
Copyright(c) 2015 Xamarin Inc.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

using System;
using System.Runtime.InteropServices;
using System.Threading;
using Android.App;
using Android.Content;
using Android.Util;
using Android.Views;
using Java.Lang;
using Org.Libsdl.App;

namespace AtomicEngine
{

    /// <summary>
    /// A controller that provides a SDLSurface that can be used in any activity.
    /// Make sure you handle these events in your Activity:
    /// - OnResume
    /// - OnPause
    /// - OnLowMemory
    /// - OnDestroy
    /// - DispatchKeyEvent
    /// - OnWindowFocusChanged
    /// </summary>
    public class AndroidSDLSurface : IAtomicSDLSurface
    {
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate int SDLEntryCallback();

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void RegisterSDLEntryCallback(SDLEntryCallback callback);

        public SDLSurface SdlSurface;

        AndroidSDLSurface(SDLSurface sdlSurface)
        {
            SdlSurface = sdlSurface;
        }

        public void Remove()
        {
            var vg = SdlSurface?.Parent as ViewGroup;
            if (SdlSurface != null && vg != null)
            {
                //vg.RemoveView(SdlSurface);
                SdlSurface.Enabled = false;
                SdlSurface.Visibility = ViewStates.Gone;
            }
        }

        public bool IsAlive => SDLActivity.MIsSurfaceReady;

        static Type appDelegateType;

        /// <summary>
        /// Creates a view (SurfaceView) that can be added anywhere
        /// </summary>
        public static SDLSurface CreateSurface(Activity activity, bool finishActivtiyOnExit = false, Type appDelegateType = null)
        {
            AndroidSDLSurface.appDelegateType = appDelegateType;
            var surface = SDLActivity.CreateSurface(activity);
            SetSDLEntryCallback(finishActivtiyOnExit, surface);
            return surface;
        }

        public static void OnResume()
        {
            SDLActivity.OnResume();
        }

        public static void OnPause()
        {
            SDLActivity.OnPause();
        }

        public static void OnLowMemory()
        {
            SDLActivity.OnLowMemory();
        }

        public static void OnDestroy()
        {
            SDLActivity.OnDestroy();
        }

        public static bool DispatchKeyEvent(KeyEvent keyEvent)
        {
            return SDLActivity.DispatchKeyEvent(keyEvent);
        }

        public static void OnWindowFocusChanged(bool focus)
        {
            SDLActivity.OnWindowFocusChanged(focus);
        }

        /// <summary>
        /// The simpliest way to launch a game. It opens a special full-screen activity
        /// </summary>
        public static void RunInActivity()
        {
            SetSDLEntryCallback(true, null);
            var context = Android.App.Application.Context;
            var intent = new Intent(context, typeof(Org.Libsdl.App.AtomicActivity));
            intent.AddFlags(ActivityFlags.NewTask);
            context.StartActivity(intent);
        }

        static AndroidSDLSurface __surface;

        static void SetSDLEntryCallback(bool finishActivityOnExit, SDLSurface surface)
        {
            __surface = new AndroidSDLSurface(surface);

            SDLActivity.FinishActivityOnNativeExit = finishActivityOnExit;

            RegisterSDLEntryCallback( () => {
                return Application.Run(appDelegateType, new string[0]);
            });
        }
    }
}