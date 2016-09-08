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

        private Dictionary<SenderEventKey, SenderEventDelegate> SenderEventHandlers
        {
            get
            {
                if (senderEventHandlers == null)
                {
                    SenderEventKeyComparer comparer = SenderEventKeyComparer.Default;
                    senderEventHandlers = new Dictionary<SenderEventKey, SenderEventDelegate>(comparer);
                }
                return senderEventHandlers;
            }
        }
        private Dictionary<SenderEventKey, SenderEventDelegate> senderEventHandlers;

        public static T GetSubsystem<T>() where T : AObject
        {
            return AtomicNET.GetSubsystem<T>();
        }

        internal void HandleEvent(uint eventType, ScriptVariantMap eventData)
        {
            eventHandlers[eventType](eventType, eventData);
        }

        internal void HandleEvent(AObject sender, uint eventType, ScriptVariantMap eventData)
        {
            var key = new SenderEventKey(eventType, sender.nativeInstance);
            senderEventHandlers[key](sender, eventType, eventData);
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
        }

        public void SubscribeToEvent(AObject sender, uint eventType, SenderEventDelegate eventDelegate)
        {
            if (sender == null)
            {
                throw new InvalidOperationException("AObject.SubscribeToEvent - trying to subscribe to events from a null object");
            }

            NETCore.RegisterNETEventType(eventType);
            var key = new SenderEventKey(eventType, sender.nativeInstance);
            SenderEventHandlers[key] = eventDelegate;
            NativeCore.SubscribeToEvent(this, sender, eventType);
        }

        public void SubscribeToEvent(AObject sender, string eventType, SenderEventDelegate eventDelegate)
        {
            SubscribeToEvent(sender, AtomicNET.StringToStringHash(eventType), eventDelegate);
        }

        public void UnsubscribeFromEvent(AObject sender, uint eventType)
        {
            NativeCore.UnsubscribeFromEvent(this, sender, eventType);
            var key = new SenderEventKey(eventType, sender.nativeInstance);
            SenderEventHandlers.Remove(key);
        }

        public void SendEvent(string eventType, ScriptVariantMap eventData = null)
        {

            csi_Atomic_AObject_SendEvent(this.nativeInstance, eventType, eventData == null ? IntPtr.Zero : eventData.nativeInstance);

        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void csi_Atomic_AObject_SendEvent(IntPtr self, string eventType, IntPtr variantMap);

        private struct SenderEventKey
        {
            public readonly uint EventType;
            public readonly IntPtr Sender; 

            public SenderEventKey(uint eventType, IntPtr sender)
            {
                EventType = eventType;
                Sender = sender;
            }
        }

        // Use a comparer to avoid boxing on struct .Equals override
        private class SenderEventKeyComparer : IEqualityComparer<SenderEventKey>
        {
            public static readonly SenderEventKeyComparer Default = new SenderEventKeyComparer();

            private SenderEventKeyComparer() { }

            public bool Equals(SenderEventKey lhs, SenderEventKey rhs)
            {
                return (lhs.EventType == rhs.EventType &&
                    lhs.Sender == rhs.Sender);
            }

            public int GetHashCode(SenderEventKey key)
            {
                // Based on http://stackoverflow.com/a/263416/156328
                unchecked
                {
                    int hash = 17;
                    hash = hash * 23 + key.EventType.GetHashCode();
                    hash = hash * 23 + key.Sender.GetHashCode();
                    return hash;
                }
            }
        }
    }

}
