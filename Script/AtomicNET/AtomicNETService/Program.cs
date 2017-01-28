using System;
using AtomicEngine;

namespace AtomicTools
{

    public class Program
    {
        public static void Main(string[] args)
        {
            for (uint i = 0; i < args.Length; i++)
            {
                if (args[i] == "--debugassembly" && i + 1 < args.Length)
                {
                    var assemblyJSON = AtomicTools.InspectAssembly(args[i + 1]);
                    Console.WriteLine(assemblyJSON);
                    return;
                }
            }

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
