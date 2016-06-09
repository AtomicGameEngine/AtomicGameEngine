//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
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

#ifndef ATOMIC_PLATFORM_WINDOWS
#include <unistd.h>
#endif

#include "../Core/StringUtils.h"
#include "../Core/Timer.h"
#include "../IO/Log.h"

#include "IPCUnix.h"
#include "IPCBroker.h"


namespace Atomic
{

unsigned IPCBroker::idCounter_ = 1;

IPCBroker::IPCBroker(Context* context) : IPCChannel(context, idCounter_)
{
    idCounter_++;
}

IPCBroker::~IPCBroker()
{

}

void IPCBroker::ThreadFunction()
{
    while (shouldRun_)
    {
        if (!otherProcess_->IsRunning())
        {
            break;
        }

        if (!Receive())
        {
            break;
        }

        // sleep thread a bit so we don't gobble CPU
        Time::Sleep(10);
    }

    shouldRun_ = false;

}

bool IPCBroker::Update()
{

    if (otherProcess_.Null())
        return false;

    if (!shouldRun_)
    {
        Stop();
#ifndef ATOMIC_PLATFORM_WINDOWS
        close(pp_.fd1());
#endif
        return false;
    }

    return true;
}

bool IPCBroker::SpawnWorker(const String& command, const Vector<String>& args, const String& initialDirectory)
{
    Vector<String> pargs;

#ifdef ATOMIC_PLATFORM_WINDOWS
    otherProcess_ = new IPCProcess(context_, pp_.clientRead(), pp_.clientWrite());
    transport_.OpenServer(pp_.serverRead(), pp_.serverWrite());
#else
    otherProcess_ = new IPCProcess(context_, pp_.fd1(), pp_.fd2());
    transport_.OpenServer(pp_.fd1());
#endif

    // copy args
    for (unsigned i = 0; i < args.Size(); i++)
        pargs.Push(args[i]);

#ifdef ATOMIC_PLATFORM_WINDOWS

    wchar_t pipe_num[10];
    _i64tow_s(reinterpret_cast<__int64>(pp_.clientWrite()), pipe_num, sizeof(pipe_num)/sizeof(pipe_num[0]), 10);

    String cpipe;
    cpipe.SetUTF8FromWChar(pipe_num);

    String ipc_client = "--ipc-client=" + cpipe;
    pargs.Push(ipc_client);

    _i64tow_s(reinterpret_cast<__int64>(pp_.clientRead()), pipe_num, sizeof(pipe_num)/sizeof(pipe_num[0]), 10);

    String spipe;
    spipe.SetUTF8FromWChar(pipe_num);

    String ipc_server = "--ipc-server=" + spipe;
    pargs.Push(ipc_server);

#else
    pargs.Push(ToString("--ipc-server=%i", pp_.fd1()));
    pargs.Push(ToString("--ipc-client=%i", pp_.fd2()));
#endif

    pargs.Push(ToString("--ipc-id=%i", id_));

    if (!otherProcess_->Launch(command, pargs, initialDirectory))
        return false;

#ifndef ATOMIC_PLATFORM_WINDOWS
    close(pp_.fd2());
#endif

    return Run();

}


}
