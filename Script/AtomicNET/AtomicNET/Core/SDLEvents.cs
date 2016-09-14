using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
    public static class SDLEvents
    {
        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        static extern int SDL_SendWindowEvent(IntPtr window, byte windowevent, int data1, int data2);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        static extern int SDL_SendAppEvent(byte eventType);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        static extern void SDL_SetMainReady();

        public static void SetMainReady() => SDL_SetMainReady();

        public static void SendWindowEvent(SdlWindowEvent wndEvent, int data1 = 0, int data2 = 0)
        {
            var graphics = AtomicNET.GetSubsystem<Graphics>();
            SDL_SendWindowEvent(graphics.SDLWindow, (byte) wndEvent, data1, data2);
        }

        public static void SendAppEvent(SdlEventType eventType) => SDL_SendAppEvent((byte)eventType);
    }

    //see SDL_Video.h
    public enum SdlWindowEvent
    {
        SDL_WINDOWEVENT_NONE,           /* Never used */
        SDL_WINDOWEVENT_SHOWN,          /* Window has been shown */
        SDL_WINDOWEVENT_HIDDEN,         /* Window has been hidden */
        SDL_WINDOWEVENT_EXPOSED,        /* Window has been exposed and should be redrawn */
        SDL_WINDOWEVENT_MOVED,          /* Window has been moved to data1, data2 */
        SDL_WINDOWEVENT_RESIZED,        /* Window has been resized to data1xdata2 */
        SDL_WINDOWEVENT_SIZE_CHANGED,   /* The window size has changed, either as a result of an API call or through the system or user changing the window size. */
        SDL_WINDOWEVENT_MINIMIZED,      /* Window has been minimized */
        SDL_WINDOWEVENT_MAXIMIZED,      /* Window has been maximized */
        SDL_WINDOWEVENT_RESTORED,       /* Window has been restored to normal size and position */
        SDL_WINDOWEVENT_ENTER,          /* Window has gained mouse focus */
        SDL_WINDOWEVENT_LEAVE,          /* Window has lost mouse focus */
        SDL_WINDOWEVENT_FOCUS_GAINED,   /* Window has gained keyboard focus */
        SDL_WINDOWEVENT_FOCUS_LOST,     /* Window has lost keyboard focus */
        SDL_WINDOWEVENT_CLOSE           /* The window manager requests that the window be closed */
    }

    //see SDL_Events.h
    public enum SdlEventType
    {
        SDL_FIRSTEVENT = 0,     /**< Unused (do not remove) */

        /* Application events */
        SDL_QUIT = 0x100, /**< User-requested quit */

        /* These application events have special meaning on iOS, see README-ios.txt for details */
        SDL_APP_TERMINATING,        /**< The application is being terminated by the OS
                                     Called on iOS in applicationWillTerminate()
                                     Called on Android in onDestroy()
                                */
        SDL_APP_LOWMEMORY,          /**< The application is low on memory, free memory if possible.
                                     Called on iOS in applicationDidReceiveMemoryWarning()
                                     Called on Android in onLowMemory()
                                */
        SDL_APP_WILLENTERBACKGROUND, /**< The application is about to enter the background
                                     Called on iOS in applicationWillResignActive()
                                     Called on Android in onPause()
                                */
        SDL_APP_DIDENTERBACKGROUND, /**< The application did enter the background and may not get CPU for some time
                                     Called on iOS in applicationDidEnterBackground()
                                     Called on Android in onPause()
                                */
        SDL_APP_WILLENTERFOREGROUND, /**< The application is about to enter the foreground
                                     Called on iOS in applicationWillEnterForeground()
                                     Called on Android in onResume()
                                */
        SDL_APP_DIDENTERFOREGROUND, /**< The application is now interactive
                                     Called on iOS in applicationDidBecomeActive()
                                     Called on Android in onResume()
                                */

        /* Window events */
        SDL_WINDOWEVENT = 0x200, /**< Window state change */
        SDL_SYSWMEVENT,             /**< System specific event */

        /* Keyboard events */
        SDL_KEYDOWN = 0x300, /**< Key pressed */
        SDL_KEYUP,                  /**< Key released */
        SDL_TEXTEDITING,            /**< Keyboard text editing (composition) */
        SDL_TEXTINPUT,              /**< Keyboard text input */

        /* Mouse events */
        SDL_MOUSEMOTION = 0x400, /**< Mouse moved */
        SDL_MOUSEBUTTONDOWN,        /**< Mouse button pressed */
        SDL_MOUSEBUTTONUP,          /**< Mouse button released */
        SDL_MOUSEWHEEL,             /**< Mouse wheel motion */

        /* Joystick events */
        SDL_JOYAXISMOTION = 0x600, /**< Joystick axis motion */
        SDL_JOYBALLMOTION,          /**< Joystick trackball motion */
        SDL_JOYHATMOTION,           /**< Joystick hat position change */
        SDL_JOYBUTTONDOWN,          /**< Joystick button pressed */
        SDL_JOYBUTTONUP,            /**< Joystick button released */
        SDL_JOYDEVICEADDED,         /**< A new joystick has been inserted into the system */
        SDL_JOYDEVICEREMOVED,       /**< An opened joystick has been removed */

        /* Game controller events */
        SDL_CONTROLLERAXISMOTION = 0x650, /**< Game controller axis motion */
        SDL_CONTROLLERBUTTONDOWN,          /**< Game controller button pressed */
        SDL_CONTROLLERBUTTONUP,            /**< Game controller button released */
        SDL_CONTROLLERDEVICEADDED,         /**< A new Game controller has been inserted into the system */
        SDL_CONTROLLERDEVICEREMOVED,       /**< An opened Game controller has been removed */
        SDL_CONTROLLERDEVICEREMAPPED,      /**< The controller mapping was updated */

        /* Touch events */
        SDL_FINGERDOWN = 0x700,
        SDL_FINGERUP,
        SDL_FINGERMOTION,

        /* Gesture events */
        SDL_DOLLARGESTURE = 0x800,
        SDL_DOLLARRECORD,
        SDL_MULTIGESTURE,

        /* Clipboard events */
        SDL_CLIPBOARDUPDATE = 0x900, /**< The clipboard changed */

        /* Drag and drop events */
        SDL_DROPFILE = 0x1000, /**< The system requests a file open */

        /* Render events */
        SDL_RENDER_TARGETS_RESET = 0x2000, /**< The render targets have been reset */

        /** Events ::SDL_USEREVENT through ::SDL_LASTEVENT are for your use,
         *  and should be allocated with SDL_RegisterEvents()
         */
        SDL_USEREVENT = 0x8000,

        /**
         *  This last event is only for bounding internal arrays
         */
        SDL_LASTEVENT = 0xFFFF
    }
}
