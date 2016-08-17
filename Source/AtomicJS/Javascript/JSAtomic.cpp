//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
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

#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Input/Input.h>
#include <Atomic/Graphics/Renderer.h>
#include <Atomic/Graphics/Graphics.h>
#include <Atomic/Engine/Engine.h>
#include <Atomic/Audio/Audio.h>
#include <Atomic/UI/UI.h>

#ifdef ATOMIC_NETWORK
#include <Atomic/Network/Network.h>
#endif

#ifdef ATOMIC_WEB
#include <Atomic/Web/Web.h>
#endif

#include "JSEvents.h"
#include "JSVM.h"
#include "JSComponent.h"
#include "JSCore.h"
#include "JSFileSystem.h"
#include "JSGraphics.h"
#ifdef ATOMIC_3D
#include "JSAtomic3D.h"
#endif
#include "JSIO.h"
#include "JSInput.h"
#include "JSUIAPI.h"
#include "JSScene.h"

#ifdef ATOMIC_NETWORK
#include "JSNetwork.h"
#endif

#include "JSAtomicPlayer.h"
#include "JSAtomic.h"

#include <Atomic/Scene/Scene.h>
#include <Atomic/Environment/ProcSky.h>

#include <AtomicBuildInfo/AtomicBuildInfo.h>

namespace Atomic
{

extern void jsb_package_atomic_init(JSVM* vm);

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

    ATOMIC_LOGINFOF("%s", duk_to_string(ctx, -1));
    return 0;
}

static int js_openConsoleWindow(duk_context* ctx)
{
#ifdef _WIN32
    OpenConsoleWindow();
#endif

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

static int js_atomic_GetInput(duk_context* ctx)
{
    JSVM* vm = JSVM::GetJSVM(ctx);
    js_push_class_object_instance(ctx, vm->GetSubsystem<Input>());
    return 1;
}

static int js_atomic_GetFileSystem(duk_context* ctx)
{
    JSVM* vm = JSVM::GetJSVM(ctx);
    js_push_class_object_instance(ctx, vm->GetSubsystem<FileSystem>());
    return 1;
}

#ifdef ATOMIC_NETWORK
static int js_atomic_GetNetwork(duk_context* ctx)
{
    JSVM* vm = JSVM::GetJSVM(ctx);
    js_push_class_object_instance(ctx, vm->GetSubsystem<Network>());
    return 1;
}
#endif

#ifdef ATOMIC_WEB
static int js_atomic_GetWeb(duk_context* ctx)
{
  JSVM* vm = JSVM::GetJSVM(ctx);
  js_push_class_object_instance(ctx, vm->GetSubsystem<Web>());
  return 1;
}
#endif

static int js_atomic_GetUI(duk_context* ctx)
{
    JSVM* vm = JSVM::GetJSVM(ctx);
    js_push_class_object_instance(ctx, vm->GetSubsystem<UI>());
    return 1;
}

static int js_atomic_GetGitRevision(duk_context* ctx)
{
    duk_push_string(ctx, GetGitSHA());
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

static void js_atomic_destroy_node(Node* node, duk_context* ctx, bool root = false)
{

    if (root)
    {
        PODVector<Node*> children;
        node->GetChildren(children, true);

        for (unsigned i = 0; i < children.Size(); i++)
        {
            if (children.At(i)->JSGetHeapPtr())
                js_atomic_destroy_node(children.At(i), ctx);
        }
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
    node->Remove();

}

static void js_atomic_destroy_scene(Scene* scene, duk_context* ctx)
{
    js_atomic_destroy_node(scene, ctx, true);
}

static int js_atomic_destroy(duk_context* ctx)
{
    if (!duk_is_object(ctx, 0))
        return 0;

    Object* obj = js_to_class_instance<Object>(ctx, 0, 0);

    if (!obj)
        return 0;

    if (obj->GetType() == Node::GetTypeStatic())
    {
        Node* node = (Node*) obj;
        js_atomic_destroy_node(node, ctx, true);
        return 0;
    }
    if (obj->GetType() == Scene::GetTypeStatic())
    {
        Scene* scene = (Scene*) obj;
        js_atomic_destroy_scene(scene, ctx);
        return 0;
    }
    else if (obj->GetType() == JSComponent::GetTypeStatic())
    {
        // FIXME: want to be able to destroy a single component
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
    jsb_package_atomic_init(vm);

    // extensions
    jsapi_init_core(vm);
    jsapi_init_filesystem(vm);
    jsapi_init_io(vm);
#ifdef ATOMIC_NETWORK
    jsapi_init_network(vm);
#endif
    jsapi_init_graphics(vm);
#ifdef ATOMIC_3D
    jsapi_init_atomic3d(vm);
#endif
    jsapi_init_input(vm);
    jsapi_init_ui(vm);
    jsapi_init_scene(vm);

    jsapi_init_atomicplayer(vm);

    duk_context* ctx = vm->GetJSContext();

    // globals
    duk_push_global_object(ctx);

    // console.log
    duk_push_object(ctx);
    duk_dup(ctx, -1);
    duk_put_prop_string(ctx, -3, "console");
    duk_push_c_function(ctx, js_print, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "log");
    duk_pop(ctx);

    duk_push_c_function(ctx, js_print, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "print");
    duk_push_c_function(ctx, js_assert, 1);
    duk_put_prop_string(ctx, -2, "assert");
    duk_push_c_function(ctx, js_module_read_file, 1);
    duk_put_prop_string(ctx, -2, "js_module_read_file");
    duk_pop(ctx);

    // Atomic
    duk_get_global_string(ctx, "Atomic");

    // Atomic.print
    duk_push_c_function(ctx, js_print, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "print");

    String platform = GetPlatform();
    if (platform == "Mac OS X")
        platform = "MacOSX";

    duk_push_string(ctx, platform.CString());
    duk_put_prop_string(ctx, -2, "platform");

    // Node registry
    duk_push_global_stash(ctx);
    duk_push_object(ctx);
    duk_put_prop_index(ctx, -2, JS_GLOBALSTASH_VARIANTMAP_CACHE);
    duk_push_object(ctx);
    duk_put_prop_index(ctx, -2, JS_GLOBALSTASH_INDEX_REFCOUNTED_REGISTRY);
    duk_pop(ctx);

    duk_push_c_function(ctx, js_openConsoleWindow, 0);
    duk_put_prop_string(ctx, -2, "openConsoleWindow");

    // subsystems

    duk_push_c_function(ctx, js_atomic_GetEngine, 0);
    duk_put_prop_string(ctx, -2, "getEngine");

    js_push_class_object_instance(ctx, vm->GetSubsystem<Engine>(), "Engine");
    duk_put_prop_string(ctx, -2, "engine");

    duk_push_c_function(ctx, js_atomic_GetGraphics, 0);
    duk_put_prop_string(ctx, -2, "getGraphics");

    js_push_class_object_instance(ctx, vm->GetSubsystem<Graphics>(), "Graphics");
    duk_put_prop_string(ctx, -2, "graphics");

    duk_push_c_function(ctx, js_atomic_GetRenderer, 0);
    duk_put_prop_string(ctx, -2, "getRenderer");

    js_push_class_object_instance(ctx, vm->GetSubsystem<Renderer>(), "Renderer");
    duk_put_prop_string(ctx, -2, "renderer");

    duk_push_c_function(ctx, js_atomic_GetResourceCache, 0);
    duk_put_prop_string(ctx, -2, "getResourceCache");

    js_push_class_object_instance(ctx, vm->GetSubsystem<ResourceCache>(), "ResourceCache");
    duk_put_prop_string(ctx, -2, "cache");

    duk_push_c_function(ctx, js_atomic_GetInput, 0);
    duk_put_prop_string(ctx, -2, "getInput");

    js_push_class_object_instance(ctx, vm->GetSubsystem<Audio>(), "Audio");
    duk_put_prop_string(ctx, -2, "audio");

    js_push_class_object_instance(ctx, vm->GetSubsystem<Input>(), "Input");
    duk_put_prop_string(ctx, -2, "input");

    duk_push_c_function(ctx, js_atomic_GetFileSystem, 0);
    duk_put_prop_string(ctx, -2, "getFileSystem");

    js_push_class_object_instance(ctx, vm->GetSubsystem<FileSystem>(), "FileSystem");
    duk_put_prop_string(ctx, -2, "fileSystem");

#ifdef ATOMIC_NETWORK
    duk_push_c_function(ctx, js_atomic_GetNetwork, 0);
    duk_put_prop_string(ctx, -2, "getNetwork");

    js_push_class_object_instance(ctx, vm->GetSubsystem<Network>(), "Network");
    duk_put_prop_string(ctx, -2, "network");
#endif

#ifdef ATOMIC_WEB
    duk_push_c_function(ctx, js_atomic_GetWeb, 0);
    duk_put_prop_string(ctx, -2, "getWeb");

    js_push_class_object_instance(ctx, vm->GetSubsystem<Web>(), "Web");
    duk_put_prop_string(ctx, -2, "web");
#endif

    duk_push_c_function(ctx, js_atomic_GetUI, 0);
    duk_put_prop_string(ctx, -2, "getUI");

    js_push_class_object_instance(ctx, vm->GetSubsystem<UI>(), "UI");
    duk_put_prop_string(ctx, -2, "ui");

    // end subsystems
    duk_push_c_function(ctx, js_atomic_GetGitRevision, 0);
    duk_put_prop_string(ctx, -2, "getGitRevision");

    duk_push_c_function(ctx, js_atomic_script, 1);
    duk_put_prop_string(ctx, -2, "script");

    duk_push_c_function(ctx, js_atomic_destroy, 1);
    duk_put_prop_string(ctx, -2, "destroy");


    duk_pop(ctx);


}

}
