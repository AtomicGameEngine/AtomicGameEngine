/************************************************************************
* file name         : profile_manager.cpp
* ----------------- :
* creation time     : 2016/02/16
* authors           : Sergey Yagovtsev, Victor Zarubkin
* emails            : yse.sey@gmail.com, v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains implementation of Profile manager and implement access c-function
*                   :
* license           : Lightweight profiler library for c++
*                   : Copyright(C) 2016-2017  Sergey Yagovtsev, Victor Zarubkin
*                   :
*                   : Licensed under either of
*                   :     * MIT license (LICENSE.MIT or http://opensource.org/licenses/MIT)
*                   :     * Apache License, Version 2.0, (LICENSE.APACHE or http://www.apache.org/licenses/LICENSE-2.0)
*                   : at your option.
*                   :
*                   : The MIT License
*                   :
*                   : Permission is hereby granted, free of charge, to any person obtaining a copy
*                   : of this software and associated documentation files (the "Software"), to deal
*                   : in the Software without restriction, including without limitation the rights
*                   : to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
*                   : of the Software, and to permit persons to whom the Software is furnished
*                   : to do so, subject to the following conditions:
*                   :
*                   : The above copyright notice and this permission notice shall be included in all
*                   : copies or substantial portions of the Software.
*                   :
*                   : THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
*                   : INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
*                   : PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
*                   : LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
*                   : TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
*                   : USE OR OTHER DEALINGS IN THE SOFTWARE.
*                   :
*                   : The Apache License, Version 2.0 (the "License")
*                   :
*                   : You may not use this file except in compliance with the License.
*                   : You may obtain a copy of the License at
*                   :
*                   : http://www.apache.org/licenses/LICENSE-2.0
*                   :
*                   : Unless required by applicable law or agreed to in writing, software
*                   : distributed under the License is distributed on an "AS IS" BASIS,
*                   : WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*                   : See the License for the specific language governing permissions and
*                   : limitations under the License.
************************************************************************/

#include <algorithm>
#include <fstream>
#include <assert.h>
#include "profile_manager.h"

#include <easy/serialized_block.h>
#include <easy/easy_net.h>
#include <easy/easy_socket.h>

#include "event_trace_win.h"
#include "current_time.h"

#ifdef __APPLE__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#if EASY_OPTION_LOG_ENABLED != 0
# include <iostream>

# ifndef EASY_ERRORLOG
#  define EASY_ERRORLOG ::std::cerr
# endif

# ifndef EASY_LOG
#  define EASY_LOG ::std::cerr
# endif

# ifndef EASY_ERROR
#  define EASY_ERROR(LOG_MSG) EASY_ERRORLOG << "EasyProfiler ERROR: " << LOG_MSG
# endif

# ifndef EASY_WARNING
#  define EASY_WARNING(LOG_MSG) EASY_ERRORLOG << "EasyProfiler WARNING: " << LOG_MSG
# endif

# ifndef EASY_LOGMSG
#  define EASY_LOGMSG(LOG_MSG) EASY_LOG << "EasyProfiler INFO: " << LOG_MSG
# endif

# ifndef EASY_LOG_ONLY
#  define EASY_LOG_ONLY(CODE) CODE
# endif

#else

# ifndef EASY_ERROR
#  define EASY_ERROR(LOG_MSG)
# endif

# ifndef EASY_WARNING
#  define EASY_WARNING(LOG_MSG)
# endif

# ifndef EASY_LOGMSG
#  define EASY_LOGMSG(LOG_MSG)
# endif

# ifndef EASY_LOG_ONLY
#  define EASY_LOG_ONLY(CODE)
# endif

#endif

#ifdef min
#undef min
#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

using namespace profiler;

//////////////////////////////////////////////////////////////////////////

#if !defined(EASY_PROFILER_VERSION_MAJOR) || !defined(EASY_PROFILER_VERSION_MINOR) || !defined(EASY_PROFILER_VERSION_PATCH)
# ifdef _WIN32
#  error EASY_PROFILER_VERSION_MAJOR and EASY_PROFILER_VERSION_MINOR and EASY_PROFILER_VERSION_PATCH macros must be defined
# else
#  error "EASY_PROFILER_VERSION_MAJOR and EASY_PROFILER_VERSION_MINOR and EASY_PROFILER_VERSION_PATCH macros must be defined"
# endif
#endif

# define EASY_PROFILER_PRODUCT_VERSION "v" EASY_STRINGIFICATION(EASY_PROFILER_VERSION_MAJOR) "." \
                                           EASY_STRINGIFICATION(EASY_PROFILER_VERSION_MINOR) "." \
                                           EASY_STRINGIFICATION(EASY_PROFILER_VERSION_PATCH)

# define EASY_VERSION_INT(v_major, v_minor, v_patch) ((static_cast<uint32_t>(v_major) << 24) | (static_cast<uint32_t>(v_minor) << 16) | static_cast<uint32_t>(v_patch))
extern const uint32_t PROFILER_SIGNATURE = ('E' << 24) | ('a' << 16) | ('s' << 8) | 'y';
extern const uint32_t EASY_CURRENT_VERSION = EASY_VERSION_INT(EASY_PROFILER_VERSION_MAJOR, EASY_PROFILER_VERSION_MINOR, EASY_PROFILER_VERSION_PATCH);
# undef EASY_VERSION_INT

//////////////////////////////////////////////////////////////////////////

# define EASY_PROF_DISABLED 0
# define EASY_PROF_ENABLED 1
# define EASY_PROF_DUMP 2

//////////////////////////////////////////////////////////////////////////

//auto& MANAGER = ProfileManager::instance();
# define MANAGER ProfileManager::instance()
const uint8_t FORCE_ON_FLAG = profiler::FORCE_ON & ~profiler::ON;

#if defined(EASY_CHRONO_CLOCK)
const int64_t CPU_FREQUENCY = EASY_CHRONO_CLOCK::period::den / EASY_CHRONO_CLOCK::period::num;
# define TICKS_TO_US(ticks) ticks * 1000000LL / CPU_FREQUENCY
#elif defined(_WIN32)
const decltype(LARGE_INTEGER::QuadPart) CPU_FREQUENCY = ([](){ LARGE_INTEGER freq; QueryPerformanceFrequency(&freq); return freq.QuadPart; })();
# define TICKS_TO_US(ticks) ticks * 1000000LL / CPU_FREQUENCY
#else
int64_t calculate_cpu_frequency()
{
    double g_TicksPerNanoSec;
    uint64_t begin = 0, end = 0;
#ifdef __APPLE__
    clock_serv_t cclock;
    mach_timespec_t begints, endts;
    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
    clock_get_time(cclock, &begints);
#else
    struct timespec begints, endts;
    clock_gettime(CLOCK_MONOTONIC, &begints);
#endif
    begin = getCurrentTime();
    volatile uint64_t i;
    for (i = 0; i < 100000000; i++); /* must be CPU intensive */
    end = getCurrentTime();
#ifdef __APPLE__
    clock_get_time(cclock, &endts);
    mach_port_deallocate(mach_task_self(), cclock);
#else
    clock_gettime(CLOCK_MONOTONIC, &endts);
#endif
    struct timespec tmpts;
    const int NANO_SECONDS_IN_SEC = 1000000000;
    tmpts.tv_sec = endts.tv_sec - begints.tv_sec;
    tmpts.tv_nsec = endts.tv_nsec - begints.tv_nsec;
    if (tmpts.tv_nsec < 0)
    {
        tmpts.tv_sec--;
        tmpts.tv_nsec += NANO_SECONDS_IN_SEC;
    }

    uint64_t nsecElapsed = tmpts.tv_sec * 1000000000LL + tmpts.tv_nsec;
    g_TicksPerNanoSec = (double)(end - begin) / (double)nsecElapsed;

    int64_t cpu_frequency = int(g_TicksPerNanoSec * 1000000);

    return cpu_frequency;
}

static std::atomic<int64_t> CPU_FREQUENCY = ATOMIC_VAR_INIT(1);
# define TICKS_TO_US(ticks) ticks * 1000 / CPU_FREQUENCY.load(std::memory_order_acquire)
#endif

extern const profiler::color_t EASY_COLOR_INTERNAL_EVENT = 0xffffffff; // profiler::colors::White
const profiler::color_t EASY_COLOR_THREAD_END = 0xff212121; // profiler::colors::Dark
const profiler::color_t EASY_COLOR_START = 0xff4caf50; // profiler::colors::Green
const profiler::color_t EASY_COLOR_END = 0xfff44336; // profiler::colors::Red

//////////////////////////////////////////////////////////////////////////

EASY_THREAD_LOCAL static ::ThreadStorage* THIS_THREAD = nullptr;
EASY_THREAD_LOCAL static int32_t THIS_THREAD_STACK_SIZE = 0;
EASY_THREAD_LOCAL static profiler::timestamp_t THIS_THREAD_FRAME_T = 0ULL;
EASY_THREAD_LOCAL static bool THIS_THREAD_FRAME = false;
EASY_THREAD_LOCAL static bool THIS_THREAD_HALT = false;
EASY_THREAD_LOCAL static bool THIS_THREAD_IS_MAIN = false;

EASY_THREAD_LOCAL static profiler::timestamp_t THIS_THREAD_FRAME_T_MAX = 0ULL;
EASY_THREAD_LOCAL static profiler::timestamp_t THIS_THREAD_FRAME_T_CUR = 0ULL;
EASY_THREAD_LOCAL static profiler::timestamp_t THIS_THREAD_FRAME_T_ACC = 0ULL;
EASY_THREAD_LOCAL static uint32_t THIS_THREAD_N_FRAMES = 0;
EASY_THREAD_LOCAL static bool THIS_THREAD_FRAME_T_RESET_MAX = false;
EASY_THREAD_LOCAL static bool THIS_THREAD_FRAME_T_RESET_AVG = false;

//////////////////////////////////////////////////////////////////////////

#ifdef BUILD_WITH_EASY_PROFILER
# define EASY_EVENT_RES(res, name, ...)\
    EASY_LOCAL_STATIC_PTR(const ::profiler::BaseBlockDescriptor*, EASY_UNIQUE_DESC(__LINE__), MANAGER.addBlockDescriptor(\
        ::profiler::extract_enable_flag(__VA_ARGS__), EASY_UNIQUE_LINE_ID, EASY_COMPILETIME_NAME(name),\
            __FILE__, __LINE__, ::profiler::BLOCK_TYPE_EVENT, ::profiler::extract_color(__VA_ARGS__)));\
    res = MANAGER.storeBlock(EASY_UNIQUE_DESC(__LINE__), EASY_RUNTIME_NAME(name))

# define EASY_FORCE_EVENT(timestamp, name, ...)\
    EASY_LOCAL_STATIC_PTR(const ::profiler::BaseBlockDescriptor*, EASY_UNIQUE_DESC(__LINE__), addBlockDescriptor(\
        ::profiler::extract_enable_flag(__VA_ARGS__), EASY_UNIQUE_LINE_ID, EASY_COMPILETIME_NAME(name),\
            __FILE__, __LINE__, ::profiler::BLOCK_TYPE_EVENT, ::profiler::extract_color(__VA_ARGS__)));\
    storeBlockForce(EASY_UNIQUE_DESC(__LINE__), EASY_RUNTIME_NAME(name), timestamp)

# define EASY_FORCE_EVENT2(timestamp, name, ...)\
    EASY_LOCAL_STATIC_PTR(const ::profiler::BaseBlockDescriptor*, EASY_UNIQUE_DESC(__LINE__), addBlockDescriptor(\
        ::profiler::extract_enable_flag(__VA_ARGS__), EASY_UNIQUE_LINE_ID, EASY_COMPILETIME_NAME(name),\
            __FILE__, __LINE__, ::profiler::BLOCK_TYPE_EVENT, ::profiler::extract_color(__VA_ARGS__)));\
    storeBlockForce2(EASY_UNIQUE_DESC(__LINE__), EASY_RUNTIME_NAME(name), timestamp)

# define EASY_FORCE_EVENT3(ts, timestamp, name, ...)\
    EASY_LOCAL_STATIC_PTR(const ::profiler::BaseBlockDescriptor*, EASY_UNIQUE_DESC(__LINE__), addBlockDescriptor(\
        ::profiler::extract_enable_flag(__VA_ARGS__), EASY_UNIQUE_LINE_ID, EASY_COMPILETIME_NAME(name),\
            __FILE__, __LINE__, ::profiler::BLOCK_TYPE_EVENT, ::profiler::extract_color(__VA_ARGS__)));\
    storeBlockForce2(ts, EASY_UNIQUE_DESC(__LINE__), EASY_RUNTIME_NAME(name), timestamp)
#else
# ifndef EASY_PROFILER_API_DISABLED
#  define EASY_PROFILER_API_DISABLED
# endif
# define EASY_EVENT_RES(res, name, ...)
# define EASY_FORCE_EVENT(timestamp, name, ...)
# define EASY_FORCE_EVENT2(timestamp, name, ...)
# define EASY_FORCE_EVENT3(ts, timestamp, name, ...)
#endif

//////////////////////////////////////////////////////////////////////////

extern "C" {

#if !defined(EASY_PROFILER_API_DISABLED)
    PROFILER_API timestamp_t currentTime()
    {
        return getCurrentTime();
    }

    PROFILER_API timestamp_t toNanoseconds(timestamp_t _ticks)
    {
#if defined(EASY_CHRONO_CLOCK) || defined(_WIN32)
        return _ticks * 1000000000LL / CPU_FREQUENCY;
#else
        return _ticks / CPU_FREQUENCY.load(std::memory_order_acquire);
#endif
    }

    PROFILER_API timestamp_t toMicroseconds(timestamp_t _ticks)
    {
        return TICKS_TO_US(_ticks);
    }

    PROFILER_API const BaseBlockDescriptor* registerDescription(EasyBlockStatus _status, const char* _autogenUniqueId, const char* _name, const char* _filename, int _line, block_type_t _block_type, color_t _color, bool _copyName)
    {
        return MANAGER.addBlockDescriptor(_status, _autogenUniqueId, _name, _filename, _line, _block_type, _color, _copyName);
    }

    PROFILER_API void endBlock()
    {
        MANAGER.endBlock();
    }

    PROFILER_API void setEnabled(bool isEnable)
    {
        MANAGER.setEnabled(isEnable);
    }

    PROFILER_API bool isEnabled()
    {
        return MANAGER.isEnabled();
    }

    PROFILER_API void storeEvent(const BaseBlockDescriptor* _desc, const char* _runtimeName)
    {
        MANAGER.storeBlock(_desc, _runtimeName);
    }

    PROFILER_API void storeBlock(const BaseBlockDescriptor* _desc, const char* _runtimeName, timestamp_t _beginTime, timestamp_t _endTime)
    {
        MANAGER.storeBlock(_desc, _runtimeName, _beginTime, _endTime);
    }

    PROFILER_API void beginBlock(Block& _block)
    {
        MANAGER.beginBlock(_block);
    }

    PROFILER_API void beginNonScopedBlock(const BaseBlockDescriptor* _desc, const char* _runtimeName)
    {
        MANAGER.beginNonScopedBlock(_desc, _runtimeName);
    }

    PROFILER_API uint32_t dumpBlocksToFile(const char* filename)
    {
        return MANAGER.dumpBlocksToFile(filename);
    }

    PROFILER_API const char* registerThreadScoped(const char* name, ThreadGuard& threadGuard)
    {
        return MANAGER.registerThread(name, threadGuard);
    }

    PROFILER_API const char* registerThread(const char* name)
    {
        return MANAGER.registerThread(name);
    }

    PROFILER_API void setEventTracingEnabled(bool _isEnable)
    {
        MANAGER.setEventTracingEnabled(_isEnable);
    }

    PROFILER_API bool isEventTracingEnabled()
    {
        return MANAGER.isEventTracingEnabled();
    }

# ifdef _WIN32
    PROFILER_API void setLowPriorityEventTracing(bool _isLowPriority)
    {
        EasyEventTracer::instance().setLowPriority(_isLowPriority);
    }

    PROFILER_API bool isLowPriorityEventTracing()
    {
        return EasyEventTracer::instance().isLowPriority();
    }
# else
    PROFILER_API void setLowPriorityEventTracing(bool) { }
    PROFILER_API bool isLowPriorityEventTracing() { return false; }
# endif

    PROFILER_API void setContextSwitchLogFilename(const char* name)
    {
        return MANAGER.setContextSwitchLogFilename(name);
    }

    PROFILER_API const char* getContextSwitchLogFilename()
    {
        return MANAGER.getContextSwitchLogFilename();
    }

    PROFILER_API void   startListen(uint16_t _port)
    {
        return MANAGER.startListen(_port);
    }

    PROFILER_API void   stopListen()
    {
        return MANAGER.stopListen();
    }

    PROFILER_API bool isListening()
    {
        return MANAGER.isListening();
    }

    PROFILER_API bool isMainThread()
    {
        return THIS_THREAD_IS_MAIN;
    }

    PROFILER_API timestamp_t this_thread_frameTime(Duration _durationCast)
    {
        if (_durationCast == profiler::TICKS)
            return THIS_THREAD_FRAME_T_CUR;
        return TICKS_TO_US(THIS_THREAD_FRAME_T_CUR);
    }

    PROFILER_API timestamp_t this_thread_frameTimeLocalMax(Duration _durationCast)
    {
        THIS_THREAD_FRAME_T_RESET_MAX = true;
        if (_durationCast == profiler::TICKS)
            return THIS_THREAD_FRAME_T_MAX;
        return TICKS_TO_US(THIS_THREAD_FRAME_T_MAX);
    }

    PROFILER_API timestamp_t this_thread_frameTimeLocalAvg(Duration _durationCast)
    {
        THIS_THREAD_FRAME_T_RESET_AVG = true;
        auto avgDuration = THIS_THREAD_N_FRAMES > 0 ? THIS_THREAD_FRAME_T_ACC / THIS_THREAD_N_FRAMES : 0;
        if (_durationCast == profiler::TICKS)
            return avgDuration;
        return TICKS_TO_US(avgDuration);
    }

    PROFILER_API timestamp_t main_thread_frameTime(Duration _durationCast)
    {
        const auto ticks = THIS_THREAD_IS_MAIN ? THIS_THREAD_FRAME_T_CUR : MANAGER.curFrameDuration();
        if (_durationCast == profiler::TICKS)
            return ticks;
        return TICKS_TO_US(ticks);
    }

    PROFILER_API timestamp_t main_thread_frameTimeLocalMax(Duration _durationCast)
    {
        if (THIS_THREAD_IS_MAIN)
        {
            THIS_THREAD_FRAME_T_RESET_MAX = true;
            if (_durationCast == profiler::TICKS)
                return THIS_THREAD_FRAME_T_MAX;
            return TICKS_TO_US(THIS_THREAD_FRAME_T_MAX);
        }

        if (_durationCast == profiler::TICKS)
            return MANAGER.maxFrameDuration();
        return TICKS_TO_US(MANAGER.maxFrameDuration());
    }

    PROFILER_API timestamp_t main_thread_frameTimeLocalAvg(Duration _durationCast)
    {
        if (THIS_THREAD_IS_MAIN)
        {
            THIS_THREAD_FRAME_T_RESET_AVG = true;
            auto avgDuration = THIS_THREAD_N_FRAMES > 0 ? THIS_THREAD_FRAME_T_ACC / THIS_THREAD_N_FRAMES : 0;
            if (_durationCast == profiler::TICKS)
                return avgDuration;
            return TICKS_TO_US(avgDuration);
        }

        if (_durationCast == profiler::TICKS)
            return MANAGER.avgFrameDuration();
        return TICKS_TO_US(MANAGER.avgFrameDuration());
    }

#else
    PROFILER_API timestamp_t currentTime() { return 0; }
    PROFILER_API timestamp_t toNanoseconds(timestamp_t) { return 0; }
    PROFILER_API timestamp_t toMicroseconds(timestamp_t) { return 0; }
    PROFILER_API const BaseBlockDescriptor* registerDescription(EasyBlockStatus, const char*, const char*, const char*, int, block_type_t, color_t, bool) { return reinterpret_cast<const BaseBlockDescriptor*>(0xbad); }
    PROFILER_API void endBlock() { }
    PROFILER_API void setEnabled(bool) { }
    PROFILER_API bool isEnabled() { return false; }
    PROFILER_API void storeEvent(const BaseBlockDescriptor*, const char*) { }
    PROFILER_API void storeBlock(const BaseBlockDescriptor*, const char*, timestamp_t, timestamp_t) { }
    PROFILER_API void beginBlock(Block&) { }
    PROFILER_API void beginNonScopedBlock(const BaseBlockDescriptor*, const char*) { }
    PROFILER_API uint32_t dumpBlocksToFile(const char*) { return 0; }
    PROFILER_API const char* registerThreadScoped(const char*, ThreadGuard&) { return ""; }
    PROFILER_API const char* registerThread(const char*) { return ""; }
    PROFILER_API void setEventTracingEnabled(bool) { }
    PROFILER_API bool isEventTracingEnabled() { return false; }
    PROFILER_API void setLowPriorityEventTracing(bool) { }
    PROFILER_API bool isLowPriorityEventTracing(bool) { return false; }
    PROFILER_API void setContextSwitchLogFilename(const char*) { }
    PROFILER_API const char* getContextSwitchLogFilename() { return ""; }
    PROFILER_API void startListen(uint16_t) { }
    PROFILER_API void stopListen() { }
    PROFILER_API bool isListening() { return false; }

    PROFILER_API bool isMainThread() { return false; }
    PROFILER_API timestamp_t this_thread_frameTime(Duration) { return 0; }
    PROFILER_API timestamp_t this_thread_frameTimeLocalMax(Duration) { return 0; }
    PROFILER_API timestamp_t this_thread_frameTimeLocalAvg(Duration) { return 0; }
    PROFILER_API timestamp_t main_thread_frameTime(Duration) { return 0; }
    PROFILER_API timestamp_t main_thread_frameTimeLocalMax(Duration) { return 0; }
    PROFILER_API timestamp_t main_thread_frameTimeLocalAvg(Duration) { return 0; }
#endif

    PROFILER_API uint8_t versionMajor()
    {
        static_assert(0 <= EASY_PROFILER_VERSION_MAJOR && EASY_PROFILER_VERSION_MAJOR <= 255, "EASY_PROFILER_VERSION_MAJOR must be defined in range [0, 255]");
        return EASY_PROFILER_VERSION_MAJOR;
    }

    PROFILER_API uint8_t versionMinor()
    {
        static_assert(0 <= EASY_PROFILER_VERSION_MINOR && EASY_PROFILER_VERSION_MINOR <= 255, "EASY_PROFILER_VERSION_MINOR must be defined in range [0, 255]");
        return EASY_PROFILER_VERSION_MINOR;
    }

    PROFILER_API uint16_t versionPatch()
    {
        static_assert(0 <= EASY_PROFILER_VERSION_PATCH && EASY_PROFILER_VERSION_PATCH <= 65535, "EASY_PROFILER_VERSION_PATCH must be defined in range [0, 65535]");
        return EASY_PROFILER_VERSION_PATCH;
    }

    PROFILER_API uint32_t version()
    {
        return EASY_CURRENT_VERSION;
    }

    PROFILER_API const char* versionName()
    {
        return EASY_PROFILER_PRODUCT_VERSION
#ifdef EASY_PROFILER_API_DISABLED
            "_disabled"
#endif
            ;
    }

}

//////////////////////////////////////////////////////////////////////////

SerializedBlock::SerializedBlock(const Block& block, uint16_t name_length)
    : BaseBlockData(block)
{
    auto pName = const_cast<char*>(name());
    if (name_length) strncpy(pName, block.name(), name_length);
    pName[name_length] = 0;
}

SerializedCSwitch::SerializedCSwitch(const CSwitchBlock& block, uint16_t name_length)
    : CSwitchEvent(block)
{
    auto pName = const_cast<char*>(name());
    if (name_length) strncpy(pName, block.name(), name_length);
    pName[name_length] = 0;
}

//////////////////////////////////////////////////////////////////////////

BaseBlockDescriptor::BaseBlockDescriptor(block_id_t _id, EasyBlockStatus _status, int _line, block_type_t _block_type, color_t _color)
    : m_id(_id)
    , m_line(_line)
    , m_type(_block_type)
    , m_color(_color)
    , m_status(_status)
{

}

//////////////////////////////////////////////////////////////////////////

#ifndef EASY_BLOCK_DESC_FULL_COPY
# define EASY_BLOCK_DESC_FULL_COPY 1
#endif

#if EASY_BLOCK_DESC_FULL_COPY == 0
# define EASY_BLOCK_DESC_STRING const char*
# define EASY_BLOCK_DESC_STRING_LEN(s) static_cast<uint16_t>(strlen(s) + 1)
# define EASY_BLOCK_DESC_STRING_VAL(s) s
#else
# define EASY_BLOCK_DESC_STRING std::string
# define EASY_BLOCK_DESC_STRING_LEN(s) static_cast<uint16_t>(s.size() + 1)
# define EASY_BLOCK_DESC_STRING_VAL(s) s.c_str()
#endif

class BlockDescriptor : public BaseBlockDescriptor
{
    friend ProfileManager;

    EASY_BLOCK_DESC_STRING m_filename; ///< Source file name where this block is declared
    EASY_BLOCK_DESC_STRING     m_name; ///< Static name of all blocks of the same type (blocks can have dynamic name) which is, in pair with descriptor id, a unique block identifier

public:

    BlockDescriptor(block_id_t _id, EasyBlockStatus _status, const char* _name, const char* _filename, int _line, block_type_t _block_type, color_t _color)
        : BaseBlockDescriptor(_id, _status, _line, _block_type, _color)
        , m_filename(_filename)
        , m_name(_name)
    {
    }

    const char* name() const {
        return EASY_BLOCK_DESC_STRING_VAL(m_name);
    }

    const char* filename() const {
        return EASY_BLOCK_DESC_STRING_VAL(m_filename);
    }

    uint16_t nameSize() const {
        return EASY_BLOCK_DESC_STRING_LEN(m_name);
    }

    uint16_t filenameSize() const {
        return EASY_BLOCK_DESC_STRING_LEN(m_filename);
    }

}; // END of class BlockDescriptor.

//////////////////////////////////////////////////////////////////////////

NonscopedBlock::NonscopedBlock(const profiler::BaseBlockDescriptor* _desc, const char* _runtimeName, bool)
    : profiler::Block(_desc, _runtimeName, false), m_runtimeName(nullptr)
{

}

NonscopedBlock::~NonscopedBlock()
{
    // Actually destructor should not be invoked because StackBuffer do manual memory management

    m_end = m_begin; // to restrict profiler::Block to invoke profiler::endBlock() on destructor.
    free(m_runtimeName);
}

void NonscopedBlock::copyname()
{
    // Here we need to copy m_name to m_runtimeName to ensure that
    // it would be alive to the moment we will serialize the block

    if ((m_status & profiler::ON) == 0)
        return;

    if (*m_name != 0)
    {
        auto len = strlen(m_name);
        m_runtimeName = static_cast<char*>(malloc(len + 1));

        // memcpy should be faster than strncpy because we know
        // actual bytes number and both strings have the same size
        memcpy(m_runtimeName, m_name, len);

        m_runtimeName[len] = 0;
        m_name = m_runtimeName;
    }
    else
    {
        m_name = "";
    }
}

void NonscopedBlock::destroy()
{
    // free memory used by m_runtimeName
    free(m_runtimeName);
    m_name = "";
}

//////////////////////////////////////////////////////////////////////////

ThreadStorage::ThreadStorage() : nonscopedBlocks(16), id(getCurrentThreadId()), allowChildren(true), named(false), guarded(false)
#ifndef _WIN32
, pthread_id(pthread_self())
#endif

{
    expired = ATOMIC_VAR_INIT(0);
    frame = ATOMIC_VAR_INIT(false);
}

void ThreadStorage::storeBlock(const profiler::Block& block)
{
#if EASY_OPTION_MEASURE_STORAGE_EXPAND != 0
    EASY_LOCAL_STATIC_PTR(const BaseBlockDescriptor*, desc,\
        MANAGER.addBlockDescriptor(EASY_OPTION_STORAGE_EXPAND_BLOCKS_ON ? profiler::ON : profiler::OFF, EASY_UNIQUE_LINE_ID, "EasyProfiler.ExpandStorage",\
            __FILE__, __LINE__, profiler::BLOCK_TYPE_BLOCK, EASY_COLOR_INTERNAL_EVENT));

    EASY_THREAD_LOCAL static profiler::timestamp_t beginTime = 0ULL;
    EASY_THREAD_LOCAL static profiler::timestamp_t endTime = 0ULL;
#endif

    auto name_length = static_cast<uint16_t>(strlen(block.name()));
    auto size = static_cast<uint16_t>(sizeof(BaseBlockData) + name_length + 1);

#if EASY_OPTION_MEASURE_STORAGE_EXPAND != 0
    const bool expanded = (desc->m_status & profiler::ON) && blocks.closedList.need_expand(size);
    if (expanded) beginTime = getCurrentTime();
#endif

    auto data = blocks.closedList.allocate(size);

#if EASY_OPTION_MEASURE_STORAGE_EXPAND != 0
    if (expanded) endTime = getCurrentTime();
#endif

    ::new (data) SerializedBlock(block, name_length);
    blocks.usedMemorySize += size;

#if EASY_OPTION_MEASURE_STORAGE_EXPAND != 0
    if (expanded)
    {
        profiler::Block b(beginTime, desc->id(), "");
        b.finish(endTime);

        size = static_cast<uint16_t>(sizeof(BaseBlockData) + 1);
        data = blocks.closedList.allocate(size);
        ::new (data) SerializedBlock(b, 0);
        blocks.usedMemorySize += size;
    }
#endif
}

void ThreadStorage::storeCSwitch(const CSwitchBlock& block)
{
    auto name_length = static_cast<uint16_t>(strlen(block.name()));
    auto size = static_cast<uint16_t>(sizeof(CSwitchEvent) + name_length + 1);
    auto data = sync.closedList.allocate(size);
    ::new (data) SerializedCSwitch(block, name_length);
    sync.usedMemorySize += size;
}

void ThreadStorage::clearClosed()
{
    blocks.clearClosed();
    sync.clearClosed();
}

void ThreadStorage::popSilent()
{
    if (!blocks.openedList.empty())
    {
        Block& top = blocks.openedList.back();
        top.m_end = top.m_begin;
        if (!top.m_isScoped)
            nonscopedBlocks.pop();
        blocks.openedList.pop_back();
    }
}

//////////////////////////////////////////////////////////////////////////

ThreadGuard::~ThreadGuard()
{
#ifndef EASY_PROFILER_API_DISABLED
    if (m_id != 0 && THIS_THREAD != nullptr && THIS_THREAD->id == m_id)
    {
        bool isMarked = false;
        EASY_EVENT_RES(isMarked, "ThreadFinished", EASY_COLOR_THREAD_END, ::profiler::FORCE_ON);
        THIS_THREAD->frame.store(false, std::memory_order_release);
        THIS_THREAD->expired.store(isMarked ? 2 : 1, std::memory_order_release);
        THIS_THREAD = nullptr;
    }
#endif
}

//////////////////////////////////////////////////////////////////////////

ProfileManager::ProfileManager() :
#ifdef _WIN32
    m_processId(GetProcessId(GetCurrentProcess()))
#else
    m_processId((processid_t)getpid())
#endif
    , m_usedMemorySize(0)
    , m_beginTime(0)
    , m_endTime(0)
{
    m_profilerStatus = ATOMIC_VAR_INIT(EASY_PROF_DISABLED);
    m_isEventTracingEnabled = ATOMIC_VAR_INIT(EASY_OPTION_EVENT_TRACING_ENABLED);
    m_isAlreadyListening = ATOMIC_VAR_INIT(false);
    m_stopListen = ATOMIC_VAR_INIT(false);

    m_mainThreadId = ATOMIC_VAR_INIT(0);
    m_frameMax = ATOMIC_VAR_INIT(0);
    m_frameAvg = ATOMIC_VAR_INIT(0);
    m_frameCur = ATOMIC_VAR_INIT(0);
    m_frameMaxReset = ATOMIC_VAR_INIT(false);
    m_frameAvgReset = ATOMIC_VAR_INIT(false);

#if !defined(EASY_PROFILER_API_DISABLED) && EASY_OPTION_START_LISTEN_ON_STARTUP != 0
    startListen(profiler::DEFAULT_PORT);
#endif

#if !defined(EASY_PROFILER_API_DISABLED) && !defined(EASY_CHRONO_CLOCK) && !defined(_WIN32)
    const int64_t cpu_frequency = calculate_cpu_frequency();
    CPU_FREQUENCY.store(cpu_frequency, std::memory_order_release);
#endif
}

ProfileManager::~ProfileManager()
{
#ifndef EASY_PROFILER_API_DISABLED
    stopListen();
#endif

    for (auto desc : m_descriptors) {
#if EASY_BLOCK_DESC_FULL_COPY == 0
        if (desc)
            desc->~BlockDescriptor();
        free(desc);
#else
        delete desc;
#endif
    }
}

#ifndef EASY_MAGIC_STATIC_CPP11
class ProfileManagerInstance {
    friend ProfileManager;
    ProfileManager instance;
} PROFILE_MANAGER;
#endif

//////////////////////////////////////////////////////////////////////////

ProfileManager& ProfileManager::instance()
{
#ifndef EASY_MAGIC_STATIC_CPP11
    return PROFILE_MANAGER.instance;
#else
    ///C++11 makes possible to create Singleton without any warry about thread-safeness
    ///http://preshing.com/20130930/double-checked-locking-is-fixed-in-cpp11/
    static ProfileManager profileManager;
    return profileManager;
#endif
}

//////////////////////////////////////////////////////////////////////////

ThreadStorage& ProfileManager::_threadStorage(profiler::thread_id_t _thread_id)
{
    return m_threads[_thread_id];
}

ThreadStorage* ProfileManager::_findThreadStorage(profiler::thread_id_t _thread_id)
{
    auto it = m_threads.find(_thread_id);
    return it != m_threads.end() ? &it->second : nullptr;
}

//////////////////////////////////////////////////////////////////////////

const BaseBlockDescriptor* ProfileManager::addBlockDescriptor(EasyBlockStatus _defaultStatus,
                                                        const char* _autogenUniqueId,
                                                        const char* _name,
                                                        const char* _filename,
                                                        int _line,
                                                        block_type_t _block_type,
                                                        color_t _color,
                                                        bool _copyName)
{
    guard_lock_t lock(m_storedSpin);

    descriptors_map_t::key_type key(_autogenUniqueId);
    auto it = m_descriptorsMap.find(key);
    if (it != m_descriptorsMap.end())
        return m_descriptors[it->second];

    const auto nameLen = strlen(_name);
    m_usedMemorySize += sizeof(profiler::SerializedBlockDescriptor) + nameLen + strlen(_filename) + 2;

#if EASY_BLOCK_DESC_FULL_COPY == 0
    BlockDescriptor* desc = nullptr;

    if (_copyName)
    {
        void* data = malloc(sizeof(BlockDescriptor) + nameLen + 1);
        char* name = reinterpret_cast<char*>(data) + sizeof(BlockDescriptor);
        strncpy(name, _name, nameLen);
        desc = ::new (data)BlockDescriptor(static_cast<block_id_t>(m_descriptors.size()), _defaultStatus, name, _filename, _line, _block_type, _color);
    }
    else
    {
        void* data = malloc(sizeof(BlockDescriptor));
        desc = ::new (data)BlockDescriptor(static_cast<block_id_t>(m_descriptors.size()), _defaultStatus, _name, _filename, _line, _block_type, _color);
    }
#else
    auto desc = new BlockDescriptor(static_cast<block_id_t>(m_descriptors.size()), _defaultStatus, _name, _filename, _line, _block_type, _color);
#endif

    m_descriptors.emplace_back(desc);
    m_descriptorsMap.emplace(key, desc->id());

    return desc;
}

//////////////////////////////////////////////////////////////////////////

bool ProfileManager::storeBlock(const profiler::BaseBlockDescriptor* _desc, const char* _runtimeName)
{
    const auto state = m_profilerStatus.load(std::memory_order_acquire);
    if (state == EASY_PROF_DISABLED || !(_desc->m_status & profiler::ON))
        return false;

    if (state == EASY_PROF_DUMP)
    {
        if (THIS_THREAD == nullptr || THIS_THREAD->blocks.openedList.empty())
            return false;
    }
    else if (THIS_THREAD == nullptr)
    {
        THIS_THREAD = &threadStorage(getCurrentThreadId());
    }

#if EASY_ENABLE_BLOCK_STATUS != 0
    if (!THIS_THREAD->allowChildren && !(_desc->m_status & FORCE_ON_FLAG))
        return false;
#endif

    profiler::Block b(_desc, _runtimeName);
    b.start();
    b.m_end = b.m_begin;

    THIS_THREAD->storeBlock(b);

    return true;
}

bool ProfileManager::storeBlock(const profiler::BaseBlockDescriptor* _desc, const char* _runtimeName, profiler::timestamp_t _beginTime, profiler::timestamp_t _endTime)
{
    const auto state = m_profilerStatus.load(std::memory_order_acquire);
    if (state == EASY_PROF_DISABLED || !(_desc->m_status & profiler::ON))
        return false;

    if (state == EASY_PROF_DUMP)
    {
        if (THIS_THREAD == nullptr || THIS_THREAD->blocks.openedList.empty())
            return false;
    }
    else if (THIS_THREAD == nullptr)
    {
        THIS_THREAD = &threadStorage(getCurrentThreadId());
    }

#if EASY_ENABLE_BLOCK_STATUS != 0
    if (!THIS_THREAD->allowChildren && !(_desc->m_status & FORCE_ON_FLAG))
        return false;
#endif

    profiler::Block b(_beginTime, _endTime, _desc->id(), _runtimeName);
    THIS_THREAD->storeBlock(b);
    b.m_end = b.m_begin;

    return true;
}

//////////////////////////////////////////////////////////////////////////

void ProfileManager::storeBlockForce(const profiler::BaseBlockDescriptor* _desc, const char* _runtimeName, ::profiler::timestamp_t& _timestamp)
{
    if (!(_desc->m_status & profiler::ON))
        return;

    if (THIS_THREAD == nullptr)
        THIS_THREAD = &threadStorage(getCurrentThreadId());

#if EASY_ENABLE_BLOCK_STATUS != 0
    if (!THIS_THREAD->allowChildren && !(_desc->m_status & FORCE_ON_FLAG))
        return;
#endif

    profiler::Block b(_desc, _runtimeName);
    b.start();
    b.m_end = b.m_begin;

    _timestamp = b.m_begin;
    THIS_THREAD->storeBlock(b);
}

void ProfileManager::storeBlockForce2(const profiler::BaseBlockDescriptor* _desc, const char* _runtimeName, ::profiler::timestamp_t _timestamp)
{
    if (!(_desc->m_status & profiler::ON))
        return;

    if (THIS_THREAD == nullptr)
        THIS_THREAD = &threadStorage(getCurrentThreadId());

#if EASY_ENABLE_BLOCK_STATUS != 0
    if (!THIS_THREAD->allowChildren && !(_desc->m_status & FORCE_ON_FLAG))
        return;
#endif

    profiler::Block b(_desc, _runtimeName);
    b.m_end = b.m_begin = _timestamp;

    THIS_THREAD->storeBlock(b);
}

void ProfileManager::storeBlockForce2(ThreadStorage& _registeredThread, const profiler::BaseBlockDescriptor* _desc, const char* _runtimeName, ::profiler::timestamp_t _timestamp)
{
    profiler::Block b(_desc, _runtimeName);
    b.m_end = b.m_begin = _timestamp;
    _registeredThread.storeBlock(b);
}

//////////////////////////////////////////////////////////////////////////

void ProfileManager::beginBlock(Block& _block)
{
    if (THIS_THREAD == nullptr)
        THIS_THREAD = &threadStorage(getCurrentThreadId());

    if (++THIS_THREAD_STACK_SIZE > 1)
    {
        _block.m_status = profiler::OFF;
        THIS_THREAD->blocks.openedList.emplace_back(_block);
        return;
    }

    const auto state = m_profilerStatus.load(std::memory_order_acquire);
    if (state == EASY_PROF_DISABLED)
    {
        THIS_THREAD_HALT = false;
        _block.m_status = profiler::OFF;
        THIS_THREAD->blocks.openedList.emplace_back(_block);
        beginFrame();
        return;
    }

    bool empty = true;
    if (state == EASY_PROF_DUMP)
    {
        if (THIS_THREAD_HALT || THIS_THREAD->blocks.openedList.empty())
        {
            _block.m_status = profiler::OFF;
            THIS_THREAD->blocks.openedList.emplace_back(_block);

            if (!THIS_THREAD_HALT)
            {
                THIS_THREAD_HALT = true;
                beginFrame();
            }

            return;
        }

        empty = false;
    }
    else
    {
        empty = THIS_THREAD->blocks.openedList.empty();
    }

    THIS_THREAD_HALT = false;
    THIS_THREAD_STACK_SIZE = 0;

#if EASY_ENABLE_BLOCK_STATUS != 0
    if (THIS_THREAD->allowChildren)
    {
#endif
        if (_block.m_status & profiler::ON)
            _block.start();
#if EASY_ENABLE_BLOCK_STATUS != 0
        THIS_THREAD->allowChildren = !(_block.m_status & profiler::OFF_RECURSIVE);
    }
    else if (_block.m_status & FORCE_ON_FLAG)
    {
        _block.start();
        _block.m_status = profiler::FORCE_ON_WITHOUT_CHILDREN;
    }
    else
    {
        _block.m_status = profiler::OFF_RECURSIVE;
    }
#endif

    if (empty)
    {
        beginFrame();
        THIS_THREAD->frame.store(true, std::memory_order_release);
    }

    THIS_THREAD->blocks.openedList.emplace_back(_block);
}

void ProfileManager::beginNonScopedBlock(const profiler::BaseBlockDescriptor* _desc, const char* _runtimeName)
{
    if (THIS_THREAD == nullptr)
        THIS_THREAD = &threadStorage(getCurrentThreadId());

    NonscopedBlock& b = THIS_THREAD->nonscopedBlocks.push(_desc, _runtimeName, false);
    beginBlock(b);
    b.copyname();
}

void ProfileManager::beginContextSwitch(profiler::thread_id_t _thread_id, profiler::timestamp_t _time, profiler::thread_id_t _target_thread_id, const char* _target_process, bool _lockSpin)
{
    auto ts = _lockSpin ? findThreadStorage(_thread_id) : _findThreadStorage(_thread_id);
    if (ts != nullptr)
        // Dirty hack: _target_thread_id will be written to the field "block_id_t m_id"
        // and will be available calling method id().
        ts->sync.openedList.emplace_back(_time, _target_thread_id, _target_process);
}

//////////////////////////////////////////////////////////////////////////

void ProfileManager::endBlock()
{
    if (--THIS_THREAD_STACK_SIZE > 0)
    {
        THIS_THREAD->popSilent();
        return;
    }

    THIS_THREAD_STACK_SIZE = 0;
    if (THIS_THREAD_HALT || m_profilerStatus.load(std::memory_order_acquire) == EASY_PROF_DISABLED)
    {
        THIS_THREAD->popSilent();
        endFrame();
        return;
    }

    if (THIS_THREAD->blocks.openedList.empty())
        return;

    Block& top = THIS_THREAD->blocks.openedList.back();
    if (top.m_status & profiler::ON)
    {
        if (!top.finished())
            top.finish();
        THIS_THREAD->storeBlock(top);
    }
    else
    {
        top.m_end = top.m_begin; // this is to restrict endBlock() call inside ~Block()
    }

    if (!top.m_isScoped)
        THIS_THREAD->nonscopedBlocks.pop();

    THIS_THREAD->blocks.openedList.pop_back();
    const bool empty = THIS_THREAD->blocks.openedList.empty();
    if (empty)
    {
        THIS_THREAD->frame.store(false, std::memory_order_release);
        endFrame();
#if EASY_ENABLE_BLOCK_STATUS != 0
        THIS_THREAD->allowChildren = true;
    }
    else
    {
        THIS_THREAD->allowChildren = !(THIS_THREAD->blocks.openedList.back().get().m_status & profiler::OFF_RECURSIVE);
    }
#else
    }
#endif
}

void ProfileManager::endContextSwitch(profiler::thread_id_t _thread_id, processid_t _process_id, profiler::timestamp_t _endtime, bool _lockSpin)
{
    ThreadStorage* ts = nullptr;
    if (_process_id == m_processId)
        // If thread owned by current process then create new ThreadStorage if there is no one
        ts = _lockSpin ? &threadStorage(_thread_id) : &_threadStorage(_thread_id);
    else
        // If thread owned by another process OR _process_id IS UNKNOWN then do not create ThreadStorage for this
        ts = _lockSpin ? findThreadStorage(_thread_id) : _findThreadStorage(_thread_id);

    if (ts == nullptr || ts->sync.openedList.empty())
        return;

    CSwitchBlock& lastBlock = ts->sync.openedList.back();
    lastBlock.m_end = _endtime;

    ts->storeCSwitch(lastBlock);
    ts->sync.openedList.pop_back();
}

//////////////////////////////////////////////////////////////////////////

void ProfileManager::beginFrame()
{
    if (!THIS_THREAD_FRAME)
    {
        THIS_THREAD_FRAME_T = getCurrentTime();
        THIS_THREAD_FRAME = true;
    }
}

void ProfileManager::endFrame()
{
    if (!THIS_THREAD_FRAME)
        return;

    const profiler::timestamp_t duration = getCurrentTime() - THIS_THREAD_FRAME_T;

    THIS_THREAD_FRAME = false;

    if (THIS_THREAD_FRAME_T_RESET_MAX)
    {
        THIS_THREAD_FRAME_T_RESET_MAX = false;
        THIS_THREAD_FRAME_T_MAX = 0;
    }

    THIS_THREAD_FRAME_T_CUR = duration;
    if (duration > THIS_THREAD_FRAME_T_MAX)
        THIS_THREAD_FRAME_T_MAX = duration;

    if (THIS_THREAD_N_FRAMES > 10000)
        THIS_THREAD_FRAME_T_RESET_AVG = true;

    if (THIS_THREAD_IS_MAIN)
    {
        if (m_frameAvgReset.exchange(false, std::memory_order_release) || THIS_THREAD_FRAME_T_RESET_AVG)
        {
            if (THIS_THREAD_N_FRAMES > 0)
                m_frameAvg.store(THIS_THREAD_FRAME_T_ACC / THIS_THREAD_N_FRAMES, std::memory_order_release);
            THIS_THREAD_FRAME_T_RESET_AVG = false;
            THIS_THREAD_FRAME_T_ACC = duration;
            THIS_THREAD_N_FRAMES = 1;
        }
        else
        {
            THIS_THREAD_FRAME_T_ACC += duration;
            ++THIS_THREAD_N_FRAMES;
            m_frameAvg.store(THIS_THREAD_FRAME_T_ACC / THIS_THREAD_N_FRAMES, std::memory_order_release);
        }

        auto maxDuration = m_frameMax.load(std::memory_order_acquire);
        if (m_frameMaxReset.exchange(false, std::memory_order_release))
            maxDuration = 0;

        if (duration > maxDuration)
            m_frameMax.store(duration, std::memory_order_release);

        if (m_frameMaxReset.exchange(false, std::memory_order_release))
            maxDuration = 0;

        m_frameCur.store(duration, std::memory_order_release);
    }
    else if (THIS_THREAD_FRAME_T_RESET_AVG)
    {
        THIS_THREAD_FRAME_T_RESET_AVG = false;
        THIS_THREAD_FRAME_T_ACC = duration;
        THIS_THREAD_N_FRAMES = 1;
    }
    else
    {
        THIS_THREAD_FRAME_T_ACC += duration;
        ++THIS_THREAD_N_FRAMES;
    }
}

profiler::timestamp_t ProfileManager::maxFrameDuration()
{
    auto duration = m_frameMax.load(std::memory_order_acquire);
    m_frameMaxReset.store(true, std::memory_order_release);
    return duration;
}

profiler::timestamp_t ProfileManager::avgFrameDuration()
{
    auto duration = m_frameAvg.load(std::memory_order_acquire);
    m_frameAvgReset.store(true, std::memory_order_release);
    return duration;
}

profiler::timestamp_t ProfileManager::curFrameDuration() const
{
    return m_frameCur.load(std::memory_order_acquire);
}

//////////////////////////////////////////////////////////////////////////

void ProfileManager::enableEventTracer()
{
#ifdef _WIN32
    if (m_isEventTracingEnabled.load(std::memory_order_acquire))
        EasyEventTracer::instance().enable(true);
#endif
}

void ProfileManager::disableEventTracer()
{
#ifdef _WIN32
    EasyEventTracer::instance().disable();
#endif
}

void ProfileManager::setEnabled(bool isEnable)
{
    guard_lock_t lock(m_dumpSpin);

    auto time = getCurrentTime();
    const auto status = isEnable ? EASY_PROF_ENABLED : EASY_PROF_DISABLED;
    const auto prev = m_profilerStatus.exchange(status, std::memory_order_release);
    if (prev == status)
        return;

    if (isEnable)
    {
        EASY_LOGMSG("Enabled profiling\n");
        enableEventTracer();
        m_beginTime = time;
    }
    else
    {
        EASY_LOGMSG("Disabled profiling\n");
        disableEventTracer();
        m_endTime = time;
    }
}

bool ProfileManager::isEnabled() const
{
    return m_profilerStatus.load(std::memory_order_acquire) == EASY_PROF_ENABLED;
}

void ProfileManager::setEventTracingEnabled(bool _isEnable)
{
    m_isEventTracingEnabled.store(_isEnable, std::memory_order_release);
}

bool ProfileManager::isEventTracingEnabled() const
{
    return m_isEventTracingEnabled.load(std::memory_order_acquire);
}

//////////////////////////////////////////////////////////////////////////

char ProfileManager::checkThreadExpired(ThreadStorage& _registeredThread)
{
    const char val = _registeredThread.expired.load(std::memory_order_acquire);
    if (val != 0)
        return val;

    if (_registeredThread.guarded)
        return 0;

#ifdef _WIN32

    // Check thread for Windows

    DWORD exitCode = 0;
    auto hThread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, (DWORD)_registeredThread.id);
    if (hThread == nullptr || GetExitCodeThread(hThread, &exitCode) == FALSE || exitCode != STILL_ACTIVE)
    {
        // Thread has been expired
        _registeredThread.expired.store(1, std::memory_order_release);
        if (hThread != nullptr)
            CloseHandle(hThread);
        return 1;
    }

    if (hThread != nullptr)
        CloseHandle(hThread);

    return 0;

#else

    return 0;//pthread_kill(_registeredThread.pthread_id, 0) != 0;

#endif

}

//////////////////////////////////////////////////////////////////////////

uint32_t ProfileManager::dumpBlocksToStream(profiler::OStream& _outputStream, bool _lockSpin)
{
    EASY_LOGMSG("dumpBlocksToStream(_lockSpin = " << _lockSpin << ")...\n");

    if (_lockSpin)
        m_dumpSpin.lock();

    const auto state = m_profilerStatus.load(std::memory_order_acquire);

#ifndef _WIN32
    const bool eventTracingEnabled = m_isEventTracingEnabled.load(std::memory_order_acquire);
#endif

    if (state == EASY_PROF_ENABLED) {
        m_profilerStatus.store(EASY_PROF_DUMP, std::memory_order_release);
        disableEventTracer();
        m_endTime = getCurrentTime();
    }


    // This is to make sure that no new descriptors or new threads will be
    // added until we finish sending data.
    //m_spin.lock();
    // This is the only place using both spins, so no dead-lock will occur


    // Wait for some time to be sure that all operations which began before setEnabled(false) will be finished.
    // This is much better than inserting spin-lock or atomic variable check into each store operation.
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    // wait for all threads finish opened frames
    EASY_LOG_ONLY(bool logged = false);
    for (auto it = m_threads.begin(), end = m_threads.end(); it != end;)
    {
        if (!it->second.frame.load(std::memory_order_acquire))
        {
            ++it;
            EASY_LOG_ONLY(logged = false);
        }
        else
        {
            EASY_LOG_ONLY(
                if (!logged)
                {
                    logged = true;
                    if (it->second.named)
                        EASY_WARNING("Waiting for thread \"" << it->second.name << "\" finish opened frame (which is top EASY_BLOCK for this thread)...\n");
                    else
                        EASY_WARNING("Waiting for thread " << it->first << " finish opened frame (which is top EASY_BLOCK for this thread)...\n");
                }
            );

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    m_profilerStatus.store(EASY_PROF_DISABLED, std::memory_order_release);

    EASY_LOGMSG("All threads have closed frames\n");
    EASY_LOGMSG("Disabled profiling\n");

    m_spin.lock();
    m_storedSpin.lock();
    // TODO: think about better solution because this one is not 100% safe...

    const profiler::timestamp_t now = getCurrentTime();
    const profiler::timestamp_t endtime = m_endTime == 0 ? now : std::min(now, m_endTime);

#ifndef _WIN32
    if (eventTracingEnabled)
    {
        // Read thread context switch events from temporary file

        EASY_LOGMSG("Writing context switch events...\n");

        uint64_t timestamp = 0;
        profiler::thread_id_t thread_from = 0, thread_to = 0;

        std::ifstream infile(m_csInfoFilename.c_str());
        if(infile.is_open())
        {
            EASY_LOG_ONLY(uint32_t num = 0);
            std::string next_task_name;
            pid_t process_to = 0;
            while (infile >> timestamp >> thread_from >> thread_to >> next_task_name >> process_to)
            {
                beginContextSwitch(thread_from, timestamp, thread_to, next_task_name.c_str(), false);
                endContextSwitch(thread_to, (processid_t)process_to, timestamp, false);
                EASY_LOG_ONLY(++num);
            }

            EASY_LOGMSG("Done, " << num << " context switch events wrote\n");
        }
        EASY_LOG_ONLY(
            else {
                EASY_ERROR("Can not open context switch log-file \"" << m_csInfoFilename << "\"\n");
            }
        )
    }
#endif

    bool mainThreadExpired = false;

    // Calculate used memory total size and total blocks number
    uint64_t usedMemorySize = 0;
    uint32_t blocks_number = 0;
    for (auto it = m_threads.begin(), end = m_threads.end(); it != end;)
    {
        auto& t = it->second;
        uint32_t num = static_cast<uint32_t>(t.blocks.closedList.size()) + static_cast<uint32_t>(t.sync.closedList.size());

        const char expired = checkThreadExpired(t);
        if (num == 0 && (expired != 0 || !t.guarded)) {
            // Remove thread if it contains no profiled information and has been finished or is not guarded.
            profiler::thread_id_t id = it->first;
            if (!mainThreadExpired && m_mainThreadId.compare_exchange_weak(id, 0, std::memory_order_release, std::memory_order_acquire))
                mainThreadExpired = true;
            m_threads.erase(it++);
            continue;
        }

        if (expired == 1) {
            EASY_FORCE_EVENT3(t, endtime, "ThreadExpired", EASY_COLOR_THREAD_END);
            ++num;
        }

        usedMemorySize += t.blocks.usedMemorySize + t.sync.usedMemorySize;
        blocks_number += num;
        ++it;
    }

    // Write profiler signature and version
    _outputStream.write(PROFILER_SIGNATURE);
    _outputStream.write(EASY_CURRENT_VERSION);
    _outputStream.write(m_processId);

    // Write CPU frequency to let GUI calculate real time value from CPU clocks
#if defined(EASY_CHRONO_CLOCK) || defined(_WIN32)
    _outputStream.write(CPU_FREQUENCY);
#else
    EASY_LOGMSG("Calculating CPU frequency\n");
    const int64_t cpu_frequency = calculate_cpu_frequency();
    _outputStream.write(cpu_frequency * 1000LL);
    EASY_LOGMSG("Done calculating CPU frequency\n");

    CPU_FREQUENCY.store(cpu_frequency, std::memory_order_release);
#endif

    // Write begin and end time
    _outputStream.write(m_beginTime);
    _outputStream.write(m_endTime);

    // Write blocks number and used memory size
    _outputStream.write(blocks_number);
    _outputStream.write(usedMemorySize);
    _outputStream.write(static_cast<uint32_t>(m_descriptors.size()));
    _outputStream.write(m_usedMemorySize);

    // Write block descriptors
    for (const auto descriptor : m_descriptors)
    {
        const auto name_size = descriptor->nameSize();
        const auto filename_size = descriptor->filenameSize();
        const auto size = static_cast<uint16_t>(sizeof(profiler::SerializedBlockDescriptor) + name_size + filename_size);

        _outputStream.write(size);
        _outputStream.write<profiler::BaseBlockDescriptor>(*descriptor);
        _outputStream.write(name_size);
        _outputStream.write(descriptor->name(), name_size);
        _outputStream.write(descriptor->filename(), filename_size);
    }

    // Write blocks and context switch events for each thread
    for (auto it = m_threads.begin(), end = m_threads.end(); it != end;)
    {
        auto& t = it->second;

        _outputStream.write(it->first);

        const auto name_size = static_cast<uint16_t>(t.name.size() + 1);
        _outputStream.write(name_size);
        _outputStream.write(name_size > 1 ? t.name.c_str() : "", name_size);

        _outputStream.write(t.sync.closedList.size());
        if (!t.sync.closedList.empty())
            t.sync.closedList.serialize(_outputStream);

        _outputStream.write(t.blocks.closedList.size());
        if (!t.blocks.closedList.empty())
            t.blocks.closedList.serialize(_outputStream);

        t.clearClosed();
        //t.blocks.openedList.clear();
        t.sync.openedList.clear();

        if (t.expired.load(std::memory_order_acquire) != 0)
        {
            // Remove expired thread after writing all profiled information
            profiler::thread_id_t id = it->first;
            if (!mainThreadExpired && m_mainThreadId.compare_exchange_weak(id, 0, std::memory_order_release, std::memory_order_acquire))
                mainThreadExpired = true;
            m_threads.erase(it++);
        }
        else
        {
            ++it;
        }
    }

    m_storedSpin.unlock();
    m_spin.unlock();

    if (_lockSpin)
        m_dumpSpin.unlock();

    EASY_LOGMSG("Done dumpBlocksToStream(). Dumped " << blocks_number << " blocks\n");

    return blocks_number;
}

uint32_t ProfileManager::dumpBlocksToFile(const char* _filename)
{
    EASY_LOGMSG("dumpBlocksToFile(\"" << _filename << "\")...\n");

    std::ofstream outputFile(_filename, std::fstream::binary);
    if (!outputFile.is_open())
    {
        EASY_ERROR("Can not open \"" << _filename << "\" for writing\n");
        return 0;
    }

    profiler::OStream outputStream;

    // Replace outputStream buffer to outputFile buffer to avoid redundant copying
    typedef ::std::basic_iostream<std::stringstream::char_type, std::stringstream::traits_type> stringstream_parent;
    stringstream_parent& s = outputStream.stream();
    auto oldbuf = s.rdbuf(outputFile.rdbuf());

    // Write data directly to file
    const auto blocksNumber = dumpBlocksToStream(outputStream, true);

    // Restore old outputStream buffer to avoid possible second memory free on stringstream destructor
    s.rdbuf(oldbuf);

    EASY_LOGMSG("Done dumpBlocksToFile()\n");

    return blocksNumber;
}

const char* ProfileManager::registerThread(const char* name, ThreadGuard& threadGuard)
{
    if (THIS_THREAD == nullptr)
        THIS_THREAD = &threadStorage(getCurrentThreadId());

    THIS_THREAD->guarded = true;
    if (!THIS_THREAD->named)
    {
        THIS_THREAD->named = true;
        THIS_THREAD->name = name;

        if (THIS_THREAD->name == "Main")
        {
            profiler::thread_id_t id = 0;
            THIS_THREAD_IS_MAIN = m_mainThreadId.compare_exchange_weak(id, THIS_THREAD->id, std::memory_order_release, std::memory_order_acquire);
        }
    }

    threadGuard.m_id = THIS_THREAD->id;

    return THIS_THREAD->name.c_str();
}

const char* ProfileManager::registerThread(const char* name)
{
    if (THIS_THREAD == nullptr)
        THIS_THREAD = &threadStorage(getCurrentThreadId());

    if (!THIS_THREAD->named)
    {
        THIS_THREAD->named = true;
        THIS_THREAD->name = name;

        if (THIS_THREAD->name == "Main")
        {
            profiler::thread_id_t id = 0;
            THIS_THREAD_IS_MAIN = m_mainThreadId.compare_exchange_weak(id, THIS_THREAD->id, std::memory_order_release, std::memory_order_acquire);
        }
    }

    return THIS_THREAD->name.c_str();
}

void ProfileManager::setBlockStatus(block_id_t _id, EasyBlockStatus _status)
{
    if (m_profilerStatus.load(std::memory_order_acquire) != EASY_PROF_DISABLED)
        return; // Changing blocks statuses is restricted while profile session is active

    guard_lock_t lock(m_storedSpin);
    if (_id < m_descriptors.size())
    {
        auto desc = m_descriptors[_id];
        lock.unlock();
        desc->m_status = _status;
    }
}

void ProfileManager::startListen(uint16_t _port)
{
    if (!m_isAlreadyListening.exchange(true, std::memory_order_release))
    {
        m_stopListen.store(false, std::memory_order_release);
        m_listenThread = std::thread(&ProfileManager::listen, this, _port);
    }
}

void ProfileManager::stopListen()
{
    m_stopListen.store(true, std::memory_order_release);
    if (m_listenThread.joinable())
        m_listenThread.join();
    m_isAlreadyListening.store(false, std::memory_order_release);

    EASY_LOGMSG("Listening stopped\n");
}

bool ProfileManager::isListening() const
{
    return m_isAlreadyListening.load(std::memory_order_acquire);
}

//////////////////////////////////////////////////////////////////////////

void ProfileManager::listen(uint16_t _port)
{
    EASY_THREAD_SCOPE("EasyProfiler.Listen");

    EASY_LOGMSG("Listening started\n");

    EasySocket socket;
    profiler::net::Message replyMessage(profiler::net::MESSAGE_TYPE_REPLY_START_CAPTURING);

    socket.bind(_port);
    int bytes = 0;
    while (!m_stopListen.load(std::memory_order_acquire))
    {
        bool hasConnect = false;

        socket.listen();
        socket.accept();

        hasConnect = true;

        // Send reply
        {
            const bool wasLowPriorityET =
#ifdef _WIN32
                EasyEventTracer::instance().isLowPriority();
#else
                false;
#endif
            const profiler::net::EasyProfilerStatus connectionReply(m_profilerStatus.load(std::memory_order_acquire) == EASY_PROF_ENABLED, m_isEventTracingEnabled.load(std::memory_order_acquire), wasLowPriorityET);
            bytes = socket.send(&connectionReply, sizeof(profiler::net::EasyProfilerStatus));
            hasConnect = bytes > 0;
        }

        while (hasConnect && !m_stopListen.load(std::memory_order_acquire))
        {
            char buffer[256] = {};

            bytes = socket.receive(buffer, 255);

            hasConnect = bytes > 0;

            char *buf = &buffer[0];

            if (bytes > 0)
            {
                profiler::net::Message* message = (profiler::net::Message*)buf;
                if (!message->isEasyNetMessage()){
                    continue;
                }

                switch (message->type)
                {
                    case profiler::net::MESSAGE_TYPE_CHECK_CONNECTION:
                    {
                        EASY_LOGMSG("receive MESSAGE_TYPE_CHECK_CONNECTION\n");
                        break;
                    }

                    case profiler::net::MESSAGE_TYPE_REQUEST_MAIN_FRAME_TIME_MAX_AVG_US:
                    {
                        profiler::timestamp_t maxDuration = maxFrameDuration(), avgDuration = avgFrameDuration();
                        maxDuration = TICKS_TO_US(maxDuration);
                        avgDuration = TICKS_TO_US(avgDuration);
                        const profiler::net::TimestampMessage reply(profiler::net::MESSAGE_TYPE_REPLY_MAIN_FRAME_TIME_MAX_AVG_US, (uint32_t)maxDuration, (uint32_t)avgDuration);
                        bytes = socket.send(&reply, sizeof(profiler::net::TimestampMessage));
                        hasConnect = bytes > 0;
                        break;
                    }

                    case profiler::net::MESSAGE_TYPE_REQUEST_START_CAPTURE:
                    {
                        EASY_LOGMSG("receive REQUEST_START_CAPTURE\n");

                        ::profiler::timestamp_t t = 0;
                        EASY_FORCE_EVENT(t, "StartCapture", EASY_COLOR_START, profiler::OFF);

                        m_dumpSpin.lock();
                        const auto prev = m_profilerStatus.exchange(EASY_PROF_ENABLED, std::memory_order_release);
                        if (prev != EASY_PROF_ENABLED) {
                            enableEventTracer();
                            m_beginTime = t;
                        }
                        m_dumpSpin.unlock();

                        replyMessage.type = profiler::net::MESSAGE_TYPE_REPLY_START_CAPTURING;
                        bytes = socket.send(&replyMessage, sizeof(replyMessage));
                        hasConnect = bytes > 0;

                        break;
                    }

                    case profiler::net::MESSAGE_TYPE_REQUEST_STOP_CAPTURE:
                    {
                        EASY_LOGMSG("receive REQUEST_STOP_CAPTURE\n");

                        m_dumpSpin.lock();
                        auto time = getCurrentTime();
                        const auto prev = m_profilerStatus.exchange(EASY_PROF_DUMP, std::memory_order_release);
                        if (prev == EASY_PROF_ENABLED) {
                            disableEventTracer();
                            m_endTime = time;
                        }
                        EASY_FORCE_EVENT2(m_endTime, "StopCapture", EASY_COLOR_END, profiler::OFF);

                        //TODO
                        //if connection aborted - ignore this part

                        profiler::OStream os;
                        dumpBlocksToStream(os, false);
                        m_dumpSpin.unlock();

                        const auto size = os.stream().tellp();
                        static const decltype(size) badSize = -1;
                        if (size != badSize)
                        {
                            const profiler::net::DataMessage dm(static_cast<uint32_t>(size), profiler::net::MESSAGE_TYPE_REPLY_BLOCKS);

                            const size_t packet_size = sizeof(dm) + dm.size;
                            std::string sendbuf;
                            sendbuf.reserve(packet_size + 1);

                            if (sendbuf.capacity() >= packet_size) // check if there is enough memory
                            {
                                sendbuf.append((const char*)&dm, sizeof(dm));
                                sendbuf += os.stream().str(); // TODO: Avoid double-coping data from stringstream!
                                os.clear();

                                bytes = socket.send(sendbuf.c_str(), packet_size);
                                hasConnect = bytes > 0;
                            }
                            else
                            {
                                EASY_ERROR("Can not send blocks. Not enough memory for allocating " << packet_size << " bytes");
                            }
                        }
                        else
                        {
                            EASY_ERROR("Can not send blocks. Bad std::stringstream.tellp() == -1");
                        }

                        replyMessage.type = profiler::net::MESSAGE_TYPE_REPLY_BLOCKS_END;
                        bytes = socket.send(&replyMessage, sizeof(replyMessage));
                        hasConnect = bytes > 0;

                        break;
                    }

                    case profiler::net::MESSAGE_TYPE_REQUEST_BLOCKS_DESCRIPTION:
                    {
                        EASY_LOGMSG("receive REQUEST_BLOCKS_DESCRIPTION\n");

                        profiler::OStream os;

                        // Write profiler signature and version
                        os.write(PROFILER_SIGNATURE);
                        os.write(EASY_CURRENT_VERSION);

                        // Write block descriptors
                        m_storedSpin.lock();
                        os.write(static_cast<uint32_t>(m_descriptors.size()));
                        os.write(m_usedMemorySize);
                        for (const auto descriptor : m_descriptors)
                        {
                            const auto name_size = descriptor->nameSize();
                            const auto filename_size = descriptor->filenameSize();
                            const auto size = static_cast<uint16_t>(sizeof(profiler::SerializedBlockDescriptor) + name_size + filename_size);

                            os.write(size);
                            os.write<profiler::BaseBlockDescriptor>(*descriptor);
                            os.write(name_size);
                            os.write(descriptor->name(), name_size);
                            os.write(descriptor->filename(), filename_size);
                        }
                        m_storedSpin.unlock();
                        // END of Write block descriptors.

                        const auto size = os.stream().tellp();
                        static const decltype(size) badSize = -1;
                        if (size != badSize)
                        {
                            const profiler::net::DataMessage dm(static_cast<uint32_t>(size), profiler::net::MESSAGE_TYPE_REPLY_BLOCKS_DESCRIPTION);

                            const size_t packet_size = sizeof(dm) + dm.size;
                            std::string sendbuf;
                            sendbuf.reserve(packet_size + 1);

                            if (sendbuf.capacity() >= packet_size) // check if there is enough memory
                            {
                                sendbuf.append((const char*)&dm, sizeof(dm));
                                sendbuf += os.stream().str(); // TODO: Avoid double-coping data from stringstream!
                                os.clear();

                                bytes = socket.send(sendbuf.c_str(), packet_size);
                                hasConnect = bytes > 0;
                            }
                            else
                            {
                                EASY_ERROR("Can not send block descriptions. Not enough memory for allocating " << packet_size << " bytes");
                            }
                        }
                        else
                        {
                            EASY_ERROR("Can not send block descriptions. Bad std::stringstream.tellp() == -1");
                        }

                        replyMessage.type = profiler::net::MESSAGE_TYPE_REPLY_BLOCKS_DESCRIPTION_END;
                        bytes = socket.send(&replyMessage, sizeof(replyMessage));
                        hasConnect = bytes > 0;

                        break;
                    }

                    case profiler::net::MESSAGE_TYPE_EDIT_BLOCK_STATUS:
                    {
                        auto data = reinterpret_cast<const profiler::net::BlockStatusMessage*>(message);

                        EASY_LOGMSG("receive EDIT_BLOCK_STATUS id=" << data->id << " status=" << data->status << std::endl);

                        setBlockStatus(data->id, static_cast<::profiler::EasyBlockStatus>(data->status));

                        break;
                    }

                    case profiler::net::MESSAGE_TYPE_EVENT_TRACING_STATUS:
                    {
                        auto data = reinterpret_cast<const profiler::net::BoolMessage*>(message);

                        EASY_LOGMSG("receive EVENT_TRACING_STATUS on=" << data->flag << std::endl);

                        m_isEventTracingEnabled.store(data->flag, std::memory_order_release);
                        break;
                    }

                    case profiler::net::MESSAGE_TYPE_EVENT_TRACING_PRIORITY:
                    {
#if defined(_WIN32) || EASY_OPTION_LOG_ENABLED != 0
                        auto data = reinterpret_cast<const profiler::net::BoolMessage*>(message);
#endif

                        EASY_LOGMSG("receive EVENT_TRACING_PRIORITY low=" << data->flag << std::endl);

#ifdef _WIN32
                        EasyEventTracer::instance().setLowPriority(data->flag);
#endif
                        break;
                    }

                    default:
                        break;
                }

                //nn_freemsg (buf);
            }
        }



    }

}

//////////////////////////////////////////////////////////////////////////

