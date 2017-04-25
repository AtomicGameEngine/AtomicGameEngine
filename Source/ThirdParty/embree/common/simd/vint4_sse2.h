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

#include "../math/math.h"

namespace embree
{
  /* 4-wide SSE integer type */
  template<>
  struct vint<4>
  {
    typedef vboolf4 Bool;
    typedef vint4   Int;
    typedef vfloat4 Float;

    enum  { size = 4 };             // number of SIMD elements
    union { __m128i v; int i[4]; }; // data

    ////////////////////////////////////////////////////////////////////////////////
    /// Constructors, Assignment & Cast Operators
    ////////////////////////////////////////////////////////////////////////////////
    
    __forceinline vint            ( ) {}
    __forceinline vint            ( const vint4& a ) { v = a.v; }
    __forceinline vint4& operator=( const vint4& a ) { v = a.v; return *this; }

    __forceinline vint( const __m128i a ) : v(a) {}
    __forceinline operator const __m128i&( void ) const { return v; }
    __forceinline operator       __m128i&( void )       { return v; }


    __forceinline vint( const int&  a ) : v(_mm_shuffle_epi32(_mm_castps_si128(_mm_load_ss((float*)&a)), _MM_SHUFFLE(0, 0, 0, 0))) {}
    __forceinline vint( const uint32_t& a ) : v(_mm_shuffle_epi32(_mm_castps_si128(_mm_load_ss((float*)&a)), _MM_SHUFFLE(0, 0, 0, 0))) {}
#if defined(__X86_64__)
    __forceinline vint( const size_t a  ) : v(_mm_set1_epi32((int)a)) {}
#endif
    __forceinline vint( int  a, int  b, int  c, int  d) : v(_mm_set_epi32(d, c, b, a)) {}

    __forceinline explicit vint( const __m128 a ) : v(_mm_cvtps_epi32(a)) {}
#if defined(__AVX512VL__)
    __forceinline explicit vint( const vboolf4 &a ) : v(_mm_movm_epi32(a)) {}
#else
    __forceinline explicit vint( const vboolf4 &a ) : v(_mm_castps_si128((__m128)a)) {}
#endif

    ////////////////////////////////////////////////////////////////////////////////
    /// Constants
    ////////////////////////////////////////////////////////////////////////////////

    __forceinline vint( ZeroTy   ) : v(_mm_setzero_si128()) {}
    __forceinline vint( OneTy    ) : v(_mm_set_epi32(1, 1, 1, 1)) {}
    __forceinline vint( PosInfTy ) : v(_mm_set_epi32(pos_inf, pos_inf, pos_inf, pos_inf)) {}
    __forceinline vint( NegInfTy ) : v(_mm_set_epi32(neg_inf, neg_inf, neg_inf, neg_inf)) {}
    __forceinline vint( StepTy   ) : v(_mm_set_epi32(3, 2, 1, 0)) {}
    __forceinline vint( TrueTy   ) { v = _mm_cmpeq_epi32(v,v); }


    ////////////////////////////////////////////////////////////////////////////////
    /// Loads and Stores
    ////////////////////////////////////////////////////////////////////////////////

    static __forceinline vint4 load ( const void* const a ) { return _mm_load_si128((__m128i*)a); }
    static __forceinline vint4 loadu( const void* const a ) { return _mm_loadu_si128((__m128i*)a); }

    static __forceinline void store (void* ptr, const vint4& v) { _mm_store_si128((__m128i*)ptr,v); }
    static __forceinline void storeu(void* ptr, const vint4& v) { _mm_storeu_si128((__m128i*)ptr,v); }
    
#if defined(__AVX512VL__)
    static __forceinline vint4 load ( const vboolf4& mask, const void* const ptr ) { return _mm_mask_load_epi32 (_mm_setzero_si128(),mask,ptr); }
    static __forceinline vint4 loadu( const vboolf4& mask, const void* const ptr ) { return _mm_mask_loadu_epi32(_mm_setzero_si128(),mask,ptr); }

    static __forceinline void store ( const vboolf4& mask, void* ptr, const vint4& v ) { _mm_mask_store_epi32 (ptr,mask,v); }
    static __forceinline void storeu( const vboolf4& mask, void* ptr, const vint4& v ) { _mm_mask_storeu_epi32(ptr,mask,v); }
#elif defined(__AVX__)
    static __forceinline vint4 load ( const vbool4& mask, const void* const a ) { return _mm_castps_si128(_mm_maskload_ps((float*)a,mask)); }
    static __forceinline vint4 loadu( const vbool4& mask, const void* const a ) { return _mm_castps_si128(_mm_maskload_ps((float*)a,mask)); }

    static __forceinline void store ( const vboolf4& mask, void* ptr, const vint4& i ) { _mm_maskstore_ps((float*)ptr,(__m128i)mask,_mm_castsi128_ps(i)); }
    static __forceinline void storeu( const vboolf4& mask, void* ptr, const vint4& i ) { _mm_maskstore_ps((float*)ptr,(__m128i)mask,_mm_castsi128_ps(i)); }
#else
    static __forceinline vint4 load ( const vbool4& mask, const void* const a ) { return _mm_and_si128(_mm_load_si128 ((__m128i*)a),mask); }
    static __forceinline vint4 loadu( const vbool4& mask, const void* const a ) { return _mm_and_si128(_mm_loadu_si128((__m128i*)a),mask); }

    static __forceinline void store ( const vboolf4& mask, void* ptr, const vint4& i ) { store (ptr,select(mask,i,load (ptr))); }
    static __forceinline void storeu( const vboolf4& mask, void* ptr, const vint4& i ) { storeu(ptr,select(mask,i,loadu(ptr))); }
#endif

#if defined(__SSE4_1__)
    static __forceinline vint4 load( const unsigned char* const ptr ) {
      return _mm_cvtepu8_epi32(_mm_load_si128((__m128i*)ptr));
    }

    static __forceinline vint4 loadu( const unsigned char* const ptr ) {
      return  _mm_cvtepu8_epi32(_mm_loadu_si128((__m128i*)ptr));
    }

#endif

    static __forceinline vint4 load(const unsigned short* const ptr) {
#if defined (__SSE4_1__)
      return _mm_cvtepu16_epi32(_mm_loadu_si128((__m128i*)ptr));
#else
      return vint4(ptr[0],ptr[1],ptr[2],ptr[3]);
#endif
    } 

    static __forceinline void store_uchar( unsigned char* const ptr, const vint4& v ) {
#if defined(__SSE4_1__)
      __m128i x = v;
      x = _mm_packus_epi32(x, x);
      x = _mm_packus_epi16(x, x);
      *(int*)ptr = _mm_cvtsi128_si32(x);
#else
      for (size_t i=0;i<4;i++)
        ptr[i] = (unsigned char)v[i];
#endif
    }

    static __forceinline vint4 load_nt (void* ptr) {
#if defined(__SSE4_1__)
      return _mm_stream_load_si128((__m128i*)ptr); 
#else
      return _mm_load_si128((__m128i*)ptr); 
#endif
    }
    
    static __forceinline void store_nt(void* ptr, const vint4& v) {
#if defined(__SSE4_1__)
      _mm_stream_ps((float*)ptr,_mm_castsi128_ps(v)); 
#else
      _mm_store_si128((__m128i*)ptr,v);
#endif
    }

#if defined(__x86_64__)
    static __forceinline vint4 broadcast64(const long long &a) { return _mm_set1_epi64x(a); }
#endif

    ////////////////////////////////////////////////////////////////////////////////
    /// Array Access
    ////////////////////////////////////////////////////////////////////////////////

    __forceinline const int& operator []( const size_t index ) const { assert(index < 4); return i[index]; }
    __forceinline       int& operator []( const size_t index )       { assert(index < 4); return i[index]; }

    friend __forceinline const vint4 select( const vboolf4& m, const vint4& t, const vint4& f ) {
#if defined(__AVX512VL__)
      return _mm_mask_blend_epi32(m, f, t);
#elif defined(__SSE4_1__)
      return _mm_castps_si128(_mm_blendv_ps(_mm_castsi128_ps(f), _mm_castsi128_ps(t), m)); 
#else
      return _mm_or_si128(_mm_and_si128(m, t), _mm_andnot_si128(m, f)); 
#endif
    }
  };

  ////////////////////////////////////////////////////////////////////////////////
  /// Unary Operators
  ////////////////////////////////////////////////////////////////////////////////

  __forceinline const vint4 asInt     ( const __m128& a ) { return _mm_castps_si128(a); }
  __forceinline const vint4 operator +( const vint4&  a ) { return a; }
  __forceinline const vint4 operator -( const vint4&  a ) { return _mm_sub_epi32(_mm_setzero_si128(), a.v); }
#if defined(__SSSE3__)
  __forceinline const vint4 abs       ( const vint4&  a ) { return _mm_abs_epi32(a.v); }
#endif

  ////////////////////////////////////////////////////////////////////////////////
  /// Binary Operators
  ////////////////////////////////////////////////////////////////////////////////

  __forceinline const vint4 operator +( const vint4& a, const vint4& b ) { return _mm_add_epi32(a.v, b.v); }
  __forceinline const vint4 operator +( const vint4& a, const int&   b ) { return a + vint4(b); }
  __forceinline const vint4 operator +( const int&   a, const vint4& b ) { return vint4(a) + b; }

  __forceinline const vint4 operator -( const vint4& a, const vint4& b ) { return _mm_sub_epi32(a.v, b.v); }
  __forceinline const vint4 operator -( const vint4& a, const int&   b ) { return a - vint4(b); }
  __forceinline const vint4 operator -( const int&   a, const vint4& b ) { return vint4(a) - b; }

#if defined(__SSE4_1__)
  __forceinline const vint4 operator *( const vint4& a, const vint4& b ) { return _mm_mullo_epi32(a.v, b.v); }
#else
  __forceinline const vint4 operator *( const vint4& a, const vint4& b ) { return vint4(a[0]*b[0],a[1]*b[1],a[2]*b[2],a[3]*b[3]); }
#endif
  __forceinline const vint4 operator *( const vint4& a, const int&   b ) { return a * vint4(b); }
  __forceinline const vint4 operator *( const int&   a, const vint4& b ) { return vint4(a) * b; }

  __forceinline const vint4 operator &( const vint4& a, const vint4& b ) { return _mm_and_si128(a.v, b.v); }
  __forceinline const vint4 operator &( const vint4& a, const int&   b ) { return a & vint4(b); }
  __forceinline const vint4 operator &( const int&   a, const vint4& b ) { return vint4(a) & b; }

  __forceinline const vint4 operator |( const vint4& a, const vint4& b ) { return _mm_or_si128(a.v, b.v); }
  __forceinline const vint4 operator |( const vint4& a, const int&   b ) { return a | vint4(b); }
  __forceinline const vint4 operator |( const int&   a, const vint4& b ) { return vint4(a) | b; }

  __forceinline const vint4 operator ^( const vint4& a, const vint4& b ) { return _mm_xor_si128(a.v, b.v); }
  __forceinline const vint4 operator ^( const vint4& a, const int&   b ) { return a ^ vint4(b); }
  __forceinline const vint4 operator ^( const int&   a, const vint4& b ) { return vint4(a) ^ b; }

  __forceinline const vint4 operator <<( const vint4& a, const int& n ) { return _mm_slli_epi32(a.v, n); }
  __forceinline const vint4 operator >>( const vint4& a, const int& n ) { return _mm_srai_epi32(a.v, n); }

  __forceinline const vint4 sll ( const vint4& a, const int& b ) { return _mm_slli_epi32(a.v, b); }
  __forceinline const vint4 sra ( const vint4& a, const int& b ) { return _mm_srai_epi32(a.v, b); }
  __forceinline const vint4 srl ( const vint4& a, const int& b ) { return _mm_srli_epi32(a.v, b); }
  
#if defined(__SSE4_1__)
  __forceinline const vint4 min( const vint4& a, const vint4& b ) { return _mm_min_epi32(a.v, b.v); }
  __forceinline const vint4 max( const vint4& a, const vint4& b ) { return _mm_max_epi32(a.v, b.v); }

  __forceinline const vint4 umin( const vint4& a, const vint4& b ) { return _mm_min_epu32(a.v, b.v); }
  __forceinline const vint4 umax( const vint4& a, const vint4& b ) { return _mm_max_epu32(a.v, b.v); }

#else
  __forceinline const vint4 min( const vint4& a, const vint4& b ) { return vint4(min(a[0],b[0]),min(a[1],b[1]),min(a[2],b[2]),min(a[3],b[3])); }
  __forceinline const vint4 max( const vint4& a, const vint4& b ) { return vint4(max(a[0],b[0]),max(a[1],b[1]),max(a[2],b[2]),max(a[3],b[3])); }
#endif

  __forceinline const vint4 min( const vint4& a, const int&   b ) { return min(a,vint4(b)); }
  __forceinline const vint4 min( const int&   a, const vint4& b ) { return min(vint4(a),b); }
  __forceinline const vint4 max( const vint4& a, const int&   b ) { return max(a,vint4(b)); }
  __forceinline const vint4 max( const int&   a, const vint4& b ) { return max(vint4(a),b); }

  ////////////////////////////////////////////////////////////////////////////////
  /// Assignment Operators
  ////////////////////////////////////////////////////////////////////////////////

  __forceinline vint4& operator +=( vint4& a, const vint4& b ) { return a = a + b; }
  __forceinline vint4& operator +=( vint4& a, const int&   b ) { return a = a + b; }
  
  __forceinline vint4& operator -=( vint4& a, const vint4& b ) { return a = a - b; }
  __forceinline vint4& operator -=( vint4& a, const int&   b ) { return a = a - b; }

#if defined(__SSE4_1__)
  __forceinline vint4& operator *=( vint4& a, const vint4& b ) { return a = a * b; }
  __forceinline vint4& operator *=( vint4& a, const int&   b ) { return a = a * b; }
#endif
  
  __forceinline vint4& operator &=( vint4& a, const vint4& b ) { return a = a & b; }
  __forceinline vint4& operator &=( vint4& a, const int&   b ) { return a = a & b; }
  
  __forceinline vint4& operator |=( vint4& a, const vint4& b ) { return a = a | b; }
  __forceinline vint4& operator |=( vint4& a, const int&   b ) { return a = a | b; }
  
  __forceinline vint4& operator <<=( vint4& a, const int&  b ) { return a = a << b; }
  __forceinline vint4& operator >>=( vint4& a, const int&  b ) { return a = a >> b; }

  ////////////////////////////////////////////////////////////////////////////////
  /// Comparison Operators + Select
  ////////////////////////////////////////////////////////////////////////////////

#if defined(__AVX512VL__)
  __forceinline const vboolf4 operator ==( const vint4& a, const vint4& b ) { return _mm_cmp_epi32_mask(a,b,_MM_CMPINT_EQ); }
  __forceinline const vboolf4 operator !=( const vint4& a, const vint4& b ) { return _mm_cmp_epi32_mask(a,b,_MM_CMPINT_NE); }
  __forceinline const vboolf4 operator < ( const vint4& a, const vint4& b ) { return _mm_cmp_epi32_mask(a,b,_MM_CMPINT_LT); }
  __forceinline const vboolf4 operator >=( const vint4& a, const vint4& b ) { return _mm_cmp_epi32_mask(a,b,_MM_CMPINT_GE); }
  __forceinline const vboolf4 operator > ( const vint4& a, const vint4& b ) { return _mm_cmp_epi32_mask(a,b,_MM_CMPINT_GT); }
  __forceinline const vboolf4 operator <=( const vint4& a, const vint4& b ) { return _mm_cmp_epi32_mask(a,b,_MM_CMPINT_LE); }
#else
  __forceinline const vboolf4 operator ==( const vint4& a, const vint4& b ) { return _mm_castsi128_ps(_mm_cmpeq_epi32(a, b)); }
  __forceinline const vboolf4 operator !=( const vint4& a, const vint4& b ) { return !(a == b); }
  __forceinline const vboolf4 operator < ( const vint4& a, const vint4& b ) { return _mm_castsi128_ps(_mm_cmplt_epi32(a, b)); }
  __forceinline const vboolf4 operator >=( const vint4& a, const vint4& b ) { return !(a <  b); }
  __forceinline const vboolf4 operator > ( const vint4& a, const vint4& b ) { return _mm_castsi128_ps(_mm_cmpgt_epi32(a, b)); }
  __forceinline const vboolf4 operator <=( const vint4& a, const vint4& b ) { return !(a >  b); }
#endif

  __forceinline const vboolf4 operator ==( const vint4& a, const int&   b ) { return a == vint4(b); }
  __forceinline const vboolf4 operator ==( const int&   a, const vint4& b ) { return vint4(a) == b; }

  __forceinline const vboolf4 operator !=( const vint4& a, const int&   b ) { return a != vint4(b); }
  __forceinline const vboolf4 operator !=( const int&   a, const vint4& b ) { return vint4(a) != b; }

  __forceinline const vboolf4 operator < ( const vint4& a, const int&   b ) { return a <  vint4(b); }
  __forceinline const vboolf4 operator < ( const int&   a, const vint4& b ) { return vint4(a) <  b; }

  __forceinline const vboolf4 operator >=( const vint4& a, const int&   b ) { return a >= vint4(b); }
  __forceinline const vboolf4 operator >=( const int&   a, const vint4& b ) { return vint4(a) >= b; }

  __forceinline const vboolf4 operator > ( const vint4& a, const int&   b ) { return a >  vint4(b); }
  __forceinline const vboolf4 operator > ( const int&   a, const vint4& b ) { return vint4(a) >  b; }

  __forceinline const vboolf4 operator <=( const vint4& a, const int&   b ) { return a <= vint4(b); }
  __forceinline const vboolf4 operator <=( const int&   a, const vint4& b ) { return vint4(a) <= b; }

  __forceinline vboolf4 eq(const vint4& a, const vint4& b) { return a == b; }
  __forceinline vboolf4 ne(const vint4& a, const vint4& b) { return a != b; }
  __forceinline vboolf4 lt(const vint4& a, const vint4& b) { return a <  b; }
  __forceinline vboolf4 ge(const vint4& a, const vint4& b) { return a >= b; }
  __forceinline vboolf4 gt(const vint4& a, const vint4& b) { return a >  b; }
  __forceinline vboolf4 le(const vint4& a, const vint4& b) { return a <= b; }

#if defined(__AVX512VL__)
  __forceinline vboolf4 eq(const vboolf4& mask, const vint4& a, const vint4& b) { return _mm_mask_cmp_epi32_mask(mask, a, b, _MM_CMPINT_EQ); }
  __forceinline vboolf4 ne(const vboolf4& mask, const vint4& a, const vint4& b) { return _mm_mask_cmp_epi32_mask(mask, a, b, _MM_CMPINT_NE); }
  __forceinline vboolf4 lt(const vboolf4& mask, const vint4& a, const vint4& b) { return _mm_mask_cmp_epi32_mask(mask, a, b, _MM_CMPINT_LT); }
  __forceinline vboolf4 ge(const vboolf4& mask, const vint4& a, const vint4& b) { return _mm_mask_cmp_epi32_mask(mask, a, b, _MM_CMPINT_GE); }
  __forceinline vboolf4 gt(const vboolf4& mask, const vint4& a, const vint4& b) { return _mm_mask_cmp_epi32_mask(mask, a, b, _MM_CMPINT_GT); }
  __forceinline vboolf4 le(const vboolf4& mask, const vint4& a, const vint4& b) { return _mm_mask_cmp_epi32_mask(mask, a, b, _MM_CMPINT_LE); }
#else
  __forceinline vboolf4 eq(const vboolf4& mask, const vint4& a, const vint4& b) { return mask & (a == b); }
  __forceinline vboolf4 ne(const vboolf4& mask, const vint4& a, const vint4& b) { return mask & (a != b); }
  __forceinline vboolf4 lt(const vboolf4& mask, const vint4& a, const vint4& b) { return mask & (a <  b); }
  __forceinline vboolf4 ge(const vboolf4& mask, const vint4& a, const vint4& b) { return mask & (a >= b); }
  __forceinline vboolf4 gt(const vboolf4& mask, const vint4& a, const vint4& b) { return mask & (a >  b); }
  __forceinline vboolf4 le(const vboolf4& mask, const vint4& a, const vint4& b) { return mask & (a <= b); }
#endif

  template<int mask>
  __forceinline const vint4 select(const vint4& t, const vint4& f) {
#if defined(__SSE4_1__) 
    return _mm_castps_si128(_mm_blend_ps(_mm_castsi128_ps(f), _mm_castsi128_ps(t), mask));
#else
    return select(vboolf4(mask), t, f);
#endif    
  }

  ////////////////////////////////////////////////////////////////////////////////
  // Movement/Shifting/Shuffling Functions
  ////////////////////////////////////////////////////////////////////////////////

  __forceinline vint4 unpacklo( const vint4& a, const vint4& b ) { return _mm_castps_si128(_mm_unpacklo_ps(_mm_castsi128_ps(a.v), _mm_castsi128_ps(b.v))); }
  __forceinline vint4 unpackhi( const vint4& a, const vint4& b ) { return _mm_castps_si128(_mm_unpackhi_ps(_mm_castsi128_ps(a.v), _mm_castsi128_ps(b.v))); }

  template<size_t i0, size_t i1, size_t i2, size_t i3> __forceinline const vint4 shuffle( const vint4& a ) {
    return _mm_shuffle_epi32(a, _MM_SHUFFLE(i3, i2, i1, i0));
  }

  template<size_t i0, size_t i1, size_t i2, size_t i3> __forceinline const vint4 shuffle( const vint4& a, const vint4& b ) {
    return _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(a), _mm_castsi128_ps(b), _MM_SHUFFLE(i3, i2, i1, i0)));
  }

#if defined(__SSE3__)
  template<> __forceinline const vint4 shuffle<0, 0, 2, 2>( const vint4& a ) { return _mm_castps_si128(_mm_moveldup_ps(_mm_castsi128_ps(a))); }
  template<> __forceinline const vint4 shuffle<1, 1, 3, 3>( const vint4& a ) { return _mm_castps_si128(_mm_movehdup_ps(_mm_castsi128_ps(a))); }
  template<> __forceinline const vint4 shuffle<0, 1, 0, 1>( const vint4& a ) { return _mm_castpd_si128(_mm_movedup_pd (_mm_castsi128_pd(a))); }
#endif

  template<size_t i0> __forceinline const vint4 shuffle( const vint4& b ) {
    return shuffle<i0,i0,i0,i0>(b);
  }

#if defined(__SSE4_1__)
  template<size_t src> __forceinline int extract( const vint4& b ) { return _mm_extract_epi32(b, src); }
  template<size_t dst> __forceinline const vint4 insert( const vint4& a, const int b ) { return _mm_insert_epi32(a, b, dst); }
#else
  template<size_t src> __forceinline int extract( const vint4& b ) { return b[src]; }
  template<size_t dst> __forceinline const vint4 insert( const vint4& a, const int b ) { vint4 c = a; c[dst] = b; return c; }
#endif


  template<> __forceinline int extract<0>( const vint4& b ) { return _mm_cvtsi128_si32(b); }

  __forceinline int toScalar(const vint4& a) { return _mm_cvtsi128_si32(a); }

  ////////////////////////////////////////////////////////////////////////////////
  /// Reductions
  ////////////////////////////////////////////////////////////////////////////////

#if defined(__SSE4_1__)
  __forceinline const vint4 vreduce_min(const vint4& v) { vint4 h = min(shuffle<1,0,3,2>(v),v); return min(shuffle<2,3,0,1>(h),h); }
  __forceinline const vint4 vreduce_max(const vint4& v) { vint4 h = max(shuffle<1,0,3,2>(v),v); return max(shuffle<2,3,0,1>(h),h); }
  __forceinline const vint4 vreduce_add(const vint4& v) { vint4 h = shuffle<1,0,3,2>(v)   + v ; return shuffle<2,3,0,1>(h)   + h ; }

  __forceinline int reduce_min(const vint4& v) { return toScalar(vreduce_min(v)); }
  __forceinline int reduce_max(const vint4& v) { return toScalar(vreduce_max(v)); }
  __forceinline int reduce_add(const vint4& v) { return toScalar(vreduce_add(v)); }

  __forceinline size_t select_min(const vint4& v) { return __bsf(movemask(v == vreduce_min(v))); }
  __forceinline size_t select_max(const vint4& v) { return __bsf(movemask(v == vreduce_max(v))); }

  __forceinline size_t select_min(const vboolf4& valid, const vint4& v) { const vint4 a = select(valid,v,vint4(pos_inf)); return __bsf(movemask(valid & (a == vreduce_min(a)))); }
  __forceinline size_t select_max(const vboolf4& valid, const vint4& v) { const vint4 a = select(valid,v,vint4(neg_inf)); return __bsf(movemask(valid & (a == vreduce_max(a)))); }

#else

  __forceinline int reduce_min(const vint4& v) { return min(v[0],v[1],v[2],v[3]); }
  __forceinline int reduce_max(const vint4& v) { return max(v[0],v[1],v[2],v[3]); }
  __forceinline int reduce_add(const vint4& v) { return v[0]+v[1]+v[2]+v[3]; }

#endif

  ////////////////////////////////////////////////////////////////////////////////
  /// Sorting networks
  ////////////////////////////////////////////////////////////////////////////////

#if defined(__SSE4_1__)
  __forceinline vint4 sortNetwork(const vint4& v)
  {
    const vint4 a0 = v;
    const vint4 b0 = shuffle<1,0,3,2>(a0);
    const vint4 c0 = umin(a0,b0);
    const vint4 d0 = umax(a0,b0);
    const vint4 a1 = select<0x5 /* 0b0101 */>(c0,d0);
    const vint4 b1 = shuffle<2,3,0,1>(a1);
    const vint4 c1 = umin(a1,b1);
    const vint4 d1 = umax(a1,b1);
    const vint4 a2 = select<0x3 /* 0b0011 */>(c1,d1);
    const vint4 b2 = shuffle<0,2,1,3>(a2);
    const vint4 c2 = umin(a2,b2);
    const vint4 d2 = umax(a2,b2);
    const vint4 a3 = select<0x2 /* 0b0010 */>(c2,d2);
    return a3;
  }
#endif

  ////////////////////////////////////////////////////////////////////////////////
  /// Output Operators
  ////////////////////////////////////////////////////////////////////////////////

  inline std::ostream& operator<<(std::ostream& cout, const vint4& a) {
    return cout << "<" << a[0] << ", " << a[1] << ", " << a[2] << ", " << a[3] << ">";
  }
}

