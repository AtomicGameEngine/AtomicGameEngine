
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

#if ATOMIC_DESKTOP

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

            return app;
        }

    }

}

#endif
