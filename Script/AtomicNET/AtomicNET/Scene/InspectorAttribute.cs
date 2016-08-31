using System;

namespace AtomicEngine
{

    [AttributeUsage(AttributeTargets.Field)]
    public class InspectorAttribute : Attribute
    {
        public InspectorAttribute(string DefaultValue = "")
        {
        }

        public readonly string DefaultValue;
    }

}
