using System;
using System.IO;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
    public partial class File : AObject, AbstractFile
    {
        private class Stream : System.IO.Stream
        {
            private File file;

            public override bool CanRead
            {
                get
                {
                    return file.Mode == FileMode.FILE_READ || file.Mode == FileMode.FILE_READWRITE;
                }
            }

            public override bool CanSeek
            {
                get
                {
                    return true;
                }
            }

            public override bool CanWrite
            {
                get
                {
                    return file.Mode == FileMode.FILE_WRITE ||
                        file.Mode == FileMode.FILE_READWRITE ||
                        file.Mode == FileMode.FILE_APPEND;
                }
            }

            public override long Length
            {
                get
                {
                    return file.GetSize();
                }
            }

            public override long Position
            {
                get
                {
                    return file.GetPosition();
                }

                set
                {
                    file.Seek((uint)value);
                }
            }

            public Stream(File file)
            {
                if (file == null)
                    throw new ArgumentNullException(nameof(file));

                this.file = file;
            }

            public override void Flush()
            {
                file.Flush();
            }

            public override int Read(byte[] buffer, int offset, int count)
            {
                if (buffer == null)
                    throw new ArgumentNullException(nameof(buffer));
                if (offset + count > buffer.Length)
                    throw new IndexOutOfRangeException("buffer.Length");

                byte[] readData = file.Read(count);
                int limit = Math.Min(count, readData.Length);
                Array.Copy(readData, 0, buffer, offset, limit);

                return limit;
            }

            public override long Seek(long offset, SeekOrigin origin)
            {
                if (origin == SeekOrigin.Current)
                {
                    offset += Position;
                }
                else if (origin == SeekOrigin.End)
                {
                    offset += Length;
                }

                return file.Seek((uint)offset);
            }

            public override void SetLength(long value)
            {
                throw new InvalidOperationException();
            }

            public override void Write(byte[] buffer, int offset, int count)
            {
                if (buffer == null)
                    throw new ArgumentNullException(nameof(buffer));
                if (offset + count >= buffer.Length)
                    throw new IndexOutOfRangeException("buffer.Length");

                file.Write(buffer, offset, count);
            }

            protected override void Dispose(bool disposing)
            {
                if (file != null)
                {
                    file.Close();
                    file = null;
                }

                base.Dispose(disposing);
            }
        }

        public System.IO.Stream ToStream()
        {
            return new Stream(this);
        }

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

