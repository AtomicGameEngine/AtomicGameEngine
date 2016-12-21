//
// Copyright (c) 2008-2014 the Urho3D project.
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

#include <Atomic/Engine/Engine.h>
#include <Atomic/Engine/EngineConfig.h>
#include <Atomic/IO/FileSystem.h>

#include "NETCore.h"
#include <AtomicNET/NETScript/NETScript.h>

#include "NETAtomicPlayer.h"

#ifdef ATOMIC_PLATFORM_OSX
#include <unistd.h>
#endif

#ifdef ATOMIC_PLATFORM_WINDOWS
#include <stdio.h>
#endif

// Android: uses SDL_main, defined for IOS only to satifsy linker
#if defined(__ANDROID__) || defined(IOS)

typedef int(*sdl_entry_callback)();

static sdl_entry_callback sdlEntryCallback = 0;

extern "C" void RegisterSDLEntryCallback(sdl_entry_callback callback)
{
    sdlEntryCallback = callback;
}

extern "C" int SDL_main(int argc, char** argv);
int SDL_main(int argc, char** argv)
{
#if defined(__ANDROID__)
    Atomic::ParseArguments(argc, argv);
    if (sdlEntryCallback != 0)
    {
        return sdlEntryCallback();
    }
#endif    
    return 0;
}
#endif

namespace Atomic
{

    NETAtomicPlayer::NETAtomicPlayer(Context* context) :
        PlayerApp(context)
    {

    }

    void NETAtomicPlayer::Setup()
    {
        PlayerApp::Setup();

        engineParameters_["ResourcePaths"] = "AtomicResources";
    }

    int NETAtomicPlayer::Initialize()
    {
        Setup();

        // Read the engine configuration
        ReadEngineConfig();

        RegisterNETScriptLibrary(context_);

        if (exitCode_)
            return exitCode_;

        if (!engine_->Initialize(engineParameters_))
        {
            ErrorExit();
            return exitCode_;
        }

        Start();

        if (exitCode_)
            return exitCode_;

        return 0;
    }

    void NETAtomicPlayer::Stop()
    {
        PlayerApp::Stop();
    }

    NETAtomicPlayer* NETAtomicPlayer::CreateInternal()
    {
        return new NETAtomicPlayer(NETCore::GetContext());
    }

    void NETAtomicPlayer::ReadEngineConfig()
    {
        FileSystem* fileSystem = GetSubsystem<FileSystem>();

#ifdef ATOMIC_PLATFORM_OSX
        String filename = fileSystem->GetProgramDir() + "../Resources/Settings/Engine.json";
#else
        String filename = fileSystem->GetProgramDir() + "Settings/Engine.json";
#endif

        if (!fileSystem->FileExists(filename))
            return;

        if (EngineConfig::LoadFromFile(context_, filename))
        {
            EngineConfig::ApplyConfig(engineParameters_, true);
        }
    }


}
