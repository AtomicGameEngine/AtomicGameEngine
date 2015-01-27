//
// Copyright (c) 2008-2014 the Urho3D project.
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

#include <Atomic/Atomic.h>
#include <Atomic/Engine/Engine.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Core/Main.h>
#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/ResourceEvents.h>

// Move me
#include <Atomic/Environment/Environment.h>

#include <Atomic/Javascript/Javascript.h>
#include <Atomic/Javascript/JSFile.h>

#include "AtomicPlayer.h"

#include <Atomic/DebugNew.h>

#include <Atomic/UI/TBUI.h>

#ifdef EMSCRIPTEN

#include "emscripten.h"

class EmscriptenApp
{
public:
    Atomic::SharedPtr<Atomic::Context> context_;
    Atomic::SharedPtr<AtomicPlayer> application_;

    static EmscriptenApp* sInstance_;

    EmscriptenApp() : context_(new Atomic::Context()), application_(new AtomicPlayer(context_))
    {
        sInstance_ = this;
    }

};

EmscriptenApp* EmscriptenApp::sInstance_ = NULL;

static void RunFrame()
{
    Engine* engine = EmscriptenApp::sInstance_->application_->GetSubsystem<Engine>();
    if (engine->IsInitialized())
        engine->RunFrame();
    else
        printf("ENGINE NOT INITIALIZED\n");
}

int main(int argc, char** argv)
{    
    Atomic::ParseArguments(argc, argv);
    // leak
    new EmscriptenApp();

    EmscriptenApp::sInstance_->application_->Run();

    int firefox = EM_ASM_INT ( return ((navigator.userAgent.toLowerCase().indexOf('firefox') > -1) ? 1 : 0), 0);
    if (firefox)
        emscripten_set_main_loop(RunFrame, 0, 1);
    else
        emscripten_set_main_loop(RunFrame, 60, 1);
    return 0;
}


#else

DEFINE_APPLICATION_MAIN(AtomicPlayer);

#endif

// fixme
static JSVM* vm = NULL;
static Javascript* javascript = NULL;

AtomicPlayer::AtomicPlayer(Context* context) :
    Application(context)
{
    RegisterEnvironmenttLibrary(context_);
}

void AtomicPlayer::Setup()
{
    FileSystem* filesystem = GetSubsystem<FileSystem>();       

    // On Android and iOS, read command line from a file as parameters can not otherwise be easily given
/*
    #if defined(ANDROID) || defined(IOS)
    SharedPtr<File> commandFile(new File(context_, filesystem->GetProgramDir() + "Data/CommandLine.txt",
        FILE_READ));
    String commandLine = commandFile->ReadLine();
    commandFile->Close();
    ParseArguments(commandLine, false);
    // Reparse engine startup parameters now
    engineParameters_ = Engine::ParseParameters(GetArguments());
    #endif
*/
    // Check for script file name
    const Vector<String>& arguments = GetArguments();    
    for (unsigned i = 0; i < arguments.Size(); ++i)
    {
        if (arguments[i][0] != '-')
        {
            scriptFileName_ = GetInternalPath(arguments[i]);
            break;
        }
    }

#ifdef EMSCRIPTEN
    engineParameters_["WindowWidth"] = 1280;
    engineParameters_["WindowHeight"] = 720;
    engineParameters_["FullScreen"] = false;    
    scriptFileName_ = "PhysicsPlatformer.js";

#endif
    scriptFileName_ = "Script/Main.js";

    FileSystem* fileSystem = GetSubsystem<FileSystem>();    
    engineParameters_["WindowTitle"] = "AtomicPlayer";

#if (ATOMIC_PLATFORM_ANDROID)
    engineParameters_["FullScreen"] = true;
    engineParameters_["ResourcePaths"] = "CoreData;Data;AtomicResources";
#else
    engineParameters_["ResourcePaths"] = "AtomicResources";
    engineParameters_["FullScreen"] = false;
    engineParameters_["WindowWidth"] = 1280;
    engineParameters_["WindowHeight"] = 720;
#endif

#if ATOMIC_PLATFORM_WINDOWS
    engineParameters_["ResourcePrefixPath"] = "AtomicPlayer_Resources";
#elif ATOMIC_PLATFORM_ANDROID
    //engineParameters_["ResourcePrefixPath"] = "assets";
#elif ATOMIC_PLATFORM_OSX
    engineParameters_["ResourcePrefixPath"] = "../Resources";
#endif

    // Show usage if not found
    if (scriptFileName_.Empty())
    {
        ErrorExit("Usage: AtomicPlayer <scriptfile> [options]\n\n"
            "The script file should implement the function void Start() for initializing the "
            "application and subscribing to all necessary events, such as the frame update.\n"
            #ifndef WIN32
            "\nCommand line options:\n"
            "-x <res>     Horizontal resolution\n"
            "-y <res>     Vertical resolution\n"
            "-m <level>   Enable hardware multisampling\n"
            "-v           Enable vertical sync\n"
            "-t           Enable triple buffering\n"
            "-w           Start in windowed mode\n"
            "-s           Enable resizing when in windowed mode\n"
            "-q           Enable quiet mode which does not log to standard output stream\n"
            "-b <length>  Sound buffer length in milliseconds\n"
            "-r <freq>    Sound mixing frequency in Hz\n"
            "-p <paths>   Resource path(s) to use, separated by semicolons\n"
            "-ap <paths>  Autoload resource path(s) to use, seperated by semicolons\n"
            "-log <level> Change the log level, valid 'level' values are 'debug', 'info', 'warning', 'error'\n"
            "-ds <file>   Dump used shader variations to a file for precaching\n"
            "-mq <level>  Material quality level, default 2 (high)\n"
            "-tq <level>  Texture quality level, default 2 (high)\n"
            "-tf <level>  Texture filter mode, default 2 (trilinear)\n"
            "-af <level>  Texture anisotropy level, default 4. Also sets anisotropic filter mode\n"
            "-flushgpu    Flush GPU command queue each frame. Effective only on Direct3D9\n"
            "-borderless  Borderless window mode\n"
            "-headless    Headless mode. No application window will be created\n"
            "-landscape   Use landscape orientations (iOS only, default)\n"
            "-portrait    Use portrait orientations (iOS only)\n"
            "-prepass     Use light pre-pass rendering\n"
            "-deferred    Use deferred rendering\n"
            "-lqshadows   Use low-quality (1-sample) shadow filtering\n"
            "-noshadows   Disable shadow rendering\n"
            "-nolimit     Disable frame limiter\n"
            "-nothreads   Disable worker threads\n"
            "-nosound     Disable sound output\n"
            "-noip        Disable sound mixing interpolation\n"
            "-sm2         Force SM2.0 rendering\n"
            "-touch       Touch emulation on desktop platform\n"
            #endif
        );
    }
    else
    {
        // Use the script file name as the base name for the log file
        engineParameters_["LogName"] = filesystem->GetAppPreferencesDir("urho3d", "logs") + GetFileNameAndExtension(scriptFileName_) + ".log";
    }
}

void AtomicPlayer::Start()
{
    String extension = GetExtension(scriptFileName_);

    if (extension == ".js")
    {

        // Instantiate and register the Javascript subsystem
        javascript = new Javascript(context_);
        context_->RegisterSubsystem(javascript);

        vm = javascript->InstantiateVM("MainVM");

        vm->SetModuleSearchPath("Modules");

        duk_eval_string_noresult(vm->GetJSContext(), "require(\"AtomicGame\");");

        // Hold a shared pointer to the script file to make sure it is not unloaded during runtime
        jsFile_ = GetSubsystem<ResourceCache>()->GetResource<JSFile>(scriptFileName_);        

        // TODO: Live Reload
        // Subscribe to script's reload event to allow live-reload of the application
        //SubscribeToEvent(jsFile_, E_RELOADSTARTED, HANDLER(AtomicPlayer, HandleScriptReloadStarted));
        //SubscribeToEvent(jsFile_, E_RELOADFINISHED, HANDLER(AtomicPlayer, HandleScriptReloadFinished));
        //SubscribeToEvent(jsFile_, E_RELOADFAILED, HANDLER(AtomicPlayer, HandleScriptReloadFailed));

        if (jsFile_)
        {
            bool ok = vm->ExecuteFile(jsFile_);
            if (!ok)
            {
                ErrorExit("Error executing Javascript file");
                return;
            }

            LOGINFOF("Starting %s", scriptFileName_.CString());

            vm->ExecuteFunction("Start");
        }
        else
        {
            LOGERROR("Poop");
            ErrorExit("Error loading Javascript file");
            return;
        }

        return;

        //javascript->ShutdownVM("MainVM");
    }


    // The script was not successfully loaded. Show the last error message and do not run the main loop
    ErrorExit();
}

void AtomicPlayer::Stop()
{
    context_->RemoveSubsystem<Javascript>();
    /*
    LuaScript* luaScript = GetSubsystem<LuaScript>();
    if (luaScript && luaScript->GetFunction("Stop", true))
        luaScript->ExecuteFunction("Stop");
    */
}

void AtomicPlayer::HandleScriptReloadStarted(StringHash eventType, VariantMap& eventData)
{
}

void AtomicPlayer::HandleScriptReloadFinished(StringHash eventType, VariantMap& eventData)
{
}

void AtomicPlayer::HandleScriptReloadFailed(StringHash eventType, VariantMap& eventData)
{
    ErrorExit();
}
