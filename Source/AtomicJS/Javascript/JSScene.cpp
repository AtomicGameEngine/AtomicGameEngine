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

#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/XMLFile.h>
#include <Atomic/IO/File.h>
#include <Atomic/Scene/Node.h>
#include <Atomic/Scene/Scene.h>
#include <Atomic/Scene/PrefabComponent.h>
#include <Atomic/Graphics/Camera.h>

#ifdef ATOMIC_3D
#include <Atomic/Physics/RigidBody.h>
#endif

#include "JSScene.h"
#include "JSComponent.h"
#include "JSVM.h"

namespace Atomic
{

void jsapi_init_scene_serializable(JSVM* vm);

static int Node_CreateJSComponent(duk_context* ctx)
{
    String path = duk_require_string(ctx, 0);

    bool hasArgs = false;
    int argIdx = -1;
    if (duk_get_top(ctx) > 1 && duk_is_object(ctx, 1))
    {
        hasArgs = true;
        argIdx = 1;
    }

    duk_push_this(ctx);
    Node* node = js_to_class_instance<Node>(ctx, -1, 0);

    ResourceCache* cache = node->GetContext()->GetSubsystem<ResourceCache>();
    JSComponentFile* file = cache->GetResource<JSComponentFile>(path);

    if (!file)
    {
        ATOMIC_LOGERRORF("Unable to load component file %s", path.CString());
        duk_push_undefined(ctx);
        return 1;
    }

    SharedPtr<JSComponent> jsc = file->CreateJSComponent();

    node->AddComponent(jsc, jsc->GetID(), LOCAL);

    jsc->InitInstance(hasArgs, argIdx);

    js_push_class_object_instance(ctx, jsc, "JSComponent");
    return 1;
}

static int Node_GetJSComponent(duk_context* ctx)
{
    String path = duk_require_string(ctx, 0);

    bool recursive = false;
    if (duk_get_top(ctx) == 2)
        if (duk_get_boolean(ctx, 1))
            recursive = true;

    duk_push_this(ctx);
    Node* node = js_to_class_instance<Node>(ctx, -1, 0);

    PODVector<JSComponent*> components;
    node->GetComponents<JSComponent>(components, recursive);

    for (unsigned i = 0; i < components.Size(); i++)
    {
        JSComponent* component = components[i];
        if (component->MatchScriptName(path)) {

            js_push_class_object_instance(ctx, component, "Component");
            return 1;

        }

    }
    duk_push_null(ctx);
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

static int Node_GetChildrenWithName(duk_context* ctx)
{
    StringHash nameHash = duk_to_string(ctx, 0);

    bool recursive = false;
    if (duk_get_top(ctx) == 2)
        if (duk_get_boolean(ctx, 1))
            recursive = true;

    duk_push_this(ctx);
    Node* node = js_to_class_instance<Node>(ctx, -1, 0);

    PODVector<Node*> dest;
    node->GetChildrenWithName(dest, nameHash, recursive);

    duk_push_array(ctx);

    for (unsigned i = 0; i < dest.Size(); i++)
    {
        js_push_class_object_instance(ctx, dest[i], "Node");
        duk_put_prop_index(ctx, -2, i);
    }

    return 1;
}

static int Node_GetComponents(duk_context* ctx)
{
    bool recursive = false;
    StringHash typeHash = Component::GetTypeStatic();


    if (duk_get_top(ctx) > 0)
    {
        if (duk_is_string(ctx, 0) && strlen(duk_get_string(ctx, 0)))
            typeHash = duk_get_string(ctx, 0);
    }

    if (duk_get_top(ctx) > 1)
        recursive = duk_require_boolean(ctx, 1) ? true : false;


    duk_push_this(ctx);
    Node* node = js_to_class_instance<Node>(ctx, -1, 0);

    PODVector<Component*> dest;
    node->GetComponents(dest, typeHash, recursive);

    duk_push_array(ctx);

    int count = 0;
    for (unsigned i = 0; i < dest.Size(); i++)
    {
        if (js_push_class_object_instance(ctx, dest[i], dest[i]->GetTypeName().CString()))
        {
            duk_put_prop_index(ctx, -2, count++);
        }
    }

    return 1;
}

static int Node_GetChildAtIndex(duk_context* ctx)
{
    duk_push_this(ctx);
    Node* node = js_to_class_instance<Node>(ctx, -1, 0);

    unsigned idx = (unsigned) duk_to_number(ctx, 0);

    if (node->GetNumChildren() <= idx)
    {
        duk_push_null(ctx);
        return 1;
    }

    Node* child = node->GetChild(idx);
    js_push_class_object_instance(ctx, child, "Node");

    return 1;
}

static int Node_SaveXML(duk_context* ctx)
{
    File* file = js_to_class_instance<File>(ctx, 0, 0);

    duk_push_this(ctx);
    Node* node = js_to_class_instance<Node>(ctx, -1, 0);

    duk_push_boolean(ctx, node->SaveXML(*file) ? 1 : 0);

    return 1;
}

static int Node_LoadPrefab(duk_context* ctx)
{
    const char* prefabName = duk_require_string(ctx, 0);

    duk_push_this(ctx);
    Node* node = js_to_class_instance<Node>(ctx, -1, 0);

    PrefabComponent* prefabComponent = node->CreateComponent<PrefabComponent>();
    prefabComponent->SetPrefabGUID(prefabName);

    duk_push_boolean(ctx, 1);
    return 1;

}

static int Node_CreateChildPrefab(duk_context* ctx)
{
    const char* childName = duk_require_string(ctx, 0);
    const char* prefabName = duk_require_string(ctx, 1);

    duk_push_this(ctx);
    Node* parent = js_to_class_instance<Node>(ctx, -1, 0);

    Node* node = parent->CreateChild();

    PrefabComponent* prefabComponent = node->CreateComponent<PrefabComponent>();
    prefabComponent->SetPrefabGUID(prefabName);

    // override what node name is in prefab
    node->SetName(childName);

    js_push_class_object_instance(ctx, node, "Node");

    return 1;

}


static int Scene_LoadXML(duk_context* ctx)
{
    JSVM* vm = JSVM::GetJSVM(ctx);

    String filename = duk_to_string(ctx, 0);

    ResourceCache* cache = vm->GetSubsystem<ResourceCache>();

    SharedPtr<File> file = cache->GetFile(filename);

    if (!file->IsOpen())
    {
        duk_push_false(ctx);
        return 1;
    }

    duk_push_this(ctx);
    Scene* scene = js_to_class_instance<Scene>(ctx, -1, 0);

    bool success = scene->LoadXML(*file);

    if (success)
        duk_push_true(ctx);
    else
        duk_push_false(ctx);


    return 1;

}

static int Scene_GetMainCamera(duk_context* ctx)
{
    duk_push_this(ctx);
    Scene* scene = js_to_class_instance<Scene>(ctx, -1, 0);

    PODVector<Node*> cameraNodes;
    Camera* camera = 0;
    scene->GetChildrenWithComponent(cameraNodes, Camera::GetTypeStatic(), true);
    if (cameraNodes.Size())
    {
        camera = cameraNodes[0]->GetComponent<Camera>();
    }

    if (!camera)
    {
        duk_push_null(ctx);
        return 1;
    }

    js_push_class_object_instance(ctx, camera, "Camera");

    return 1;

}

void jsapi_init_scene(JSVM* vm)
{
    duk_context* ctx = vm->GetJSContext();

    jsapi_init_scene_serializable(vm);

    js_class_get_prototype(ctx, "Atomic", "Node");
    duk_push_c_function(ctx, Node_GetChildrenWithComponent, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "getChildrenWithComponent");
    duk_push_c_function(ctx, Node_GetChildrenWithName, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "getChildrenWithName");
    duk_push_c_function(ctx, Node_GetComponents, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "getComponents");
    duk_push_c_function(ctx, Node_CreateJSComponent, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "createJSComponent");
    duk_push_c_function(ctx, Node_GetJSComponent, 1);
    duk_put_prop_string(ctx, -2, "getJSComponent");
    duk_push_c_function(ctx, Node_GetChildAtIndex, 1);
    duk_put_prop_string(ctx, -2, "getChildAtIndex");
    duk_push_c_function(ctx, Node_SaveXML, 1);
    duk_put_prop_string(ctx, -2, "saveXML");
    duk_push_c_function(ctx, Node_LoadPrefab, 1);
    duk_put_prop_string(ctx, -2, "loadPrefab");
    duk_push_c_function(ctx, Node_CreateChildPrefab, 2);
    duk_put_prop_string(ctx, -2, "createChildPrefab");
    duk_pop(ctx);

    js_class_get_prototype(ctx, "Atomic", "Scene");
    duk_push_c_function(ctx, Scene_LoadXML, 1);
    duk_put_prop_string(ctx, -2, "loadXML");
    duk_push_c_function(ctx, Scene_GetMainCamera, 0);
    duk_put_prop_string(ctx, -2, "getMainCamera");
    duk_pop(ctx);

}

}
