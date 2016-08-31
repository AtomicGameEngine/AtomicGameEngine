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