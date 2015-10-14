using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Reflection;

namespace Atomic.Bootstrap
{
    internal class AssemblyLoaderCache
    {
        private readonly ConcurrentDictionary<AssemblyName, object> _assemblyLoadLocks = new ConcurrentDictionary<AssemblyName, object>(AssemblyNameComparer.Ordinal);
        private readonly ConcurrentDictionary<AssemblyName, Assembly> _assemblyCache = new ConcurrentDictionary<AssemblyName, Assembly>(AssemblyNameComparer.Ordinal);

        public Assembly GetOrAdd(AssemblyName name, Func<AssemblyName, Assembly> factory)
        {
            // If the assembly was already loaded use it
            Assembly assembly;
            if (_assemblyCache.TryGetValue(name, out assembly))
            {
                return assembly;
            }

            var loadLock = _assemblyLoadLocks.GetOrAdd(name, new object());

            try
            {
                // Concurrently loading the assembly might result in two distinct instances of the same assembly
                // being loaded. This was observed when loading via Assembly.LoadStream. Prevent this by locking on the name.
                lock (loadLock)
                {
                    if (_assemblyCache.TryGetValue(name, out assembly))
                    {
                        // This would succeed in case the thread was previously waiting on the lock when assembly
                        // load was in progress
                        return assembly;
                    }

                    assembly = factory(name);

                    if (assembly != null)
                    {
                        _assemblyCache[name] = assembly;
                    }
                }
            }
            finally
            {
                _assemblyLoadLocks.TryRemove(name, out loadLock);
            }

            return assembly;
        }

        private class AssemblyNameComparer : IEqualityComparer<AssemblyName>
        {
            public static IEqualityComparer<AssemblyName> Ordinal = new AssemblyNameComparer();

            public bool Equals(AssemblyName x, AssemblyName y)
            {
                return
                    string.Equals(x.Name, y.Name, StringComparison.Ordinal) &&
                    string.Equals(x.CultureName ?? "", y.CultureName ?? "", StringComparison.Ordinal);
            }

            public int GetHashCode(AssemblyName obj)
            {
                var hashCode = 0;
                if (obj.Name != null)
                {
                    hashCode ^= obj.Name.GetHashCode();
                }

                hashCode ^= (obj.CultureName ?? "").GetHashCode();
                return hashCode;
            }
        }
    }
}
