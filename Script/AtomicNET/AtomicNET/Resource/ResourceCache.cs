using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace AtomicEngine
{

    public partial class ResourceCache : AObject
    {

        public T GetResource<T>(string path) where T : Resource
        {
            return (T) GetResource(typeof(T).Name, path);
        }

        public T Get<T>(string path) where T : Resource
        {
            return (T)GetResource(typeof(T).Name, path);
        }

        public System.IO.Stream GetFileStream(string name, bool sendEventOnFailure = true)
        {
            File file = GetFile(name, sendEventOnFailure);
            if (file != null &&
                file.IsOpen())
            {
                return file.ToStream();
            }

            return null;
        }

        public List<string> GetResourceDirs()
        {
            List<string> resourceDirs = new List<string>();

            ResourceCache cache = AtomicNET.GetSubsystem<ResourceCache>();

            for (uint i = 0; i < cache.GetNumResourceDirs(); i++)
            {
                resourceDirs.Add(cache.GetResourceDir(i));
            }

            return resourceDirs;
        }

        /// <summary>
        ///  Release all resources. When called with the force flag false, releases all currently unused resources.
        /// </summary>
        public void ReleaseAllResources(bool force = false)
        {
            // We need to GC before calling native ResourceCache::ReleaseAllResources, to ensure all managed resource references are down
            // otherwise, the cache will hold onto the resource
            NativeCore.RunGC();

            csi_Atomic_ResourceCache_ReleaseAllResources(nativeInstance, force);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void csi_Atomic_ResourceCache_ReleaseAllResources(IntPtr self, bool force);

    }
}
