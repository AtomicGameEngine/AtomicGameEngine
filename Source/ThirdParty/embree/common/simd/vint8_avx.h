// ======================================================================== //
// Copyright 2009-2017 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

namespace embree
{
  /* 8-wide AVX integer type */
  template<>
  struct vint<8>
  {
    typedef vboolf8 Bool;
    typedef vint8   Int;
    typedef vfloat8 Float;

    enum  { size = 8 };        // number of SIMD elements
    union {                    // data
      __m256i v;
      struct { __m128i vl,vh; };
      int i[8];
    }; 

    ////////////////////////////////////////////////////////////////////////////////
    /// Constructors, Assignment & Cast Operators
    ////////////////////////////////////////////////////////////////////////////////

    __forceinline vint            ( ) {}
    __forceinline vint            ( const vint8& a ) { v = a.v; }
    __forceinline vint8& operator=( const vint8& a ) { v = a.v; return *this; }

    __forceinline vint( const __m256i a ) : v(a) {}
    __forceinline operator const __m256i&( void ) const { return v; }
    __forceinline operator       __m256i&( void )       { return v; }

    __forceinline explicit vint( const vint4& a ) : v(_mm256_insertf128_si256(_mm256_castsi128_si256(a),a,1)) {}
    __forceinline vint( const vint4& a, const vint4& b ) : v(_mm256_insertf128_si256(_mm256_castsi128_si256(a),b,1)) {}
    __forceinline vint( const __m128i& a, const __m128i& b ) : vl(a), vh(b) {}
 
    __forceinline explicit vint   ( const int* const a ) : v(_mm256_castps_si256(_mm256_loadu_ps((const float*)a))) {}
    __forceinline vint            ( int  a ) : v(_mm256_set1_epi32(a)) {}
    __forceinline vint            ( int  a, int  b) : v(_mm256_set_epi32(b, a, b, a, b, a, b, a)) {}
    __forceinline vint            ( int  a, int  b, int  c, int  d) : v(_mm256_set_epi32(d, c, b, a, d, c, b, a)) {}
    __forceinline vint            ( int  a, int  b, int  c, int  d, int  e, int  f, int  g, int  vh) : v(_mm256_set_epi32(vh, g, f, e, d, c, b, a)) {}

    __forceinline explicit vint( const __m256 a ) : v(_mm256_cvtps_epi32(a)) {}

    ////////////////////////////////////////////////////////////////////////////////
    /// Constants
    ////////////////////////////////////////////////////////////////////////////////

    __forceinline vint( ZeroTy   ) : v(_mm256_setzero_si256()) {}
    __forceinline vint( OneTy    ) : v(_mm256_set_epi32(1,1,1,1,1,1,1,1)) {}
    __forceinline vint( PosInfTy ) : v(_mm256_set_epi32(pos_inf,pos_inf,pos_inf,pos_inf,pos_inf,pos_inf,pos_inf,pos_inf)) {}
    __forceinline vint( NegInfTy ) : v(_mm256_set_epi32(neg_inf,neg_inf,neg_inf,neg_inf,neg_inf,neg_inf,neg_inf,neg_inf)) {}
    __forceinline vint( StepTy   ) : v(_mm256_set_epi32(7, 6, 5, 4, 3, 2, 1, 0)) {}

    ////////////////////////////////////////////////////////////////////////////////
    /// Loads and Stores
    ////////////////////////////////////////////////////////////////////////////////

    static __forceinline vint8 load ( const void* const a) { return _mm256_castps_si256(_mm256_load_ps((float*)a)); }
    static __forceinline vint8 loadu( const void* const a) { return _mm256_castps_si256(_mm256_loadu_ps((float*)a)); }

    static __forceinline vint8 load ( const vboolf8& mask, const void* const a ) { return _mm256_castps_si256(_mm256_maskload_ps((float*)a,mask)); }
    static __forceinline vint8 loadu( const vboolf8& mask, const void* const a ) { return _mm256_castps_si256(_mm256_maskload_ps((float*)a,mask)); }

    static __forceinline void store ( void* ptr, const vint8& f ) { _mm256_store_ps((float*)ptr,_mm256_castsi256_ps(f)); }
    static __forceinline void storeu( void* ptr, const vint8& f ) { _mm256_storeu_ps((float*)ptr,_mm256_castsi256_ps(f)); }
    
    static __forceinline void store ( const vboolf8& mask, void* ptr, const vint8& f ) { _mm256_maskstore_ps((float*)ptr,(__m256i)mask,_mm256_castsi256_ps(f)); }
    static __forceinline void storeu( const vboolf8& mask, void* ptr, const vint8& f ) { _mm256_maskstore_ps((float*)ptr,(__m256i)mask,_mm256_castsi256_ps(f)); }

    static __forceinline void store_nt(void* ptr, const vint8& v) {
      _mm256_stream_ps((float*)ptr,_mm256_castsi256_ps(v));
    }

    static __forceinline vint8 load( const unsigned char* const ptr ) {
      vint4 il = vint4::load(ptr+0);
      vint4 ih = vint4::load(ptr+4);
      return vint8(il,ih);
    }

    static __forceinline vint8 loadu( const unsigned char* const ptr ) {
      vint4 il = vint4::loadu(ptr+0);
      vint4 ih = vint4::loadu(ptr+4);
      return vint8(il,ih);
    }

    static __forceinline void store_uchar( unsigned char* const ptr, const vint8& i ) {
      vint4 il(i.vl);
      vint4 ih(i.vh);
      vint4::store_uchar(ptr + 0,il);
      vint4::store_uchar(ptr + 4,ih);
    }

    static __forceinline vint8 broadcast64(const long long &a) { return _mm256_set1_epi64x(a); }
    
    ////////////////////////////////////////////////////////////////////////////////
    /// Array Access
    ////////////////////////////////////////////////////////////////////////////////

    __forceinline const int& operator []( const size_t index ) const { assert(index < 8); return i[index]; }
    __forceinline       int& operator []( const size_t index )       { assert(index < 8); return i[index]; }
  };

  ////////////////////////////////////////////////////////////////////////////////
  /// Unary Operators
  ////////////////////////////////////////////////////////////////////////////////

  __forceinline const vint8 operator +( const vint8& a ) { return a; }
  __forceinline const vint8 operator -( const vint8& a ) { return vint8(_mm_sub_epi32(_mm_setzero_si128(), a.vl), _mm_sub_epi32(_mm_setzero_si128(), a.vh)); }
  __forceinline const vint8 abs       ( const vint8& a ) { return vint8(_mm_abs_epi32(a.vl), _mm_abs_epi32(a.vh)); }

  ////////////////////////////////////////////////////////////////////////////////
  /// Binary Operators
  ////////////////////////////////////////////////////////////////////////////////

  __forceinline const vint8 operator +( const vint8& a, const vint8& b ) { return vint8(_mm_add_epi32(a.vl, b.vl), _mm_add_epi32(a.vh, b.vh)); }
  __forceinline const vint8 operator +( const vint8& a, const int    b ) { return a + vint8(b); }
  __forceinline const vint8 operator +( const int    a, const vint8& b ) { return vint8(a) + b; }

  __forceinline const vint8 operator -( const vint8& a, const vint8& b ) { return vint8(_mm_sub_epi32(a.vl, b.vl), _mm_sub_epi32(a.vh, b.vh)); }
  __forceinline const vint8 operator -( const vint8& a, const int    b ) { return a - vint8(b); }
  __forceinline const vint8 operator -( const int    a, const vint8& b ) { return vint8(a) - b; }

  __forceinline const vint8 operator *( const vint8& a, const vint8& b ) { return vint8(_mm_mullo_epi32(a.vl, b.vl), _mm_mullo_epi32(a.vh, b.vh)); }
  __forceinline const vint8 operator *( const vint8& a, const int    b ) { return a * vint8(b); }
  __forceinline const vint8 operator *( const int    a, const vint8& b ) { return vint8(a) * b; }

  __forceinline const vint8 operator &( const vint8& a, const vint8& b ) { return _mm256_castps_si256(_mm256_and_ps(_mm256_castsi256_ps(a), _mm256_castsi256_ps(b))); }
  __forceinline const vint8 operator &( const vint8& a, const int    b ) { return a & vint8(b); }
  __forceinline const vint8 operator &( const int    a, const vint8& b ) { return vint8(a) & b; }

  __forceinline const vint8 operator |( const vint8& a, const vint8& b ) { return _mm256_castps_si256(_mm256_or_ps (_mm256_castsi256_ps(a), _mm256_castsi256_ps(b))); }
  __forceinline const vint8 operator |( const vint8& a, const int    b ) { return a | vint8(b); }
  __forceinline const vint8 operator |( const int    a, const vint8& b ) { return vint8(a) | b; }

  __forceinline const vint8 operator ^( const vint8& a, const vint8& b ) { return _mm256_castps_si256(_mm256_xor_ps(_mm256_castsi256_ps(a), _mm256_castsi256_ps(b))); }
  __forceinline const vint8 operator ^( const vint8& a, const int    b ) { return a ^ vint8(b); }
  __forceinline const vint8 operator ^( const int    a, const vint8& b ) { return vint8(a) ^ b; }

  __forceinline const vint8 operator <<( const vint8& a, const int n ) { return vint8(_mm_slli_epi32(a.vl, n), _mm_slli_epi32(a.vh, n)); }
  __forceinline const vint8 operator >>( const vint8& a, const int n ) { return vint8(_mm_srai_epi32(a.vl, n), _mm_srai_epi32(a.vh, n)); }

  __forceinline const vint8 sll ( const vint8& a, const int b ) { return vint8(_mm_slli_epi32(a.vl, b), _mm_slli_epi32(a.vh, b)); }
  __forceinline const vint8 sra ( const vint8& a, const int b ) { return vint8(_mm_srai_epi32(a.vl, b), _mm_srai_epi32(a.vh, b)); }
  __forceinline const vint8 srl ( const vint8& a, const int b ) { return vint8(_mm_srli_epi32(a.vl, b), _mm_srli_epi32(a.vh, b)); }
  
  __forceinline const vint8 min( const vint8& a, const vint8& b ) { return vint8(_mm_min_epi32(a.vl, b.vl), _mm_min_epi32(a.vh, b.vh)); }
  __forceinline const vint8 min( const vint8& a, const int    b ) { return min(a,vint8(b)); }
  __forceinline const vint8 min( const int    a, const vint8& b ) { return min(vint8(a),b); }

  __forceinline const vint8 max( const vint8& a, const vint8& b ) { return vint8(_mm_max_epi32(a.vl, b.vl), _mm_max_epi32(a.vh, b.vh)); }
  __forceinline const vint8 max( const vint8& a, const int    b ) { return max(a,vint8(b)); }
  __forceinline const vint8 max( const int    a, const vint8& b ) { return max(vint8(a),b); }

  __forceinline const vint8 umin( const vint8& a, const vint8& b ) { return vint8(_mm_min_epu32(a.vl, b.vl), _mm_min_epu32(a.vh, b.vh)); }
  __forceinline const vint8 umin( const vint8& a, const int    b ) { return umin(a,vint8(b)); }
  __forceinline const vint8 umin( const int    a, const vint8& b ) { return umin(vint8(a),b); }

  __forceinline const vint8 umax( const vint8& a, const vint8& b ) { return vint8(_mm_max_epu32(a.vl, b.vl), _mm_max_epu32(a.vh, b.vh)); }
  __forceinline const vint8 umax( const vint8& a, const int    b ) { return umax(a,vint8(b)); }
  __forceinline const vint8 umax( const int    a, const vint8& b ) { return umax(vint8(a),b); }

  ////////////////////////////////////////////////////////////////////////////////
  /// Assignment Operators
  ////////////////////////////////////////////////////////////////////////////////

  __forceinline vint8& operator +=( vint8& a, const vint8& b ) { return a = a + b; }
  __forceinline vint8& operator +=( vint8& a, const int    b ) { return a = a + b; }
  
  __forceinline vint8& operator -=( vint8& a, const vint8& b ) { return a = a - b; }
  __forceinline vint8& operator -=( vint8& a, const int    b ) { return a = a - b; }
  
  __forceinline vint8& operator *=( vint8& a, const vint8& b ) { return a = a * b; }
  __forceinline vint8& operator *=( vint8& a, const int    b ) { return a = a * b; }
  
  __forceinline vint8& operator &=( vint8& a, const vint8& b ) { return a = a & b; }
  __forceinline vint8& operator &=( vint8& a, const int    b ) { return a = a & b; }
  
  __forceinline vint8& operator |=( vint8& a, const vint8& b ) { return a = a | b; }
  __forceinline vint8& operator |=( vint8& a, const int    b ) { return a = a | b; }
  
  __forceinline vint8& operator <<=( vint8& a, const int  b ) { return a = a << b; }
  __forceinline vint8& operator >>=( vint8& a, const int  b ) { return a = a >> b; }

  ////////////////////////////////////////////////////////////////////////////////
  /// Comparison Operators + Select
  ////////////////////////////////////////////////////////////////////////////////

  __forceinline const vboolf8 operator ==( const vint8& a, const vint8& b ) { return vboolf8(_mm_castsi128_ps(_mm_cmpeq_epi32 (a.vl, b.vl)),
                                                                                     _mm_castsi128_ps(_mm_cmpeq_epi32 (a.vh, b.vh))); }
  __forceinline const vboolf8 operator ==( const vint8& a, const int    b ) { return a == vint8(b); }
  __forceinline const vboolf8 operator ==( const int    a, const vint8& b ) { return vint8(a) == b; }
  
  __forceinline const vboolf8 operator !=( const vint8& a, const vint8& b ) { return !(a == b); }
  __forceinline const vboolf8 operator !=( const vint8& a, const int    b ) { return a != vint8(b); }
  __forceinline const vboolf8 operator !=( const int    a, const vint8& b ) { return vint8(a) != b; }
  
  __forceinline const vboolf8 operator < ( const vint8& a, const vint8& b ) { return vboolf8(_mm_castsi128_ps(_mm_cmplt_epi32 (a.vl, b.vl)),
                                                                                     _mm_castsi128_ps(_mm_cmplt_epi32 (a.vh, b.vh))); }
  __forceinline const vboolf8 operator < ( const vint8& a, const int    b ) { return a <  vint8(b); }
  __forceinline const vboolf8 operator < ( const int    a, const vint8& b ) { return vint8(a) <  b; }
  
  __forceinline const vboolf8 operator >=( const vint8& a, const vint8& b ) { return !(a <  b); }
  __forceinline const vboolf8 operator >=( const vint8& a, const int    b ) { return a >= vint8(b); }
  __forceinline const vboolf8 operator >=( const int    a, const vint8& b ) { return vint8(a) >= b; }

  __forceinline const vboolf8 operator > ( const vint8& a, const vint8& b ) { return vboolf8(_mm_castsi128_ps(_mm_cmpgt_epi32 (a.vl, b.vl)),
                                                                                     _mm_castsi128_ps(_mm_cmpgt_epi32 (a.vh, b.vh))); }
  __forceinline const vboolf8 operator > ( const vint8& a, const int    b ) { return a >  vint8(b); }
  __forceinline const vboolf8 operator > ( const int    a, const vint8& b ) { return vint8(a) >  b; }

  __forceinline const vboolf8 operator <=( const vint8& a, const vint8& b ) { return !(a >  b); }
  __forceinline const vboolf8 operator <=( const vint8& a, const int    b ) { return a <= vint8(b); }
  __forceinline const vboolf8 operator <=( const int    a, const vint8& b ) { return vint8(a) <= b; }

  __forceinline vboolf8 eq(const vint8& a, const vint8& b) { return a == b; }
  __forceinline vboolf8 ne(const vint8& a, const vint8& b) { return a != b; }
  __forceinline vboolf8 lt(const vint8& a, const vint8& b) { return a <  b; }
  __forceinline vboolf8 ge(const vint8& a, const vint8& b) { return a >= b; }
  __forceinline vboolf8 gt(const vint8& a, const vint8& b) { return a >  b; }
  __forceinline vboolf8 le(const vint8& a, const vint8& b) { return a <= b; }

  __forceinline vboolf8 eq(const vboolf8& mask, const vint8& a, const vint8& b) { return mask & (a == b); }
  __forceinline vboolf8 ne(const vboolf8& mask, const vint8& a, const vint8& b) { return mask & (a != b); }
  __forceinline vboolf8 lt(const vboolf8& mask, const vint8& a, const vint8& b) { return mask & (a <  b); }
  __forceinline vboolf8 ge(const vboolf8& mask, const vint8& a, const vint8& b) { return mask & (a >= b); }
  __forceinline vboolf8 gt(const vboolf8& mask, const vint8& a, const vint8& b) { return mask & (a >  b); }
  __forceinline vboolf8 le(const vboolf8& mask, const vint8& a, const vint8& b) { return mask & (a <= b); }

  __forceinline const vint8 select( const vboolf8& m, const vint8& t, const vint8& f ) {
    return _mm256_castps_si256(_mm256_blendv_ps(_mm256_castsi256_ps(f), _mm256_castsi256_ps(t), m)); 
  }

  __forceinline const vint8 notand( const vboolf8& m, const vint8& f) {
    return _mm256_castps_si256(_mm256_andnot_ps(m, _mm256_castsi256_ps(f))); 
  }


  ////////////////////////////////////////////////////////////////////////////////
  /// Movement/Shifting/Shuffling Functions
  ////////////////////////////////////////////////////////////////////////////////

  __forceinline vint8 unpacklo( const vint8& a, const vint8& b ) { return _mm256_castps_si256(_mm256_unpacklo_ps(_mm256_castsi256_ps(a), _mm256_castsi256_ps(b))); }
  __forceinline vint8 unpackhi( const vint8& a, const vint8& b ) { return _mm256_castps_si256(_mm256_unpackhi_ps(_mm256_castsi256_ps(a), _mm256_castsi256_ps(b))); }

  template<size_t i> __forceinline const vint8 shuffle( const vint8& a ) {
    return _mm256_castps_si256(_mm256_permute_ps(_mm256_castsi256_ps(a), _MM_SHUFFLE(i, i, i, i)));
  }

  template<size_t i0, size_t i1> __forceinline const vint8 shuffle4( const vint8& a ) {
    return _mm256_permute2f128_si256(a, a, (i1 << 4) | (i0 << 0));
  }

  template<size_t i0, size_t i1> __forceinline const vint8 shuffle4( const vint8& a,  const vint8& b) {
    return _mm256_permute2f128_si256(a, b, (i1 << 4) | (i0 << 0));
  }

  template<size_t i0, size_t i1, size_t i2, size_t i3> __forceinline const vint8 shuffle( const vint8& a ) {
    return _mm256_castps_si256(_mm256_permute_ps(_mm256_castsi256_ps(a), _MM_SHUFFLE(i3, i2, i1, i0)));
  }

  template<size_t i0, size_t i1, size_t i2, size_t i3> __forceinline const vint8 shuffle( const vint8& a, const vint8& b ) {
    return _mm256_castps_si256(_mm256_shuffle_ps(_mm256_castsi256_ps(a), _mm256_castsi256_ps(b), _MM_SHUFFLE(i3, i2, i1, i0)));
  }

  template<> __forceinline const vint8 shuffle<0, 0, 2, 2>( const vint8& b ) { return _mm256_castps_si256(_mm256_moveldup_ps(_mm256_castsi256_ps(b))); }
  template<> __forceinline const vint8 shuffle<1, 1, 3, 3>( const vint8& b ) { return _mm256_castps_si256(_mm256_movehdup_ps(_mm256_castsi256_ps(b))); }
  template<> __forceinline const vint8 shuffle<0, 1, 0, 1>( const vint8& b ) { return _mm256_castps_si256(_mm256_castpd_ps(_mm256_movedup_pd(_mm256_castps_pd(_mm256_castsi256_ps(b))))); }

  __forceinline const vint8 broadcast(const int* ptr) { return _mm256_castps_si256(_mm256_broadcast_ss((const float*)ptr)); }
  template<size_t i> __forceinline const vint8 insert4(const vint8& a, const vint4& b) { return _mm256_insertf128_si256(a, b, i); }
  template<size_t i> __forceinline const vint4 extract4   (const vint8& a) { return _mm256_extractf128_si256(a, i); }
  template<>         __forceinline const vint4 extract4<0>(const vint8& a) { return _mm256_castsi256_si128(a);      }

  __forceinline int toScalar(const vint8& a) { return _mm_cvtsi128_si32(_mm256_castsi256_si128(a)); }


  ////////////////////////////////////////////////////////////////////////////////
  /// Reductions
  ////////////////////////////////////////////////////////////////////////////////

  __forceinline const vint8 vreduce_min2(const vint8& v) { return min(v,shuffle<1,0,3,2>(v)); }
  __forceinline const vint8 vreduce_min4(const vint8& v) { vint8 v1 = vreduce_min2(v); return min(v1,shuffle<2,3,0,1>(v1)); }
  __forceinline const vint8 vreduce_min (const vint8& v) { vint8 v1 = vreduce_min4(v); return min(v1,shuffle4<1,0>(v1)); }

  __forceinline const vint8 vreduce_max2(const vint8& v) { return max(v,shuffle<1,0,3,2>(v)); }
  __forceinline const vint8 vreduce_max4(const vint8& v) { vint8 v1 = vreduce_max2(v); return max(v1,shuffle<2,3,0,1>(v1)); }
  __forceinline const vint8 vreduce_max (const vint8& v) { vint8 v1 = vreduce_max4(v); return max(v1,shuffle4<1,0>(v1)); }

  __forceinline const vint8 vreduce_add2(const vint8& v) { return v + shuffle<1,0,3,2>(v); }
  __forceinline const vint8 vreduce_add4(const vint8& v) { vint8 v1 = vreduce_add2(v); return v1 + shuffle<2,3,0,1>(v1); }
  __forceinline const vint8 vreduce_add (const vint8& v) { vint8 v1 = vreduce_add4(v); return v1 + shuffle4<1,0>(v1); }

  __forceinline int reduce_min(const vint8& v) { return toScalar(vreduce_min(v)); }
  __forceinline int reduce_max(const vint8& v) { return toScalar(vreduce_max(v)); }
  __forceinline int reduce_add(const vint8& v) { return toScalar(vreduce_add(v)); }

  __forceinline size_t select_min(const vint8& v) { return __bsf(movemask(v == vreduce_min(v))); }
  __forceinline size_t select_max(const vint8& v) { return __bsf(movemask(v == vreduce_max(v))); }

  __forceinline size_t select_min(const vboolf8& valid, const vint8& v) { const vint8 a = select(valid,v,vint8(pos_inf)); return __bsf(movemask(valid & (a == vreduce_min(a)))); }
  __forceinline size_t select_max(const vboolf8& valid, const vint8& v) { const vint8 a = select(valid,v,vint8(neg_inf)); return __bsf(movemask(valid & (a == vreduce_max(a)))); }

  ////////////////////////////////////////////////////////////////////////////////
  /// Sorting networks
  ////////////////////////////////////////////////////////////////////////////////

  __forceinline vint8 sortNetwork(const vint8& v)
  {
    const vint8 a0 = v;
    const vint8 b0 = shuffle<1,0,3,2>(a0);
    const vint8 c0 = umin(a0,b0);
    const vint8 d0 = umax(a0,b0);
    const vint8 a1 = select(0x99 /* 0b10011001 */,c0,d0);
    const vint8 b1 = shuffle<2,3,0,1>(a1);
    const vint8 c1 = umin(a1,b1);
    const vint8 d1 = umax(a1,b1);
    const vint8 a2 = select(0xc3 /* 0b11000011 */,c1,d1);
    const vint8 b2 = shuffle<1,0,3,2>(a2);
    const vint8 c2 = umin(a2,b2);
    const vint8 d2 = umax(a2,b2);
    const vint8 a3 = select(0xa5 /* 0b10100101 */,c2,d2);
    const vint8 b3 = shuffle4<1,0>(a3);
    const vint8 c3 = umin(a3,b3);
    const vint8 d3 = umax(a3,b3);
    const vint8 a4 = select(0xf /* 0b00001111 */,c3,d3);
    const vint8 b4 = shuffle<2,3,0,1>(a4);
    const vint8 c4 = umin(a4,b4);
    const vint8 d4 = umax(a4,b4);
    const vint8 a5 = select(0x33 /* 0b00110011 */,c4,d4);
    const vint8 b5 = shuffle<1,0,3,2>(a5);
    const vint8 c5 = umin(a5,b5);
    const vint8 d5 = umax(a5,b5);
    const vint8 a6 = select(0x55 /* 0b01010101 */,c5,d5);
    return a6;
  }

  ////////////////////////////////////////////////////////////////////////////////
  /// Output Operators
  ////////////////////////////////////////////////////////////////////////////////

  inline std::ostream& operator<<(std::ostream& cout, const vint8& a) {
    return cout << "<" << a[0] << ", " << a[1] << ", " << a[2] << ", " << a[3] << ", " << a[4] << ", " << a[5] << ", " << a[6] << ", " << a[7] << ">";
  }
}
