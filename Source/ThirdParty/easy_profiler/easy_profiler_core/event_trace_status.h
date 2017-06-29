


#ifndef EASY_PROFILER__EVENT_TRACE_STATUS__H_
#define EASY_PROFILER__EVENT_TRACE_STATUS__H_

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

namespace profiler {

    enum EventTracingEnableStatus : unsigned char
    {
        EVENT_TRACING_LAUNCHED_SUCCESSFULLY = 0,
        EVENT_TRACING_NOT_ENOUGH_ACCESS_RIGHTS,
        EVENT_TRACING_WAS_LAUNCHED_BY_SOMEBODY_ELSE,
        EVENT_TRACING_BAD_PROPERTIES_SIZE,
        EVENT_TRACING_OPEN_TRACE_ERROR,
        EVENT_TRACING_MISTERIOUS_ERROR,
    };

} // END of namespace profiler.

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#endif // EASY_PROFILER__EVENT_TRACE_STATUS__H_
