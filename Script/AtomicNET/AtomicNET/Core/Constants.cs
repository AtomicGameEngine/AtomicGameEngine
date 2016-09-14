namespace AtomicEngine
{

    public static partial class Constants
    {

#if ATOMIC_IOS
        public const string LIBNAME = "@rpath/AtomicNETNative.framework/AtomicNETNative";
#elif __MonoCS__
        public const string LIBNAME = "AtomicNETNative";
#else
        public const string LIBNAME = "AtomicNETNative.dll";
#endif

    }

}
