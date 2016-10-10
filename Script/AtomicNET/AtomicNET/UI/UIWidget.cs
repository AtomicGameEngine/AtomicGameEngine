using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using static System.Reflection.IntrospectionExtensions;

namespace AtomicEngine
{
    public partial class WidgetEvent : NativeEventData
    {
        public bool Handled
        {
            get
            {
                return GetHandled();
            }
            set
            {
                SetHandled(value);
            }
        }

        public bool GetHandled()
        {
            return scriptMap.GetBool("Handled");
        }


        public void SetHandled(bool handled = true)
        {
            scriptMap.SetBool("Handled", true); // this isn't great, though syncs up for future reads
            // sets the native event data
            Set("Handled", handled);
        }
    }

    public partial class UIWidget : AObject
    {
        static Dictionary<IntPtr, UIWidget> widgets = new Dictionary<IntPtr, UIWidget>();

        internal override void PostNativeUpdate()
        {
            widgets[nativeInstance] = this;

            SubscribeToEvent<WidgetDeletedEvent>(this, e =>
            {
                widgets.Remove(nativeInstance);

            });
        }
    }

}
