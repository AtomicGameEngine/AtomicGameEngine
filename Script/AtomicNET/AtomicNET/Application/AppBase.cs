

namespace AtomicEngine
{

    public partial class AppBase : Application
    {

        public virtual int Run()
        {

            // Managed code in charge of main loop
            while (RunFrame())
            {

            }

            // Shut 'er down
            Shutdown();

            return 0;
        }

    }

}
