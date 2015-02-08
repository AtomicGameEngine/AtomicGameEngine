
#include "Precompiled.h"
#include "../IO/FileSystem.h"
#include "../IO/Log.h"
#include "../Resource/ResourceCache.h"
#include "../Input/Input.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/Graphics.h"

#include "../UI/UI.h"

#include "../Engine/Engine.h"

#include "../Javascript/JSEvents.h"
#include "../Javascript/JSVM.h"
#include "../Javascript/JSComponent.h"
#include "../Javascript/JSGraphics.h"
#include "../Javascript/JSScene.h"

#include "../Javascript/JSAtomicGame.h"
#include "../Javascript/JSAtomic.h"

#include "../Scene/Scene.h"
#include "../Environment/ProcSky.h"

namespace Atomic
{

extern void jsb_modules_init(JSVM* vm);

static int js_module_read_file(duk_context* ctx)
{
    JSVM* vm = JSVM::GetJSVM(ctx);

    ResourceCache* cache = vm->GetContext()->GetSubsystem<ResourceCache>();

    String path = duk_to_string(ctx, 0);

    SharedPtr<File> file = cache->GetFile(path);

    if (!file->IsOpen())
    {
        duk_push_string(ctx, "Unable to open module file");
        duk_throw(ctx);
        return 0;
    }

    unsigned size = file->GetSize();

    SharedArrayPtr<char> data;
    data = new char[size + 1];
    data[size] = '\0';
    file->Read(data, size);

    duk_push_string(ctx, data);

    return 1;
}

static int js_print(duk_context* ctx)
{
    duk_concat(ctx, duk_get_top(ctx));

    VariantMap eventData;
    using namespace JSPrint;
    eventData[P_TEXT] =  duk_to_string(ctx, -1);

    JSVM* vm = JSVM::GetJSVM(ctx);
    vm->SendEvent(E_JSPRINT, eventData);

    LOGINFOF("%s", duk_to_string(ctx, -1));
    return 0;
}

static int js_assert(duk_context* ctx)
{
    if (!duk_to_boolean(ctx, 0))
    {
        assert(0);
    }
    return 0;
}


static int js_atomic_GetEngine(duk_context* ctx)
{
    JSVM* vm = JSVM::GetJSVM(ctx);
    js_push_class_object_instance(ctx, vm->GetSubsystem<Engine>());
    return 1;
}

static int js_atomic_GetResourceCache(duk_context* ctx)
{
    JSVM* vm = JSVM::GetJSVM(ctx);
    js_push_class_object_instance(ctx, vm->GetSubsystem<ResourceCache>());
    return 1;
}

static int js_atomic_GetRenderer(duk_context* ctx)
{
    JSVM* vm = JSVM::GetJSVM(ctx);
    js_push_class_object_instance(ctx, vm->GetSubsystem<Renderer>());
    return 1;
}

static int js_atomic_GetGraphics(duk_context* ctx)
{
    JSVM* vm = JSVM::GetJSVM(ctx);
    js_push_class_object_instance(ctx, vm->GetSubsystem<Graphics>());
    return 1;
}

static int js_atomic_GetUI(duk_context* ctx)
{
    JSVM* vm = JSVM::GetJSVM(ctx);
    js_push_class_object_instance(ctx, vm->GetSubsystem<UI>());
    return 1;
}

static int js_atomic_GetInput(duk_context* ctx)
{
    JSVM* vm = JSVM::GetJSVM(ctx);
    js_push_class_object_instance(ctx, vm->GetSubsystem<Input>());
    return 1;
}

static int js_atomic_script(duk_context* ctx)
{
    JSVM* vm = JSVM::GetJSVM(ctx);

    if (duk_is_string(ctx, 0))
    {
        if ( vm->ExecuteScript(duk_to_string(ctx, 0)))
            duk_push_boolean(ctx, 1);
        else
            duk_push_boolean(ctx, 0);
    }
    else
        duk_push_boolean(ctx, 0);

    return 1;
}


static int js_atomic_destroy(duk_context* ctx)
{
    if (!duk_is_object(ctx, 0))
        return 0;

    Object* obj = js_to_class_instance<Object>(ctx, 0, 0);

    if (!obj)
        return 0;

    JSVM* vm = JSVM::GetJSVM(ctx);

    if (obj->GetType() == Node::GetTypeStatic())
    {

        Node* node = (Node*) obj;

        if (node->JSGetHeapPtr())
        {
            int top = duk_get_top(ctx);
            duk_push_global_stash(ctx);
            duk_get_prop_index(ctx, -1, JS_GLOBALSTASH_INDEX_NODE_REGISTRY);
            duk_push_pointer(ctx, node->JSGetHeapPtr());
            duk_del_prop(ctx, -2);
            duk_pop_2(ctx);
            assert(top = duk_get_top(ctx));
        }

        const Vector<SharedPtr<Component> >& components = node->GetComponents();

        for (unsigned i = 0; i < components.Size(); i++)
        {
             Component* component = components[i];

             if (component->GetType() == JSComponent::GetTypeStatic())
             {
                 JSComponent* jscomponent = (JSComponent*) component;
                 jscomponent->SetDestroyed();
             }

             component->UnsubscribeFromAllEvents();
        }

        node->RemoveAllComponents();
        node->UnsubscribeFromAllEvents();

        assert(node->Refs() >= 2);

        node->Remove();

        return 0;
    }
    else if (obj->GetType() == JSComponent::GetTypeStatic())
    {
        assert(0);
        JSComponent* component = (JSComponent*) obj;
        component->UnsubscribeFromAllEvents();
        component->Remove();
        return 0;
    }

    return 0;
}

void jsapi_init_atomic(JSVM* vm)
{
    // core modules
    jsb_modules_init(vm);

    // extensions
    jsapi_init_graphics(vm);
    jsapi_init_scene(vm);

    jsapi_init_atomicgame(vm);

    duk_context* ctx = vm->GetJSContext();

    // globals
    duk_push_global_object(ctx);
    duk_push_c_function(ctx, js_print, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "print");
    duk_push_c_function(ctx, js_assert, 1);
    duk_put_prop_string(ctx, -2, "assert");
    duk_push_c_function(ctx, js_module_read_file, 1);
    duk_put_prop_string(ctx, -2, "js_module_read_file");
    duk_pop(ctx);

    // Atomic
    duk_get_global_string(ctx, "Atomic");

#ifdef ATOMIC_PLATFORM_OSX
    duk_push_string(ctx, "Mac");
    duk_put_prop_string(ctx, -2, "platform");
#elif ATOMIC_PLATFORM_WINDOWS
    duk_push_string(ctx, "Windows");
    duk_put_prop_string(ctx, -2, "platform");
#elif ATOMIC_PLATFORM_ANDROID
    duk_push_string(ctx, "Android");
    duk_put_prop_string(ctx, -2, "platform");
#endif

    // Node registry
    duk_push_global_stash(ctx);
    duk_push_object(ctx);
    duk_put_prop_index(ctx, -2, JS_GLOBALSTASH_INDEX_NODE_REGISTRY);
    duk_pop(ctx);

    duk_push_c_function(ctx, js_atomic_GetEngine, 0);
    duk_put_prop_string(ctx, -2, "GetEngine");

    duk_push_c_function(ctx, js_atomic_GetGraphics, 0);
    duk_put_prop_string(ctx, -2, "GetGraphics");

    duk_push_c_function(ctx, js_atomic_GetRenderer, 0);
    duk_put_prop_string(ctx, -2, "GetRenderer");

    duk_push_c_function(ctx, js_atomic_GetResourceCache, 0);
    duk_put_prop_string(ctx, -2, "GetResourceCache");

    duk_push_c_function(ctx, js_atomic_GetInput, 0);
    duk_put_prop_string(ctx, -2, "GetInput");

    duk_push_c_function(ctx, js_atomic_GetUI, 0);
    duk_put_prop_string(ctx, -2, "GetUI");

    duk_push_c_function(ctx, js_atomic_script, 1);
    duk_put_prop_string(ctx, -2, "script");

    duk_push_c_function(ctx, js_atomic_destroy, 1);
    duk_put_prop_string(ctx, -2, "destroy");


    duk_pop(ctx);


}

}
