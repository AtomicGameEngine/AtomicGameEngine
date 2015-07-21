
#include "JSAtomicPlayer.h"

namespace Atomic
{


static int AtomicPlayer_LoadScene(duk_context* ctx) {


    duk_push_boolean(ctx, 1);

    return 1;
}

void jsapi_init_atomicplayer(JSVM* vm)
{
    duk_context* ctx = vm->GetJSContext();

    duk_get_global_string(ctx, "Atomic");

    duk_push_object(ctx);
    duk_dup_top(ctx);
    duk_put_prop_string(ctx, -3, "Player");


    duk_pop(ctx); // pop Atomic object

}

}
