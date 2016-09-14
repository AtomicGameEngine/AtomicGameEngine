
using System;

namespace AtomicEngine
{

    public interface IAtomicSDLSurface
    {
        void Remove();
        bool IsAlive { get; }
    }

    public partial class Application : AObject
    {

        public static int Run<T>(string[] args) where T : AppDelegate, new()
        {
            return Run(typeof(T), args);
        }

        public static int Run(Type appDelegateType, string[] args)
        {
            // Create the Application

            AppBase app = null;

#if ATOMIC_DESKTOP
            app = NETIPCPlayerApp.Create(args);
#endif

#if ATOMIC_IOS
            // On iOS, we need to set main ready as main() isn't being called
            SDLEvents.SetMainReady();
#endif


#if ATOMIC_ANDROID || ATOMIC_IOS

            app = NETAtomicPlayer.Create(args);
            
            var renderer = AtomicNET.GetSubsystem<Renderer>();
            renderer.ReuseShadowMaps = false;
            renderer.ShadowQuality = ShadowQuality.SHADOWQUALITY_SIMPLE_16BIT;
#endif

            appDelegate = (AppDelegate)Activator.CreateInstance(appDelegateType);

            appDelegate.Start();

            // Managed code in charge of main loop
            while (app.RunFrame())
            {
                appDelegate.PostFrame();
            }

            appDelegate.Shutdown();

            // Shut 'er down
            app.Shutdown();

            return 0;
        }


        internal static AppDelegate appDelegate;

    }

}
