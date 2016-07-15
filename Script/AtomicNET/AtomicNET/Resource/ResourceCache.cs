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

    }

}
