using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
    public delegate void EventDelegate(uint eventType, ScriptVariantMap eventData);

    public delegate void NativeEventDelegate(NativeEventData eventData);
    public delegate void NativeEventDelegate<T>(T eventData);

    public partial class ScriptVariantMap
    {
        public void CopyVariantMap(IntPtr vm)
        {
            csi_Atomic_AtomicNET_ScriptVariantMapCopyVariantMap(nativeInstance, vm);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr csi_Atomic_AtomicNET_ScriptVariantMapCopyVariantMap(IntPtr svm, IntPtr vm);

    }

    public class NativeEventData 
    {
        public ScriptVariantMap scriptMap;

        public void Set(string name, bool value)
        {
            VariantMap.SetBool(sourceEventData, name, value);
        }

        // Variant Map
        internal IntPtr sourceEventData;
    }

    public static partial class NativeEvents
    {
        class NativeEventDataCache
        {
            public NativeEventDataCache(Type type)
            {
                eventData = new NativeEventData[eventDataMax];

                for (int i = 0; i < eventDataMax; i++)
                    eventData[i] = (NativeEventData)Activator.CreateInstance(type);
            }

            public NativeEventData[] eventData;
            public int eventDataDepth = 0;
            public const int eventDataMax = 256;
        }

        public static uint GetEventID<T>() where T : NativeEventData
        {
            uint eventID;

            if (eventIDLookup.TryGetValue(typeof(T), out eventID))
            {
                return eventID;
            }

            return 0;
        }

        public static void RegisterEventID<T>(string eventName) where T : NativeEventData
        {
            var eventID = AtomicNET.StringToStringHash(eventName);
            var type = typeof(T);

            eventIDLookup[type] = eventID;
            typeLookup[eventID] = type;

            // create event data cache
            var cache = new NativeEventDataCache(type);
            eventCache[type] = cache;
            eventCacheByEventID[eventID] = cache;

        }

        public static NativeEventData GetNativeEventData(uint eventID, ScriptVariantMap eventMap)
        {
            NativeEventDataCache cache;

            if (eventCacheByEventID.TryGetValue(eventID, out cache))
            {
                if (cache.eventDataDepth == NativeEventDataCache.eventDataMax)
                {
                    throw new InvalidOperationException("GetNativeEventData - max recursive event");
                }

                var eventData = cache.eventData[cache.eventDataDepth++];
                eventData.scriptMap = eventMap;
                return eventData;
            }

            return null;
        }

        public static void ReleaseNativeEventData(NativeEventData eventData)
        {
            Type dataType = eventData.GetType();
            eventData.scriptMap = null;
            eventData.sourceEventData = IntPtr.Zero;

            NativeEventDataCache cache;

            if (eventCache.TryGetValue(dataType, out cache))
            {
#if DEBUG
                if (cache.eventData[cache.eventDataDepth - 1] != eventData)
                {
                    throw new InvalidOperationException("ReleaseNativeEventData - unexpected event data");
                }    
#endif                                
                cache.eventDataDepth--;
            }

        }

        // event cache
        static Dictionary<Type, NativeEventDataCache> eventCache = new Dictionary<Type, NativeEventDataCache>();
        static Dictionary<uint, NativeEventDataCache> eventCacheByEventID = new Dictionary<uint, NativeEventDataCache>();

        // native event type -> native eventType hash id
        static Dictionary<Type, uint> eventIDLookup = new Dictionary<Type, uint>();

        // native eventType hash id -> NativeEvent type
        static Dictionary<uint, Type> typeLookup = new Dictionary<uint, Type>();
    }

}