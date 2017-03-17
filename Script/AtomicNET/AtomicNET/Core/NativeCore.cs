using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Linq;
using static System.Reflection.IntrospectionExtensions;

#if ATOMIC_IOS
using ObjCRuntime;
#endif

namespace AtomicEngine
{

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void EventDispatchDelegate(IntPtr sender, uint eventType, IntPtr eventData);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void UpdateDispatchDelegate(float timeStep);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void RefCountedDeletedDelegate(IntPtr refCounted);

    public class NativeType
    {

        public Type Type => type;

        public NativeType(IntPtr nativeClassID, Type type, Func<IntPtr, RefCounted> managedConstructor)
        {
            this.nativeClassID = nativeClassID;
            this.type = type;
            this.managedConstructor = managedConstructor;

            NativeCore.RegisterNativeType(this);
        }

        internal Type type;
        internal IntPtr nativeClassID;
        internal Func<IntPtr, RefCounted> managedConstructor;

    }

    public static class NativeCore
    {

        static internal void SubscribeToEvent(AObject receiver, uint eventType)
        {
            SubscribeToEvent(receiver, null, eventType);
        }

        static internal void SubscribeToEvent(AObject receiver, AObject sender, uint eventType)
        {
            List<EventSubscription> eventReceivers;

            if (!eventReceiverLookup.TryGetValue(eventType, out eventReceivers))
            {
                eventReceivers = eventReceiverLookup[eventType] = new List<EventSubscription>();
            }

            AObject obj;
            foreach (EventSubscription er in eventReceivers)
            {
                if (!er.Receiver.TryGetTarget(out obj))
                    continue; // GC'd

                // already on list?
                if (obj == receiver)
                    return;
            }

            WeakReference<RefCounted> w = null;
            if (!nativeLookup.TryGetValue(receiver.nativeInstance, out w))
            {
                throw new InvalidOperationException("NativeCore.SubscribeToEvent - unable to find native receiver instance");
            }

            eventReceivers.Add(new EventSubscription(receiver, sender));
        }

        static internal void UnsubscribeFromEvent(AObject receiver, uint eventType)
        {
            List<EventSubscription> eventReceivers;
            if (!eventReceiverLookup.TryGetValue(eventType, out eventReceivers))
                return;

            AObject obj;
            foreach (EventSubscription er in eventReceivers)
            {
                if (!er.Receiver.TryGetTarget(out obj))
                    continue; // GC'd

                if (obj == receiver)
                {
                    eventReceivers.Remove(er);
                    return;
                }
            }
        }

        static internal void UnsubscribeFromAllEvents(AObject receiver)
        {
            // TODO: Optimize

            AObject obj;

            foreach (var subList in eventReceiverLookup.Values)
            {
                subList.RemoveAll(item => item.Receiver.TryGetTarget(out obj) && obj == receiver);
            }

        }

        static internal void RemoveEventSender(IntPtr sender)
        {
            //TODO: OPTIMIZE

            if (sender == IntPtr.Zero)
                return;

            foreach (var subList in eventReceiverLookup.Values)
            {
                var len = subList.Count;

                subList.RemoveAll(item => item.Sender == sender);

                //TODO: The events are still in the receiver lookup tables!
            }
        }

        static ScriptVariantMap[] svm;
        static int svmDepth = 0;
        const int svmMax = 256;

        internal static void Initialize()
        {
            // preallocate script variant maps
            svm = new ScriptVariantMap[svmMax];
            for (int i = 0; i < svmMax; i++)
                svm[i] = new ScriptVariantMap();
        }


        static float expireDelta = 0.0f;

        // called ahead of E_UPDATE event
        #if ATOMIC_IOS
        [MonoPInvokeCallback(typeof(UpdateDispatchDelegate))]
        #endif
        public static void UpdateDispatch(float timeStep)
        {
            expireDelta += timeStep;
            if (expireDelta > 2.0f)
            {
                expireDelta = 0.0f;


                // TODO: tune GC
                /*
                GC.Collect();
                GC.WaitForPendingFinalizers();
                GC.Collect();
                */

                ExpireNatives();
            }
        }

        #if ATOMIC_IOS
        [MonoPInvokeCallback(typeof(EventDispatchDelegate))]
        #endif
        public static void EventDispatch(IntPtr sender, uint eventType, IntPtr eventData)
        {
            List<EventSubscription> eventReceivers;

            if (!eventReceiverLookup.TryGetValue(eventType, out eventReceivers))
            {
                // This should not happen, as event NET objects are subscribed to are filtered
                throw new InvalidOperationException("NativeCore.EventDispatch - received unregistered event type");

            }

            // iterate over copy of list so we can modify it while running
            ScriptVariantMap scriptMap = null;
            NativeEventData nativeEventData = null;

            AObject receiver;
            foreach (EventSubscription er in eventReceivers.ToList())
            {
                // GC'd?
                if (!er.Receiver.TryGetTarget(out receiver))
                    continue;

                if (er.Sender != IntPtr.Zero && er.Sender != sender)
                    continue;

                if (scriptMap == null)
                {
                    if (svmDepth == svmMax)
                    {
                        throw new InvalidOperationException("NativeCore.EventDispatch - exceeded max svm");
                    }

                    scriptMap = svm[svmDepth++];
                    scriptMap.CopyVariantMap(eventData);
                    nativeEventData = NativeEvents.GetNativeEventData(eventType, scriptMap);

                    // This check can be removed once ATOMIC-1381 is resolved
                    // https://github.com/AtomicGameEngine/AtomicGameEngine/issues/1381
                    if (nativeEventData != null)
                        nativeEventData.sourceEventData = eventData;
                }

                receiver.HandleEvent(eventType, scriptMap, nativeEventData);
            }

            if (scriptMap != null)
            {
                svmDepth--;

                if (nativeEventData != null)
                {
                    NativeEvents.ReleaseNativeEventData(nativeEventData);
                }
            }

        }

        /// <summary>
        ///  Runs a GC collection, waits for any finalizers, and then expires any natives collected 
        /// </summary>
        public static void RunGC()
        {
            GC.Collect();
            GC.WaitForPendingFinalizers();
            ExpireNatives();
        }

        private static void ExpireNatives()
        {
            var watch = new Stopwatch();
            watch.Start();

            // expire event listeners

            //int eventListenersRemoved = 0;
            //int nativesRemoved = 0;

            AObject obj;

            foreach (List<EventSubscription> receiverList in eventReceiverLookup.Values)
            {
                foreach (EventSubscription er in receiverList.ToList())
                {
                    if (!er.Receiver.TryGetTarget(out obj))
                    {
                        receiverList.Remove(er);
                        //eventListenersRemoved++;
                    }

                    if (watch.ElapsedMilliseconds > 16)
                        break;

                }

                if (watch.ElapsedMilliseconds > 16)
                    break;
            }

            RefCounted r;

            foreach (var native in nativeLookup.Keys.ToList())
            {
                var w = nativeLookup[native];

                if (!w.TryGetTarget(out r))
                {
                    // expired
                    csi_AtomicEngine_ReleaseRef(native);
                    nativeLookup.Remove(native);
                    //nativesRemoved++;
                }

                if (watch.ElapsedMilliseconds > 16)
                    break;
            }

            /*
            if (nativesRemoved != 0 || eventListenersRemoved != 0)
            {
                Console.WriteLine("Released {0} natives and {1} event receivers", nativesRemoved, eventListenersRemoved);
            }
            */

        }

        // Called from RefCounted native destructor, refCounted is not valid for any operations here
        #if ATOMIC_IOS
        [MonoPInvokeCallback(typeof(RefCountedDeletedDelegate))]
        #endif
        public static void RefCountedDeleted(IntPtr refCounted)
        {
            nativeLookup.Remove(refCounted);
            RemoveEventSender(refCounted);
        }

        // register a newly created native
        public static IntPtr RegisterNative(IntPtr native, RefCounted r)
        {
            if (nativeLookup.ContainsKey(native))
            {
                throw new InvalidOperationException("NativeCore.RegisterNative - Duplicate IntPtr key");
            }

            r.nativeInstance = native;
            // keep native side alive
            r.AddRef();

            nativeLookup[native] = new WeakReference<RefCounted>(r);

            r.InstantiationType = InstantiationType.INSTANTIATION_NET;

            r.PostNativeUpdate();

            return native;
        }


        // wraps an existing native instance, with downcast support
        public static T WrapNative<T>(IntPtr native) where T : RefCounted
        {
            if (native == IntPtr.Zero)
                return null;

            RefCounted r;
            WeakReference<RefCounted> w;

            // first see if we're already available
            if (nativeLookup.TryGetValue(native, out w))
            {

                if (w.TryGetTarget(out r))
                {
                    // we're alive!
                    return (T)r;
                }
                else
                {
                    // we were seen before, but have since been GC'd, remove!
                    nativeLookup.Remove(native);

                    if (csi_Atomic_RefCounted_Refs(native) == 1)
                    {
                        // only managed ref remains, so release and return null
                        csi_AtomicEngine_ReleaseRef(native);
                        return null;
                    }

                    csi_AtomicEngine_ReleaseRef(native);
                }
            }

            IntPtr classID = RefCounted.csi_Atomic_RefCounted_GetClassID(native);

            // Check whether this is a valid native class to wrap
            NativeType nativeType;
            if (!nativeClassIDToNativeType.TryGetValue(classID, out nativeType))
            {
                if (logWrapUnknownNative)
                {
                    Log.Info("WrapNative returning null for unknown class: " + GetNativeTypeName(native));
                }

                return null;
            }

            // TODO: make CSComponent abstract and have general abstract logic here?
            if (nativeType.Type == typeof(CSComponent))
                return null;

            // and store, with downcast support for instance Component -> StaticModel
            // we never want to hit this path for script inherited natives

            r = nativeType.managedConstructor(native);

            w = new WeakReference<RefCounted>(r);
            NativeCore.nativeLookup[native] = w;

            // store a ref, so native side will not be released while we still have a reference in managed code
            r.AddRef();

            // Note: r.InstantiationType may be INSTANTIATION_NET here is we were GC'd, native still had a reference, and we came back
            if (r.InstantiationType == InstantiationType.INSTANTIATION_NET)
            {
                //Log.Warn($"Wrapped {r.GetType().Name} was originally instantiated in NET, changing to native, this is likely an error");
                //r.InstantiationType = InstantiationType.INSTANTIATION_NATIVE;
            }

            r.PostNativeUpdate();

            return (T)r;
        }

        internal static string GetNativeTypeName(IntPtr native)
        {
            return System.Runtime.InteropServices.Marshal.PtrToStringAnsi(csi_Atomic_RefCounted_GetTypeName(native));
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr csi_Atomic_RefCounted_GetTypeName(IntPtr self);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern int csi_Atomic_RefCounted_Refs(IntPtr self);

        public static void RegisterNativeType(NativeType nativeType)
        {
            if (nativeClassIDToNativeType.ContainsKey(nativeType.nativeClassID))
            {
                throw new InvalidOperationException("NativeCore.RegisterNativeType - Duplicate NativeType class id registered");
            }
            if (typeToNativeType.ContainsKey(nativeType.type))
            {
                throw new InvalidOperationException("NativeCore.RegisterNativeType - Duplicate NativeType type registered");
            }

            nativeClassIDToNativeType[nativeType.nativeClassID] = nativeType;
            typeToNativeType[nativeType.type] = nativeType;

        }

        public static bool IsNativeType(Type type)
        {
            if (typeToNativeType.ContainsKey(type))
                return true;

            return false;
        }

        public static Type GetNativeAncestorType(Type type)
        {
            Type ancestorType = type;
            do
            {
                ancestorType = ancestorType.GetTypeInfo().BaseType;
            } while (ancestorType != null && !IsNativeType(ancestorType));

            return ancestorType;
        }

        // weak references here, hold a ref native side
        internal static Dictionary<IntPtr, WeakReference<RefCounted>> nativeLookup = new Dictionary<IntPtr, WeakReference<RefCounted>>();

        // weak references here, hold a ref native side
        internal static Dictionary<uint, List<EventSubscription>> eventReceiverLookup = new Dictionary<uint, List<EventSubscription>>();


        // Native ClassID to NativeType lookup
        internal static Dictionary<IntPtr, NativeType> nativeClassIDToNativeType = new Dictionary<IntPtr, NativeType>();


        // Managed Type to NativeType lookup
        internal static Dictionary<Type, NativeType> typeToNativeType = new Dictionary<Type, NativeType>();

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void csi_AtomicEngine_ReleaseRef(IntPtr refCounted);

        internal struct EventSubscription
        {
            public WeakReference<AObject> Receiver;
            public IntPtr Sender;

            public EventSubscription(AObject receiver)
            {
                Receiver = new WeakReference<AObject>(receiver);
                Sender = IntPtr.Zero;
            }

            public EventSubscription(AObject receiver, IntPtr sender)
            {
                Receiver = new WeakReference<AObject>(receiver);
                Sender = sender;
            }
        }

        // If true, we will log any attempted access to instances of unknown native classes
        static bool logWrapUnknownNative = false;

    }


}
