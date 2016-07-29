using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace AtomicEngine
{

    public partial class AObject : RefCounted
    {
        internal Dictionary<uint, EventDelegate> eventHandlers = new Dictionary<uint, EventDelegate>();

        public static T GetSubsystem<T>() where T : AObject
        {
            return AtomicNET.GetSubsystem<T>();
        }



        internal void HandleEvent(uint eventType, ScriptVariantMap eventData)
        {
            eventHandlers[eventType](eventType, eventData);
        }

        public void SubscribeToEvent(uint eventType, EventDelegate eventDelegate)
        {
            NETCore.RegisterNETEventType(eventType);
            eventHandlers[eventType] = eventDelegate;
            NativeCore.SubscribeToEvent(this, eventType);
        }

        public void SubscribeToEvent(string eventType, EventDelegate eventDelegate)
        {
            SubscribeToEvent(AtomicNET.StringToStringHash(eventType), eventDelegate);
        }

        public void SendEvent(string eventType, ScriptVariantMap eventData = null)
        {

            csb_Atomic_AObject_SendEvent(this.nativeInstance, eventType, eventData == null ? IntPtr.Zero : eventData.nativeInstance);

        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void csb_Atomic_AObject_SendEvent(IntPtr self, string eventType, IntPtr variantMap);


    }

}