using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
    public partial class Profiler : AObject
    {
        public static void Block(string name, Action block, uint color = 0xffffecb3,
                                 ProfilerBlockStatus status = ProfilerBlockStatus.ON,
                                 [CallerFilePath] string file = "",
                                 [CallerLineNumber] int line = 0)
        {
#if ATOMIC_PROFILING
            var profiler = AtomicNET.Context.GetProfiler();
            if (profiler != null)
                csi_Atomic_Profiler_BeginBlock(profiler, name, file, line, color, (byte)status);
#endif
            block();
#if ATOMIC_PROFILING
            profiler?.EndBlock();
#endif
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void csi_Atomic_Profiler_BeginBlock(IntPtr self, string name, string file, int line, uint argb, byte status);
    }
}
