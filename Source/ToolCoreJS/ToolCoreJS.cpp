//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
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

#include <Atomic/Graphics/Animation.h>

#include <AtomicJS/Javascript/JSVM.h>
#include <ToolCore/ToolEnvironment.h>
#include <ToolCore/ToolSystem.h>
#include <ToolCore/Assets/AssetDatabase.h>
#include <ToolCore/Project/Project.h>
#include <ToolCore/License/LicenseSystem.h>
#include <ToolCore/Build/BuildSystem.h>

#include <ToolCore/NETTools/NETProjectSystem.h>

#include <ToolCore/Assets/ModelImporter.h>

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

static int js_atomic_GetLicenseSystem(duk_context* ctx)
{
    JSVM* vm = JSVM::GetJSVM(ctx);
    js_push_class_object_instance(ctx, vm->GetSubsystem<LicenseSystem>());
    return 1;
}

static int js_atomic_GetAssetDatabase(duk_context* ctx)
{
    JSVM* vm = JSVM::GetJSVM(ctx);
    js_push_class_object_instance(ctx, vm->GetSubsystem<AssetDatabase>());
    return 1;
}

static int AssetDatabase_GetFolderAssets(duk_context* ctx)
{
    JSVM* vm = JSVM::GetJSVM(ctx);
    ToolSystem* ts = vm->GetSubsystem<ToolSystem>();
    AssetDatabase* db = vm->GetSubsystem<AssetDatabase>();
    Project* project = ts->GetProject();

    String folder = duk_require_string(ctx, 0);

    duk_push_array(ctx);

    if (!project)
        return 1;

    PODVector<Asset*> assets;
    db->GetFolderAssets(folder, assets);

    for(unsigned i = 0; i < assets.Size(); i++)
    {
        js_push_class_object_instance(ctx, assets[i], 0);
        duk_put_prop_index(ctx, -2, i);
    }

    return 1;
}

static int AssetDatabase_GetAssetsByImporterType(duk_context* ctx)
{
    JSVM* vm = JSVM::GetJSVM(ctx);
    ToolSystem* ts = vm->GetSubsystem<ToolSystem>();
    AssetDatabase* db = vm->GetSubsystem<AssetDatabase>();
    Project* project = ts->GetProject();

    StringHash type = duk_require_string(ctx, 0);
    String resourceType = duk_require_string(ctx, 1);

    duk_push_array(ctx);

    if (!project)
        return 1;

    PODVector<Asset*> assets;
    db->GetAssetsByImporterType(type, resourceType, assets);

    for(unsigned i = 0; i < assets.Size(); i++)
    {
        js_push_class_object_instance(ctx, assets[i], 0);
        duk_put_prop_index(ctx, -2, i);
    }

    return 1;
}

static int ModelImporter_GetAnimations(duk_context* ctx)
{
    duk_push_this(ctx);
    ModelImporter* importer = js_to_class_instance<ModelImporter>(ctx, -1, 0);

    PODVector<Animation*> animations;
    importer->GetAnimations(animations);

    duk_push_array(ctx);

    for(unsigned i = 0; i < animations.Size(); i++)
    {
        js_push_class_object_instance(ctx, animations[i], 0);
        duk_put_prop_index(ctx, -2, i);
    }

    return 1;
}

void jsapi_init_toolcore(JSVM* vm)
{
    duk_context* ctx = vm->GetJSContext();

    jsb_package_toolcore_init(vm);

    duk_get_global_string(ctx, "ToolCore");

    duk_push_c_function(ctx, js_atomic_GetToolEnvironment, 0);
    duk_put_prop_string(ctx, -2, "getToolEnvironment");

    js_push_class_object_instance(ctx, vm->GetSubsystem<ToolEnvironment>(), "ToolEnvironment");
    duk_put_prop_string(ctx, -2, "toolEnvironment");

    duk_push_c_function(ctx, js_atomic_GetToolSystem, 0);
    duk_put_prop_string(ctx, -2, "getToolSystem");

    js_push_class_object_instance(ctx, vm->GetSubsystem<ToolSystem>(), "ToolSystem");
    duk_put_prop_string(ctx, -2, "toolSystem");

    js_push_class_object_instance(ctx, vm->GetSubsystem<BuildSystem>(), "BuildSystem");
    duk_put_prop_string(ctx, -2, "buildSystem");

    js_push_class_object_instance(ctx, vm->GetSubsystem<NETProjectSystem>(), "NETProjectSystem");
    duk_put_prop_string(ctx, -2, "netProjectSystem");

    duk_push_c_function(ctx, js_atomic_GetLicenseSystem, 0);
    duk_put_prop_string(ctx, -2, "getLicenseSystem");

    js_push_class_object_instance(ctx, vm->GetSubsystem<LicenseSystem>(), "LicenseSystem");
    duk_put_prop_string(ctx, -2, "licenseSystem");

    duk_push_c_function(ctx, js_atomic_GetAssetDatabase, 0);
    duk_put_prop_string(ctx, -2, "getAssetDatabase");

    js_push_class_object_instance(ctx, vm->GetSubsystem<AssetDatabase>(), "AssetDatabase");
    duk_put_prop_string(ctx, -2, "assetDatabase");

    duk_pop(ctx);

    js_class_get_prototype(ctx, "ToolCore", "AssetDatabase");
    duk_push_c_function(ctx, AssetDatabase_GetFolderAssets, 1);
    duk_put_prop_string(ctx, -2, "getFolderAssets");
    duk_push_c_function(ctx, AssetDatabase_GetAssetsByImporterType, 2);
    duk_put_prop_string(ctx, -2, "getAssetsByImporterType");
    duk_pop(ctx);

    js_class_get_prototype(ctx, "ToolCore", "ModelImporter");
    duk_push_c_function(ctx, ModelImporter_GetAnimations, 0);
    duk_put_prop_string(ctx, -2, "getAnimations");
    duk_pop(ctx);
}

}
