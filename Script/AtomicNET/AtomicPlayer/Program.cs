
using AtomicEngine;

public class Program
{
    public static void Main(string[] args)
    {               
        // Create the Application
        var app = NETIPCPlayerApp.Create(args);

        // Managed code in charge of main loop
        while (app.RunFrame())
        {

        }

        // Shut 'er down
        app.Shutdown();
    }
}


