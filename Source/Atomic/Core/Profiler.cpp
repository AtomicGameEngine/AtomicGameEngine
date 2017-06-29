//
// Copyright (c) 2017 the Atomic project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "../Precompiled.h"
#include "../Core/Profiler.h"
#include "../Core/StringUtils.h"

namespace Atomic
{

Profiler::Profiler(Context* context)
    : Object(context)
{
    SetEnabled(true);
#if !ATOMIC_PROFILING
    enableEventProfiling_ = false;
#endif
}

Profiler::~Profiler()
{
}

void Profiler::SetEnabled(bool enabled)
{
#if ATOMIC_PROFILING
    ::profiler::setEnabled(enabled);
#endif
}

bool Profiler::GetEnabled() const
{
#if ATOMIC_PROFILING
    return ::profiler::isEnabled();
#else
    return false;
#endif
}

void Profiler::StartListen(unsigned short port)
{
#if ATOMIC_PROFILING
    ::profiler::startListen(port);
#endif
}

void Profiler::StopListen()
{
#if ATOMIC_PROFILING
    ::profiler::stopListen();
#endif
}

bool Profiler::GetListening() const
{
#if ATOMIC_PROFILING
    return ::profiler::isListening();
#else
    return false;
#endif
}

void Profiler::SetEventTracingEnabled(bool enable)
{
#if ATOMIC_PROFILING
    ::profiler::setEventTracingEnabled(enable);
#endif
}

bool Profiler::GetEventTracingEnabled()
{
#if ATOMIC_PROFILING
    return ::profiler::isEventTracingEnabled();
#else
    return false;
#endif
}

void Profiler::SetLowPriorityEventTracing(bool isLowPriority)
{
#if ATOMIC_PROFILING
    ::profiler::setLowPriorityEventTracing(isLowPriority);
#endif
}

bool Profiler::GetLowPriorityEventTracing()
{
#if ATOMIC_PROFILING
    return ::profiler::isLowPriorityEventTracing();
#else
    return false;
#endif
}

void Profiler::SaveProfilerData(const String& filePath)
{
#if ATOMIC_PROFILING
    ::profiler::dumpBlocksToFile(filePath.CString());
#endif
}

void Profiler::SetEventProfilingEnabled(bool enabled)
{
#if ATOMIC_PROFILING
    enableEventProfiling_ = enabled;
#endif
}

bool Profiler::GetEventProfilingEnabled() const
{
    return enableEventProfiling_;
}

void Profiler::BeginBlock(const char* name, const char* file, int line, unsigned int argb, unsigned char status)
{
#if ATOMIC_PROFILING
    // Line used as starting hash value for efficiency.
    // This is likely to not play well with hot code reload.
    unsigned hash = StringHash::Calculate(file, (unsigned)line);
    HashMap<unsigned, ::profiler::BaseBlockDescriptor*>::Iterator it = blockDescriptorCache_.Find(hash);
    const ::profiler::BaseBlockDescriptor* desc = 0;
    if (it == blockDescriptorCache_.End())
    {
        String uniqueName = ToString("%s:%d", file, line);
        desc = ::profiler::registerDescription((::profiler::EasyBlockStatus)status, uniqueName.CString(), name, file,
                                               line, ::profiler::BLOCK_TYPE_BLOCK, argb, true);
    }
    else
        desc = it->second_;
    ::profiler::beginNonScopedBlock(desc, name);
#endif
}

void Profiler::EndBlock()
{
#if ATOMIC_PROFILING
    ::profiler::endBlock();
#endif
}

}
