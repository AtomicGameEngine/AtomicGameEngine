using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace AtomicEngine
{

    public class InspectorAttribute : Attribute
    {
        public InspectorAttribute(string DefaultValue = "")
        {
        }

        public string DefaultValue;
    }

}
