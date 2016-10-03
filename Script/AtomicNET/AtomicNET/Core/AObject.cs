using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace AtomicEngine
{

    public partial class AObject : RefCounted
    {
        private Dictionary<uint, EventDelegate> EventHandlers
        {
            get
            {
                if (eventHandlers == null)
                {
                    eventHandlers = new Dictionary<uint, EventDelegate>();
                }
                return eventHandlers;
            }
        }
        private Dictionary<uint, EventDelegate> eventHandlers;

        private Dictionary<uint, NativeEventDelegate> NativeEventHandlers
        {
            get
            {
                if (nativeHandlers == null)
                {
                    nativeHandlers = new Dictionary<uint, NativeEventDelegate>();
                }
                return nativeHandlers;
            }
        }
        private Dictionary<uint, NativeEventDelegate> nativeHandlers;

        public static T GetSubsystem<T>() where T : AObject
        {
            return AtomicNET.GetSubsystem<T>();
        }

        public void UnsubscribeFromAllEvents()
        {
            if (eventHandlers != null)
                eventHandlers.Clear();

            if(nativeHandlers != null)
                nativeHandlers.Clear();

            NativeCore.UnsubscribeFromAllEvents(this);
            csi_Atomic_AObject_UnsubscribeFromAllEvents(nativeInstance);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void csi_Atomic_AObject_UnsubscribeFromAllEvents(IntPtr self);

        internal void HandleEvent(uint eventType, ScriptVariantMap eventData, NativeEventData nativeEventData)
        {
            NativeEventDelegate nativeDelegate;

            if (NativeEventHandlers.TryGetValue(eventType, out nativeDelegate))
            {
                nativeDelegate(nativeEventData);
                return;
            }
            
            EventDelegate eventDelegate;

            if (EventHandlers.TryGetValue(eventType, out eventDelegate))
            {
                eventDelegate(eventType, eventData);
                return;
            }
          
            // Reaching here should be a warning, possibly an error

        }

        public void SubscribeToEvent(uint eventType, EventDelegate eventDelegate)
        {
            NETCore.RegisterNETEventType(eventType);
            EventHandlers[eventType] = eventDelegate;
            NativeCore.SubscribeToEvent(this, eventType);
        }

        public void SubscribeToEvent(string eventType, EventDelegate eventDelegate)
        {
            SubscribeToEvent(AtomicNET.StringToStringHash(eventType), eventDelegate);
        }

        public void UnsubscribeFromEvent(uint eventType)
        {
            NativeCore.UnsubscribeFromEvent(this, eventType);
            EventHandlers.Remove(eventType);
            NativeEventHandlers.Remove(eventType);
        }

        public void SubscribeToEvent(AObject sender, uint eventType, EventDelegate eventDelegate)
        {
            if (sender == null)
            {
                throw new InvalidOperationException("AObject.SubscribeToEvent - trying to subscribe to events from a null object");
            }

            // Move this
            NETCore.RegisterNETEventType(eventType);
            NativeCore.SubscribeToEvent(this, sender, eventType);
        }

        public void SubscribeToEvent(AObject sender, string eventType, EventDelegate eventDelegate)
        {
            SubscribeToEvent(sender, AtomicNET.StringToStringHash(eventType), eventDelegate);
        }

        // Native events


        public void UnsubscribeFromEvent<T>() where T : NativeEventData
        {
            uint eventType = NativeEvents.GetEventID<T>();
            UnsubscribeFromEvent(eventType);
        }


        public void SubscribeToEvent<T>(NativeEventDelegate<T> eventDelegate) where T : NativeEventData
        {
            SubscribeToEvent<T>(null, eventDelegate);
        }

        public void SubscribeToEvent<T>(AObject sender, NativeEventDelegate<T> eventDelegate) where T : NativeEventData
        {
            uint eventType = NativeEvents.GetEventID<T>();

            if (eventType == 0)
            {
                throw new InvalidOperationException("AObject.SubscribeToEvent<T>(EventDelegate<T> eventDelegate) - Unknown native event id");
            }

            // Move this
            NETCore.RegisterNETEventType(eventType);

            NativeEventHandlers[eventType] = (eventData) =>
            {
                eventDelegate((T)eventData);
            };

            if (sender != null)
            {
                NativeCore.SubscribeToEvent(this, sender, eventType);
            }
            else
            {
                NativeCore.SubscribeToEvent(this, eventType);
            }
           
        }

        public void SendEvent(string eventType, ScriptVariantMap eventData = null)
        {

            csi_Atomic_AObject_SendEvent(this.nativeInstance, eventType, eventData == null ? IntPtr.Zero : eventData.nativeInstance);

        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void csi_Atomic_AObject_SendEvent(IntPtr self, string eventType, IntPtr variantMap);

    }

}
