// Copyright (c) 2010 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SIMPLE_IPC_WIRE_TYPES_H_
#define SIMPLE_IPC_WIRE_TYPES_H_

#include "os_includes.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// This header defines the basic c++ types that can be transported via IPC.
// The main class is WireType which both the channel and the message dispatcher know about.
//

namespace ipc {

// These are the types that the IPC knows about. They are divided in four blocks. The first
// two blocks are 'value'-like types and the second two blocks are 'array' like types.
enum {
  TYPE_NONE,
  TYPE_INT32,
  TYPE_UINT32,
  TYPE_LONG32,
  TYPE_ULONG32,
  TYPE_CHAR8,
  TYPE_CHAR16,
  TYPE_VOIDPTR,         // void pointer (can be 32 or 64 bits).
  TYPE_NULLSTRING8,     // like TYPE_STRING8 but its value is NULL.
  TYPE_NULLSTRING16,    // like TYPE_STRING16 but its value is NULL.
  TYPE_NULLBARRAY,      // like TYPE_BARRAY but its value is NULL.

  TYPE_CHAR32,          // not used.
  TYPE_INT64,           // not used.
  TYPE_UINT64,          // not used.
  TYPE_FLOAT32,         // not used.
  TYPE_FLOAT64,         // not used.
  TYPE_LONG64,          // not used.
  TYPE_ULONG64,         // not used.
  TYPE_NULLINT32ARRAY,  // not used.
  TYPE_NULLUINT32ARRAY, // not used.
  TYPE_NULLINT64ARRAY,  // not used.
  TYPE_NULLUINT64ARRAY, // not used.

  TYPE_STRING8,         // 8-bit string any encoding.
  TYPE_STRING16,        // 16-bit string any encoding.
  TYPE_BARRAY,          // counted byte array.

  TYPE_INT32ARRAY,      // not used.
  TYPE_UINT32ARRAY,     // not used.
  TYPE_INT64ARRAY,      // not used.
  TYPE_UINT64ARRAY,     // not used.

  TYPE_LAST
};

// Wrapper for an array of bytes.
struct ByteArray {
  size_t sz_;
  const char* buf_;
  ByteArray(size_t sz, const char* buf) : sz_(sz), buf_(buf) {}
};

// Variant-like structure without the ownership madness.
class MultiType {
 public:
  MultiType(int id) : id_(id) {}
  int Id() const { return id_; }

 protected:
  void SetId(int id) { id_ = id; }

  union {
    int v_int;
    unsigned int v_uint;
    long v_long;
    unsigned long v_ulong;
    char v_char;
    wchar_t v_wchar;
    void* v_pvoid;
  } store;

  mutable IPCString store_str8;
  mutable IPCWString store_str16;

 private:
  int id_;
};

// At its heart, WireType provides three things:
// 1. Implicit constructors from each supported type, this is required for the template magic
//    to work.
// 2. Type safety; the original type is id is stored and each value will be casted via an union
//    thus ensuring that we don't trip over c++ promotion or casting rules.
// 3. Can distinguish between empty strings and NULL strings.
//
class WireType : public MultiType {
 public:
  // Ctors for supported types
  WireType(int v) : MultiType(ipc::TYPE_INT32) { Set(v); }

  WireType(unsigned int v) : MultiType(ipc::TYPE_UINT32) { Set(v); }

  WireType(long v) : MultiType(ipc::TYPE_LONG32) { Set(v); }

  WireType(unsigned long v) : MultiType(ipc::TYPE_ULONG32) { Set(v); }

  WireType(char v) : MultiType(ipc::TYPE_CHAR8) { Set(v); }

  WireType(wchar_t v) : MultiType(ipc::TYPE_CHAR16) { Set(v); }

  WireType(const char* pc) : MultiType(ipc::TYPE_STRING8) { Set(pc); }

  WireType(const wchar_t* pc) : MultiType(ipc::TYPE_STRING16) { Set(pc); }

  WireType(const ByteArray& ba) : MultiType(ipc::TYPE_BARRAY) { Set(ba); }

  WireType(const void* vp) : MultiType(ipc::TYPE_VOIDPTR) { Set(vp); }

  ////////////////////////////////////////////////////////////////////////
  // Getters: these are used by the sending side of the channel.
  //
  void* GetAsBits() const {
    return store.v_pvoid; 
  }

  void GetString8(IPCString* out) const {
    out->swap(store_str8);
  }

  void GetString16(IPCWString* out) const {
    out->swap(store_str16);
  }
  
  bool IsNullArray() const {
    return (store.v_int < 0);
  }

  ///////////////////////////////////////////////////////////////////////////
  // Recoverers: these are used by the receiving side of the channel.
  //
  // These functions throw. Why you ask? well, because the type inference used
  // elsewhere in the code requires that the error checking to be done at type
  // conversion time and there is no way to insert a callback there.
  // This exception does not propagate as it is catched by the callback processor.

  int RecoverInt32() const {
    if (Id() != ipc::TYPE_INT32) throw int(ipc::TYPE_INT32);
    return store.v_int;
  }

  unsigned int RecoverUInt32() const {
    if (Id() != ipc::TYPE_UINT32) throw int(ipc::TYPE_UINT32);
    return store.v_uint;
  }

  unsigned int RecoverLong32() const {
    if (Id() != ipc::TYPE_LONG32) throw int(ipc::TYPE_LONG32);
    return store.v_long;
  }

  unsigned int RecoverULong32() const {
    if (Id() != ipc::TYPE_ULONG32) throw int(ipc::TYPE_ULONG32);
    return store.v_ulong;
  }

  char RecoverChar8() const {
    if (Id() != ipc::TYPE_CHAR8) throw int(ipc::TYPE_CHAR8);
    return store.v_char;
  }

  wchar_t RecoverChar16() const {
    if (Id() != ipc::TYPE_CHAR16) throw int(ipc::TYPE_CHAR16);
    return store.v_wchar;
  }

  void* RecoverVoidPtr() const {
    if (Id() != ipc::TYPE_VOIDPTR) throw int (ipc::TYPE_VOIDPTR);
    return store.v_pvoid;
  }

  const char* RecoverString8() const {
    if (Id() == ipc::TYPE_STRING8) return store_str8.c_str(); 
    else if (Id() == ipc::TYPE_NULLSTRING8) return NULL;
    else throw int(ipc::TYPE_STRING8);
  }
  
  const wchar_t* RecoverString16() const {
    if (Id() == ipc::TYPE_STRING16) return store_str16.c_str(); 
    else if (Id() == ipc::TYPE_NULLSTRING16) return NULL;
    else throw int(ipc::TYPE_STRING16);
  }

  const ByteArray RecoverByteArray() const {
    if (Id() == ipc::TYPE_BARRAY) return ByteArray(store_str8.size(), store_str8.c_str());
    else if (Id() == ipc::TYPE_NULLBARRAY) return ByteArray(0, NULL);
    else throw int(ipc::TYPE_BARRAY);
  }

 private:
  void Set(int v) { store.v_int = v; }
  void Set(unsigned int v) { store.v_uint = v; }
  void Set(long v) { store.v_long = v; }
  void Set(unsigned long v) { store.v_ulong = v; }
  void Set(char v) { store.v_int = 0; store.v_char = v; }
  void Set(wchar_t v) { store.v_int = 0; store.v_wchar = v; }
  void Set(const void* v) { store.v_pvoid = const_cast<void*>(v); }
  
  void Set(const char* pc) { 
    if (!pc) {
      store.v_int = -1;
      SetId(TYPE_NULLSTRING8);
      return;
    }
    store_str8 = pc;
  }

  void Set(const wchar_t* pc) {
    if (!pc) {
      store.v_int = -1;
      SetId(TYPE_NULLSTRING16);
      return;
    }
    store_str16 = pc;
  }

  void Set(const ByteArray& ba) {
    if (!ba.buf_) {
      store.v_int = -1;
      SetId(TYPE_NULLBARRAY);
      return;
    }
    store_str8.assign(ba.buf_, ba.sz_);
  }

};

}  // namespace ipc.

#endif  // SIMPLE_IPC_WIRE_TYPES_H_

