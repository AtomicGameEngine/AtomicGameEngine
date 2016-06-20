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

#include <Poco/Exception.h>
#include <Poco/Environment.h>


#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>

#include "../ToolSystem.h"
#include "../ToolEnvironment.h"
#include "../Subprocess/SubprocessSystem.h"

#include "NETCompile.h"

namespace ToolCore
{

    NETCompile::NETCompile(Context* context) :
        Object(context)
    {

    }

    NETCompile::~NETCompile()
    {
    }

    void NETCompile::HandleSubprocessOutput(StringHash eventType, VariantMap& eventData)
    {
        const String& text = eventData[SubprocessOutput::P_TEXT].GetString();

        compileText_ += text;

    }

    void NETCompile::HandleCompileProcessComplete(StringHash eventType, VariantMap& eventData)
    {
        int code = eventData[SubprocessComplete::P_RETCODE].GetInt();

        VariantMap compileData;

        using namespace NETCompilerResult;

        if (!code)
        {
            compileData[P_SUCCESS] = true;            
        }
        else
        {
            compileData[P_SUCCESS] = false; 
            compileText_ += ToString("\nCompilation Command: %s", allArgs_.CString());
            compileData[P_ERRORTEXT] = compileText_;
        }   

        SendEvent(E_NETCOMPILERESULT, compileData);
    }

    bool NETCompile::Compile(const String& solutionPath, const String& configuration)
    {
        FileSystem* fileSystem = GetSubsystem<FileSystem>();
        ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();
        ToolPrefs* prefs = tenv->GetToolPrefs();
        SubprocessSystem* subs = GetSubsystem<SubprocessSystem>();

        solutionPath_ = solutionPath;
        compileText_.Clear();

        String cmdToolsPath = Poco::Environment::get("VS140COMNTOOLS").c_str();

        VariantMap compileData;

        if (!cmdToolsPath.Length())
        {            
            compileData[NETCompilerResult::P_SUCCESS] = false;
            compileData[NETCompilerResult::P_ERRORTEXT] = "NETCompile::Compile - VS140COMNTOOLS environment variable not found, cannot proceed";
            SendEvent(E_NETCOMPILERESULT, compileData);
            return false;
        }

        // This will need to be fixed for deployed build
        String nugetBin = ToString("%s/Build/Managed/nuget/nuget", tenv->GetRootSourceDir().CString());
            
        String vcvars64 = ToString("%s..\\..\\VC\\bin\\amd64\\vcvars64.bat", cmdToolsPath.CString());
       
        String cmd = "cmd";
        Vector<String> args;
        args.Push("/A");
        args.Push("/C");        
        args.Push(ToString("\"\"%s\" && \"%s\" restore \"%s\" && msbuild \"%s\" /p:Configuration=%s /p:Platform=\"Any CPU\"\"", 
                            vcvars64.CString(), nugetBin.CString(), solutionPath.CString(), solutionPath.CString(), configuration.CString()));


        allArgs_.Join(args, " ");
        
        LOGINFOF("Compiling NET Solution: %s", solutionPath.CString());

        Subprocess* subprocess = nullptr;

        try
        {
            subprocess = subs->Launch(cmd, args, "C:\\");
        }
        catch (Poco::SystemException)
        {
            subprocess = nullptr;
        }        

        if (!subprocess)
        {            
            String allArgs;
            
            compileData[NETCompilerResult::P_SUCCESS] = false;
            compileData[NETCompilerResult::P_ERRORTEXT] = ToString("NETCompile::Compile - Unable to launch MSBuild subprocess\n%s", allArgs_.CString());
            SendEvent(E_NETCOMPILERESULT, compileData);            
            return false;
        }

        SubscribeToEvent(subprocess, E_SUBPROCESSCOMPLETE, HANDLER(NETCompile, HandleCompileProcessComplete));
        SubscribeToEvent(subprocess, E_SUBPROCESSOUTPUT, HANDLER(NETCompile, HandleSubprocessOutput));
        
        return true;
    }


}
