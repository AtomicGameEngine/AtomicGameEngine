using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{

    public partial class IPC : AObject
    {
        public void SendEventToBroker(string eventType, ScriptVariantMap eventData)
        {
            csi_Atomic_IPC_SendEventToBrokerWithEventData(nativeInstance, eventType, eventData == null ? IntPtr.Zero : eventData.nativeInstance);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void csi_Atomic_IPC_SendEventToBrokerWithEventData(IntPtr self, string eventType, IntPtr variantMap);

    }

}