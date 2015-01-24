
#include "Precompiled.h"
#include "../Javascript/JSScene.h"
#include "../Javascript/JSComponent.h"
#include "../Javascript/JSVM.h"
#include "../Scene/Node.h"

namespace Atomic
{

static int Node_CreateJSComponent(duk_context* ctx)
{
    duk_push_this(ctx);
    Node* node = js_to_class_instance<Node>(ctx, -1, 0);
    JSComponent* jsc = node->CreateComponent<JSComponent>();
    jsc->SetClassName(duk_to_string(ctx, 0));
    js_push_class_object_instance(ctx, jsc, "JSComponent");
    return 1;
}

static int Node_GetChildrenWithComponent(duk_context* ctx)
{
    StringHash type = duk_to_string(ctx, 0);

    bool recursive = false;
    if (duk_get_top(ctx) == 2)
        if (duk_get_boolean(ctx, 1))
            recursive = true;

    duk_push_this(ctx);
    Node* node = js_to_class_instance<Node>(ctx, -1, 0);

    PODVector<Node*> dest;
    node->GetChildrenWithComponent(dest, type, recursive);

    duk_push_array(ctx);

    for (unsigned i = 0; i < dest.Size(); i++)
    {
        js_push_class_object_instance(ctx, dest[i], "Node");
        duk_put_prop_index(ctx, -2, i);
    }

    return 1;
}


void jsapi_init_scene(JSVM* vm)
{
    duk_context* ctx = vm->GetJSContext();

    js_class_get_prototype(ctx, "Node");
    duk_push_c_function(ctx, Node_GetChildrenWithComponent, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "getChildrenWithComponent");
    duk_push_c_function(ctx, Node_CreateJSComponent, 1);
    duk_put_prop_string(ctx, -2, "createJSComponent");
    duk_pop(ctx);


}

}
