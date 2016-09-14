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

            RefCounted refcounted;
            if (!w.TryGetTarget(out refcounted))
            {
                throw new InvalidOperationException("NativeCore.SubscribeToEvent - attempting to subscribe a GC'd AObject");
            }

            IntPtr nativeSender = IntPtr.Zero;
            if (sender != null)
            {
                nativeSender = sender.nativeInstance;
                if (!nativeLookup.TryGetValue(sender.nativeInstance, out w))
                {
                    throw new InvalidOperationException("NativeCore.SubscribeToEvent - unable to find native sender instance");
                }

                if (!w.TryGetTarget(out refcounted))
                {
                    throw new InvalidOperationException("NativeCore.SubscribeToEvent - attempting to subscribe to a GC'd AObject");
                }
            }

            eventReceivers.Add(new EventSubscription(receiver, nativeSender));
        }

        static internal void UnsubscribeFromEvent(AObject receiver, uint eventType)
        {
            UnsubscribeFromEvent(receiver, null, eventType);
        }

        static internal void UnsubscribeFromEvent(AObject receiver, RefCounted sender, uint eventType)
        {
            List<EventSubscription> eventReceivers;
            if (!eventReceiverLookup.TryGetValue(eventType, out eventReceivers))
                return;

            AObject obj;
            foreach (EventSubscription er in eventReceivers)
            {
                if (!er.Receiver.TryGetTarget(out obj))
                    continue; // GC'd

                if (obj == receiver &&
                    (sender == null || er.Sender == sender.nativeInstance))
                {
                    eventReceivers.Remove(er);
                    return;
                }
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

            AObject managedSender = null;
            WeakReference<RefCounted> wr;
            if (sender != IntPtr.Zero &&
                nativeLookup.TryGetValue(sender, out wr))
            {
                RefCounted refCounted;
                if (wr.TryGetTarget(out refCounted))
                {
                    managedSender = refCounted as AObject;
                }
            }

            // iterate over copy of list so we can modify it while running
            ScriptVariantMap scriptMap = null;
            AObject receiver;
            foreach (EventSubscription er in eventReceivers.ToList())
            {
                // GC'd?
                if (!er.Receiver.TryGetTarget(out receiver))
                    continue;

                if (scriptMap == null)
                {
                    if (svmDepth == svmMax)
                    {
                        throw new InvalidOperationException("NativeCore.EventDispatch - exceeded max svm");
                    }

                    scriptMap = svm[svmDepth++];
                    scriptMap.CopyVariantMap(eventData);
                }

                if (managedSender != null && er.Sender == sender)
                {
                    receiver.HandleEvent(managedSender, eventType, scriptMap);
                }
                else if (er.Sender == IntPtr.Zero)
                {
                    receiver.HandleEvent(eventType, scriptMap);
                }
            }

            if (scriptMap != null)
                svmDepth--;

        }

        static void ExpireNatives()
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

            // and store, with downcast support for instance Component -> StaticModel
            // we never want to hit this path for script inherited natives

            NativeType nativeType;

            if (!nativeClassIDToNativeType.TryGetValue(classID, out nativeType))
            {
                throw new InvalidOperationException("NativeCore.WrapNative - Attempting to wrap unknown native class id");
            }

            r = nativeType.managedConstructor(native);

            w = new WeakReference<RefCounted>(r);
            NativeCore.nativeLookup[native] = w;

            // store a ref, so native side will not be released while we still have a reference in managed code
            r.AddRef();

            return (T)r;
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr csi_Atomic_AObject_GetTypeName(IntPtr self);

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

        static public IntPtr NativeContructorOverride
        {
            get
            {
                IntPtr value = nativeContructorOverride;
                nativeContructorOverride = IntPtr.Zero;
                return value;
            }

            set
            {
                if (nativeContructorOverride != IntPtr.Zero)
                {
                    throw new InvalidOperationException("NativeCore.NativeContructorOverride - Previous nativeContructorOverride not consumed");
                }

                nativeContructorOverride = value;
            }
        }

        static public void VerifyNativeContructorOverrideConsumed()
        {
            if (nativeContructorOverride != IntPtr.Zero)
            {
                throw new InvalidOperationException("NativeCore.VerifyNativeContructorOverrideConsumed -  NativeContructorOverride not consumed");
            }
        }


        private static IntPtr nativeContructorOverride = IntPtr.Zero;

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

            public EventSubscription(AObject obj, IntPtr source)
            {
                Receiver = new WeakReference<AObject>(obj);
                Sender = source;
            }
        }

    }


}
