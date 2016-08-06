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


#include <Atomic/Input/InputEvents.h>
#include <Atomic/Engine/Engine.h>
#include <Atomic/Graphics/Graphics.h>
#include <Atomic/UI/UI.h>
#include <AtomicJS/Javascript/Javascript.h>

#include <AtomicPlayer/Player.h>

#include "PlayerApp.h"

namespace AtomicPlayer
{
    extern void jsapi_init_atomicplayer(JSVM* vm);
}

namespace Atomic
{

    PlayerApp::PlayerApp(Context* context) :
        AppBase(context),
        executeJSMain_(true)
    {

    }

    PlayerApp::~PlayerApp()
    {

    }

    void PlayerApp::Setup()
    {
        AppBase::Setup();

        engineParameters_.InsertNew("WindowTitle", "AtomicPlayer");

#if (ATOMIC_PLATFORM_ANDROID)
        engineParameters_.InsertNew("FullScreen", true);
        engineParameters_.InsertNew("ResourcePaths", "CoreData;PlayerData;Cache;AtomicResources");
#elif ATOMIC_PLATFORM_WEB
        engineParameters_.InsertNew("FullScreen", false);
        engineParameters_.InsertNew("ResourcePaths", "AtomicResources");
        // engineParameters_.InsertNew("WindowWidth", 1280);
        // engineParameters_.InsertNew("WindowHeight", 720);
#elif ATOMIC_PLATFORM_IOS
        engineParameters_.InsertNew("FullScreen", false);
        engineParameters_.InsertNew("ResourcePaths", "AtomicResources");
#else
        engineParameters_.InsertNew("FullScreen", false);
        engineParameters_.InsertNew("WindowWidth", 1280);
        engineParameters_.InsertNew("WindowHeight", 720);
        engineParameters_.InsertNew("ResourcePaths", "AtomicResources");
#endif

#if ATOMIC_PLATFORM_WINDOWS || ATOMIC_PLATFORM_LINUX

        engineParameters_.InsertNew("WindowIcon", "Images/AtomicLogo32.png");
        engineParameters_.InsertNew("ResourcePrefixPaths", "AtomicPlayer_Resources");

#elif ATOMIC_PLATFORM_ANDROID
        //engineParameters_.InsertNew("ResourcePrefixPaths"], "assets");
#elif ATOMIC_PLATFORM_OSX
        engineParameters_.InsertNew("ResourcePrefixPaths", "../Resources");
#endif

        // Setup player log
        FileSystem *filesystem = GetSubsystem<FileSystem>();
        engineParameters_.InsertNew("LogName", filesystem->GetAppPreferencesDir("AtomicPlayer", "Logs") + "AtomicPlayer.log");

        // Register JS packages
        JSVM::RegisterPackage(AtomicPlayer::jsapi_init_atomicplayer);

    }

    void PlayerApp::Start()
    {
        UI* ui = GetSubsystem<UI>();
        ui->Initialize("DefaultUI/language/lng_en.tb.txt");
        ui->LoadDefaultPlayerSkin();

        vm_->SetModuleSearchPaths("Modules");    

        // Instantiate and register the Player subsystem
        context_->RegisterSubsystem(new AtomicPlayer::Player(context_));

        AppBase::Start();

        if (executeJSMain_)
        {

            JSVM* vm = JSVM::GetJSVM(0);

            if (!vm->ExecuteMain())
            {
                SendEvent(E_EXITREQUESTED);
            }

        }

        GetSubsystem<Graphics>()->RaiseWindow();

    }

    void PlayerApp::Stop()
    {

        AppBase::Stop();

    }

    void PlayerApp::ProcessArguments()
    {

        AppBase::ProcessArguments();

        for (unsigned i = 0; i < arguments_.Size(); ++i)
        {
            if (arguments_[i].Length() > 1)
            {
                String argument = arguments_[i].ToLower();
                String value = i + 1 < arguments_.Size() ? arguments_[i + 1] : String::EMPTY;

                if (argument == "--windowposx" && value.Length())
                {
                    engineParameters_["WindowPositionX"] = atoi(value.CString());
                }
                else if (argument == "--windowposy" && value.Length())
                {
                    engineParameters_["WindowPositionY"] = atoi(value.CString());
                }
                else if (argument == "--windowwidth" && value.Length())
                {
                    engineParameters_["WindowWidth"] = atoi(value.CString());
                }
                else if (argument == "--windowheight" && value.Length())
                {
                    engineParameters_["WindowHeight"] = atoi(value.CString());
                }
                else if (argument == "--resizable")
                {
                    engineParameters_["WindowResizable"] = true;
                }
                else if (argument == "--maximize")
                {
                    engineParameters_["WindowMaximized"] = true;
                }
            }
        }
    }



}


