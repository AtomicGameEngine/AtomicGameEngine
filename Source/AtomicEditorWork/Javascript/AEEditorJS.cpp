
#include <AtomicJS/Javascript/JSVM.h>
#include "../Application/AEPreferences.h"

using namespace Atomic;

namespace Atomic
{
    extern void jsb_package_editor_init(JSVM* vm);
}

namespace AtomicEditor
{

static int Editor_GetPreferences(duk_context* ctx)
{
    JSVM* vm = JSVM::GetJSVM(ctx);
    js_push_class_object_instance(ctx, vm->GetSubsystem<AEPreferences>(), "AEPreferences");
    return 1;
}

void jsapi_init_editor(JSVM* vm)
{
    duk_context* ctx = vm->GetJSContext();

    duk_push_object(ctx);

    duk_push_c_function(ctx, Editor_GetPreferences, 0);
    duk_put_prop_string(ctx, -2, "getPreferences");

    duk_put_global_string(ctx, "Editor");

    jsb_package_editor_init(vm);
}

}


