using System;
using AtomicEngine;

// net genproject C:\Dev\atomic\AtomicGameEngine\Script\AtomicNET\AtomicNETProject.json WINDOWS
// net parse C:\Dev\atomic\AtomicGameEngine\Artifacts\AtomicNET\Build\AtomicNETService\bin\Debug\AtomicNETService.exe
// net compile C:\Dev\atomic\AtomicGameEngine\Artifacts\AtomicNET\Build\AtomicNET.sln


namespace AtomicTools
{

    public class Program
    {
        public static void Main(string[] args)
        {
            // create the service
            var app = NETServiceApplication.Create();

            // Subscribe to IPC NET commands
            app.SubscribeToEvent("IPCCmd", (eventType, eventData) =>
            {
                // get the command
                string command = eventData["command"];

                switch (command)
                {
                    // parse assembly for component information
                    case "parse":

                        // Get the assembly to parse
                        string assemblyPath = eventData["assemblyPath"];

                        // Inspect the assembly for components
                        var assemblyJSON = AtomicTools.InspectAssembly(assemblyPath);

                        // Return result
                        var vmap = new ScriptVariantMap();

                        // FIXME: update index operator to a generic
                        vmap.SetUInt("id", eventData.GetUInt("id"));

                        vmap["command"] = command;
                        vmap["result"] = assemblyJSON;

                        AtomicNET.GetSubsystem<IPC>().SendEventToBroker("IPCCmdResult", vmap);

                        break;

                    // exit service
                    case "exit":

                        app.SendEvent("ExitRequested");
                        break;

                }

            });

            // Managed code in charge of main loop
            while (app.RunFrame())
            {

            }

            // Shut 'er down
            app.Shutdown();

        }
    }
}


