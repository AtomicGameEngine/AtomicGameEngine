
#include "../Atomic.h"
#include "../Core/ProcessUtils.h"
#include "../Resource/ResourceCache.h"

#include "JSBind.h"
#include "JSBindings.h"

#ifdef WIN32
#include <Windows.h>
#endif

using namespace Atomic;

SharedPtr<Context> JSBind::context_;
SharedPtr<FileSystem> JSBind::fileSystem_;
SharedPtr<Engine> JSBind::engine_;

#ifdef WIN32
String JSBind::ROOT_FOLDER("C:/Dev/atomic/AtomicRuntime");
#else
String JSBind::ROOT_FOLDER("/Users/josh/Dev/atomic/AtomicRuntime");
#endif

void JSBind::Initialize()
{
    context_ = new Context();
    fileSystem_ = new FileSystem(context_);
    engine_ = new Engine(context_);

}

void Run(const Vector<String>& arguments)
{
    JSBind::Initialize();
    VariantMap engineParameters;
    engineParameters["Headless"] = true;
    engineParameters["WorkerThreads"] = false;
    engineParameters["LogName"] = String::EMPTY;

    JSBind::engine_->Initialize(engineParameters);

    ResourceCache* cache = JSBind::context_->GetSubsystem<ResourceCache>();
    cache->AddResourceDir(JSBind::ROOT_FOLDER + "/Source/Tools/JSBind");

    JSBindings* bindings = new JSBindings();
    bindings->Initialize();
    bindings->ParseHeaders();
}

int main(int argc, char** argv)
{
    Vector<String> arguments;

#ifdef WIN32
    arguments = ParseArguments(GetCommandLineW());
#else
    arguments = ParseArguments(argc, argv);
#endif

    Run(arguments);
    return 0;
}

