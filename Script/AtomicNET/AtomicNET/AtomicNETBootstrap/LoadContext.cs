using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Threading;
//#if DNXCORE50
using System.Runtime.Loader;
//#endif

namespace Atomic.Bootstrap
{
    public abstract class LoadContext : AssemblyLoadContext, IAssemblyLoadContext
    {
        private readonly AssemblyLoaderCache _cache = new AssemblyLoaderCache();
        //private readonly string _friendlyName;

        public LoadContext()
        {
        }

        public LoadContext(string friendlyName)
        {
            //_friendlyName = friendlyName;
        }

        protected override Assembly Load(AssemblyName assemblyName)
        {
            return _cache.GetOrAdd(assemblyName, LoadAssembly);
        }

        Assembly IAssemblyLoadContext.Load(AssemblyName assemblyName)
        {
            return LoadFromAssemblyName(assemblyName);
        }

        public abstract Assembly LoadAssembly(AssemblyName assemblyName);

        public Assembly LoadFile(string path)
        {
            // Look for platform specific native image
            string nativeImagePath = GetNativeImagePath(path);

            if (nativeImagePath != null)
            {
                return LoadFromNativeImagePath(nativeImagePath, path);
            }

            return LoadFromAssemblyPath(path);
        }

        public Assembly LoadStream(Stream assembly, Stream assemblySymbols)
        {
            if (assemblySymbols == null)
            {
                return LoadFromStream(assembly);
            }

            return LoadFromStream(assembly, assemblySymbols);
        }

        public static void InitializeDefaultContext(LoadContext loadContext)
        {
            AssemblyLoadContext.InitializeDefaultContext(loadContext);
        }

        private string GetNativeImagePath(string ilPath)
        {
            var directory = Path.GetDirectoryName(ilPath);
            var arch = IntPtr.Size == 4 ? "x86" : "AMD64";

            var nativeImageName = Path.GetFileNameWithoutExtension(ilPath) + ".ni.dll";
            var nativePath = Path.Combine(directory, arch, nativeImageName);

            if (File.Exists(nativePath))
            {
                return nativePath;
            }
            else
            {
                // Runtime is arch sensitive so the ni is in the same folder as IL
                nativePath = Path.Combine(directory, nativeImageName);
                if (File.Exists(nativePath))
                {
                    return nativePath;
                }
            }

            return null;
        }

        public virtual void Dispose()
        {

        }
    }

}
