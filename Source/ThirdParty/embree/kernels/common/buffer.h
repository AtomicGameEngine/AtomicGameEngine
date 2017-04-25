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

#include "default.h"

namespace embree
{
  /*! Implements a reference to a data buffer, this class does not own the buffer content. */
  class BufferRef
  {
  public:

    /*! Buffer construction */
    BufferRef (size_t num = 0, size_t stride = 0)
      : ptr_ofs(nullptr), stride(stride), num(num) {}

  public:
    
    /*! sets shared buffer */
    void set(char* ptr_ofs_in, size_t stride_in) {
      ptr_ofs = ptr_ofs_in;
      stride = stride_in;
    }

    /*! returns pointer to first element */
    __forceinline const char* getPtr( size_t i = 0 ) const {
      assert(i<num);
      return ptr_ofs + i*stride;
    }

    /*! returns the number of elements of the buffer */
    __forceinline size_t size() const { 
      return num; 
    }

    /*! returns the number of bytes of the buffer */
    __forceinline size_t bytes() const { 
      return num*stride; 
    }
    
    /*! returns buffer stride */
    __forceinline size_t getStride() const {
      return stride;
    }

  protected:
    char* ptr_ofs;   //!< base pointer plus offset
    size_t stride;   //!< stride of the stream in bytes
    size_t num;      //!< number of elements in the stream
  };

  /*! Implements a typed data stream from a data buffer reference. */
  template<typename T>
    class BufferRefT : public BufferRef
  {
  public:

    typedef T value_type;

    BufferRefT (size_t num = 0, size_t stride = 0) 
      : BufferRef(num,stride) {}

    /*! access to the ith element of the buffer stream */
    __forceinline       T& operator[](size_t i)       { assert(i<num); return *(T*)(ptr_ofs + i*stride); }
    __forceinline const T& operator[](size_t i) const { assert(i<num); return *(T*)(ptr_ofs + i*stride); }
  };

  /*! Implements a typed data stream from a data buffer reference. */
  template<>
    class BufferRefT<Vec3fa> : public BufferRef
  {
  public:

    typedef Vec3fa value_type;

    BufferRefT (size_t num = 0, size_t stride = 0) 
      : BufferRef(num,stride) {}

    /*! access to the ith element of the buffer stream */
    __forceinline const Vec3fa operator[](size_t i) const
    {
      assert(i<num);
      return Vec3fa(vfloat4::loadu((float*)(ptr_ofs + i*stride)));
    }
    
    /*! writes the i'th element */
    __forceinline void store(size_t i, const Vec3fa& v)
    {
      assert(i<num);
      vfloat4::storeu((float*)(ptr_ofs + i*stride),(vfloat4)v);
    }
  };

  /*! Implements an API data buffer object. This class may or may not own the data. */
  template<typename T>
    class APIBuffer : public BufferRefT<T>
  {
  public:

    /*! Buffer construction */
    APIBuffer () 
      : device(nullptr), ptr(nullptr), allocated(false), shared(false), mapped(false), modified(true), userdata(0) {}
    
    APIBuffer (const BufferRefT<T>& other) 
      : BufferRefT<T>(other), device(nullptr), ptr(nullptr), allocated(false), shared(true), mapped(false), modified(true), userdata(0) {}

    /*! Buffer construction */
    APIBuffer (MemoryMonitorInterface* device, size_t num_in, size_t stride_in, bool allocate = false) 
      : BufferRefT<T>(num_in,stride_in), device(device), ptr(nullptr), allocated(false), shared(false), mapped(false), modified(true), userdata(0) 
    {
      if (allocate) alloc();
    }
    
    /*! Buffer destruction */
    ~APIBuffer () {
      free();
    }
    
    /*! this class is not copyable */
  private:
    APIBuffer (const APIBuffer& other) DELETED; // do not implement
    APIBuffer& operator= (const APIBuffer& other) DELETED; // do not implement
    
    /*! make the class movable */
  public:
    APIBuffer (APIBuffer&& other) : BufferRefT<T>(std::move(other))
    {
      device = other.device;       other.device = nullptr;
      ptr = other.ptr;             other.ptr = nullptr;
      allocated = other.allocated; other.allocated = false;
      shared = other.shared;       other.shared = false;
      mapped = other.mapped;       other.mapped = false;
      modified = other.modified;   other.modified = false;
      userdata = other.userdata;   other.userdata = 0;
    }
    
    APIBuffer& operator= (APIBuffer&& other)
    {
      device = other.device;       other.device = nullptr;
      ptr = other.ptr;             other.ptr = nullptr;
      allocated = other.allocated; other.allocated = false;
      shared = other.shared;       other.shared = false;
      mapped = other.mapped;       other.mapped = false;
      modified = other.modified;   other.modified = false;
      userdata = other.userdata;   other.userdata = 0;
      BufferRefT<T>::operator=(std::move(other));
      return *this;
    }
    
  public:

    /* inits the buffer */
    void init(MemoryMonitorInterface* device_in, size_t num_in, size_t stride_in) 
    {
      device = device_in;
      ptr = nullptr;
      this->ptr_ofs = nullptr;
      this->num = num_in;
      this->stride = stride_in;
      shared = false;
      mapped = false;
      modified = true;
    }

    /*! sets shared buffer */
    void set(void* ptr_in, size_t ofs_in, size_t stride_in, size_t num_in)
    {
      /* report error if buffer is not existing */
      if (!device)
        throw_RTCError(RTC_INVALID_ARGUMENT,"invalid buffer specified");
      
      ptr = (char*) ptr_in;
      if (num_in != (size_t)-1) this->num = num_in;
      shared = true;

      BufferRefT<T>::set(ptr+ofs_in,stride_in);
    }
    
    /*! allocated buffer */
    void alloc() {
      if (device) device->memoryMonitor(this->bytes(),false);
      ptr = this->ptr_ofs = (char*) alignedMalloc(this->bytes());
      allocated = true; // this flag is sticky, such that we do never allocated a buffer again after it was freed
    }
    
    /*! frees the buffer */
    void free()
    {
      if (shared || !ptr) return;
      alignedFree(ptr); 
      if (device) device->memoryMonitor(-ssize_t(this->bytes()),true);
      ptr = nullptr; this->ptr_ofs = nullptr;
    }
    
    /*! maps the buffer */
    void* map(std::atomic<size_t>& cntr)
    {
      /* report error if buffer is not existing */
      if (!device)
        throw_RTCError(RTC_INVALID_ARGUMENT,"invalid buffer specified");
      
      /* report error if buffer is already mapped */
      if (mapped)
        throw_RTCError(RTC_INVALID_OPERATION,"buffer is already mapped");
      
      /* allocate buffer */
      if (!ptr && !shared && !allocated)
        alloc();
      
      /* return mapped buffer */
      cntr++;
      mapped = true;
      return ptr;
    }
    
    /*! unmaps the buffer */
    void unmap(std::atomic<size_t>& cntr)
    {
      /* report error if buffer not mapped */
      if (!mapped)
        throw_RTCError(RTC_INVALID_OPERATION,"buffer is not mapped");
      
      /* unmap buffer */
      cntr--;
      mapped = false;
    }
    
    /*! checks if the buffer is mapped */
    __forceinline bool isMapped() const {
      return mapped; 
    }
    
    /*! mark buffer as modified or unmodified */
    __forceinline void setModified(bool b) {
      modified = b;
    }
    
    /*! mark buffer as modified or unmodified */
    __forceinline bool isModified() const {
      return modified;
    }
    
    /*! returns true of the buffer is not empty */
    __forceinline operator bool() const { 
      return ptr; 
    }
    
    /*! checks padding to 16 byte check, fails hard */
    __forceinline void checkPadding16() const 
    {
      if (BufferRef::size()) 
        volatile int MAYBE_UNUSED w = *((int*)BufferRef::getPtr(BufferRef::size()-1)+3); // FIXME: is failing hard avoidable?
    }

  protected:
    MemoryMonitorInterface* device; //!< device to report memory usage to 
    char* ptr;       //!< pointer to buffer data
    bool allocated;  //!< set if buffer got allocated by us
    bool shared;     //!< set if memory is shared with application
    bool mapped;     //!< set if buffer is mapped
    bool modified;   //!< true if the buffer got modified
  public:
    int userdata;    //!< special data
  };
}
