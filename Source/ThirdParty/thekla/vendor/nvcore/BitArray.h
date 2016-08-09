// This code is in the public domain -- Ignacio Castaño <castano@gmail.com>

//#pragma once
//#ifndef NV_CORE_BITARRAY_H
//#define NV_CORE_BITARRAY_H

#include "nvcore.h"
#include "Array.inl"

namespace nv
{

    /// Count the bits of @a x.
    inline uint bitsSet(uint8 x) {
        uint count = 0;
        for(; x != 0; x >>= 1) {
            count += (x & 1);
        }
        return count;
    }


    /// Count the bits of @a x.
    inline uint bitsSet(uint32 x, int bits) {
        uint count = 0;
        for(; x != 0 && bits != 0; x >>= 1, bits--) {
            count += (x & 1);
        }
        return count;
    }


    /// Simple bit array.
    class BitArray
    {
    public:

        /// Default ctor.
        BitArray() {}

        /// Ctor with initial m_size.
        BitArray(uint sz)
        {
            resize(sz);
        }

        /// Get array m_size.
        uint size() const { return m_size; }

        /// Clear array m_size.
        void clear() { resize(0); }

        /// Set array m_size.
        void resize(uint sz)
        { 
            m_size = sz;
            m_wordArray.resize( (m_size + 31) >> 5 );
        }

        /// Get bit.
        bool bitAt(uint b) const
        {
            nvDebugCheck( b < m_size );
            return (m_wordArray[b >> 5] & (1 << (b & 31))) != 0;
        }

        /// Set a bit.
        void setBitAt(uint b)
        {
            nvDebugCheck( b < m_size );
            m_wordArray[b >> 5] |=  (1 << (b & 31));
        }

        /// Clear a bit.
        void clearBitAt(uint b)
        {
            nvDebugCheck( b < m_size );
            m_wordArray[b >> 5] &= ~(1 << (b & 31));
        }

        /// Toggle a bit.
        void toggleBitAt(uint b)
        {
            nvDebugCheck(b < m_size);
            m_wordArray[b >> 5] ^= (1 << (b & 31));
        }

        /// Clear all the bits.
        void clearAll()
        {
            memset(m_wordArray.buffer(), 0, m_wordArray.size() * sizeof(uint));
        }

        /// Set all the bits.
        void setAll()
        {
            memset(m_wordArray.buffer(), 0xFF, m_wordArray.size() * sizeof(uint));
        }

        /// Toggle all the bits.
        void toggleAll()
        {
            const uint wordCount = m_wordArray.count();
            for(uint b = 0; b < wordCount; b++) {
                m_wordArray[b] ^= 0xFFFFFFFF;
            }
        }

        /// Count the number of bits set.
        uint countSetBits() const
        {
            const uint num = m_wordArray.size();
            if( num == 0 ) {
                return 0;
            }

            uint count = 0;				
            for(uint i = 0; i < num - 1; i++) {
                count += bitsSet(m_wordArray[i]);
            }
            count += bitsSet(m_wordArray[num-1], m_size & 31);

            //piDebugCheck(count + countClearBits() == m_size);
            return count;
        }

        /// Count the number of bits clear.
        uint countClearBits() const {

            const uint num = m_wordArray.size();
            if( num == 0 ) {
                return 0;
            }

            uint count = 0;
            for(uint i = 0; i < num - 1; i++) {
                count += bitsSet(~m_wordArray[i]);
            }
            count += bitsSet(~m_wordArray[num-1], m_size & 31);

            //piDebugCheck(count + countSetBits() == m_size);
            return count;
        }

        friend void swap(BitArray & a, BitArray & b)
        {
            swap(a.m_size, b.m_size);
            swap(a.m_wordArray, b.m_wordArray);
        }


    private:

        /// Number of bits stored.
        uint m_size;

        /// Array of bits.
        Array<uint> m_wordArray;

    };

} // nv namespace

//#endif // _PI_CORE_BITARRAY_H_
