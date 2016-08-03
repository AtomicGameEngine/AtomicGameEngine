//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
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

#pragma once

#include <Poco/Process.h>
#include <Poco/Pipe.h>
#include <Poco/PipeStream.h>

#include <Atomic/Container/Str.h>
#include <Atomic/Core/Mutex.h>
#include <Atomic/Core/Thread.h>
#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace ToolCore
{

class SubprocessSystem;

class Subprocess : public Object, public Thread
{
    friend class SubprocessSystem;

    ATOMIC_OBJECT(Subprocess, Object);

public:
    /// Construct.
    Subprocess(Context* context);
    /// Destruct.
    ~Subprocess();

    void GetOutput(String& outputText, String& errorText)
    {
        mutex_.Acquire();
        outputText = output_;
        output_.Clear();
        errorText = errorOutput_;
        errorOutput_.Clear();
        mutex_.Release(); }

private:

    void ThreadFunction();

    mutable Mutex mutex_;

    void ProcessOutput(SubprocessSystem* system);
    bool Update(SubprocessSystem* system);


    bool Launch(const String& command, const Vector<String>& args, const String& initialDirectory = "");
    bool Launch(const String& command, const Vector<String>& args, const String& initialDirectory, const Poco::Process::Env& env);

    String output_;
    String errorOutput_;
    int returnCode_;

    Poco::ProcessHandle* processHandle_;
    Poco::Pipe pipeIn_;
    Poco::Pipe pipeOut_;
    Poco::Pipe pipeError_;
    Poco::PipeInputStream inStream_;
    Poco::PipeInputStream errorStream_;
};

}
