
#include <AtomicJS/Javascript/JSVM.h>

using namespace Atomic;

namespace Atomic
{
    extern void jsb_package_editor_init(JSVM* vm);
}

namespace AtomicEditor
{

void jsapi_init_editor(JSVM* vm)
{
    duk_context* ctx = vm->GetJSContext();

    duk_push_object(ctx);
    duk_put_global_string(ctx, "Editor");

    jsb_package_editor_init(vm);
}

}


