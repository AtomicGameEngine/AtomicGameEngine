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

#include "platform.h"
#include "mutex.h"
#include "alloc.h"
#include "vector.h"
#include <vector>

namespace embree
{
  /*! type for thread */
  typedef struct opaque_thread_t* thread_t;

  /*! signature of thread start function */
  typedef void (*thread_func)(void*);

  /*! creates a hardware thread running on specific logical thread */
  thread_t createThread(thread_func f, void* arg, size_t stack_size = 0, ssize_t threadID = -1);

  /*! set affinity of the calling thread */
  void setAffinity(ssize_t affinity);

  /*! the thread calling this function gets yielded */
  void yield();

  /*! waits until the given thread has terminated */
  void join(thread_t tid);

  /*! destroy handle of a thread */
  void destroyThread(thread_t tid);

  /*! type for handle to thread local storage */
  typedef struct opaque_tls_t* tls_t;

  /*! creates thread local storage */
  tls_t createTls();

  /*! set the thread local storage pointer */
  void setTls(tls_t tls, void* const ptr);

  /*! return the thread local storage pointer */
  void* getTls(tls_t tls);

  /*! destroys thread local storage identifier */
  void destroyTls(tls_t tls);

  /*! thread local storage implementation that supports arbirary number of thread locals */
  struct ThreadLocalStorage
  {
    struct ThreadLocal
    {
    public:

      ThreadLocal (size_t num) 
      {
        ptrs.resize(num);
        for (size_t i=0; i<num; i++) ptrs[i] = nullptr; 
      }

      __forceinline void init(size_t id)
      {
        if (likely(id < ptrs.size())) {
          ptrs[id] = nullptr;
        }
        else 
        {
          Lock<SpinLock> lock(mutex);
          if (id >= ptrs.size()) ptrs.resize(id+1);
          ptrs[id] = nullptr;
        }
      }

      __forceinline void* get(size_t id)
      {
        Lock<SpinLock> lock(mutex);
        return ptrs[id];
      }

      __forceinline void set(size_t id, void* const ptr) 
      {
        Lock<SpinLock> lock(mutex);
        ptrs[id] = ptr;
      }

    private:
      SpinLock mutex;
      avector<void*> ptrs;
      MAYBE_UNUSED char align[64];
    };

    ThreadLocalStorage() 
      : key(createTls()) {}

    ~ThreadLocalStorage() 
    {
      destroyTls(key);
      for (auto thread : threads)
        delete thread;
    }
    
    size_t create() 
    {
      Lock<SpinLock> lock(mutex);
      size_t id = ids.allocate();
      for (auto thread : threads) thread->init(id);
      return id;
    }

    __forceinline void* get(size_t id) {
      return get_thread_local()->get(id);
    }

    __forceinline void set(size_t id, void* const ptr) {
      get_thread_local()->set(id,ptr);
    }
    
    void destroy(size_t id)
    {
      Lock<SpinLock> lock(mutex);
      ids.deallocate(id);
    }

    static __forceinline ThreadLocalStorage* instance() {
      return &single_instance;
    }
    
  private:
    
    __forceinline ThreadLocal* get_thread_local() 
    {
      ThreadLocal* tls = (ThreadLocal*) getTls(key);
      if (unlikely(tls == nullptr)) {
        Lock<SpinLock> lock(mutex);
        tls = new ThreadLocal(ids.size());
        setTls(key,tls);
        threads.push_back(tls);
      }
      return tls;
    }

  private:
    SpinLock mutex;
    IDPool<size_t> ids;
    tls_t key;
    std::vector<ThreadLocal*> threads;
    static ThreadLocalStorage single_instance;
  };

#if 0

  /*! manages thread local variables */
  template<typename Type, typename InitType>
  struct ThreadLocalData
  {
  public:

    __forceinline ThreadLocalData (InitType init) 
      : ptr(-1), init(init) {}

    __forceinline ~ThreadLocalData () {
      clear();
    }

    __forceinline void clear() 
    {
      if (ptr != -1) ThreadLocalStorage::instance()->destroy(ptr); ptr = -1;
      for (size_t i=0; i<threads.size(); i++)
	delete threads[i];
      threads.clear();
    }

    /*! disallow copy */
    ThreadLocalData(const ThreadLocalData&) DELETED;
    ThreadLocalData& operator=(const ThreadLocalData&) DELETED;

    template<typename Closure>
      __forceinline void apply(const Closure& closure)
    {
      for (size_t i=0; i<threads.size(); i++)
        closure(threads[i]);
    }
    
    __forceinline Type* get() const
    {
      if (ptr == -1) {
	Lock<SpinLock> lock(mutex);
	if (ptr == -1) ptr = ThreadLocalStorage::instance()->create();
      }
      Type* lptr = (Type*) ThreadLocalStorage::instance()->get(ptr);
      if (lptr) return lptr;
      lptr = new Type(init);
      ThreadLocalStorage::instance()->set(ptr,lptr);
      Lock<SpinLock> lock(mutex);
      threads.push_back(lptr);
      return lptr;
    }

    __forceinline const Type& operator  *( void ) const { return *get(); }
    __forceinline       Type& operator  *( void )       { return *get(); }
    __forceinline const Type* operator ->( void ) const { return  get(); }
    __forceinline       Type* operator ->( void )       { return  get(); }
    
    
  private:
    mutable size_t ptr;
    InitType init;
    mutable SpinLock mutex;
  public:
    mutable std::vector<Type*> threads;
  };

#else

  /*! manages thread local variables */
  template<typename Type, typename InitType>
  struct ThreadLocalData
  {
  public:

    __forceinline ThreadLocalData (InitType init) 
      : ptr(nullptr), init(init) {}

    __forceinline ~ThreadLocalData () {
      clear();
    }

    __forceinline void clear() 
    {
      if (ptr) destroyTls(ptr); ptr = nullptr;
      for (size_t i=0; i<threads.size(); i++)
	delete threads[i];
      threads.clear();
    }

    /*! disallow copy */
    ThreadLocalData(const ThreadLocalData&) DELETED;
    ThreadLocalData& operator=(const ThreadLocalData&) DELETED;

    template<typename Closure>
      __forceinline void apply(const Closure& closure)
    {
      for (size_t i=0; i<threads.size(); i++)
        closure(threads[i]);
    }
    
    __forceinline Type* get() const
    {
      if (ptr == nullptr) {
	Lock<SpinLock> lock(mutex);
	if (ptr == nullptr) ptr = createTls();
      }
      Type* lptr = (Type*) getTls(ptr);
      if (lptr) return lptr;
      lptr = new Type(init);
      setTls(ptr,lptr);
      Lock<SpinLock> lock(mutex);
      threads.push_back(lptr);
      return lptr;
    }

    __forceinline const Type& operator  *( void ) const { return *get(); }
    __forceinline       Type& operator  *( void )       { return *get(); }
    __forceinline const Type* operator ->( void ) const { return  get(); }
    __forceinline       Type* operator ->( void )       { return  get(); }
    
    
  private:
    mutable tls_t ptr;
    InitType init;
    mutable SpinLock mutex;
  public:
    mutable std::vector<Type*> threads;
  };

#endif
}
