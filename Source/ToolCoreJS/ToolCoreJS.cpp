
#include <AtomicJS/Javascript/JSVM.h>
#include <ToolCore/ToolEnvironment.h>
#include <ToolCore/ToolSystem.h>

using namespace Atomic;

namespace Atomic
{

    extern void jsb_package_toolcore_init(JSVM* vm);

}

namespace ToolCore
{

static int js_atomic_GetToolEnvironment(duk_context* ctx)
{
    JSVM* vm = JSVM::GetJSVM(ctx);
    js_push_class_object_instance(ctx, vm->GetSubsystem<ToolEnvironment>());
    return 1;
}

static int js_atomic_GetToolSystem(duk_context* ctx)
{
    JSVM* vm = JSVM::GetJSVM(ctx);
    js_push_class_object_instance(ctx, vm->GetSubsystem<ToolSystem>());
    return 1;
}

void jsapi_init_toolcore(JSVM* vm)
{
    duk_context* ctx = vm->GetJSContext();

    duk_push_object(ctx);
    duk_put_global_string(ctx, "ToolCore");

    jsb_package_toolcore_init(vm);

    duk_get_global_string(ctx, "ToolCore");

    duk_push_c_function(ctx, js_atomic_GetToolEnvironment, 0);
    duk_put_prop_string(ctx, -2, "getToolEnvironment");

    duk_push_c_function(ctx, js_atomic_GetToolSystem, 0);
    duk_put_prop_string(ctx, -2, "getToolSystem");

    duk_pop(ctx);

}

}


