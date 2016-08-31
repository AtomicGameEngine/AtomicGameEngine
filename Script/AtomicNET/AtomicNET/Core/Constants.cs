namespace AtomicEngine
{

    public static partial class Constants
    {

#if __MonoCS__
        public const string LIBNAME = "AtomicNETNative";
#else
        public const string LIBNAME = "AtomicNETNative.dll";
#endif

    }

}
