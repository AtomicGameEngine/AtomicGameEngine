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

#include "../sys/platform.h"
#include "../sys/alloc.h"
#include "../sys/barrier.h"
#include "../sys/thread.h"
#include "../sys/mutex.h"
#include "../sys/condition.h"
#include "../sys/ref.h"
#include "../sys/atomic.h"
#include "../math/range.h"

#include <list>

namespace embree
{
  struct TaskScheduler : public RefCount
  {
    ALIGNED_STRUCT;
    friend class Device;

    static const size_t TASK_STACK_SIZE = 2*1024;           //!< task structure stack
    static const size_t CLOSURE_STACK_SIZE = 256*1024;    //!< stack for task closures

    struct Thread;
    
    /*! virtual interface for all tasks */
    struct TaskFunction {
      virtual void execute() = 0;
    };

    /*! builds a task interface from a closure */
    template<typename Closure>
    struct ClosureTaskFunction : public TaskFunction
    {
      Closure closure;
      __forceinline ClosureTaskFunction (const Closure& closure) : closure(closure) {}
      void execute() { closure(); };
    };

    struct __aligned(64) Task 
    {
      /*! states a task can be in */
      enum { DONE, INITIALIZED };

      /*! switch from one state to another */
      __forceinline void switch_state(int from, int to) 
      {
	__memory_barrier();
	bool success = state.compare_exchange_strong(from,to);
	assert(success);
      }

      /*! try to switch from one state to another */
      __forceinline bool try_switch_state(int from, int to) {
	__memory_barrier();
	return state.compare_exchange_strong(from,to);
      }

       /*! increment/decrement dependency counter */
      void add_dependencies(int n) { 
	dependencies+=n; 
      }

      /*! initialize all tasks to DONE state by default */
      __forceinline Task()
	: state(DONE) {} 

      /*! construction of new task */
      __forceinline Task (TaskFunction* closure, Task* parent, size_t stackPtr, size_t N) 
        : dependencies(1), stealable(true), closure(closure), parent(parent), stackPtr(stackPtr), N(N)
      {
        if (parent) parent->add_dependencies(+1);
	switch_state(DONE,INITIALIZED);
      }

      /*! construction of stolen task, stealing thread will decrement initial dependency */
      __forceinline Task (TaskFunction* closure, Task* parent) 
        : dependencies(1), stealable(false), closure(closure), parent(parent), stackPtr(-1), N(1)
      {
	switch_state(DONE,INITIALIZED);
      }

      /*! try to steal this task */
      bool try_steal(Task& child)
      {
        if (!stealable) return false;
	if (!try_switch_state(INITIALIZED,DONE)) return false;
	new (&child) Task(closure, this);
        return true;
      } 
      
      /*! run this task */
      __dllexport void run(Thread& thread);

    public:
      std::atomic<int> state;            //!< state this task is in
      std::atomic<int> dependencies;     //!< dependencies to wait for
      std::atomic<bool> stealable;       //!< true if task can be stolen
      TaskFunction* closure;             //!< the closure to execute
      Task* parent;                      //!< parent task to signal when we are finished
      size_t stackPtr;                   //!< stack location where closure is stored
      size_t N;                          //!< approximative size of task
    };

    struct TaskQueue
    {
      TaskQueue ()
      : left(0), right(0), stackPtr(0) {}
      
      __forceinline void* alloc(size_t bytes, size_t align = 64) {
        stackPtr += bytes + ((align - stackPtr) & (align-1));
        assert(stackPtr <= CLOSURE_STACK_SIZE);
        return &stack[stackPtr-bytes];
      }
      
      template<typename Closure>
      __forceinline void push_right(Thread& thread, const size_t size, const Closure& closure) 
      {
        assert(right < TASK_STACK_SIZE);
        
	/* allocate new task on right side of stack */
        size_t oldStackPtr = stackPtr;
        TaskFunction* func = new (alloc(sizeof(ClosureTaskFunction<Closure>))) ClosureTaskFunction<Closure>(closure);
        new (&tasks[right++]) Task(func,thread.task,oldStackPtr,size);

	/* also move left pointer */
	if (left >= right-1) left = right-1;
      }
      
      __dllexport bool execute_local(Thread& thread, Task* parent);
      bool steal(Thread& thread);
      size_t getTaskSizeAtLeft();

      bool empty() { return right == 0; }

    public:

      /* task stack */
      Task tasks[TASK_STACK_SIZE];
      __aligned(64) std::atomic<size_t> left;   //!< threads steal from left
      __aligned(64) std::atomic<size_t> right;  //!< new tasks are added to the right
      
      /* closure stack */
      __aligned(64) char stack[CLOSURE_STACK_SIZE];
      size_t stackPtr;
    };
    
    /*! thread local structure for each thread */
    struct Thread 
    {
      ALIGNED_STRUCT;

      Thread (size_t threadIndex, const Ref<TaskScheduler>& scheduler)
      : threadIndex(threadIndex), task(nullptr), scheduler(scheduler) {}

      __forceinline size_t threadCount() {
        return scheduler->threadCounter;
      }
      
      size_t threadIndex;              //!< ID of this thread
      TaskQueue tasks;                 //!< local task queue
      Task* task;                      //!< current active task
      Ref<TaskScheduler> scheduler;     //!< pointer to task scheduler
    };

    /*! pool of worker threads */
    struct ThreadPool
    {
      ThreadPool (bool set_affinity);
      ~ThreadPool ();

      /*! starts the threads */
      __dllexport void startThreads();

      /*! sets number of threads to use */
      void setNumThreads(size_t numThreads, bool startThreads = false);

      /*! adds a task scheduler object for scheduling */
      __dllexport void add(const Ref<TaskScheduler>& scheduler);

      /*! remove the task scheduler object again */
      __dllexport void remove(const Ref<TaskScheduler>& scheduler);

      /*! returns number of threads of the thread pool */
      size_t size() const { return numThreads; }

      /*! main loop for all threads */
      void thread_loop(size_t threadIndex);
      
    private:
      std::atomic<size_t> numThreads;
      std::atomic<size_t> numThreadsRunning;
      bool set_affinity;
      std::atomic<bool> running;
      std::vector<thread_t> threads;

    private:
      MutexSys mutex;
      ConditionSys condition;
      std::list<Ref<TaskScheduler> > schedulers;
    };

    TaskScheduler ();
    ~TaskScheduler ();

    /*! initializes the task scheduler */
    static void create(size_t numThreads, bool set_affinity, bool start_threads);

    /*! destroys the task scheduler again */
    static void destroy();
    
    /*! lets new worker threads join the tasking system */
    void join();
    void reset();

    /*! let a worker thread allocate a thread index */
    __dllexport ssize_t allocThreadIndex();

    /*! wait for some number of threads available (threadCount includes main thread) */
    void wait_for_threads(size_t threadCount);

    /*! thread loop for all worker threads */
    std::exception_ptr thread_loop(size_t threadIndex);

    /*! steals a task from a different thread */
    bool steal_from_other_threads(Thread& thread);

    template<typename Predicate, typename Body>
      static void steal_loop(Thread& thread, const Predicate& pred, const Body& body);

    /* spawn a new task at the top of the threads task stack */
    template<typename Closure>
      void spawn_root(const Closure& closure, size_t size = 1, bool useThreadPool = true) 
    {
      if (useThreadPool) startThreads();
      
      size_t threadIndex = allocThreadIndex();
      std::unique_ptr<Thread> mthread(new Thread(threadIndex,this)); // too large for stack allocation
      Thread& thread = *mthread;
      assert(threadLocal[threadIndex].load() == nullptr);
      threadLocal[threadIndex] = &thread;
      Thread* oldThread = swapThread(&thread);
      thread.tasks.push_right(thread,size,closure);
      {
        Lock<MutexSys> lock(mutex);
	anyTasksRunning++;
        hasRootTask = true;
        condition.notify_all();
      }
      
      if (useThreadPool) addScheduler(this);

      while (thread.tasks.execute_local(thread,nullptr));
      anyTasksRunning--;
      if (useThreadPool) removeScheduler(this);
      
      threadLocal[threadIndex] = nullptr;
      swapThread(oldThread);

      /* remember exception to throw */
      std::exception_ptr except = nullptr;
      if (cancellingException != nullptr) except = cancellingException;

      /* wait for all threads to terminate */
      threadCounter--;
      while (threadCounter > 0) yield();
      cancellingException = nullptr;

      /* re-throw proper exception */
      if (except != nullptr) 
        std::rethrow_exception(except);
    }

    /* spawn a new task at the top of the threads task stack */
    template<typename Closure>
    static __forceinline void spawn(size_t size, const Closure& closure) 
    {
      Thread* thread = TaskScheduler::thread();
      if (likely(thread != nullptr)) thread->tasks.push_right(*thread,size,closure);
      else                           instance()->spawn_root(closure,size);
    }

    /* spawn a new task at the top of the threads task stack */
    template<typename Closure>
    static __forceinline void spawn(const Closure& closure) {
      spawn(1,closure);
    }

    /* spawn a new task set  */
    template<typename Index, typename Closure>
    static void spawn(const Index begin, const Index end, const Index blockSize, const Closure& closure) 
    {
      spawn(end-begin, [=,&closure]() 
        {
	  if (end-begin <= blockSize) {
	    return closure(range<Index>(begin,end));
	  }
	  const Index center = (begin+end)/2;
	  spawn(begin,center,blockSize,closure);
	  spawn(center,end  ,blockSize,closure);
	  wait();
	});
    }

    /* work on spawned subtasks and wait until all have finished */
    __dllexport static bool wait();

    /* returns the index of the current thread */
    __dllexport static size_t threadIndex();

    /* returns the total number of threads */
    __dllexport static size_t threadCount();

  private:

    /* returns the thread local task list of this worker thread */
    __dllexport static Thread* thread();

    /* sets the thread local task list of this worker thread */
    __dllexport static Thread* swapThread(Thread* thread);

    /*! returns the taskscheduler object to be used by the master thread */
    __dllexport static TaskScheduler* instance();

    /*! starts the threads */
    __dllexport static void startThreads();

    /*! adds a task scheduler object for scheduling */
    __dllexport static void addScheduler(const Ref<TaskScheduler>& scheduler);

    /*! remove the task scheduler object again */
    __dllexport static void removeScheduler(const Ref<TaskScheduler>& scheduler);

  private:
    std::vector<atomic<Thread*>> threadLocal;
    std::atomic<size_t> threadCounter;
    std::atomic<size_t> anyTasksRunning;
    std::atomic<bool> hasRootTask;
    std::exception_ptr cancellingException;
    MutexSys mutex;
    ConditionSys condition;

  private:
    static size_t g_numThreads;
    static __thread TaskScheduler* g_instance;
    static __thread Thread* thread_local_thread;
    static ThreadPool* threadPool;
  };
};

