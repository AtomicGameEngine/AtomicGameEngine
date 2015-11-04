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

#include <Atomic/Input/Input.h>

#include "JSInput.h"

namespace Atomic
{

    static int Input_GetTouch(duk_context* ctx)
    {
        duk_push_this(ctx);

        Input* input = js_to_class_instance<Input>(ctx, -1, 0);

        unsigned index = (unsigned)duk_to_number(ctx, 0);

        TouchState* touchState = input->GetTouch(index);

        VariantMap vmap;
        vmap["touchID"] = touchState->touchID_;
        vmap["position"] = touchState->position_;
        vmap["lastPosition"] = touchState->lastPosition_;
        vmap["delta"] = touchState->delta_;
        vmap["pressure"] = touchState->pressure_;
        vmap["touchedWidget"] = touchState->touchedWidget_;

        js_push_variantmap(ctx, vmap);
        
        return 1;
    }

    void jsapi_init_input(JSVM* vm)
    {
        duk_context* ctx = vm->GetJSContext();

        js_class_get_prototype(ctx, "Atomic", "Input");
        duk_push_c_function(ctx, Input_GetTouch, 1);
        duk_put_prop_string(ctx, -2, "getTouch");
        duk_pop(ctx); // pop AObject prototype

    }

}
