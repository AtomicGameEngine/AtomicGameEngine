using System;
namespace AtomicEngine
{

    public partial class CSComponent : ScriptComponent
    {

        public override string GetTypeName()
        {
            return GetType().Name;
        }

        public bool Started
        {
            get { return started; }
        }

        internal bool started = false;

    }

}
