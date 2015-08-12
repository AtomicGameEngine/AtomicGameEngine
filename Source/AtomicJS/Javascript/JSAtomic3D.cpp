
#include <Atomic/Atomic3D/StaticModel.h>

#include "JSAtomic3D.h"

namespace Atomic
{


static int StaticModel_SetMaterialIndex(duk_context* ctx) {

    unsigned index = (unsigned) duk_require_number(ctx, 0);
    Material* material = js_to_class_instance<Material>(ctx, 1, 0);

    duk_push_this(ctx);

    // event receiver
    StaticModel* model = js_to_class_instance<StaticModel>(ctx, -1, 0);


    model->SetMaterial(index, material);


    return 0;
}

void jsapi_init_atomic3d(JSVM* vm)
{
    duk_context* ctx = vm->GetJSContext();

    js_class_get_prototype(ctx, "Atomic", "StaticModel");
    duk_push_c_function(ctx, StaticModel_SetMaterialIndex, 2);
    duk_put_prop_string(ctx, -2, "setMaterialIndex");
    duk_pop(ctx); // pop AObject prototype

}

}
