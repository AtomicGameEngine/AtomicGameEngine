using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace AtomicEngine
{

    public partial class NETServiceApplication : IPCClientApp
    {

        public static NETServiceApplication Create()
        {
            // Initialize AtomicNET
            AtomicNET.Initialize();

            var app = CreateInternal();

            app.Initialize();

            AtomicNET.RegisterSubsystem("IPC");

            return app;
        }

    }

}