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

#include <Atomic/IO/Log.h>

#include "JSPlugin.h"
#include "JSPluginExports.h"
#include "JSVM.h"

#include <ThirdParty/SDL/include/SDL.h>

namespace Atomic
{

    typedef bool(*atomic_plugin_validate_function)(int version, void *jsvmImports, size_t jsvmImportsSize);

    static void jsplugin_bind_jsvmexports();
    static JSVMImports gJSVMExports;

    static bool jsplugin_get_entry_points(const String& pluginLibrary, atomic_plugin_validate_function* fvalidate,
                                          duk_c_function* finit, String& errorMsg)
    {
        *fvalidate = NULL;
        *finit = NULL;

        // TODO: cache and use SDL_UnloadObject (when no longer needed)
        void* handle = SDL_LoadObject(pluginLibrary.CString());

        if (handle == NULL)
        {
            errorMsg = ToString("Native Plugin: Unable to load %s with error %s",
                pluginLibrary.CString(),
                SDL_GetError());
            return false;
        }

        *fvalidate = (atomic_plugin_validate_function) SDL_LoadFunction(handle, "atomic_plugin_validate");

        if (!*fvalidate)
        {
            errorMsg = ToString("Native Plugin: Unable to get atomic_plugin_validate entry point in %s", pluginLibrary.CString());
            return false;
        }

        *finit = (duk_c_function) SDL_LoadFunction(handle, "atomic_plugin_init");

        if (!*finit)
        {
            ATOMIC_LOGERRORF("Native Plugin: Unable to get atomic_plugin_init entry point in %s", pluginLibrary.CString());
            return false;
        }

        return true;

    }

    bool jsplugin_load(JSVM* vm, const String& pluginLibrary)
    {
         String errorMsg;
         atomic_plugin_validate_function validatefunc;
         duk_c_function initfunc;

         duk_context* ctx = vm->GetJSContext();

         ATOMIC_LOGINFOF("Loading Native Plugin: %s", pluginLibrary.CString());

         if (!jsplugin_get_entry_points(pluginLibrary, &validatefunc, &initfunc, errorMsg))
         {
             ATOMIC_LOGERRORF("%s", errorMsg.CString());
             return false;
         }

         int version = ATOMIC_JSPLUGIN_VERSION;
         if (!validatefunc(version, &gJSVMExports, sizeof(JSVMImports)))
         {
             ATOMIC_LOGERRORF("Native Plugin: atomic_plugin_validate failed: %s", pluginLibrary.CString());
             return false;
         }

        // just to verify that we're not doing anything funky with the stack
        int top = duk_get_top(ctx);

        // the import function is a standard duktape c function, neat
        duk_push_c_function(ctx, initfunc, 1);

        // requires exports to be at index 2
        duk_dup(ctx, 2);

        bool success = true;
        if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS)
        {
            success = false;
            ATOMIC_LOGERRORF("Native Plugin: error calling atomic_plugin_init %s with error %s",
                pluginLibrary.CString(),
                duk_safe_to_string(ctx, -1));
        }
        else
        {
            if (!duk_is_boolean(ctx, -1) || !duk_to_boolean(ctx, -1))
            {
                success = false;
                ATOMIC_LOGERRORF("Native Plugin: error calling atomic_plugin_init, didn't return true %s", pluginLibrary.CString());
            }
        }

        duk_pop(ctx);
        assert(top == duk_get_top(ctx));

        return success;


    }

    void js_init_jsplugin(JSVM* vm)
    {
        // bind the public Duktape API for export to plugins
        jsplugin_bind_jsvmexports();
    }

    void jsplugin_bind_jsvmexports()
    {
        gJSVMExports.duk_create_heap = duk_create_heap;
        gJSVMExports.duk_destroy_heap = duk_destroy_heap;

        gJSVMExports.duk_alloc_raw = duk_alloc_raw;
        gJSVMExports.duk_free_raw = duk_free_raw;
        gJSVMExports.duk_realloc_raw = duk_realloc_raw;
        gJSVMExports.duk_alloc = duk_alloc;
        gJSVMExports.duk_free = duk_free;
        gJSVMExports.duk_realloc = duk_realloc;
        gJSVMExports.duk_get_memory_functions = duk_get_memory_functions;
        gJSVMExports.duk_gc = duk_gc;

        gJSVMExports.duk_throw = duk_throw;
        gJSVMExports.duk_fatal = duk_fatal;
        gJSVMExports.duk_error_raw = duk_error_raw;
        gJSVMExports.duk_error_va_raw = duk_error_va_raw;

        gJSVMExports.duk_is_strict_call = duk_is_strict_call;
        gJSVMExports.duk_is_constructor_call = duk_is_constructor_call;

        gJSVMExports.duk_normalize_index = duk_normalize_index;
        gJSVMExports.duk_require_normalize_index = duk_require_normalize_index;
        gJSVMExports.duk_is_valid_index = duk_is_valid_index;
        gJSVMExports.duk_require_valid_index = duk_require_valid_index;
        gJSVMExports.duk_get_top = duk_get_top;
        gJSVMExports.duk_set_top = duk_set_top;
        gJSVMExports.duk_get_top_index = duk_get_top_index;
        gJSVMExports.duk_require_top_index = duk_require_top_index;
        gJSVMExports.duk_check_stack = duk_check_stack;
        gJSVMExports.duk_require_stack = duk_require_stack;
        gJSVMExports.duk_check_stack_top = duk_check_stack_top;
        gJSVMExports.duk_require_stack_top = duk_require_stack_top;
        gJSVMExports.duk_swap = duk_swap;
        gJSVMExports.duk_swap_top = duk_swap_top;
        gJSVMExports.duk_dup = duk_dup;
        gJSVMExports.duk_dup_top = duk_dup_top;
        gJSVMExports.duk_insert = duk_insert;
        gJSVMExports.duk_replace = duk_replace;
        gJSVMExports.duk_copy = duk_copy;
        gJSVMExports.duk_remove = duk_remove;
        gJSVMExports.duk_xcopymove_raw = duk_xcopymove_raw;

        gJSVMExports.duk_push_undefined = duk_push_undefined;
        gJSVMExports.duk_push_null = duk_push_null;
        gJSVMExports.duk_push_boolean = duk_push_boolean;
        gJSVMExports.duk_push_true = duk_push_true;
        gJSVMExports.duk_push_false = duk_push_false;
        gJSVMExports.duk_push_number = duk_push_number;
        gJSVMExports.duk_push_nan = duk_push_nan;
        gJSVMExports.duk_push_int = duk_push_int;
        gJSVMExports.duk_push_uint = duk_push_uint;
        gJSVMExports.duk_push_string = duk_push_string;
        gJSVMExports.duk_push_lstring = duk_push_lstring;
        gJSVMExports.duk_push_pointer = duk_push_pointer;
        gJSVMExports.duk_push_sprintf = duk_push_sprintf;
        gJSVMExports.duk_push_vsprintf = duk_push_vsprintf;
        gJSVMExports.duk_push_string_file_raw = duk_push_string_file_raw;

        gJSVMExports.duk_push_this = duk_push_this;
        gJSVMExports.duk_push_current_function = duk_push_current_function;
        gJSVMExports.duk_push_current_thread = duk_push_current_thread;
        gJSVMExports.duk_push_global_object = duk_push_global_object;
        gJSVMExports.duk_push_heap_stash = duk_push_heap_stash;
        gJSVMExports.duk_push_global_stash = duk_push_global_stash;
        gJSVMExports.duk_push_thread_stash = duk_push_thread_stash;

        gJSVMExports.duk_push_object = duk_push_object;
        gJSVMExports.duk_push_array = duk_push_array;
        gJSVMExports.duk_push_c_function = duk_push_c_function;
        gJSVMExports.duk_push_c_lightfunc = duk_push_c_lightfunc;
        gJSVMExports.duk_push_thread_raw = duk_push_thread_raw;

        gJSVMExports.duk_push_error_object_raw = duk_push_error_object_raw;
        gJSVMExports.duk_push_error_object_va_raw = duk_push_error_object_va_raw;
        gJSVMExports.duk_push_buffer_raw = duk_push_buffer_raw;
        gJSVMExports.duk_push_heapptr = duk_push_heapptr;

        gJSVMExports.duk_pop = duk_pop;
        gJSVMExports.duk_pop_n = duk_pop_n;
        gJSVMExports.duk_pop_2 = duk_pop_2;
        gJSVMExports.duk_pop_3 = duk_pop_3;

        gJSVMExports.duk_get_type = duk_get_type;
        gJSVMExports.duk_check_type = duk_check_type;
        gJSVMExports.duk_get_type_mask = duk_get_type_mask;
        gJSVMExports.duk_check_type_mask = duk_check_type_mask;

        gJSVMExports.duk_is_undefined = duk_is_undefined;
        gJSVMExports.duk_is_null = duk_is_null;
        gJSVMExports.duk_is_null_or_undefined = duk_is_null_or_undefined;
        gJSVMExports.duk_is_boolean = duk_is_boolean;
        gJSVMExports.duk_is_number = duk_is_number;
        gJSVMExports.duk_is_nan = duk_is_nan;
        gJSVMExports.duk_is_string = duk_is_string;
        gJSVMExports.duk_is_object = duk_is_object;
        gJSVMExports.duk_is_buffer = duk_is_buffer;
        gJSVMExports.duk_is_pointer = duk_is_pointer;
        gJSVMExports.duk_is_lightfunc = duk_is_lightfunc;

        gJSVMExports.duk_is_array = duk_is_array;
        gJSVMExports.duk_is_function = duk_is_function;
        gJSVMExports.duk_is_c_function = duk_is_c_function;
        gJSVMExports.duk_is_ecmascript_function = duk_is_ecmascript_function;
        gJSVMExports.duk_is_bound_function = duk_is_bound_function;
        gJSVMExports.duk_is_thread = duk_is_thread;

        //gJSVMExports.duk_is_callable = duk_is_callable;
        gJSVMExports.duk_is_dynamic_buffer = duk_is_dynamic_buffer;
        gJSVMExports.duk_is_fixed_buffer = duk_is_fixed_buffer;
        //gJSVMExports.duk_is_primitive = duk_is_primitive;
        gJSVMExports.duk_get_error_code = duk_get_error_code;

        gJSVMExports.duk_get_boolean = duk_get_boolean;
        gJSVMExports.duk_get_number = duk_get_number;
        gJSVMExports.duk_get_int = duk_get_int;
        gJSVMExports.duk_get_uint = duk_get_uint;
        gJSVMExports.duk_get_string = duk_get_string;
        gJSVMExports.duk_get_lstring = duk_get_lstring;
        gJSVMExports.duk_get_buffer = duk_get_buffer;
        gJSVMExports.duk_get_pointer = duk_get_pointer;
        gJSVMExports.duk_get_c_function = duk_get_c_function;
        gJSVMExports.duk_get_context = duk_get_context;
        gJSVMExports.duk_get_heapptr = duk_get_heapptr;
        gJSVMExports.duk_get_length = duk_get_length;

        gJSVMExports.duk_require_undefined = duk_require_undefined;
        gJSVMExports.duk_require_null = duk_require_null;
        gJSVMExports.duk_require_boolean = duk_require_boolean;
        gJSVMExports.duk_require_number = duk_require_number;
        gJSVMExports.duk_require_int = duk_require_int;
        gJSVMExports.duk_require_uint = duk_require_uint;
        gJSVMExports.duk_require_string = duk_require_string;
        gJSVMExports.duk_require_lstring = duk_require_lstring;
        gJSVMExports.duk_require_buffer = duk_require_buffer;
        gJSVMExports.duk_require_pointer = duk_require_pointer;
        gJSVMExports.duk_require_c_function = duk_require_c_function;
        gJSVMExports.duk_require_context = duk_require_context;
        gJSVMExports.duk_require_heapptr = duk_require_heapptr;

        gJSVMExports.duk_to_undefined = duk_to_undefined;
        gJSVMExports.duk_to_null = duk_to_null;
        gJSVMExports.duk_to_boolean = duk_to_boolean;
        gJSVMExports.duk_to_number = duk_to_number;
        gJSVMExports.duk_to_int = duk_to_int;
        gJSVMExports.duk_to_uint = duk_to_uint;
        gJSVMExports.duk_to_int32 = duk_to_int32;
        gJSVMExports.duk_to_uint32 = duk_to_uint32;
        gJSVMExports.duk_to_uint16 = duk_to_uint16;
        gJSVMExports.duk_to_string = duk_to_string;
        gJSVMExports.duk_to_lstring = duk_to_lstring;
        gJSVMExports.duk_to_buffer_raw = duk_to_buffer_raw;
        gJSVMExports.duk_to_pointer = duk_to_pointer;
        gJSVMExports.duk_to_object = duk_to_object;
        gJSVMExports.duk_to_defaultvalue = duk_to_defaultvalue;
        gJSVMExports.duk_to_primitive = duk_to_primitive;

        gJSVMExports.duk_safe_to_lstring = duk_safe_to_lstring;
        gJSVMExports.duk_base64_encode = duk_base64_encode;
        gJSVMExports.duk_base64_decode = duk_base64_decode;
        gJSVMExports.duk_hex_encode = duk_hex_encode;
        gJSVMExports.duk_hex_decode = duk_hex_decode;
        gJSVMExports.duk_json_encode = duk_json_encode;
        gJSVMExports.duk_json_decode = duk_json_decode;
        gJSVMExports.duk_resize_buffer = duk_resize_buffer;

        gJSVMExports.duk_get_prop = duk_get_prop;
        gJSVMExports.duk_get_prop_string = duk_get_prop_string;
        gJSVMExports.duk_get_prop_index = duk_get_prop_index;
        gJSVMExports.duk_put_prop = duk_put_prop;
        gJSVMExports.duk_put_prop_string = duk_put_prop_string;
        gJSVMExports.duk_put_prop_index = duk_put_prop_index;
        gJSVMExports.duk_del_prop = duk_del_prop;
        gJSVMExports.duk_del_prop_string = duk_del_prop_string;
        gJSVMExports.duk_del_prop_index = duk_del_prop_index;
        gJSVMExports.duk_has_prop = duk_has_prop;
        gJSVMExports.duk_has_prop_string = duk_has_prop_string;
        gJSVMExports.duk_has_prop_index = duk_has_prop_index;
        gJSVMExports.duk_def_prop = duk_def_prop;

        gJSVMExports.duk_get_global_string = duk_get_global_string;
        gJSVMExports.duk_put_global_string = duk_put_global_string;
        gJSVMExports.duk_get_prototype = duk_get_prototype;
        gJSVMExports.duk_set_prototype = duk_set_prototype;
        gJSVMExports.duk_get_finalizer = duk_get_finalizer;
        gJSVMExports.duk_set_finalizer = duk_set_finalizer;

        gJSVMExports.duk_set_global_object = duk_set_global_object;

        gJSVMExports.duk_get_magic = duk_get_magic;
        gJSVMExports.duk_set_magic = duk_set_magic;
        gJSVMExports.duk_get_current_magic = duk_get_current_magic;

        gJSVMExports.duk_put_function_list = duk_put_function_list;
        gJSVMExports.duk_put_number_list = duk_put_number_list;

        gJSVMExports.duk_compact = duk_compact;
        gJSVMExports.duk_enum = duk_enum;
        gJSVMExports.duk_next = duk_next;

        gJSVMExports.duk_concat = duk_concat;
        gJSVMExports.duk_join = duk_join;
        gJSVMExports.duk_decode_string = duk_decode_string;
        gJSVMExports.duk_map_string = duk_map_string;
        gJSVMExports.duk_substring = duk_substring;
        gJSVMExports.duk_trim = duk_trim;
        gJSVMExports.duk_char_code_at = duk_char_code_at;

        gJSVMExports.duk_equals = duk_equals;
        gJSVMExports.duk_strict_equals = duk_strict_equals;

        gJSVMExports.duk_call = duk_call;
        gJSVMExports.duk_call_method = duk_call_method;
        gJSVMExports.duk_call_prop = duk_call_prop;
        gJSVMExports.duk_pcall = duk_pcall;
        gJSVMExports.duk_pcall_method = duk_pcall_method;
        gJSVMExports.duk_pcall_prop = duk_pcall_prop;
        gJSVMExports.duk_new = duk_new;
        gJSVMExports.duk_safe_call = duk_safe_call;

        gJSVMExports.duk_eval_raw = duk_eval_raw;
        gJSVMExports.duk_compile_raw = duk_compile_raw;
        gJSVMExports.duk_log = duk_log;
        gJSVMExports.duk_log_va = duk_log_va;
        gJSVMExports.duk_push_context_dump = duk_push_context_dump;
        gJSVMExports.duk_debugger_attach = duk_debugger_attach;
        gJSVMExports.duk_debugger_detach = duk_debugger_detach;
        gJSVMExports.duk_debugger_cooperate = duk_debugger_cooperate;

    }

}
