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

        // Tooltip which will be displayed in editor
        public string Tooltip
        {
            get { return tooltip; }
            set { tooltip = value; }
        }

        private string tooltip;

    }


}
