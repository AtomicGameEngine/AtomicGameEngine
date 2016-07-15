using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Linq;

namespace AtomicEngine
{

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
            List<WeakReference<AObject>> eventReceivers;

            if (!eventReceiverLookup.TryGetValue(eventType, out eventReceivers))
            {
                eventReceivers = eventReceiverLookup[eventType] = new List<WeakReference<AObject>>();
            }

            AObject obj;

            foreach (WeakReference<AObject> wr in eventReceivers)
            {

                if (!wr.TryGetTarget(out obj))
                    continue; // GC'd

                // already on list?
                if (obj == receiver)
                    return;
            }

            WeakReference<RefCounted> w = null;

            if (!nativeLookup.TryGetValue(receiver.nativeInstance, out w))
            {
                throw new System.InvalidOperationException("NativeCore.SubscribeToEvent - unable to find native instance");
            }

            RefCounted refcounted;

            if (!w.TryGetTarget(out refcounted))
            {
                throw new System.InvalidOperationException("NativeCore.SubscribeToEvent - attempting to subscribe a GC'd AObject");
            }

            eventReceivers.Add(new WeakReference<AObject>(receiver));
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

        public static void EventDispatch(uint eventType, IntPtr eventData)
        {
            List<WeakReference<AObject>> eventReceivers;

            if (!eventReceiverLookup.TryGetValue(eventType, out eventReceivers))
            {
                // This should not happen, as event NET objects are subscribed to are filtered 
                throw new System.InvalidOperationException("NativeCore.EventDispatch - received unregistered event type");

            }

            ScriptVariantMap scriptMap = null;
            AObject receiver;

            // iterate over copy of list so we can modify it while running
            foreach (var w in eventReceivers.ToList())
            {
                // GC'd?
                if (!w.TryGetTarget(out receiver))
                    continue;

                if (scriptMap == null)
                {
                    if (svmDepth == svmMax)
                    {
                        throw new System.InvalidOperationException("NativeCore.EventDispatch - exceeded max svm");
                    }

                    scriptMap = svm[svmDepth++];
                    scriptMap.CopyVariantMap(eventData);
                }

                receiver.HandleEvent(eventType, scriptMap);
            }

            if (scriptMap != null)
                svmDepth--;

        }

        static void ExpireNatives()
        {
            var watch = new Stopwatch();
            watch.Start();

            // expire event listeners

            int eventListenersRemoved = 0;
            int nativesRemoved = 0;

            AObject obj;

            foreach (List<WeakReference<AObject>> receiverList in eventReceiverLookup.Values)
            {
                foreach (WeakReference<AObject> receiver in receiverList.ToList())
                {
                    if (!receiver.TryGetTarget(out obj))
                    {
                        receiverList.Remove(receiver);
                        eventListenersRemoved++;
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
                    csb_AtomicEngine_ReleaseRef(native);
                    nativeLookup.Remove(native);
                    nativesRemoved++;
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

        static float expireDelta = 0.0f;

        // called ahead of E_UPDATE event
        public static void UpdateDispatch(float timeStep)
        {
            expireDelta += timeStep;
            if (expireDelta > 2.0f)
            {
                expireDelta = 0.0f;
                ExpireNatives();
            }
        }

        // register a newly created native
        public static IntPtr RegisterNative(IntPtr native, RefCounted r)
        {
            if (nativeLookup.ContainsKey(native))
            {
                throw new System.InvalidOperationException("NativeCore.RegisterNative - Duplicate IntPtr key");
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

                    if (csb_Atomic_RefCounted_Refs(native) == 1)
                    {
                        // only managed ref remains, so release and return null
                        csb_AtomicEngine_ReleaseRef(native);
                        return null;
                    }

                    csb_AtomicEngine_ReleaseRef(native);
                }
            }

            IntPtr classID = RefCounted.csb_Atomic_RefCounted_GetClassID(native);

            // and store, with downcast support for instance Component -> StaticModel
            // we never want to hit this path for script inherited natives

            NativeType nativeType;

            if (!nativeClassIDToNativeType.TryGetValue(classID, out nativeType))
            {
                throw new System.InvalidOperationException("NativeCore.WrapNative - Attempting to wrap unknown native class id");
            }

            r = nativeType.managedConstructor(native);

            w = new WeakReference<RefCounted>(r);
            NativeCore.nativeLookup[native] = w;

            // store a ref, so native side will not be released while we still have a reference in managed code
            r.AddRef();

            return (T)r;
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr csb_Atomic_AObject_GetTypeName(IntPtr self);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern int csb_Atomic_RefCounted_Refs(IntPtr self);

        public static void RegisterNativeType(NativeType nativeType)
        {
            if (nativeClassIDToNativeType.ContainsKey(nativeType.nativeClassID))
            {
                throw new System.InvalidOperationException("NativeCore.RegisterNativeType - Duplicate NativeType class id registered");
            }
            if (typeToNativeType.ContainsKey(nativeType.type))
            {
                throw new System.InvalidOperationException("NativeCore.RegisterNativeType - Duplicate NativeType type registered");
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
                    throw new System.InvalidOperationException("NativeCore.NativeContructorOverride - Previous nativeContructorOverride not consumed");
                }

                nativeContructorOverride = value;
            }
        }

        static public void VerifyNativeContructorOverrideConsumed()
        {
            if (nativeContructorOverride != IntPtr.Zero)
            {
                throw new System.InvalidOperationException("NativeCore.VerifyNativeContructorOverrideConsumed -  NativeContructorOverride not consumed");
            }
        }


        private static IntPtr nativeContructorOverride = IntPtr.Zero;

        // weak references here, hold a ref native side
        internal static Dictionary<IntPtr, WeakReference<RefCounted>> nativeLookup = new Dictionary<IntPtr, WeakReference<RefCounted>>();

        // weak references here, hold a ref native side
        internal static Dictionary<uint, List<WeakReference<AObject>>> eventReceiverLookup = new Dictionary<uint, List<WeakReference<AObject>>>();


        // Native ClassID to NativeType lookup
        internal static Dictionary<IntPtr, NativeType> nativeClassIDToNativeType = new Dictionary<IntPtr, NativeType>();


        // Managed Type to NativeType lookup
        internal static Dictionary<Type, NativeType> typeToNativeType = new Dictionary<Type, NativeType>();

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void csb_AtomicEngine_ReleaseRef(IntPtr refCounted);

    }


}
