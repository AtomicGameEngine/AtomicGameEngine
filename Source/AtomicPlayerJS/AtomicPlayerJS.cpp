
#include <AtomicJS/Javascript/JSVM.h>
#include <AtomicPlayer/Player.h>

using namespace Atomic;
using namespace AtomicPlayer;

namespace Atomic
{

    extern void jsb_package_atomicplayer_init(JSVM* vm);

}

namespace AtomicPlayer
{

void jsapi_init_atomicplayer(JSVM* vm)
{
    duk_context* ctx = vm->GetJSContext();

    jsb_package_atomicplayer_init(vm);

    duk_get_global_string(ctx, "Atomic");

    js_push_class_object_instance(ctx, vm->GetSubsystem<Player>(), "Player");
    duk_put_prop_string(ctx, -2, "player");

    duk_pop(ctx);

}

}


