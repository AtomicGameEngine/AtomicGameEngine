using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace AtomicEngine
{

    public partial class NETIPCPlayerApp : IPCPlayerApp
    {

        public static NETIPCPlayerApp Create(bool headless = false)
        {
            // Initialize AtomicNET
            AtomicNET.Initialize();

            var app = CreateInternal();

            app.Initialize();

            AtomicNET.RegisterSubsystem("Graphics");
            AtomicNET.RegisterSubsystem("Player");

            return app;
        }

    }

}
