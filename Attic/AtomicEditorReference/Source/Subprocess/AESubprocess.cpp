// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"
#include "AEEvents.h"
#include <Atomic/IO/Log.h>
#include <Atomic/Core/Timer.h>

#include "AESubprocess.h"
#include "AESubprocessSystem.h"

namespace AtomicEditor
{

Subprocess::Subprocess(Context* context) :
    Object(context),
    processHandle_(NULL),
    inStream_(pipeOut_),
    errorStream_(pipeError_),
    returnCode_(-1)
{

}

Subprocess::~Subprocess()
{
    if (processHandle_)
        delete processHandle_;
}


void Subprocess::ThreadFunction()
{
    while (shouldRun_)
    {
        int read;
        char data[129];

        if (!Poco::Process::isRunning(*processHandle_))
        {
            shouldRun_ = false;
            break;
        }

        inStream_.read(data, 128);
        read = inStream_.gcount();
        data[read] = 0;

        if (read)
        {
            mutex_.Acquire();
            output_ += (const char*) data;
            mutex_.Release();

        }

        if (inStream_.eof())
        {
            returnCode_ = processHandle_->wait();

            if (returnCode_)
            {
                while (true)
                {
                    errorStream_.read(data, 128);
                    read = errorStream_.gcount();
                    data[read] = 0;

                    if (read)
                    {
                        mutex_.Acquire();
                        errorOutput_ += (const char*) data;
                        mutex_.Release();
                    }
                    else
                    {
                        break;
                    }
                }
            }

            shouldRun_ = false;
            break;
        }

    }
}

void Subprocess::ProcessOutput(SubprocessSystem* system)
{
    String output, errorOutput;

    GetOutput(output, errorOutput);
    if (output.Length())
    {
        VariantMap eventData;        
        eventData[SubprocessOutput::P_TEXT] = output;
        SendEvent(E_SUBPROCESSOUTPUT, eventData);
    }

    if (errorOutput.Length())
    {
        VariantMap eventData;
        eventData[SubprocessOutput::P_TEXT] = errorOutput;
        SendEvent(E_SUBPROCESSOUTPUT, eventData);
    }

}

bool Subprocess::Update(SubprocessSystem* system)
{
    ProcessOutput(system);

    if (!shouldRun_) {

        ProcessOutput(system);

        VariantMap eventData;
        eventData[SubprocessComplete::P_RETCODE] = returnCode_;
        SendEvent(E_SUBPROCESSCOMPLETE, eventData);
        return false;
    }


    return true;

}

bool Subprocess::Launch(const String& command, const Vector<String>& args, const String& initialDirectory)
{
    Poco::Process::Env env;
    return Launch(command, args, initialDirectory, env);
}

bool Subprocess::Launch(const String& command, const Vector<String>& args, const String& initialDirectory, const Poco::Process::Env& env)
{
    Poco::Process::Args pargs;

    for (unsigned i = 0; i < args.Size(); i++)
        pargs.push_back(args[i].CString());

    std::string pcommand = command.CString();
    std::string pinitialDirectory = initialDirectory.CString();

    processHandle_ = new Poco::ProcessHandle(Poco::Process::launch(pcommand, pargs, pinitialDirectory, &pipeIn_, &pipeOut_, &pipeError_, env));

    if (!Poco::Process::isRunning(*processHandle_))
        return false;

    return Run();

}

}
