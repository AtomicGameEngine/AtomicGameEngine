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

#include <Atomic/Input/Input.h>
#include <Atomic/UI/UI.h>
#include <AtomicJS/Javascript/Javascript.h>
#include <Atomic/IPC/IPC.h>

// This can be removed once bone hack is fixed
#include <Atomic/Graphics/AnimatedModel.h>

#include <ToolCore/License/LicenseSystem.h>
#include <ToolCore/ToolSystem.h>
#include <ToolCore/ToolEnvironment.h>
#include <ToolCore/NETTools/NETBuildSystem.h>

#include "../EditorMode/AEEditorMode.h"
#include "../EditorMode/AEEditorNETService.h"

#include "../Components/EditorComponents.h"

#include "AEEditorPrefs.h"
#include "AEEditorApp.h"

using namespace ToolCore;

// Fix these externs
namespace Atomic
{
    void jsapi_init_webview(JSVM* vm, const VariantMap& engineParameters);
    extern void jsb_package_atomicnetscript_init(JSVM* vm);
}

namespace ToolCore
{
    extern void jsapi_init_toolcore(JSVM* vm);
}

using namespace ToolCore;

namespace AtomicEditor
{

    extern void jsapi_init_editor(JSVM* vm);

    AEEditorApp::AEEditorApp(Context* context) :
        AppBase(context)
    {

    }

    AEEditorApp::~AEEditorApp()
    {

    }

    void AEEditorApp::Setup()
    {
        context_->RegisterSubsystem(new AEEditorPrefs(context_));

        context_->SetEditorContext(true);

        AppBase::Setup();

        RegisterEditorComponentLibrary(context_);

        // Register IPC system
        context_->RegisterSubsystem(new IPC(context_));

        ToolEnvironment* env = new ToolEnvironment(context_);
        context_->RegisterSubsystem(env);

#ifdef ATOMIC_DEV_BUILD

        if (!env->InitFromJSON())
        {
            ErrorExit(ToString("Unable to initialize tool environment from %s", env->GetDevConfigFilename().CString()));
            return;
        }
#else

        env->InitFromPackage();

#endif

        ToolSystem* system = new ToolSystem(context_);
        context_->RegisterSubsystem(system);

        engineParameters_["WindowTitle"] = "AtomicEditor";
        engineParameters_["WindowResizable"] = true;
        engineParameters_["FullScreen"] = false;
        engineParameters_["LogLevel"] = LOG_DEBUG;

        FileSystem* filesystem = GetSubsystem<FileSystem>();
        engineParameters_["LogName"] = filesystem->GetAppPreferencesDir("AtomicEditor", "Logs") + "AtomicEditor.log";

#ifdef ATOMIC_PLATFORM_OSX
        engineParameters_["WindowIcon"] = "Images/AtomicLogo32.png";
#endif

#ifdef ATOMIC_DEV_BUILD
        engineParameters_["ResourcePrefixPaths"] = "";
        String resourcePaths = env->GetCoreDataDir() + ";" + env->GetEditorDataDir();
        // for dev builds, add the compile editor scripts from artifacts
        resourcePaths += ";" + env->GetRootSourceDir() + "Artifacts/Build/Resources/EditorData/";
        engineParameters_["ResourcePaths"] = resourcePaths;
#else

#ifdef ATOMIC_PLATFORM_OSX
        engineParameters_["ResourcePrefixPaths"] = "../Resources";

#else
        engineParameters_["ResourcePrefixPaths"] = filesystem->GetProgramDir() + "Resources";
#endif

        engineParameters_["ResourcePaths"] = "CoreData;EditorData";

#endif // ATOMIC_DEV_BUILD

        GetSubsystem<AEEditorPrefs>()->ReadPreferences(engineParameters_);

        // Register JS packages

        JSVM::RegisterPackage(jsapi_init_toolcore);
        JSVM::RegisterPackage(jsapi_init_editor);
        JSVM::RegisterPackage(jsb_package_atomicnetscript_init);

#ifdef ATOMIC_WEBVIEW
        JSVM::RegisterPackage(jsapi_init_webview, engineParameters_);
#endif

    }

    void AEEditorApp::Start()
    {
        GetSubsystem<AEEditorPrefs>()->ValidateWindow();

        context_->RegisterSubsystem(new EditorMode(context_));
        context_->RegisterSubsystem(new NETBuildSystem(context_));
        context_->RegisterSubsystem(new EditorNETService(context_));        

        AppBase::Start();

        vm_->SetModuleSearchPaths("AtomicEditor/JavaScript;AtomicEditor/EditorScripts;AtomicEditor/EditorScripts/AtomicEditor");

        // move UI initialization to JS
        UI* ui = GetSubsystem<UI>();
        ui->Initialize("AtomicEditor/resources/language/lng_en.tb.txt");        

        duk_get_global_string(vm_->GetJSContext(), "require");
        duk_push_string(vm_->GetJSContext(), "main");
        if (duk_pcall(vm_->GetJSContext(), 1) != 0)
        {
            vm_->SendJSErrorEvent();
            ErrorExit("Error executing main.js");
        }

        GetSubsystem<LicenseSystem>()->Initialize();

        Input* input = GetSubsystem<Input>();
        // Ensure exclusive fullscreen is disabled in Editor application
        input->SetToggleFullscreen(false);
        input->SetMouseVisible(true);

    }

    void AEEditorApp::Stop()
    {
        context_->RemoveSubsystem<IPC>();

        AppBase::Stop();
    }

    void AEEditorApp::ProcessArguments()
    {
        AppBase::ProcessArguments();
    }

}
