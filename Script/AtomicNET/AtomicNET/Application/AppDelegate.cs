using System;
using System.Reflection;
using System.Linq;

namespace AtomicEngine
{

    public abstract class AppDelegate : NETScriptObject
    {

        public virtual void Start() { }

        // called after application RunFrame
        public virtual void PostFrame() { }

        public virtual void Shutdown() { }

    }


}
