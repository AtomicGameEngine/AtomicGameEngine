// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/IO/File.h>
#include <Atomic/Scene/Node.h>
#include <Atomic/Scene/Scene.h>

#include "JSScene.h"
#include "JSComponent.h"
#include "JSVM.h"

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

static int Node_GetJSComponent(duk_context* ctx)
{
	StringHash classNameHash = duk_to_string(ctx, 0);
	duk_push_this(ctx);
	Node* node = js_to_class_instance<Node>(ctx, -1, 0);

	PODVector<JSComponent*> jsComponents;
	node->GetComponents<JSComponent>(jsComponents);

	for (unsigned i = 0; i < jsComponents.Size(); i++)
	{
		if (StringHash(jsComponents[i]->GetClassName()) == classNameHash)
		{
			js_push_class_object_instance(ctx, jsComponents[i]);
			break;
		}
	}

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


void jsapi_init_scene(JSVM* vm)
{
    duk_context* ctx = vm->GetJSContext();

    js_class_get_prototype(ctx, "Node");
    duk_push_c_function(ctx, Node_GetChildrenWithComponent, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "getChildrenWithComponent");
    duk_push_c_function(ctx, Node_GetChildrenWithName, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "getChildrenWithName");
    duk_push_c_function(ctx, Node_CreateJSComponent, 1);
    duk_put_prop_string(ctx, -2, "createJSComponent");
	duk_push_c_function(ctx, Node_GetJSComponent, 1);
	duk_put_prop_string(ctx, -2, "getJSComponent");
	duk_pop(ctx);

    js_class_get_prototype(ctx, "Scene");
    duk_push_c_function(ctx, Scene_LoadXML, 1);
    duk_put_prop_string(ctx, -2, "loadXML");
    duk_pop(ctx);

}

}
