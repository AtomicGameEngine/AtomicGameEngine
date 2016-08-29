using System;
using System.IO;
using System.Reflection;

namespace AtomicEngine
{


    public partial class PlayerApp : AppBase
    {

        static protected void RegisterSubsystems()
        {
            // TODO: Refactor these registrations
            AtomicNET.RegisterSubsystem("FileSystem");

            AtomicNET.RegisterSubsystem("Graphics");
            AtomicNET.RegisterSubsystem("Player");

            AtomicNET.RegisterSubsystem("Input");
            AtomicNET.RegisterSubsystem("Renderer");

            AtomicNET.RegisterSubsystem("ResourceCache");
            AtomicNET.Cache = AtomicNET.GetSubsystem<ResourceCache>();

        }

    }


}