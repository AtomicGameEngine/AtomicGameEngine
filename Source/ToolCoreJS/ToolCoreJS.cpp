
#include <AtomicJS/Javascript/JSVM.h>
#include <ToolCore/ToolEnvironment.h>
#include <ToolCore/ToolSystem.h>
#include <ToolCore/Assets/AssetDatabase.h>
#include <ToolCore/Project/Project.h>
#include <ToolCore/License/LicenseSystem.h>

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

    duk_push_array(ctx);

    if (!project)
        return 1;

    PODVector<Asset*> assets;
    db->GetAssetsByImporterType(type, assets);

    for(unsigned i = 0; i < assets.Size(); i++)
    {
        js_push_class_object_instance(ctx, assets[i], 0);
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
    duk_push_c_function(ctx, AssetDatabase_GetAssetsByImporterType, 1);
    duk_put_prop_string(ctx, -2, "getAssetsByImporterType");
    duk_pop(ctx);


}

}


