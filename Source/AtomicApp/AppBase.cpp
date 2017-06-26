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


#include <Atomic/IO/Log.h>
#include <Atomic/IO/IOEvents.h>
#include <Atomic/Engine/Engine.h>
#include <Atomic/Engine/EngineConfig.h>

#include <Atomic/Script/ScriptSystem.h>
#include <AtomicJS/Javascript/Javascript.h>

// Move me!
#include <Atomic/Environment/Environment.h>

#include "AppBase.h"

namespace Atomic
{

    Vector<String> AppBase::engineConfigSearchPaths_;
    Vector<String> AppBase::arguments_;

    AppBase::AppBase(Context* context) :
        Application(context)
    {
        // Copy arguments
        if (!arguments_.Size())
        {
            arguments_ = GetArguments();
        }
        else
        {
            // Add quotes to any arguments that need them, and construct command line
            Vector<String> args = arguments_;

            QuoteArguments(args);

            String commandline = String::Joined(args, " ");

            ParseArguments(commandline, false);

            arguments_ = GetArguments();
        }
        
    }

    AppBase::~AppBase()
    {

    }

    void AppBase::AddArgument(const String& argument)
    {
        if (argument == "--autometrics")
        {
            Application::autoMetrics_ = true;
        }

        arguments_.Push(argument);
    }

    unsigned AppBase::GetNumArguments()
    {
        return arguments_.Size();
    }

    String AppBase::GetArgument(unsigned index)
    {
        return arguments_[index];
    }

    void AppBase::ProcessArguments()
    {
        for (unsigned i = 0; i < arguments_.Size(); ++i)
        {
            if (arguments_[i].Length() > 1)
            {
                String argument = arguments_[i].ToLower();
                String value = i + 1 < arguments_.Size() ? arguments_[i + 1] : String::EMPTY;

                if (argument == "--log-std")
                {
                    SubscribeToEvent(E_LOGMESSAGE, ATOMIC_HANDLER(AppBase, HandleLogMessage));
                }
            }
        }
    }

    void AppBase::Setup()
    {
        Application::Setup();

#ifdef ATOMIC_3D
        // Move me!
        RegisterEnvironmentLibrary(context_);
#endif

        ProcessArguments();

        // Read the engine configuration
        ReadEngineConfig();

        context_->RegisterSubsystem(new ScriptSystem(context_));

        // Instantiate and register the Javascript subsystem
        Javascript* javascript = new Javascript(context_);
        context_->RegisterSubsystem(javascript);
        vm_ = javascript->InstantiateVM("MainVM");

        SubscribeToEvent(E_JSERROR, ATOMIC_HANDLER(AppBase, HandleJSError));

    }

    void AppBase::Start()
    {
        Application::Start();

        vm_->InitJSContext();

    }

    bool AppBase::RunFrame()
    {
        engine_->RunFrame();

        if (engine_->IsExiting())
        {
            return false;
        }

        return true;

    }

    void AppBase::Stop()
    {

        vm_ = 0;
        context_->RemoveSubsystem<Javascript>();
        // make sure JSVM is really down and no outstanding refs
        // as if not, will hold on engine subsystems, which is bad
        assert(!JSVM::GetJSVM(0));

        Application::Stop();

    }

    void AppBase::ReadEngineConfig()
    {
        FileSystem* fileSystem = GetSubsystem<FileSystem>();

        // if we haven't defined any search paths, insert default
        if (!engineConfigSearchPaths_.Size())
        {
#ifdef ATOMIC_PLATFORM_OSX
        AddEngineConfigSearchPath(fileSystem->GetProgramDir() + "../Resources/Settings/");
#else
        AddEngineConfigSearchPath(fileSystem->GetProgramDir() + "Settings/");
#endif
        }

        for (unsigned i = 0; i < engineConfigSearchPaths_.Size(); i++)
        {
            const String& path = engineConfigSearchPaths_[i];

            if (!fileSystem->DirExists(path))
                continue;

            String filename = AddTrailingSlash(path) + "Engine.json";

            // take the 1st Engine.json found in the paths
            if (fileSystem->FileExists(filename))
            {

                if (EngineConfig::LoadFromFile(context_, filename))
                {
                    EngineConfig::ApplyConfig(engineParameters_);
                }

                return;
            }
        }

    }

    // Handlers

    void AppBase::HandleLogMessage(StringHash eventType, VariantMap& eventData)
    {
        using namespace LogMessage;

        int level = eventData[P_LEVEL].GetInt();
        // The message may be multi-line, so split to rows in that case
        Vector<String> rows = eventData[P_MESSAGE].GetString().Split('\n');

        for (unsigned i = 0; i < rows.Size(); ++i)
        {
            if (level == LOG_ERROR)
            {
                fprintf(stderr, "%s\n", rows[i].CString());
            }
            else
            {
                fprintf(stdout, "%s\n", rows[i].CString());
            }
        }

    }

    void AppBase::HandleJSError(StringHash eventType, VariantMap& eventData)
    {
        using namespace JSError;

        String errMessage = eventData[P_ERRORMESSAGE].GetString();
        String errFilename = eventData[P_ERRORFILENAME].GetString();
        int errLineNumber = eventData[P_ERRORLINENUMBER].GetInt();

        String errorString = ToString("%s - %s - Line: %i",
            errFilename.CString(), errMessage.CString(), errLineNumber);

        ATOMIC_LOGERROR(errorString);

    }

    bool AppBase::GetDebuggerAttached()
    {

#ifdef ATOMIC_PLATFORM_WINDOWS
        return IsDebuggerPresent() ? true : false;
#else
        return false;
#endif


    }

}
