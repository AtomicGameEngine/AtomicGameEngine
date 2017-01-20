using System;

namespace AtomicEngine
{

    [AttributeUsage(AttributeTargets.Field)]
    public class InspectorAttribute : Attribute
    {
        public InspectorAttribute(string DefaultValue = "")
        {
            this.DefaultValue = DefaultValue;
        }

        public readonly string DefaultValue;

        /// <summary>
        ///Specify a fixed array size for field 
        /// </summary>
        public int ArraySize
        {
            get { return arraySize; }
            set { arraySize = value; }
        }

        private int arraySize = 0;

        /// <summary>
        /// Tooltip which will be displayed in editor 
        /// </summary>
        public string Tooltip
        {
            get { return tooltip; }
            set { tooltip = value; }
        }

        private string tooltip;

    }


}
