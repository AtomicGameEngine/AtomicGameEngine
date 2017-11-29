
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
        public static AppBase AppBase { get; private set; }


        public static int Run<T>(string[] args) where T : AppDelegate, new()
        {
            return Run(typeof(T), args);
        }

        public static int Run(Type appDelegateType, string[] args)
        {
            // Create the Application

#if ATOMIC_DESKTOP
            AppBase = NETIPCPlayerApp.Create(args);
#endif

#if ATOMIC_IOS
            // On iOS, we need to set main ready as main() isn't being called
            SDLEvents.SetMainReady();
#endif


#if ATOMIC_MOBILE

            AppBase = NETAtomicPlayer.Create(args);
            
            var renderer = AtomicNET.GetSubsystem<Renderer>();
            renderer.ReuseShadowMaps = false;
            renderer.ShadowQuality = ShadowQuality.SHADOWQUALITY_SIMPLE_16BIT;
#endif

            CSComponentAssembly.PreloadClassAssemblies();

            appDelegate = (AppDelegate)Activator.CreateInstance(appDelegateType);

            try
            {


                appDelegate.Start();

                // Managed code in charge of main loop
                while (AppBase.RunFrame())
                {
                    appDelegate.PostFrame();
                }

                appDelegate.Shutdown();

                // Shut 'er down
                AppBase.Shutdown();
            }
            catch (Exception e)
            {
                if (e.InnerException != null)
                {
                    Log.Error(e.InnerException.StackTrace);
                    // rethrow inner exception
                    throw e.InnerException;
                }
                else
                {
                    Log.Error(e.StackTrace);
                    // rethrow
                    throw;
                }
            }
            finally
            {
                AppBase = null;
            }

            return 0;
        }


#if ATOMIC_DESKTOP
        public static int RunServer<T>(string[] args) where T : AppDelegate, new()
        {
            return RunServer(typeof(T), args);
        }

        public static int RunServer(Type appDelegateType, string[] args)
        {
            // Create the Application
            AppBase = NETIPCServerApp.Create(args);

            appDelegate = (AppDelegate)Activator.CreateInstance(appDelegateType);

            try
            {
                appDelegate.Start();

                // Managed code in charge of main loop
                while (AppBase.RunFrame())
                {
                    appDelegate.PostFrame();
                }

                appDelegate.Shutdown();

                // Shut 'er down
                AppBase.Shutdown();
            }
            catch (Exception e)
            {
                if (e.InnerException != null)
                {
                    Log.Error(e.InnerException.StackTrace);
                    // rethrow inner exception
                    throw e.InnerException;
                }
                else
                {
                    Log.Error(e.StackTrace);
                    // rethrow
                    throw e;
                }
            }
            finally
            {
                AppBase = null;
            }


            return 0;
        }
#endif
        internal static AppDelegate appDelegate;

    }
}
