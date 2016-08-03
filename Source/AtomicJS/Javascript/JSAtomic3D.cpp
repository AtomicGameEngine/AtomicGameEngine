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

#include <Atomic/Graphics/StaticModel.h>
#include <Atomic/Graphics/CustomGeometry.h>
#include <Atomic/Graphics/BillboardSet.h>

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

static int CustomGeometry_SetMaterialIndex(duk_context* ctx) {

    unsigned index = (unsigned)duk_require_number(ctx, 0);
    Material* material = js_to_class_instance<Material>(ctx, 1, 0);

    duk_push_this(ctx);

    // event receiver
    CustomGeometry* geometry = js_to_class_instance<CustomGeometry>(ctx, -1, 0);


    geometry->SetMaterial(index, material);

    return 0;
}

static int BillboardSet_GetBillboard(duk_context* ctx) 
{
    duk_push_this(ctx);

    BillboardSet* billboardSet = js_to_class_instance<BillboardSet>(ctx, -1, 0);

    unsigned index = (unsigned)duk_to_number(ctx, 0);
    
    Billboard* billboard = billboardSet->GetBillboard(index);

    js_push_class_object_instance(ctx, billboard, "Billboard");

    return 1;
}

void jsapi_init_atomic3d(JSVM* vm)
{
    duk_context* ctx = vm->GetJSContext();

    js_class_get_prototype(ctx, "Atomic", "StaticModel");
    duk_push_c_function(ctx, StaticModel_SetMaterialIndex, 2);
    duk_put_prop_string(ctx, -2, "setMaterialIndex");
    duk_pop(ctx); // pop AObject prototype

    js_class_get_prototype(ctx, "Atomic", "CustomGeometry");
    duk_push_c_function(ctx, CustomGeometry_SetMaterialIndex, 2);
    duk_put_prop_string(ctx, -2, "setMaterialIndex");
    duk_pop(ctx); // pop AObject prototype

    js_class_get_prototype(ctx, "Atomic", "BillboardSet");
    duk_push_c_function(ctx, BillboardSet_GetBillboard, 1);
    duk_put_prop_string(ctx, -2, "getBillboard");
    duk_pop(ctx); // pop AObject prototype
}

}
