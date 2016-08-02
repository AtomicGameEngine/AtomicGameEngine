using System;
using System.Reflection;


namespace AtomicEngine
{
    public partial class NETAtomicPlayer : PlayerApp
    {

        /// <summary>
        ///  AtomicPlayer for NET deployments
        /// </summary>
        /// <param name="args"></param>
        /// <param name="headless"></param>
        /// <returns></returns>
        public static NETAtomicPlayer Create(string[] args, bool headless = false)
        {
            DeployedApp = true;

            // Initialize AtomicNET
            AtomicNET.Initialize();

            var app = CreateInternal();

            app.Initialize();

            RegisterSubsystems();

            ExecuteAtomicMain(args);

            return app;
        }


    }

}
