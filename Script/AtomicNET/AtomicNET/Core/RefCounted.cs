using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace AtomicEngine
{

    /// <summary>
    ///  Class which ensures Disposed is called, without needing to implement Dispose in class Finalizer
    /// </summary>
    internal class RefCountedSafeFileHandle : SafeHandle
    {
        public RefCountedSafeFileHandle(IntPtr handle, bool ownsHandle = true)
            : base(handle, ownsHandle)
        {
            if (handle == IntPtr.Zero)
            {
                throw new InvalidOperationException("RefCountedSafeFileHandle - native == IntPtr.Zero");
            }

            NativeCore.csi_AtomicEngine_AddRef(handle);
        }

        override public bool IsInvalid { get { return handle == IntPtr.Zero; } }

        /// <summary>
        ///  Release the handle, which will release the native instance immediately if in main thread
        ///  otherwise, will queue
        /// </summary>
        override protected bool ReleaseHandle()
        {
            if (handle == IntPtr.Zero)
            {
                throw new InvalidOperationException("RefCountedSafeFileHandle.ReleaseHandle - native == IntPtr.Zero");
            }

            // We can be called from Dispose in main thread or from finalizers, which aren't in the main thread
            if (NativeCore.csi_AtomicEngine_IsMainThread())
            {
                NativeCore.csi_AtomicEngine_ReleaseRef(handle);
            }
            else
            {
                // We're in a finalizer, need to add to queue to release when
                // back in main thread
                lock (RefCounted.refCountedFinalizerQueue)
                {
                    RefCounted.refCountedFinalizerQueue.Add(handle);
                }

            }

            handle = IntPtr.Zero;

            return true;
        }
    }

    [ComVisible(true)]
    public partial class RefCounted : IDisposable
    {
        /// <summary>
        ///  If instance has been disposed, native object is in an undefined state, and instance should not be accessed
        /// </summary>
        public bool Disposed => disposed;

        // _handle is set to null to indicate disposal of this instance.
        private RefCountedSafeFileHandle refHandle;

        public RefCounted()
        {

        }

        /// <summary>
        ///  WARNING: C# finalizers can be called in any thread!!!
        ///  Don't need native cleanup code in the finalizer as use SafeHandle
        /// </summary>
        ~RefCounted()
        {
        }

        protected RefCounted(IntPtr native)
        {
            nativeInstance = native;
        }

        internal void Init()
        {   
            if (refHandle != null)
                throw new InvalidOperationException("RefCounted.Init - refHandle already initialized");

            refHandle = new RefCountedSafeFileHandle(nativeInstance);
        }

        /// <summary>
        ///  Dispose method of IDisposible interface, note that native code can hold references to 
        ///  RefCounted derived instances, disposing RefCounted instances from managed code releases
        ///  the managed reference and will only release the native RefCounted instance if no other references 
        ///  are held in native code. 
        /// </summary>
        public void Dispose()
        {
            Dispose(true);
        }

        protected virtual void Dispose(bool disposing)
        {
            disposed = true;

            if (refHandle != null && !refHandle.IsInvalid)
            {
                NativeCore.RemoveNative(nativeInstance);

                // Free the handle
                refHandle.Dispose();                
            }

            nativeInstance = IntPtr.Zero;

        }

        static internal List<IntPtr> refCountedFinalizerQueue = new List<IntPtr>();

        /// <summary>
        /// Releases RefCounted instances which were finalized (which can happen on any thread)
        /// </summary>
        static internal void ReleaseFinalized()
        {
            lock (refCountedFinalizerQueue)
            {
                foreach (var native in refCountedFinalizerQueue)
                {
                    NativeCore.RemoveNative(native);
                    NativeCore.csi_AtomicEngine_ReleaseRef(native);
                }

                refCountedFinalizerQueue.Clear();
            }

        }

        /// <summary>
        /// This method may be called multiple times, called on instance after it is either registered as a new native created in C# (InstantiationType == InstantiationType.INSTANTIATION_NET)
        /// or a native which has been wrapped ((InstantiationType != InstantiationType.INSTANTIATION_NET)
        /// Note that RefCounted that get GC'd from script, can still live in native code, and get rewrapped 
        /// </summary>
        internal virtual void PostNativeUpdate()
        {

        }

        public static implicit operator IntPtr(RefCounted refCounted)
        {
            if (refCounted == null)
                return IntPtr.Zero;
                
            return refCounted.nativeInstance;
        }

        public IntPtr NativeInstance { get { return nativeInstance; } }

        public IntPtr nativeInstance = IntPtr.Zero;

        private bool disposed = false;

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr csi_Atomic_RefCounted_GetClassID(IntPtr self);

    }


}
