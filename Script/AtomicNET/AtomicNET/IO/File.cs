
using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
    public partial class File : AObject, Deserializer, Serializer
    {


        /// <summary>
        /// Read bytes from the file. Return array of bytes of the length actually read (can be 0 length)
        /// </summary>
        unsafe public byte[] Read(int size)
        {
            byte[] bytes = null;
            byte[] buffer = new byte[size];
            int sizeRead = 0;

            fixed (byte* p = buffer)
            {
                IntPtr ptr = (IntPtr)p;

                sizeRead = (int)csi_Atomic_File_Read(nativeInstance, ptr, (uint)size);

                bytes = new byte[sizeRead];

                if (sizeRead > 0)
                {
                    Marshal.Copy(ptr, bytes, 0, (int)sizeRead);
                }
            }

            return bytes;

        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        internal static extern uint csi_Atomic_File_Read(IntPtr self, IntPtr dest, uint size);

        /// <summary>
        /// Write bytes to the file, with optional offset into array and count of bytes to write. 
        /// Return number of bytes actually written.
        /// </summary>
        unsafe public int Write(byte[] bytes, int offset = 0, int count = 0)
        {
            if (bytes == null || bytes.Length == 0)
                return 0;

            if (count == 0)
                count = bytes.Length;

            if (offset >= bytes.Length)
                return 0;

            // should this truncate?
            if ((offset + count) > bytes.Length)
                return 0;

            int bytesWritten = 0;

            fixed (byte* p = bytes)
            {
                IntPtr ptr = (IntPtr)p;
                bytesWritten = (int)csi_Atomic_File_Write(nativeInstance, ptr, (uint) offset, (uint)count);
            }

            return bytesWritten;

        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        internal static extern uint csi_Atomic_File_Write(IntPtr self, IntPtr data, uint offset, uint size);


    };

}

