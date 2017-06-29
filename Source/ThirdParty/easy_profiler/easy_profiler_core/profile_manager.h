/**
Lightweight profiler library for c++
Copyright(C) 2016-2017  Sergey Yagovtsev, Victor Zarubkin

Licensed under either of
	* MIT license (LICENSE.MIT or http://opensource.org/licenses/MIT)
    * Apache License, Version 2.0, (LICENSE.APACHE or http://www.apache.org/licenses/LICENSE-2.0)
at your option.

The MIT License
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights 
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
	of the Software, and to permit persons to whom the Software is furnished 
	to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all 
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
	LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE 
	USE OR OTHER DEALINGS IN THE SOFTWARE.


The Apache License, Version 2.0 (the "License");
	You may not use this file except in compliance with the License.
	You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.

**/

#ifndef EASY_PROFILER_MANAGER_H
#define EASY_PROFILER_MANAGER_H

#include <easy/profiler.h>
#include <easy/easy_socket.h>

#include "spin_lock.h"
#include "outstream.h"
#include "hashed_cstr.h"

#include <map>
#include <vector>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <list>

//////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#include <Windows.h>
#elif defined(__APPLE__)
#include <pthread.h>
#include <Availability.h>
#else
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <chrono>
#include <time.h>
#endif

#ifdef max
#undef max
#endif

inline profiler::thread_id_t getCurrentThreadId()
{
#ifdef _WIN32
    return (profiler::thread_id_t)::GetCurrentThreadId();
#elif defined(__APPLE__)
#   if (defined(__MAC_OS_X_VERSION_MIN_REQUIRED) && __MAC_OS_X_VERSION_MIN_REQUIRED >= __MAC_10_6) || \
       (defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_8_0)
        EASY_THREAD_LOCAL static uint64_t _id = 0;
        if (!_id)
            pthread_threadid_np(NULL, &_id);
        return (profiler::thread_id_t)_id;
#   else
        return (profiler::thread_id_t)pthread_self();
#   endif
#else
    EASY_THREAD_LOCAL static const profiler::thread_id_t _id = (profiler::thread_id_t)syscall(__NR_gettid);
    return _id;
#endif
}

namespace profiler {
    
    class SerializedBlock;

    struct do_not_calc_hash {
        template <class T> inline size_t operator()(T _value) const {
            return static_cast<size_t>(_value);
        }
    };

}

//////////////////////////////////////////////////////////////////////////

#ifndef EASY_ENABLE_BLOCK_STATUS
# define EASY_ENABLE_BLOCK_STATUS 1
#endif

#ifndef EASY_ENABLE_ALIGNMENT
# define EASY_ENABLE_ALIGNMENT 0
#endif

#ifndef EASY_ALIGNMENT_SIZE
# define EASY_ALIGNMENT_SIZE 64
#endif


#if EASY_ENABLE_ALIGNMENT == 0
# define EASY_ALIGNED(TYPE, VAR, A) TYPE VAR
# define EASY_MALLOC(MEMSIZE, A) malloc(MEMSIZE)
# define EASY_FREE(MEMPTR) free(MEMPTR)
#else
# if defined(_MSC_VER)
#  define EASY_ALIGNED(TYPE, VAR, A) __declspec(align(A)) TYPE VAR
#  define EASY_MALLOC(MEMSIZE, A) _aligned_malloc(MEMSIZE, A)
#  define EASY_FREE(MEMPTR) _aligned_free(MEMPTR)
# elif defined(__GNUC__)
#  define EASY_ALIGNED(TYPE, VAR, A) TYPE VAR __attribute__(aligned(A))
# else
#  define EASY_ALIGNED(TYPE, VAR, A) TYPE VAR
# endif
#endif

template <const uint16_t N>
class chunk_allocator
{
    struct chunk { EASY_ALIGNED(int8_t, data[N], EASY_ALIGNMENT_SIZE); chunk* prev = nullptr; };

    struct chunk_list
    {
        chunk* last = nullptr;

        ~chunk_list()
        {
            clear();
        }

        void clear()
        {
            do {
                auto p = last;
                last = last->prev;
                EASY_FREE(p);
            } while (last != nullptr);
        }

        chunk& back()
        {
            return *last;
        }

        void emplace_back()
        {
            auto prev = last;
            last = ::new (EASY_MALLOC(sizeof(chunk), EASY_ALIGNMENT_SIZE)) chunk();
            last->prev = prev;
            *(uint16_t*)last->data = 0;
        }

        /** Invert current chunks list to enable to iterate over chunks list in direct order.

        This method is used by serialize().
        */
        void invert()
        {
            chunk* next = nullptr;

            while (last->prev != nullptr) {
                auto p = last->prev;
                last->prev = next;
                next = last;
                last = p;
            }

            last->prev = next;
        }
    };

    //typedef std::list<chunk> chunk_list;

    chunk_list m_chunks;
    uint32_t     m_size;
    uint16_t    m_shift;

public:

    chunk_allocator() : m_size(0), m_shift(0)
    {
        m_chunks.emplace_back();
    }

    /** Allocate n bytes.

    Automatically checks if there is enough preserved memory to store additional n bytes
    and allocates additional buffer if needed.
    */
    void* allocate(uint16_t n)
    {
        ++m_size;

        if (!need_expand(n))
        {
            int8_t* data = m_chunks.back().data + m_shift;
            m_shift += n + sizeof(uint16_t);

            *(uint16_t*)data = n;
            data = data + sizeof(uint16_t);

            if (m_shift + 1 < N)
                *(uint16_t*)(data + n) = 0;

            return data;
        }

        m_shift = n + sizeof(uint16_t);
        m_chunks.emplace_back();
        auto data = m_chunks.back().data;

        *(uint16_t*)data = n;
        data = data + sizeof(uint16_t);

        *(uint16_t*)(data + n) = 0;
        return data;
    }

    /** Check if current storage is not enough to store additional n bytes.
    */
    inline bool need_expand(uint16_t n) const
    {
        return (m_shift + n + sizeof(uint16_t)) > N;
    }

    inline uint32_t size() const
    {
        return m_size;
    }

    inline bool empty() const
    {
        return m_size == 0;
    }

    void clear()
    {
        m_size = 0;
        m_shift = 0;
        m_chunks.clear();
        m_chunks.emplace_back();
    }

    /** Serialize data to stream.

    \warning Data will be cleared after serialization.
    */
    void serialize(profiler::OStream& _outputStream)
    {
        // Chunks are stored in reversed order (stack).
        // To be able to iterate them in direct order we have to invert chunks list.
        m_chunks.invert();

        // Iterate over chunks and perform blocks serialization
        auto current = m_chunks.last;
        do {
            const int8_t* data = current->data;
            uint16_t i = 0;
            while (i + 1 < N && *(uint16_t*)data != 0) {
                const uint16_t size = sizeof(uint16_t) + *(uint16_t*)data;
                _outputStream.write((const char*)data, size);
                data = data + size;
                i += size;
            }
            current = current->prev;
        } while (current != nullptr);

        clear();
    }

}; // END of class chunk_allocator.

//////////////////////////////////////////////////////////////////////////

class NonscopedBlock : public profiler::Block
{
    char* m_runtimeName; ///< a copy of _runtimeName to make it safe to begin block in one function and end it in another

    NonscopedBlock() = delete;
    NonscopedBlock(const NonscopedBlock&) = delete;
    NonscopedBlock(NonscopedBlock&&) = delete;
    NonscopedBlock& operator = (const NonscopedBlock&) = delete;
    NonscopedBlock& operator = (NonscopedBlock&&) = delete;

public:

    NonscopedBlock(const profiler::BaseBlockDescriptor* _desc, const char* _runtimeName, bool = false);
    ~NonscopedBlock();

    /** Copy string from m_name to m_runtimeName to make it safe to end block in another function.

    Performs any work if block is ON and m_name != ""
    */
    void copyname();

    void destroy();

}; // END of class NonscopedBlock.

//////////////////////////////////////////////////////////////////////////

template <class T>
inline void destroy_elem(T*)
{

}

inline void destroy_elem(NonscopedBlock* _elem)
{
    _elem->destroy();
}

template <class T>
class StackBuffer
{
    struct chunk { int8_t data[sizeof(T)]; };

    std::list<chunk> m_overflow; ///< List of additional stack elements if current capacity of buffer is not enough
    T*                 m_buffer; ///< Contiguous buffer used for stack
    uint32_t             m_size; ///< Current size of stack
    uint32_t         m_capacity; ///< Current capacity of m_buffer
    uint32_t      m_maxcapacity; ///< Maximum used capacity including m_buffer and m_overflow

public:

    StackBuffer(uint32_t N) : m_buffer(static_cast<T*>(malloc(N * sizeof(T)))), m_size(0), m_capacity(N), m_maxcapacity(N)
    {
    }

    ~StackBuffer()
    {
        for (uint32_t i = 0; i < m_size; ++i)
            destroy_elem(m_buffer + i);

        free(m_buffer);

        for (auto& elem : m_overflow)
            destroy_elem(reinterpret_cast<T*>(elem.data + 0));
    }

    template <class ... TArgs>
    T& push(TArgs ... _args)
    {
        if (m_size < m_capacity)
            return *(::new (m_buffer + m_size++) T(_args...));

        m_overflow.emplace_back();
        const uint32_t cap = m_capacity + static_cast<uint32_t>(m_overflow.size());
        if (m_maxcapacity < cap)
            m_maxcapacity = cap;

        return *(::new (m_overflow.back().data + 0) T(_args...));
    }

    void pop()
    {
        if (m_overflow.empty())
        {
            // m_size should not be equal to 0 here because ProfileManager behavior does not allow such situation
            destroy_elem(m_buffer + --m_size);

            if (m_size == 0 && m_maxcapacity > m_capacity)
            {
                // When stack gone empty we can resize buffer to use enough space in the future
                free(m_buffer);
                m_maxcapacity = m_capacity = std::max(m_maxcapacity, m_capacity << 1);
                m_buffer = static_cast<T*>(malloc(m_capacity * sizeof(T)));
            }

            return;
        }

        destroy_elem(reinterpret_cast<T*>(m_overflow.back().data + 0));
        m_overflow.pop_back();
    }

}; // END of class StackBuffer.

//////////////////////////////////////////////////////////////////////////

template <class T, const uint16_t N>
struct BlocksList
{
    BlocksList() = default;

    std::vector<T>            openedList;
    chunk_allocator<N>        closedList;
    uint64_t          usedMemorySize = 0;

    void clearClosed() {
        //closedList.clear();
        usedMemorySize = 0;
    }

}; // END of struct BlocksList.

//////////////////////////////////////////////////////////////////////////

class CSwitchBlock : public profiler::CSwitchEvent
{
    const char* m_name;

public:

    CSwitchBlock(profiler::timestamp_t _begin_time, profiler::thread_id_t _tid, const char* _runtimeName);
    inline const char* name() const { return m_name; }
};

//////////////////////////////////////////////////////////////////////////

const uint16_t SIZEOF_BLOCK = sizeof(profiler::BaseBlockData) + 1 + sizeof(uint16_t); // SerializedBlock stores BaseBlockData + at least 1 character for name ('\0') + 2 bytes for size of serialized data
const uint16_t SIZEOF_CSWITCH = sizeof(profiler::CSwitchEvent) + 1 + sizeof(uint16_t); // SerializedCSwitch also stores additional 4 bytes to be able to save 64-bit thread_id

struct ThreadStorage
{
    StackBuffer<NonscopedBlock>                                                 nonscopedBlocks;
    BlocksList<std::reference_wrapper<profiler::Block>, SIZEOF_BLOCK * (uint16_t)128U>   blocks;
    BlocksList<CSwitchBlock, SIZEOF_CSWITCH * (uint16_t)128U>                              sync;

    std::string name; ///< Thread name

#ifndef _WIN32
    const pthread_t pthread_id; ///< Thread pointer
#endif

    const profiler::thread_id_t id; ///< Thread ID
    std::atomic<char>      expired; ///< Is thread expired
    std::atomic_bool         frame; ///< Is new frame opened
    bool             allowChildren; ///< False if one of previously opened blocks has OFF_RECURSIVE or ON_WITHOUT_CHILDREN status
    bool                     named; ///< True if thread name was set
    bool                   guarded; ///< True if thread has been registered using ThreadGuard

    void storeBlock(const profiler::Block& _block);
    void storeCSwitch(const CSwitchBlock& _block);
    void clearClosed();
    void popSilent();

    ThreadStorage();

}; // END of struct ThreadStorage.

//////////////////////////////////////////////////////////////////////////

typedef uint64_t processid_t;

class BlockDescriptor;

class ProfileManager
{
#ifndef EASY_MAGIC_STATIC_CPP11
    friend class ProfileManagerInstance;
#endif

    ProfileManager();
    ProfileManager(const ProfileManager& p) = delete;
    ProfileManager& operator=(const ProfileManager&) = delete;

    typedef profiler::guard_lock<profiler::spin_lock> guard_lock_t;
    typedef std::map<profiler::thread_id_t, ThreadStorage> map_of_threads_stacks;
    typedef std::vector<BlockDescriptor*> block_descriptors_t;

#ifdef EASY_PROFILER_HASHED_CSTR_DEFINED
    typedef std::unordered_map<profiler::hashed_cstr, profiler::block_id_t> descriptors_map_t;
#else
    typedef std::unordered_map<profiler::hashed_stdstring, profiler::block_id_t> descriptors_map_t;
#endif

    const processid_t                     m_processId;

    map_of_threads_stacks                   m_threads;
    block_descriptors_t                 m_descriptors;
    descriptors_map_t                m_descriptorsMap;
    uint64_t                         m_usedMemorySize;
    profiler::timestamp_t                 m_beginTime;
    profiler::timestamp_t                   m_endTime;
    std::atomic<profiler::timestamp_t>     m_frameMax;
    std::atomic<profiler::timestamp_t>     m_frameAvg;
    std::atomic<profiler::timestamp_t>     m_frameCur;
    profiler::spin_lock                        m_spin;
    profiler::spin_lock                  m_storedSpin;
    profiler::spin_lock                    m_dumpSpin;
    std::atomic<profiler::thread_id_t> m_mainThreadId;
    std::atomic<char>                m_profilerStatus;
    std::atomic_bool          m_isEventTracingEnabled;
    std::atomic_bool             m_isAlreadyListening;
    std::atomic_bool                  m_frameMaxReset;
    std::atomic_bool                  m_frameAvgReset;

    std::string m_csInfoFilename = "/tmp/cs_profiling_info.log";

    uint32_t dumpBlocksToStream(profiler::OStream& _outputStream, bool _lockSpin);
    void setBlockStatus(profiler::block_id_t _id, profiler::EasyBlockStatus _status);

    std::thread m_listenThread;
    void listen(uint16_t _port);

    std::atomic_bool m_stopListen;

public:

    static ProfileManager& instance();
    ~ProfileManager();

    const profiler::BaseBlockDescriptor* addBlockDescriptor(profiler::EasyBlockStatus _defaultStatus,
                                                            const char* _autogenUniqueId,
                                                            const char* _name,
                                                            const char* _filename,
                                                            int _line,
                                                            profiler::block_type_t _block_type,
                                                            profiler::color_t _color,
                                                            bool _copyName = false);

    bool storeBlock(const profiler::BaseBlockDescriptor* _desc, const char* _runtimeName);
    bool storeBlock(const profiler::BaseBlockDescriptor* _desc, const char* _runtimeName, profiler::timestamp_t _beginTime, profiler::timestamp_t _endTime);
    void beginBlock(profiler::Block& _block);
    void beginNonScopedBlock(const profiler::BaseBlockDescriptor* _desc, const char* _runtimeName);
    void endBlock();
    profiler::timestamp_t maxFrameDuration();
    profiler::timestamp_t avgFrameDuration();
    profiler::timestamp_t curFrameDuration() const;
    void setEnabled(bool isEnable);
    bool isEnabled() const;
    void setEventTracingEnabled(bool _isEnable);
    bool isEventTracingEnabled() const;
    uint32_t dumpBlocksToFile(const char* filename);
    const char* registerThread(const char* name, profiler::ThreadGuard& threadGuard);
    const char* registerThread(const char* name);

    void setContextSwitchLogFilename(const char* name)
    {
        m_csInfoFilename = name;
    }

    const char* getContextSwitchLogFilename() const
    {
        return m_csInfoFilename.c_str();
    }

    void beginContextSwitch(profiler::thread_id_t _thread_id, profiler::timestamp_t _time, profiler::thread_id_t _target_thread_id, const char* _target_process, bool _lockSpin = true);
    void endContextSwitch(profiler::thread_id_t _thread_id, processid_t _process_id, profiler::timestamp_t _endtime, bool _lockSpin = true);
    void startListen(uint16_t _port);
    void stopListen();
    bool isListening() const;

private:

    void beginFrame();
    void endFrame();

    void enableEventTracer();
    void disableEventTracer();

    char checkThreadExpired(ThreadStorage& _registeredThread);

    void storeBlockForce(const profiler::BaseBlockDescriptor* _desc, const char* _runtimeName, ::profiler::timestamp_t& _timestamp);
    void storeBlockForce2(const profiler::BaseBlockDescriptor* _desc, const char* _runtimeName, ::profiler::timestamp_t _timestamp);
    void storeBlockForce2(ThreadStorage& _registeredThread, const profiler::BaseBlockDescriptor* _desc, const char* _runtimeName, ::profiler::timestamp_t _timestamp);

    ThreadStorage& _threadStorage(profiler::thread_id_t _thread_id);
    ThreadStorage* _findThreadStorage(profiler::thread_id_t _thread_id);

    inline ThreadStorage& threadStorage(profiler::thread_id_t _thread_id)
    {
        guard_lock_t lock(m_spin);
        return _threadStorage(_thread_id);
    }

    inline ThreadStorage* findThreadStorage(profiler::thread_id_t _thread_id)
    {
        guard_lock_t lock(m_spin);
        return _findThreadStorage(_thread_id);
    }

}; // END of class ProfileManager.

//////////////////////////////////////////////////////////////////////////

#endif // EASY_PROFILER_MANAGER_H
