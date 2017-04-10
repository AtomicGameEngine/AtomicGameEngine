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

    [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    public delegate void ThrowManagedExceptionDelegate(string errorMsg);

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

    /// <summary>
    ///  Internal class for native interop 
    /// </summary>
    internal static class NativeCore
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
            if (sender == IntPtr.Zero)
                return;

            var refCounted = refCountedCache.Get(sender)?.Reference;

            // If we're no longer registered or not an Object, early out
            if (refCounted == null || !refCounted.IsObject())
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

        // called ahead of E_UPDATE event
        #if ATOMIC_IOS
        [MonoPInvokeCallback(typeof(UpdateDispatchDelegate))]
        #endif
        public static void UpdateDispatch(float timeStep)
        {

            RefCounted.ReleaseFinalized();
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
            // run a GC collection
            GC.Collect();
            // finalizers can run on any thread, we're explicitly running a GC here
            // so wait for all the finalizers to finish            
            GC.WaitForPendingFinalizers();
            // Anything finalized on another thread will now be available to release 
            // in main thread
            RefCounted.ReleaseFinalized();

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

        }

        // Called from RefCounted native destructor, refCounted is not valid for any operations here
        #if ATOMIC_IOS
        [MonoPInvokeCallback(typeof(RefCountedDeletedDelegate))]
        #endif
        public static void RefCountedDeleted(IntPtr refCounted)
        {
            
        }

        // Called to throw a managed exception from native code
#if ATOMIC_IOS
        [MonoPInvokeCallback(typeof(RefCountedDeletedDelegate))]
#endif
        public static void ThrowManagedException(string errorMsg)
        {
            throw new InvalidOperationException("Native Exception: " + errorMsg);
        }

        internal static void RemoveNative(IntPtr refCounted)
        {
            if (refCounted == IntPtr.Zero)
                return;
           
            RemoveEventSender(refCounted);

            refCountedCache.Remove(refCounted);

        }

        // register a newly created native
        public static IntPtr RegisterNative(IntPtr native, RefCounted r, InstantiationType instantiationType = InstantiationType.INSTANTIATION_NET)
        {
            if (native == IntPtr.Zero || r == null)
            {
                throw new InvalidOperationException("NativeCore.RegisterNative - native == IntPtr.Zero || RefCounted instance == null");
            }

            if (instantiationType == InstantiationType.INSTANTIATION_NET)
            {
                if (r.nativeInstance != IntPtr.Zero)
                {
                    throw new InvalidOperationException("NativeCore.RegisterNative - NET Instantiated RefCounted with initialized nativeInstance");
                }

                r.nativeInstance = native;
            }

            r.InstantiationType = instantiationType;
            r.InternalInit();

            refCountedCache.Add(r);

            r.PostNativeUpdate();

            return native;

        }


        // wraps an existing native instance, with downcast support
        public static T WrapNative<T>(IntPtr native) where T : RefCounted
        {
            if (native == IntPtr.Zero)
            {
                throw new InvalidOperationException("NativeCore.WrapNative - Attempting to wrap native instance IntPtr.Zero");
            }

            var reference = refCountedCache.Get(native)?.Reference;

            // This isn't really a good test to verify right object, better to test if not a T and error?
            if (reference is T)
                return (T)reference;

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

            // Construct managed instance wrapper for native instance
            // this has downcast support for instance Component -> StaticModel
            // note, we never want to hit this path for script inherited natives

            var r = nativeType.managedConstructor(native);

            // IMPORTANT: if a RefCounted instance is created in managed code, has reference count increased in native code
            // and managed side is GC'd, the original NET created instance will be gone and we can get it back here reported
            // as instantiated in native code.  May want a transative boolean to be able to tell when an object has passed this "barrier"
            // which is somewhat common
            RegisterNative(native, r, InstantiationType.INSTANTIATION_NATIVE);

            return (T)r;

        }

        internal static string GetNativeTypeName(IntPtr native)
        {
            return System.Runtime.InteropServices.Marshal.PtrToStringAnsi(csi_Atomic_RefCounted_GetTypeName(native));
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr csi_Atomic_RefCounted_GetTypeName(IntPtr self);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        internal static extern int csi_Atomic_RefCounted_Refs(IntPtr self);

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

        private static RefCountedCache refCountedCache = new RefCountedCache();

        // weak references here, hold a ref native side
        internal static Dictionary<uint, List<EventSubscription>> eventReceiverLookup = new Dictionary<uint, List<EventSubscription>>();


        // Native ClassID to NativeType lookup
        internal static Dictionary<IntPtr, NativeType> nativeClassIDToNativeType = new Dictionary<IntPtr, NativeType>();


        // Managed Type to NativeType lookup
        internal static Dictionary<Type, NativeType> typeToNativeType = new Dictionary<Type, NativeType>();

        // Access to native reference counting not needing a managed RefCounted instance

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        internal static extern void csi_AtomicEngine_AddRef(IntPtr refCounted);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        internal static extern void csi_AtomicEngine_AddRefSilent(IntPtr refCounted);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        internal static extern void csi_AtomicEngine_ReleaseRef(IntPtr refCounted);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        internal static extern void csi_AtomicEngine_ReleaseSilent(IntPtr refCounted);

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
