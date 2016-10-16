//
// Copyright (c) 2015 Xamarin Inc
// Copyright (c) 2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

using System;
using System.Runtime.InteropServices;

namespace AtomicEngine {

    [StructLayout(LayoutKind.Sequential)]
    public struct StringHash {
        public uint Code;
        public StringHash (uint code)
        {
            this.Code = code;
        }

        public StringHash (string str)
        {
            this.Code = csi_Atomic_AtomicNET_StringToStringHash (str);
        }

        public static implicit operator uint(StringHash hash)
        {
            return hash.Code;
        }

        public static implicit operator StringHash(uint code)
        {
            return new StringHash(code);
        }

        public static implicit operator StringHash(string str)
        {
            return new StringHash(str);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        extern static uint csi_Atomic_AtomicNET_StringToStringHash (string str);

        public override string ToString ()
        {
            return $"StringHash({Code:x})";
        }

        public static StringHash Zero = new StringHash(0);
    }



}
