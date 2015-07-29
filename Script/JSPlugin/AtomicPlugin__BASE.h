
// PASTE CONTENTS OF duktape.h HERE

// --------------------
// BEGIN AtomicPlugin.h
// --------------------

#ifndef ATOMIC_PLUGIN_H
#define ATOMIC_PLUGIN_H

#ifdef _MSC_VER
#define PLUGIN_EXPORT_API __declspec(dllexport)
#else
#define PLUGIN_EXPORT_API
#endif

#ifdef ATOMIC_PLUGIN_MAIN

#define ATOMIC_JSPLUGIN_VERSION 1

// MUST MATCH JSPlugin.cpp definitions exactly (typedefs and JSVMImports struct)
extern "C"
{
    typedef duk_context* (*__duk_create_heap)(duk_alloc_function alloc_func,
        duk_realloc_function realloc_func,
        duk_free_function free_func,
        void *heap_udata,
        duk_fatal_function fatal_handler);

    typedef void(*__duk_destroy_heap)(duk_context *ctx);

    typedef void* (*__duk_alloc_raw)(duk_context *ctx, duk_size_t size);
    typedef void(*__duk_free_raw)(duk_context *ctx, void *ptr);
    typedef void* (*__duk_realloc_raw)(duk_context *ctx, void *ptr, duk_size_t size);
    typedef void* (*__duk_alloc)(duk_context *ctx, duk_size_t size);
    typedef void(*__duk_free)(duk_context *ctx, void *ptr);
    typedef void* (*__duk_realloc)(duk_context *ctx, void *ptr, duk_size_t size);
    typedef void(*__duk_get_memory_functions)(duk_context *ctx, duk_memory_functions *out_funcs);
    typedef void(*__duk_gc)(duk_context *ctx, duk_uint_t flags);

    typedef void(*__duk_throw)(duk_context *ctx);
    typedef void(*__duk_fatal)(duk_context *ctx, duk_errcode_t err_code, const char *err_msg);
    typedef void(*__duk_error_raw)(duk_context *ctx, duk_errcode_t err_code, const char *filename, duk_int_t line, const char *fmt, ...);
    typedef void(*__duk_error_va_raw)(duk_context *ctx, duk_errcode_t err_code, const char *filename, duk_int_t line, const char *fmt, va_list ap);

    typedef duk_bool_t(*__duk_is_strict_call)(duk_context *ctx);
    typedef duk_bool_t(*__duk_is_constructor_call)(duk_context *ctx);

    typedef duk_idx_t(*__duk_normalize_index)(duk_context *ctx, duk_idx_t index);
    typedef duk_idx_t(*__duk_require_normalize_index)(duk_context *ctx, duk_idx_t index);
    typedef duk_bool_t(*__duk_is_valid_index)(duk_context *ctx, duk_idx_t index);
    typedef void(*__duk_require_valid_index)(duk_context *ctx, duk_idx_t index);
    typedef duk_idx_t(*__duk_get_top)(duk_context *ctx);
    typedef void(*__duk_set_top)(duk_context *ctx, duk_idx_t index);
    typedef duk_idx_t(*__duk_get_top_index)(duk_context *ctx);
    typedef duk_idx_t(*__duk_require_top_index)(duk_context *ctx);
    typedef duk_bool_t(*__duk_check_stack)(duk_context *ctx, duk_idx_t extra);
    typedef void(*__duk_require_stack)(duk_context *ctx, duk_idx_t extra);
    typedef duk_bool_t(*__duk_check_stack_top)(duk_context *ctx, duk_idx_t top);
    typedef void(*__duk_require_stack_top)(duk_context *ctx, duk_idx_t top);
    typedef void(*__duk_swap)(duk_context *ctx, duk_idx_t index1, duk_idx_t index2);
    typedef void(*__duk_swap_top)(duk_context *ctx, duk_idx_t index);
    typedef void(*__duk_dup)(duk_context *ctx, duk_idx_t from_index);
    typedef void(*__duk_dup_top)(duk_context *ctx);
    typedef void(*__duk_insert)(duk_context *ctx, duk_idx_t to_index);
    typedef void(*__duk_replace)(duk_context *ctx, duk_idx_t to_index);
    typedef void(*__duk_copy)(duk_context *ctx, duk_idx_t from_index, duk_idx_t to_index);
    typedef void(*__duk_remove)(duk_context *ctx, duk_idx_t index);
    typedef void(*__duk_xcopymove_raw)(duk_context *to_ctx, duk_context *from_ctx, duk_idx_t count, duk_bool_t is_copy);

    typedef void(*__duk_push_undefined)(duk_context *ctx);
    typedef void(*__duk_push_null)(duk_context *ctx);
    typedef void(*__duk_push_boolean)(duk_context *ctx, duk_bool_t val);
    typedef void(*__duk_push_true)(duk_context *ctx);
    typedef void(*__duk_push_false)(duk_context *ctx);
    typedef void(*__duk_push_number)(duk_context *ctx, duk_double_t val);
    typedef void(*__duk_push_nan)(duk_context *ctx);
    typedef void(*__duk_push_int)(duk_context *ctx, duk_int_t val);
    typedef void(*__duk_push_uint)(duk_context *ctx, duk_uint_t val);
    typedef const char* (*__duk_push_string)(duk_context *ctx, const char *str);
    typedef const char* (*__duk_push_lstring)(duk_context *ctx, const char *str, duk_size_t len);
    typedef void(*__duk_push_pointer)(duk_context *ctx, void *p);
    typedef const char* (*__duk_push_sprintf)(duk_context *ctx, const char *fmt, ...);
    typedef const char* (*__duk_push_vsprintf)(duk_context *ctx, const char *fmt, va_list ap);

    typedef const char* (*__duk_push_string_file_raw)(duk_context *ctx, const char *path, duk_uint_t flags);

    typedef void(*__duk_push_this)(duk_context *ctx);
    typedef void(*__duk_push_current_function)(duk_context *ctx);
    typedef void(*__duk_push_current_thread)(duk_context *ctx);
    typedef void(*__duk_push_global_object)(duk_context *ctx);
    typedef void(*__duk_push_heap_stash)(duk_context *ctx);
    typedef void(*__duk_push_global_stash)(duk_context *ctx);
    typedef void(*__duk_push_thread_stash)(duk_context *ctx, duk_context *target_ctx);

    typedef duk_idx_t(*__duk_push_object)(duk_context *ctx);
    typedef duk_idx_t(*__duk_push_array)(duk_context *ctx);
    typedef duk_idx_t(*__duk_push_c_function)(duk_context *ctx, duk_c_function func, duk_idx_t nargs);
    typedef duk_idx_t(*__duk_push_c_lightfunc)(duk_context *ctx, duk_c_function func, duk_idx_t nargs, duk_idx_t length, duk_int_t magic);
    typedef duk_idx_t(*__duk_push_thread_raw)(duk_context *ctx, duk_uint_t flags);

    typedef duk_idx_t(*__duk_push_error_object_raw)(duk_context *ctx, duk_errcode_t err_code, const char *filename, duk_int_t line, const char *fmt, ...);
    typedef duk_idx_t(*__duk_push_error_object_va_raw)(duk_context *ctx, duk_errcode_t err_code, const char *filename, duk_int_t line, const char *fmt, va_list ap);
    typedef void* (*__duk_push_buffer_raw)(duk_context *ctx, duk_size_t size, duk_small_uint_t flags);
    typedef duk_idx_t(*__duk_push_heapptr)(duk_context *ctx, void *ptr);

    typedef void(*__duk_pop)(duk_context *ctx);
    typedef void(*__duk_pop_n)(duk_context *ctx, duk_idx_t count);
    typedef void(*__duk_pop_2)(duk_context *ctx);
    typedef void(*__duk_pop_3)(duk_context *ctx);

    typedef duk_int_t(*__duk_get_type)(duk_context *ctx, duk_idx_t index);
    typedef duk_bool_t(*__duk_check_type)(duk_context *ctx, duk_idx_t index, duk_int_t type);
    typedef duk_uint_t(*__duk_get_type_mask)(duk_context *ctx, duk_idx_t index);
    typedef duk_bool_t(*__duk_check_type_mask)(duk_context *ctx, duk_idx_t index, duk_uint_t mask);

    typedef duk_bool_t(*__duk_is_undefined)(duk_context *ctx, duk_idx_t index);
    typedef duk_bool_t(*__duk_is_null)(duk_context *ctx, duk_idx_t index);
    typedef duk_bool_t(*__duk_is_null_or_undefined)(duk_context *ctx, duk_idx_t index);
    typedef duk_bool_t(*__duk_is_boolean)(duk_context *ctx, duk_idx_t index);
    typedef duk_bool_t(*__duk_is_number)(duk_context *ctx, duk_idx_t index);
    typedef duk_bool_t(*__duk_is_nan)(duk_context *ctx, duk_idx_t index);
    typedef duk_bool_t(*__duk_is_string)(duk_context *ctx, duk_idx_t index);
    typedef duk_bool_t(*__duk_is_object)(duk_context *ctx, duk_idx_t index);
    typedef duk_bool_t(*__duk_is_buffer)(duk_context *ctx, duk_idx_t index);
    typedef duk_bool_t(*__duk_is_pointer)(duk_context *ctx, duk_idx_t index);
    typedef duk_bool_t(*__duk_is_lightfunc)(duk_context *ctx, duk_idx_t index);

    typedef duk_bool_t(*__duk_is_array)(duk_context *ctx, duk_idx_t index);
    typedef duk_bool_t(*__duk_is_function)(duk_context *ctx, duk_idx_t index);
    typedef duk_bool_t(*__duk_is_c_function)(duk_context *ctx, duk_idx_t index);
    typedef duk_bool_t(*__duk_is_ecmascript_function)(duk_context *ctx, duk_idx_t index);
    typedef duk_bool_t(*__duk_is_bound_function)(duk_context *ctx, duk_idx_t index);
    typedef duk_bool_t(*__duk_is_thread)(duk_context *ctx, duk_idx_t index);

    typedef duk_bool_t(*__duk_is_callable)(duk_context *ctx, duk_idx_t index);
    typedef duk_bool_t(*__duk_is_dynamic_buffer)(duk_context *ctx, duk_idx_t index);
    typedef duk_bool_t(*__duk_is_fixed_buffer)(duk_context *ctx, duk_idx_t index);
    typedef duk_bool_t(*__duk_is_primitive)(duk_context *ctx, duk_idx_t index);
    typedef duk_errcode_t(*__duk_get_error_code)(duk_context *ctx, duk_idx_t index);

    typedef duk_bool_t(*__duk_get_boolean)(duk_context *ctx, duk_idx_t index);
    typedef duk_double_t(*__duk_get_number)(duk_context *ctx, duk_idx_t index);
    typedef duk_int_t(*__duk_get_int)(duk_context *ctx, duk_idx_t index);
    typedef duk_uint_t(*__duk_get_uint)(duk_context *ctx, duk_idx_t index);
    typedef const char *(*__duk_get_string)(duk_context *ctx, duk_idx_t index);
    typedef const char *(*__duk_get_lstring)(duk_context *ctx, duk_idx_t index, duk_size_t *out_len);
    typedef void *(*__duk_get_buffer)(duk_context *ctx, duk_idx_t index, duk_size_t *out_size);
    typedef void *(*__duk_get_pointer)(duk_context *ctx, duk_idx_t index);
    typedef duk_c_function(*__duk_get_c_function)(duk_context *ctx, duk_idx_t index);
    typedef duk_context *(*__duk_get_context)(duk_context *ctx, duk_idx_t index);
    typedef void *(*__duk_get_heapptr)(duk_context *ctx, duk_idx_t index);
    typedef duk_size_t(*__duk_get_length)(duk_context *ctx, duk_idx_t index);

    typedef void(*__duk_require_undefined)(duk_context *ctx, duk_idx_t index);
    typedef void(*__duk_require_null)(duk_context *ctx, duk_idx_t index);
    typedef duk_bool_t(*__duk_require_boolean)(duk_context *ctx, duk_idx_t index);
    typedef duk_double_t(*__duk_require_number)(duk_context *ctx, duk_idx_t index);
    typedef duk_int_t(*__duk_require_int)(duk_context *ctx, duk_idx_t index);
    typedef duk_uint_t(*__duk_require_uint)(duk_context *ctx, duk_idx_t index);
    typedef const char *(*__duk_require_string)(duk_context *ctx, duk_idx_t index);
    typedef const char *(*__duk_require_lstring)(duk_context *ctx, duk_idx_t index, duk_size_t *out_len);
    typedef void *(*__duk_require_buffer)(duk_context *ctx, duk_idx_t index, duk_size_t *out_size);
    typedef void *(*__duk_require_pointer)(duk_context *ctx, duk_idx_t index);
    typedef duk_c_function(*__duk_require_c_function)(duk_context *ctx, duk_idx_t index);
    typedef duk_context *(*__duk_require_context)(duk_context *ctx, duk_idx_t index);
    typedef void *(*__duk_require_heapptr)(duk_context *ctx, duk_idx_t index);

    typedef void(*__duk_to_undefined)(duk_context *ctx, duk_idx_t index);
    typedef void(*__duk_to_null)(duk_context *ctx, duk_idx_t index);
    typedef duk_bool_t(*__duk_to_boolean)(duk_context *ctx, duk_idx_t index);
    typedef duk_double_t(*__duk_to_number)(duk_context *ctx, duk_idx_t index);
    typedef duk_int_t(*__duk_to_int)(duk_context *ctx, duk_idx_t index);
    typedef duk_uint_t(*__duk_to_uint)(duk_context *ctx, duk_idx_t index);
    typedef duk_int32_t(*__duk_to_int32)(duk_context *ctx, duk_idx_t index);
    typedef duk_uint32_t(*__duk_to_uint32)(duk_context *ctx, duk_idx_t index);
    typedef duk_uint16_t(*__duk_to_uint16)(duk_context *ctx, duk_idx_t index);
    typedef const char *(*__duk_to_string)(duk_context *ctx, duk_idx_t index);
    typedef const char *(*__duk_to_lstring)(duk_context *ctx, duk_idx_t index, duk_size_t *out_len);
    typedef void *(*__duk_to_buffer_raw)(duk_context *ctx, duk_idx_t index, duk_size_t *out_size, duk_uint_t flags);
    typedef void *(*__duk_to_pointer)(duk_context *ctx, duk_idx_t index);
    typedef void(*__duk_to_object)(duk_context *ctx, duk_idx_t index);
    typedef void(*__duk_to_defaultvalue)(duk_context *ctx, duk_idx_t index, duk_int_t hint);
    typedef void(*__duk_to_primitive)(duk_context *ctx, duk_idx_t index, duk_int_t hint);

    typedef const char *(*__duk_safe_to_lstring)(duk_context *ctx, duk_idx_t index, duk_size_t *out_len);

    typedef const char *(*__duk_base64_encode)(duk_context *ctx, duk_idx_t index);
    typedef void(*__duk_base64_decode)(duk_context *ctx, duk_idx_t index);
    typedef const char *(*__duk_hex_encode)(duk_context *ctx, duk_idx_t index);
    typedef void(*__duk_hex_decode)(duk_context *ctx, duk_idx_t index);
    typedef const char *(*__duk_json_encode)(duk_context *ctx, duk_idx_t index);
    typedef void(*__duk_json_decode)(duk_context *ctx, duk_idx_t index);

    typedef void *(*__duk_resize_buffer)(duk_context *ctx, duk_idx_t index, duk_size_t new_size);

    typedef duk_bool_t(*__duk_get_prop)(duk_context *ctx, duk_idx_t obj_index);
    typedef duk_bool_t(*__duk_get_prop_string)(duk_context *ctx, duk_idx_t obj_index, const char *key);
    typedef duk_bool_t(*__duk_get_prop_index)(duk_context *ctx, duk_idx_t obj_index, duk_uarridx_t arr_index);
    typedef duk_bool_t(*__duk_put_prop)(duk_context *ctx, duk_idx_t obj_index);
    typedef duk_bool_t(*__duk_put_prop_string)(duk_context *ctx, duk_idx_t obj_index, const char *key);
    typedef duk_bool_t(*__duk_put_prop_index)(duk_context *ctx, duk_idx_t obj_index, duk_uarridx_t arr_index);
    typedef duk_bool_t(*__duk_del_prop)(duk_context *ctx, duk_idx_t obj_index);
    typedef duk_bool_t(*__duk_del_prop_string)(duk_context *ctx, duk_idx_t obj_index, const char *key);
    typedef duk_bool_t(*__duk_del_prop_index)(duk_context *ctx, duk_idx_t obj_index, duk_uarridx_t arr_index);
    typedef duk_bool_t(*__duk_has_prop)(duk_context *ctx, duk_idx_t obj_index);
    typedef duk_bool_t(*__duk_has_prop_string)(duk_context *ctx, duk_idx_t obj_index, const char *key);
    typedef duk_bool_t(*__duk_has_prop_index)(duk_context *ctx, duk_idx_t obj_index, duk_uarridx_t arr_index);
    typedef void(*__duk_def_prop)(duk_context *ctx, duk_idx_t obj_index, duk_uint_t flags);

    typedef duk_bool_t(*__duk_get_global_string)(duk_context *ctx, const char *key);
    typedef duk_bool_t(*__duk_put_global_string)(duk_context *ctx, const char *key);

    typedef void(*__duk_get_prototype)(duk_context *ctx, duk_idx_t index);
    typedef void(*__duk_set_prototype)(duk_context *ctx, duk_idx_t index);

    typedef void(*__duk_get_finalizer)(duk_context *ctx, duk_idx_t index);
    typedef void(*__duk_set_finalizer)(duk_context *ctx, duk_idx_t index);

    typedef void(*__duk_set_global_object)(duk_context *ctx);

    typedef duk_int_t(*__duk_get_magic)(duk_context *ctx, duk_idx_t index);
    typedef void(*__duk_set_magic)(duk_context *ctx, duk_idx_t index, duk_int_t magic);
    typedef duk_int_t(*__duk_get_current_magic)(duk_context *ctx);

    typedef void(*__duk_put_function_list)(duk_context *ctx, duk_idx_t obj_index, const duk_function_list_entry *funcs);
    typedef void(*__duk_put_number_list)(duk_context *ctx, duk_idx_t obj_index, const duk_number_list_entry *numbers);

    typedef void(*__duk_compact)(duk_context *ctx, duk_idx_t obj_index);
    typedef void(*__duk_enum)(duk_context *ctx, duk_idx_t obj_index, duk_uint_t enum_flags);
    typedef duk_bool_t(*__duk_next)(duk_context *ctx, duk_idx_t enum_index, duk_bool_t get_value);

    typedef void(*__duk_concat)(duk_context *ctx, duk_idx_t count);
    typedef void(*__duk_join)(duk_context *ctx, duk_idx_t count);
    typedef void(*__duk_decode_string)(duk_context *ctx, duk_idx_t index, duk_decode_char_function callback, void *udata);
    typedef void(*__duk_map_string)(duk_context *ctx, duk_idx_t index, duk_map_char_function callback, void *udata);
    typedef void(*__duk_substring)(duk_context *ctx, duk_idx_t index, duk_size_t start_char_offset, duk_size_t end_char_offset);
    typedef void(*__duk_trim)(duk_context *ctx, duk_idx_t index);
    typedef duk_codepoint_t(*__duk_char_code_at)(duk_context *ctx, duk_idx_t index, duk_size_t char_offset);

    typedef duk_bool_t(*__duk_equals)(duk_context *ctx, duk_idx_t index1, duk_idx_t index2);
    typedef duk_bool_t(*__duk_strict_equals)(duk_context *ctx, duk_idx_t index1, duk_idx_t index2);

    typedef void(*__duk_call)(duk_context *ctx, duk_idx_t nargs);
    typedef void(*__duk_call_method)(duk_context *ctx, duk_idx_t nargs);
    typedef void(*__duk_call_prop)(duk_context *ctx, duk_idx_t obj_index, duk_idx_t nargs);
    typedef duk_int_t(*__duk_pcall)(duk_context *ctx, duk_idx_t nargs);
    typedef duk_int_t(*__duk_pcall_method)(duk_context *ctx, duk_idx_t nargs);
    typedef duk_int_t(*__duk_pcall_prop)(duk_context *ctx, duk_idx_t obj_index, duk_idx_t nargs);
    typedef void(*__duk_new)(duk_context *ctx, duk_idx_t nargs);
    typedef duk_int_t(*__duk_safe_call)(duk_context *ctx, duk_safe_call_function func, duk_idx_t nargs, duk_idx_t nrets);

    typedef duk_int_t(*__duk_eval_raw)(duk_context *ctx, const char *src_buffer, duk_size_t src_length, duk_uint_t flags);
    typedef duk_int_t(*__duk_compile_raw)(duk_context *ctx, const char *src_buffer, duk_size_t src_length, duk_uint_t flags);

    typedef void(*__duk_log)(duk_context *ctx, duk_int_t level, const char *fmt, ...);
    typedef void(*__duk_log_va)(duk_context *ctx, duk_int_t level, const char *fmt, va_list ap);

    typedef void(*__duk_push_context_dump)(duk_context *ctx);

    typedef void(*__duk_debugger_attach)(duk_context *ctx,
        duk_debug_read_function read_cb,
        duk_debug_write_function write_cb,
        duk_debug_peek_function peek_cb,
        duk_debug_read_flush_function read_flush_cb,
        duk_debug_write_flush_function write_flush_cb,
        duk_debug_detached_function detached_cb,
        void *udata);
    typedef void(*__duk_debugger_detach)(duk_context *ctx);
    typedef void(*__duk_debugger_cooperate)(duk_context *ctx);

    struct JSVMImports
    {
        __duk_create_heap duk_create_heap;
        __duk_destroy_heap duk_destroy_heap;

        __duk_alloc_raw duk_alloc_raw;
        __duk_free_raw duk_free_raw;
        __duk_realloc_raw duk_realloc_raw;
        __duk_alloc duk_alloc;
        __duk_free duk_free;
        __duk_realloc duk_realloc;
        __duk_get_memory_functions duk_get_memory_functions;
        __duk_gc duk_gc;

        __duk_throw duk_throw;
        __duk_fatal duk_fatal;
        __duk_error_raw duk_error_raw;
        __duk_error_va_raw duk_error_va_raw;

        __duk_is_strict_call duk_is_strict_call;
        __duk_is_constructor_call duk_is_constructor_call;

        __duk_normalize_index duk_normalize_index;
        __duk_require_normalize_index duk_require_normalize_index;
        __duk_is_valid_index duk_is_valid_index;
        __duk_require_valid_index duk_require_valid_index;
        __duk_get_top duk_get_top;
        __duk_set_top duk_set_top;
        __duk_get_top_index duk_get_top_index;
        __duk_require_top_index duk_require_top_index;
        __duk_check_stack duk_check_stack;
        __duk_require_stack duk_require_stack;
        __duk_check_stack_top duk_check_stack_top;
        __duk_require_stack_top duk_require_stack_top;
        __duk_swap duk_swap;
        __duk_swap_top duk_swap_top;
        __duk_dup duk_dup;
        __duk_dup_top duk_dup_top;
        __duk_insert duk_insert;
        __duk_replace duk_replace;
        __duk_copy duk_copy;
        __duk_remove duk_remove;
        __duk_xcopymove_raw duk_xcopymove_raw;

        __duk_push_undefined duk_push_undefined;
        __duk_push_null duk_push_null;
        __duk_push_boolean duk_push_boolean;
        __duk_push_true duk_push_true;
        __duk_push_false duk_push_false;
        __duk_push_number duk_push_number;
        __duk_push_nan duk_push_nan;
        __duk_push_int duk_push_int;
        __duk_push_uint duk_push_uint;
        __duk_push_string duk_push_string;
        __duk_push_lstring duk_push_lstring;
        __duk_push_pointer duk_push_pointer;
        __duk_push_sprintf duk_push_sprintf;
        __duk_push_vsprintf duk_push_vsprintf;
        __duk_push_string_file_raw duk_push_string_file_raw;

        __duk_push_this duk_push_this;
        __duk_push_current_function duk_push_current_function;
        __duk_push_current_thread duk_push_current_thread;
        __duk_push_global_object duk_push_global_object;
        __duk_push_heap_stash duk_push_heap_stash;
        __duk_push_global_stash duk_push_global_stash;
        __duk_push_thread_stash duk_push_thread_stash;

        __duk_push_object duk_push_object;
        __duk_push_array duk_push_array;
        __duk_push_c_function duk_push_c_function;
        __duk_push_c_lightfunc duk_push_c_lightfunc;
        __duk_push_thread_raw duk_push_thread_raw;

        __duk_push_error_object_raw duk_push_error_object_raw;
        __duk_push_error_object_va_raw duk_push_error_object_va_raw;
        __duk_push_buffer_raw duk_push_buffer_raw;
        __duk_push_heapptr duk_push_heapptr;

        __duk_pop duk_pop;
        __duk_pop_n duk_pop_n;
        __duk_pop_2 duk_pop_2;
        __duk_pop_3 duk_pop_3;

        __duk_get_type duk_get_type;
        __duk_check_type duk_check_type;
        __duk_get_type_mask duk_get_type_mask;
        __duk_check_type_mask duk_check_type_mask;

        __duk_is_undefined duk_is_undefined;
        __duk_is_null duk_is_null;
        __duk_is_null_or_undefined duk_is_null_or_undefined;
        __duk_is_boolean duk_is_boolean;
        __duk_is_number duk_is_number;
        __duk_is_nan duk_is_nan;
        __duk_is_string duk_is_string;
        __duk_is_object duk_is_object;
        __duk_is_buffer duk_is_buffer;
        __duk_is_pointer duk_is_pointer;
        __duk_is_lightfunc duk_is_lightfunc;

        __duk_is_array duk_is_array;
        __duk_is_function duk_is_function;
        __duk_is_c_function duk_is_c_function;
        __duk_is_ecmascript_function duk_is_ecmascript_function;
        __duk_is_bound_function duk_is_bound_function;
        __duk_is_thread duk_is_thread;

        __duk_is_callable duk_is_callable;
        __duk_is_dynamic_buffer duk_is_dynamic_buffer;
        __duk_is_fixed_buffer duk_is_fixed_buffer;
        __duk_is_primitive duk_is_primitive;
        __duk_get_error_code duk_get_error_code;

        __duk_get_boolean duk_get_boolean;
        __duk_get_number duk_get_number;
        __duk_get_int duk_get_int;
        __duk_get_uint duk_get_uint;
        __duk_get_string duk_get_string;
        __duk_get_lstring duk_get_lstring;
        __duk_get_buffer duk_get_buffer;
        __duk_get_pointer duk_get_pointer;
        __duk_get_c_function duk_get_c_function;
        __duk_get_context duk_get_context;
        __duk_get_heapptr duk_get_heapptr;
        __duk_get_length duk_get_length;

        __duk_require_undefined duk_require_undefined;
        __duk_require_null duk_require_null;
        __duk_require_boolean duk_require_boolean;
        __duk_require_number duk_require_number;
        __duk_require_int duk_require_int;
        __duk_require_uint duk_require_uint;
        __duk_require_string duk_require_string;
        __duk_require_lstring duk_require_lstring;
        __duk_require_buffer duk_require_buffer;
        __duk_require_pointer duk_require_pointer;
        __duk_require_c_function duk_require_c_function;
        __duk_require_context duk_require_context;
        __duk_require_heapptr duk_require_heapptr;

        __duk_to_undefined duk_to_undefined;
        __duk_to_null duk_to_null;
        __duk_to_boolean duk_to_boolean;
        __duk_to_number duk_to_number;
        __duk_to_int duk_to_int;
        __duk_to_uint duk_to_uint;
        __duk_to_int32 duk_to_int32;
        __duk_to_uint32 duk_to_uint32;
        __duk_to_uint16 duk_to_uint16;
        __duk_to_string duk_to_string;
        __duk_to_lstring duk_to_lstring;
        __duk_to_buffer_raw duk_to_buffer_raw;
        __duk_to_pointer duk_to_pointer;
        __duk_to_object duk_to_object;
        __duk_to_defaultvalue duk_to_defaultvalue;
        __duk_to_primitive duk_to_primitive;

        __duk_safe_to_lstring duk_safe_to_lstring;
        __duk_base64_encode duk_base64_encode;
        __duk_base64_decode duk_base64_decode;
        __duk_hex_encode duk_hex_encode;
        __duk_hex_decode duk_hex_decode;
        __duk_json_encode duk_json_encode;
        __duk_json_decode duk_json_decode;
        __duk_resize_buffer duk_resize_buffer;

        __duk_get_prop duk_get_prop;
        __duk_get_prop_string duk_get_prop_string;
        __duk_get_prop_index duk_get_prop_index;
        __duk_put_prop duk_put_prop;
        __duk_put_prop_string duk_put_prop_string;
        __duk_put_prop_index duk_put_prop_index;
        __duk_del_prop duk_del_prop;
        __duk_del_prop_string duk_del_prop_string;
        __duk_del_prop_index duk_del_prop_index;
        __duk_has_prop duk_has_prop;
        __duk_has_prop_string duk_has_prop_string;
        __duk_has_prop_index duk_has_prop_index;
        __duk_def_prop duk_def_prop;

        __duk_get_global_string duk_get_global_string;
        __duk_put_global_string duk_put_global_string;
        __duk_get_prototype duk_get_prototype;
        __duk_set_prototype duk_set_prototype;
        __duk_get_finalizer duk_get_finalizer;
        __duk_set_finalizer duk_set_finalizer;

        __duk_set_global_object duk_set_global_object;

        __duk_get_magic duk_get_magic;
        __duk_set_magic duk_set_magic;
        __duk_get_current_magic duk_get_current_magic;

        __duk_put_function_list duk_put_function_list;
        __duk_put_number_list duk_put_number_list;

        __duk_compact duk_compact;
        __duk_enum duk_enum;
        __duk_next duk_next;

        __duk_concat duk_concat;
        __duk_join duk_join;
        __duk_decode_string duk_decode_string;
        __duk_map_string duk_map_string;
        __duk_substring duk_substring;
        __duk_trim duk_trim;
        __duk_char_code_at duk_char_code_at;

        __duk_equals duk_equals;
        __duk_strict_equals duk_strict_equals;

        __duk_call duk_call;
        __duk_call_method duk_call_method;
        __duk_call_prop duk_call_prop;
        __duk_pcall duk_pcall;
        __duk_pcall_method duk_pcall_method;
        __duk_pcall_prop duk_pcall_prop;
        __duk_new duk_new;
        __duk_safe_call duk_safe_call;

        __duk_eval_raw duk_eval_raw;
        __duk_compile_raw duk_compile_raw;
        __duk_log duk_log;
        __duk_log_va duk_log_va;
        __duk_push_context_dump duk_push_context_dump;
        __duk_debugger_attach duk_debugger_attach;
        __duk_debugger_detach duk_debugger_detach;
        __duk_debugger_cooperate duk_debugger_cooperate;

        JSVMImports()
        {
            memset(this, 0, sizeof(this));
        }
    };

    static JSVMImports gJSVM;

    duk_context *duk_create_heap(duk_alloc_function alloc_func,
        duk_realloc_function realloc_func,
        duk_free_function free_func,
        void *heap_udata,
        duk_fatal_function fatal_handler)
    {
        return gJSVM.duk_create_heap(alloc_func, realloc_func, free_func, heap_udata, fatal_handler);
    }

    void duk_destroy_heap(duk_context *ctx)
    {
        gJSVM.duk_destroy_heap(ctx);
    }

    //----------------------------------------------------------------

    void *duk_alloc_raw(duk_context *ctx, duk_size_t size)
    {
        return gJSVM.duk_alloc_raw(ctx, size);
    }

    void duk_free_raw(duk_context *ctx, void *ptr)
    {
        gJSVM.duk_free_raw(ctx, ptr);
    }

    void *duk_realloc_raw(duk_context *ctx, void *ptr, duk_size_t size)
    {
        return gJSVM.duk_realloc_raw(ctx, ptr, size);
    }

    void *duk_alloc(duk_context *ctx, duk_size_t size)
    {
        return gJSVM.duk_alloc(ctx, size);
    }

    void duk_free(duk_context *ctx, void *ptr)
    {
        gJSVM.duk_free(ctx, ptr);
    }

    void *duk_realloc(duk_context *ctx, void *ptr, duk_size_t size)
    {
        return gJSVM.duk_realloc(ctx, ptr, size);
    }

    void duk_get_memory_functions(duk_context *ctx, duk_memory_functions *out_funcs)
    {
        gJSVM.duk_get_memory_functions(ctx, out_funcs);
    }

    void duk_gc(duk_context *ctx, duk_uint_t flags)
    {
        gJSVM.duk_gc(ctx, flags);
    }

    //----------------------------------------

    void duk_throw(duk_context *ctx)
    {
        gJSVM.duk_throw(ctx);
    }

    void duk_fatal(duk_context *ctx, duk_errcode_t err_code, const char *err_msg)
    {
        gJSVM.duk_fatal(ctx, err_code, err_msg);
    }

    void duk_error_raw(duk_context *ctx, duk_errcode_t err_code, const char *filename, duk_int_t line, const char *fmt, ...)
    {
        va_list arglist;
        va_start(arglist, fmt);
        gJSVM.duk_error_raw(ctx, err_code, filename, line, fmt, arglist);
        va_end(arglist);
    }

    void duk_error_va_raw(duk_context *ctx, duk_errcode_t err_code, const char *filename, duk_int_t line, const char *fmt, va_list ap)
    {
        gJSVM.duk_error_va_raw(ctx, err_code, filename, line, fmt, ap);
    }

    //--------------------------------------------

    duk_bool_t duk_is_strict_call(duk_context *ctx)
    {
        return gJSVM.duk_is_strict_call(ctx);
    }

    duk_bool_t duk_is_constructor_call(duk_context *ctx)
    {
        return gJSVM.duk_is_constructor_call(ctx);
    }

    //--------------------------------------------------------

    duk_idx_t duk_normalize_index(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_normalize_index(ctx, index);
    }
    duk_idx_t duk_require_normalize_index(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_require_normalize_index(ctx, index);
    }
    duk_bool_t duk_is_valid_index(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_is_valid_index(ctx, index);
    }
    void duk_require_valid_index(duk_context *ctx, duk_idx_t index)
    {
        gJSVM.duk_require_valid_index(ctx, index);
    }

    duk_idx_t duk_get_top(duk_context *ctx)
    {
        return gJSVM.duk_get_top(ctx);
    }
    void duk_set_top(duk_context *ctx, duk_idx_t index)
    {
        gJSVM.duk_set_top(ctx, index);
    }
    duk_idx_t duk_get_top_index(duk_context *ctx)
    {
        return gJSVM.duk_get_top_index(ctx);
    }

    duk_idx_t duk_require_top_index(duk_context *ctx)
    {
        return gJSVM.duk_require_top_index(ctx);
    }

    duk_bool_t duk_check_stack(duk_context *ctx, duk_idx_t extra)
    {
        return gJSVM.duk_check_stack(ctx, extra);
    }
    void duk_require_stack(duk_context *ctx, duk_idx_t extra)
    {
        gJSVM.duk_require_stack(ctx, extra);
    }

    duk_bool_t duk_check_stack_top(duk_context *ctx, duk_idx_t top)
    {
        return gJSVM.duk_check_stack_top(ctx, top);
    }

    void duk_require_stack_top(duk_context *ctx, duk_idx_t top)
    {
        gJSVM.duk_require_stack_top(ctx, top);
    }

    void duk_swap(duk_context *ctx, duk_idx_t index1, duk_idx_t index2)
    {
        gJSVM.duk_swap(ctx, index1, index2);
    }
    void duk_swap_top(duk_context *ctx, duk_idx_t index)
    {
        gJSVM.duk_swap_top(ctx, index);
    }
    void duk_dup(duk_context *ctx, duk_idx_t from_index)
    {
        gJSVM.duk_dup(ctx, from_index);
    }
    void duk_dup_top(duk_context *ctx)
    {
        gJSVM.duk_dup_top(ctx);
    }
    void duk_insert(duk_context *ctx, duk_idx_t to_index)
    {
        gJSVM.duk_insert(ctx, to_index);
    }
    void duk_replace(duk_context *ctx, duk_idx_t to_index)
    {
        gJSVM.duk_replace(ctx, to_index);
    }
    void duk_copy(duk_context *ctx, duk_idx_t from_index, duk_idx_t to_index)
    {
        gJSVM.duk_copy(ctx, from_index, to_index);
    }
    void duk_remove(duk_context *ctx, duk_idx_t index)
    {
        gJSVM.duk_remove(ctx, index);
    }
    void duk_xcopymove_raw(duk_context *to_ctx, duk_context *from_ctx, duk_idx_t count, duk_bool_t is_copy)
    {
        gJSVM.duk_xcopymove_raw(to_ctx, from_ctx, count, is_copy);
    }

    // -------------------------------------------------


    void duk_push_undefined(duk_context *ctx)
    {
        gJSVM.duk_push_undefined(ctx);
    }
    void duk_push_null(duk_context *ctx)
    {
        gJSVM.duk_push_null(ctx);
    }
    void duk_push_boolean(duk_context *ctx, duk_bool_t val)
    {
        gJSVM.duk_push_boolean(ctx, val);
    }
    void duk_push_true(duk_context *ctx)
    {
        gJSVM.duk_push_true(ctx);
    }
    void duk_push_false(duk_context *ctx)
    {
        gJSVM.duk_push_false(ctx);
    }
    void duk_push_number(duk_context *ctx, duk_double_t val)
    {
        gJSVM.duk_push_number(ctx, val);
    }
    void duk_push_nan(duk_context *ctx)
    {
        gJSVM.duk_push_nan(ctx);
    }
    void duk_push_int(duk_context *ctx, duk_int_t val)
    {
        gJSVM.duk_push_int(ctx, val);
    }
    void duk_push_uint(duk_context *ctx, duk_uint_t val)
    {
        gJSVM.duk_push_uint(ctx, val);
    }
    const char *duk_push_string(duk_context *ctx, const char *str)
    {
        return gJSVM.duk_push_string(ctx, str);
    }
    const char *duk_push_lstring(duk_context *ctx, const char *str, duk_size_t len)
    {
        return gJSVM.duk_push_lstring(ctx, str, len);
    }
    void duk_push_pointer(duk_context *ctx, void *p)
    {
        gJSVM.duk_push_pointer(ctx, p);
    }
    const char *duk_push_sprintf(duk_context *ctx, const char *fmt, ...)
    {
        va_list arglist;
        va_start(arglist, fmt);
        const char* returnValue = gJSVM.duk_push_sprintf(ctx, fmt, arglist);
        va_end(arglist);

        return returnValue;

    }
    const char *duk_push_vsprintf(duk_context *ctx, const char *fmt, va_list ap)
    {
        return gJSVM.duk_push_vsprintf(ctx, fmt, ap);
    }

    const char *duk_push_string_file_raw(duk_context *ctx, const char *path, duk_uint_t flags)
    {
        return gJSVM.duk_push_string_file_raw(ctx, path, flags);
    }

    void duk_push_this(duk_context *ctx)
    {
        gJSVM.duk_push_this(ctx);
    }
    void duk_push_current_function(duk_context *ctx)
    {
        gJSVM.duk_push_current_function(ctx);
    }
    void duk_push_current_thread(duk_context *ctx)
    {
        gJSVM.duk_push_current_thread(ctx);
    }
    void duk_push_global_object(duk_context *ctx)
    {
        gJSVM.duk_push_global_object(ctx);
    }
    void duk_push_heap_stash(duk_context *ctx)
    {
        gJSVM.duk_push_heap_stash(ctx);
    }
    void duk_push_global_stash(duk_context *ctx)
    {
        gJSVM.duk_push_global_stash(ctx);
    }
    void duk_push_thread_stash(duk_context *ctx, duk_context *target_ctx)
    {
        gJSVM.duk_push_thread_stash(ctx, target_ctx);
    }

    duk_idx_t duk_push_object(duk_context *ctx)
    {
        return gJSVM.duk_push_object(ctx);
    }
    duk_idx_t duk_push_array(duk_context *ctx)
    {
        return gJSVM.duk_push_array(ctx);
    }
    duk_idx_t duk_push_c_function(duk_context *ctx, duk_c_function func, duk_idx_t nargs)
    {
        return gJSVM.duk_push_c_function(ctx, func, nargs);
    }
    duk_idx_t duk_push_c_lightfunc(duk_context *ctx, duk_c_function func, duk_idx_t nargs, duk_idx_t length, duk_int_t magic)
    {
        return gJSVM.duk_push_c_lightfunc(ctx, func, nargs, length, magic);
    }
    duk_idx_t duk_push_thread_raw(duk_context *ctx, duk_uint_t flags)
    {
        return gJSVM.duk_push_thread_raw(ctx, flags);
    }

    duk_idx_t duk_push_error_object_raw(duk_context *ctx, duk_errcode_t err_code, const char *filename, duk_int_t line, const char *fmt, ...)
    {
        va_list arglist;
        va_start(arglist, fmt);
        duk_idx_t returnValue = gJSVM.duk_push_error_object_raw(ctx, err_code, filename, line, fmt, arglist);
        va_end(arglist);

        return returnValue;

    }

    duk_idx_t duk_push_error_object_va_raw(duk_context *ctx, duk_errcode_t err_code, const char *filename, duk_int_t line, const char *fmt, va_list ap)
    {
        return gJSVM.duk_push_error_object_va_raw(ctx, err_code, filename, line, fmt, ap);
    }

    void *duk_push_buffer_raw(duk_context *ctx, duk_size_t size, duk_small_uint_t flags)
    {
        return gJSVM.duk_push_buffer_raw(ctx, size, flags);
    }

    duk_idx_t duk_push_heapptr(duk_context *ctx, void *ptr)
    {
        return gJSVM.duk_push_heapptr(ctx, ptr);
    }

    // --------------------------------

    void duk_pop(duk_context *ctx)
    {
        gJSVM.duk_pop(ctx);
    }

    void duk_pop_n(duk_context *ctx, duk_idx_t count)
    {
        gJSVM.duk_pop_n(ctx, count);
    }
    void duk_pop_2(duk_context *ctx)
    {
        gJSVM.duk_pop_2(ctx);
    }
    void duk_pop_3(duk_context *ctx)
    {
        gJSVM.duk_pop_3(ctx);
    }

    duk_int_t duk_get_type(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_get_type(ctx, index);
    }
    duk_bool_t duk_check_type(duk_context *ctx, duk_idx_t index, duk_int_t type)
    {
        return gJSVM.duk_check_type(ctx, index, type);
    }
    duk_uint_t duk_get_type_mask(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_get_type_mask(ctx, index);
    }
    duk_bool_t duk_check_type_mask(duk_context *ctx, duk_idx_t index, duk_uint_t mask)
    {
        return gJSVM.duk_check_type_mask(ctx, index, mask);
    }

    duk_bool_t duk_is_undefined(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_is_undefined(ctx, index);
    }
    duk_bool_t duk_is_null(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_is_null(ctx, index);
    }
    duk_bool_t duk_is_null_or_undefined(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_is_null_or_undefined(ctx, index);
    }
    duk_bool_t duk_is_boolean(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_is_boolean(ctx, index);
    }
    duk_bool_t duk_is_number(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_is_number(ctx, index);
    }
    duk_bool_t duk_is_nan(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_is_nan(ctx, index);
    }
    duk_bool_t duk_is_string(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_is_string(ctx, index);
    }
    duk_bool_t duk_is_object(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_is_object(ctx, index);
    }
    duk_bool_t duk_is_buffer(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_is_buffer(ctx, index);
    }
    duk_bool_t duk_is_pointer(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_is_pointer(ctx, index);
    }
    duk_bool_t duk_is_lightfunc(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_is_lightfunc(ctx, index);
    }

    duk_bool_t duk_is_array(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_is_array(ctx, index);
    }
    duk_bool_t duk_is_function(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_is_function(ctx, index);
    }
    duk_bool_t duk_is_c_function(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_is_c_function(ctx, index);
    }
    duk_bool_t duk_is_ecmascript_function(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_is_ecmascript_function(ctx, index);
    }
    duk_bool_t duk_is_bound_function(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_is_bound_function(ctx, index);
    }
    duk_bool_t duk_is_thread(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_is_thread(ctx, index);
    }

    duk_bool_t duk_is_callable(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_is_callable(ctx, index);
    }
    duk_bool_t duk_is_dynamic_buffer(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_is_dynamic_buffer(ctx, index);
    }
    duk_bool_t duk_is_fixed_buffer(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_is_fixed_buffer(ctx, index);
    }

    duk_bool_t duk_is_primitive(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_is_primitive(ctx, index);
    }

    duk_errcode_t duk_get_error_code(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_get_error_code(ctx, index);
    }

    // ---------------------------------------------------------------

    duk_bool_t duk_get_boolean(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_get_boolean(ctx, index);
    }

    duk_double_t duk_get_number(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_get_number(ctx, index);
    }

    duk_int_t duk_get_int(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_get_int(ctx, index);
    }

    duk_uint_t duk_get_uint(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_get_uint(ctx, index);
    }

    const char *duk_get_string(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_get_string(ctx, index);
    }

    const char *duk_get_lstring(duk_context *ctx, duk_idx_t index, duk_size_t *out_len)
    {
        return gJSVM.duk_get_lstring(ctx, index, out_len);
    }

    void *duk_get_buffer(duk_context *ctx, duk_idx_t index, duk_size_t *out_size)
    {
        return gJSVM.duk_get_buffer(ctx, index, out_size);
    }

    void *duk_get_pointer(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_get_pointer(ctx, index);
    }

    duk_c_function duk_get_c_function(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_get_c_function(ctx, index);
    }

    duk_context *duk_get_context(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_get_context(ctx, index);
    }

    void *duk_get_heapptr(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_get_heapptr(ctx, index);
    }

    duk_size_t duk_get_length(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_get_length(ctx, index);
    }

    void duk_require_undefined(duk_context *ctx, duk_idx_t index)
    {
        gJSVM.duk_require_undefined(ctx, index);
    }

    void duk_require_null(duk_context *ctx, duk_idx_t index)
    {
        gJSVM.duk_require_null(ctx, index);
    }

    duk_bool_t duk_require_boolean(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_require_boolean(ctx, index);
    }

    duk_double_t duk_require_number(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_require_number(ctx, index);
    }

    duk_int_t duk_require_int(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_require_int(ctx, index);
    }

    duk_uint_t duk_require_uint(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_require_uint(ctx, index);
    }

    const char *duk_require_string(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_require_string(ctx, index);
    }

    const char *duk_require_lstring(duk_context *ctx, duk_idx_t index, duk_size_t *out_len)
    {
        return gJSVM.duk_require_lstring(ctx, index, out_len);
    }

    void *duk_require_buffer(duk_context *ctx, duk_idx_t index, duk_size_t *out_size)
    {
        return gJSVM.duk_require_buffer(ctx, index, out_size);
    }

    void *duk_require_pointer(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_require_pointer(ctx, index);
    }

    duk_c_function duk_require_c_function(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_require_c_function(ctx, index);
    }

    duk_context *duk_require_context(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_require_context(ctx, index);
    }

    void *duk_require_heapptr(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_require_heapptr(ctx, index);
    }

    void duk_to_undefined(duk_context *ctx, duk_idx_t index)
    {
        gJSVM.duk_to_undefined(ctx, index);
    }

    void duk_to_null(duk_context *ctx, duk_idx_t index)
    {
        gJSVM.duk_to_null(ctx, index);
    }

    duk_bool_t duk_to_boolean(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_to_boolean(ctx, index);
    }

    duk_double_t duk_to_number(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_to_number(ctx, index);
    }

    duk_int_t duk_to_int(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_to_int(ctx, index);
    }

    duk_uint_t duk_to_uint(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_to_uint(ctx, index);
    }

    duk_int32_t duk_to_int32(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_to_int32(ctx, index);
    }

    duk_uint32_t duk_to_uint32(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_to_uint32(ctx, index);
    }

    duk_uint16_t duk_to_uint16(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_to_uint16(ctx, index);
    }

    const char *duk_to_string(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_to_string(ctx, index);
    }

    const char *duk_to_lstring(duk_context *ctx, duk_idx_t index, duk_size_t *out_len)
    {
        return gJSVM.duk_to_lstring(ctx, index, out_len);
    }

    void *duk_to_buffer_raw(duk_context *ctx, duk_idx_t index, duk_size_t *out_size, duk_uint_t flags)
    {
        return gJSVM.duk_to_buffer_raw(ctx, index, out_size, flags);
    }

    void *duk_to_pointer(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_to_pointer(ctx, index);
    }

    void duk_to_object(duk_context *ctx, duk_idx_t index)
    {
        gJSVM.duk_to_object(ctx, index);
    }

    void duk_to_defaultvalue(duk_context *ctx, duk_idx_t index, duk_int_t hint)
    {
        gJSVM.duk_to_defaultvalue(ctx, index, hint);
    }

    void duk_to_primitive(duk_context *ctx, duk_idx_t index, duk_int_t hint)
    {
        gJSVM.duk_to_primitive(ctx, index, hint);
    }

    const char *duk_safe_to_lstring(duk_context *ctx, duk_idx_t index, duk_size_t *out_len)
    {
        return gJSVM.duk_safe_to_lstring(ctx, index, out_len);
    }

    const char *duk_base64_encode(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_base64_encode(ctx, index);
    }
    void duk_base64_decode(duk_context *ctx, duk_idx_t index)
    {
        gJSVM.duk_base64_decode(ctx, index);
    }
    const char *duk_hex_encode(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_hex_encode(ctx, index);
    }
    void duk_hex_decode(duk_context *ctx, duk_idx_t index)
    {
        gJSVM.duk_hex_decode(ctx, index);
    }
    const char *duk_json_encode(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_json_encode(ctx, index);
    }

    void duk_json_decode(duk_context *ctx, duk_idx_t index)
    {
        gJSVM.duk_json_decode(ctx, index);
    }

    void *duk_resize_buffer(duk_context *ctx, duk_idx_t index, duk_size_t new_size)
    {
        return gJSVM.duk_resize_buffer(ctx, index, new_size);
    }

    duk_bool_t duk_get_prop(duk_context *ctx, duk_idx_t obj_index)
    {
        return gJSVM.duk_get_prop(ctx, obj_index);
    }

    duk_bool_t duk_get_prop_string(duk_context *ctx, duk_idx_t obj_index, const char *key)
    {
        return gJSVM.duk_get_prop_string(ctx, obj_index, key);
    }

    duk_bool_t duk_get_prop_index(duk_context *ctx, duk_idx_t obj_index, duk_uarridx_t arr_index)
    {
        return gJSVM.duk_get_prop_index(ctx, obj_index, arr_index);
    }

    duk_bool_t duk_put_prop(duk_context *ctx, duk_idx_t obj_index)
    {
        return gJSVM.duk_put_prop(ctx, obj_index);
    }

    duk_bool_t duk_put_prop_string(duk_context *ctx, duk_idx_t obj_index, const char *key)
    {
        return gJSVM.duk_put_prop_string(ctx, obj_index, key);
    }

    duk_bool_t duk_put_prop_index(duk_context *ctx, duk_idx_t obj_index, duk_uarridx_t arr_index)
    {
        return gJSVM.duk_put_prop_index(ctx, obj_index, arr_index);
    }

    duk_bool_t duk_del_prop(duk_context *ctx, duk_idx_t obj_index)
    {
        return gJSVM.duk_del_prop(ctx, obj_index);
    }

    duk_bool_t duk_del_prop_string(duk_context *ctx, duk_idx_t obj_index, const char *key)
    {
        return gJSVM.duk_del_prop_string(ctx, obj_index, key);
    }

    duk_bool_t duk_del_prop_index(duk_context *ctx, duk_idx_t obj_index, duk_uarridx_t arr_index)
    {
        return gJSVM.duk_del_prop_index(ctx, obj_index, arr_index);
    }

    duk_bool_t duk_has_prop(duk_context *ctx, duk_idx_t obj_index)
    {
        return gJSVM.duk_has_prop(ctx, obj_index);
    }

    duk_bool_t duk_has_prop_string(duk_context *ctx, duk_idx_t obj_index, const char *key)
    {
        return gJSVM.duk_has_prop_string(ctx, obj_index, key);
    }

    duk_bool_t duk_has_prop_index(duk_context *ctx, duk_idx_t obj_index, duk_uarridx_t arr_index)
    {
        return gJSVM.duk_has_prop_index(ctx, obj_index, arr_index);
    }

    void duk_def_prop(duk_context *ctx, duk_idx_t obj_index, duk_uint_t flags)
    {
        gJSVM.duk_def_prop(ctx, obj_index, flags);
    }

    duk_bool_t duk_get_global_string(duk_context *ctx, const char *key)
    {
        return gJSVM.duk_get_global_string(ctx, key);
    }

    duk_bool_t duk_put_global_string(duk_context *ctx, const char *key)
    {
        return gJSVM.duk_put_global_string(ctx, key);
    }

    void duk_get_prototype(duk_context *ctx, duk_idx_t index)
    {
        gJSVM.duk_get_prototype(ctx, index);
    }

    void duk_set_prototype(duk_context *ctx, duk_idx_t index)
    {
        gJSVM.duk_set_prototype(ctx, index);
    }

    void duk_get_finalizer(duk_context *ctx, duk_idx_t index)
    {
        gJSVM.duk_get_finalizer(ctx, index);
    }

    void duk_set_finalizer(duk_context *ctx, duk_idx_t index)
    {
        gJSVM.duk_set_finalizer(ctx, index);
    }

    void duk_set_global_object(duk_context *ctx)
    {
        gJSVM.duk_set_global_object(ctx);
    }

    duk_int_t duk_get_magic(duk_context *ctx, duk_idx_t index)
    {
        return gJSVM.duk_get_magic(ctx, index);
    }

    void duk_set_magic(duk_context *ctx, duk_idx_t index, duk_int_t magic)
    {
        gJSVM.duk_set_magic(ctx, index, magic);
    }

    duk_int_t duk_get_current_magic(duk_context *ctx)
    {
        return gJSVM.duk_get_current_magic(ctx);
    }

    void duk_put_function_list(duk_context *ctx, duk_idx_t obj_index, const duk_function_list_entry *funcs)
    {
        gJSVM.duk_put_function_list(ctx, obj_index, funcs);
    }

    void duk_put_number_list(duk_context *ctx, duk_idx_t obj_index, const duk_number_list_entry *numbers)
    {
        gJSVM.duk_put_number_list(ctx, obj_index, numbers);
    }

    void duk_compact(duk_context *ctx, duk_idx_t obj_index)
    {
        gJSVM.duk_compact(ctx, obj_index);
    }

    void duk_enum(duk_context *ctx, duk_idx_t obj_index, duk_uint_t enum_flags)
    {
        gJSVM.duk_enum(ctx, obj_index, enum_flags);
    }

    duk_bool_t duk_next(duk_context *ctx, duk_idx_t enum_index, duk_bool_t get_value)
    {
        return gJSVM.duk_next(ctx, enum_index, get_value);
    }

    void duk_concat(duk_context *ctx, duk_idx_t count)
    {
        gJSVM.duk_concat(ctx, count);
    }
    void duk_join(duk_context *ctx, duk_idx_t count)
    {
        gJSVM.duk_join(ctx, count);
    }
    void duk_decode_string(duk_context *ctx, duk_idx_t index, duk_decode_char_function callback, void *udata)
    {
        gJSVM.duk_decode_string(ctx, index, callback, udata);
    }
    void duk_map_string(duk_context *ctx, duk_idx_t index, duk_map_char_function callback, void *udata)
    {
        gJSVM.duk_map_string(ctx, index, callback, udata);
    }
    void duk_substring(duk_context *ctx, duk_idx_t index, duk_size_t start_char_offset, duk_size_t end_char_offset)
    {
        gJSVM.duk_substring(ctx, index, start_char_offset, end_char_offset);
    }
    void duk_trim(duk_context *ctx, duk_idx_t index)
    {
        gJSVM.duk_trim(ctx, index);
    }
    duk_codepoint_t duk_char_code_at(duk_context *ctx, duk_idx_t index, duk_size_t char_offset)
    {
        return gJSVM.duk_char_code_at(ctx, index, char_offset);
    }

    duk_bool_t duk_equals(duk_context *ctx, duk_idx_t index1, duk_idx_t index2)
    {
        return gJSVM.duk_equals(ctx, index1, index2);
    }
    duk_bool_t duk_strict_equals(duk_context *ctx, duk_idx_t index1, duk_idx_t index2)
    {
        return gJSVM.duk_strict_equals(ctx, index1, index2);
    }

    void duk_call(duk_context *ctx, duk_idx_t nargs)
    {
        gJSVM.duk_call(ctx, nargs);
    }
    void duk_call_method(duk_context *ctx, duk_idx_t nargs)
    {
        gJSVM.duk_call_method(ctx, nargs);
    }
    void duk_call_prop(duk_context *ctx, duk_idx_t obj_index, duk_idx_t nargs)
    {
        gJSVM.duk_call_prop(ctx, obj_index, nargs);
    }
    duk_int_t duk_pcall(duk_context *ctx, duk_idx_t nargs)
    {
        return gJSVM.duk_pcall(ctx, nargs);
    }
    duk_int_t duk_pcall_method(duk_context *ctx, duk_idx_t nargs)
    {
        return gJSVM.duk_pcall_method(ctx, nargs);
    }
    duk_int_t duk_pcall_prop(duk_context *ctx, duk_idx_t obj_index, duk_idx_t nargs)
    {
        return gJSVM.duk_pcall_prop(ctx, obj_index, nargs);
    }
    void duk_new(duk_context *ctx, duk_idx_t nargs)
    {
        gJSVM.duk_new(ctx, nargs);
    }
    duk_int_t duk_safe_call(duk_context *ctx, duk_safe_call_function func, duk_idx_t nargs, duk_idx_t nrets)
    {
        return gJSVM.duk_safe_call(ctx, func, nargs, nrets);
    }

    duk_int_t duk_eval_raw(duk_context *ctx, const char *src_buffer, duk_size_t src_length, duk_uint_t flags)
    {
        return gJSVM.duk_eval_raw(ctx, src_buffer, src_length, flags);
    }
    duk_int_t duk_compile_raw(duk_context *ctx, const char *src_buffer, duk_size_t src_length, duk_uint_t flags)
    {
        return gJSVM.duk_compile_raw(ctx, src_buffer, src_length, flags);
    }

    void duk_log(duk_context *ctx, duk_int_t level, const char *fmt, ...)
    {
        va_list arglist;
        va_start(arglist, fmt);
        gJSVM.duk_log(ctx, level, fmt, arglist);
        va_end(arglist);

    }
    void duk_log_va(duk_context *ctx, duk_int_t level, const char *fmt, va_list ap)
    {
        gJSVM.duk_log_va(ctx, level, fmt, ap);
    }


    void duk_push_context_dump(duk_context *ctx)
    {
        gJSVM.duk_push_context_dump(ctx);
    }

    void duk_debugger_attach(duk_context *ctx,
        duk_debug_read_function read_cb,
        duk_debug_write_function write_cb,
        duk_debug_peek_function peek_cb,
        duk_debug_read_flush_function read_flush_cb,
        duk_debug_write_flush_function write_flush_cb,
        duk_debug_detached_function detached_cb,
        void *udata)
    {
        gJSVM.duk_debugger_attach(ctx, read_cb, write_cb, peek_cb, read_flush_cb, write_flush_cb, detached_cb, udata);
    }

    void duk_debugger_detach(duk_context *ctx)
    {
        gJSVM.duk_debugger_detach(ctx);
    }

    void duk_debugger_cooperate(duk_context *ctx)
    {
        gJSVM.duk_debugger_cooperate(ctx);
    }

    // End Duktape Bindings

    bool PLUGIN_EXPORT_API atomic_plugin_validate(int version, void *jsvmImports, size_t jsvmImportsSize)
    {
        if (version != ATOMIC_JSPLUGIN_VERSION)
            return false;

        if (jsvmImportsSize != sizeof(JSVMImports))
            return false;

        gJSVM = *((JSVMImports*)jsvmImports);

        return true;
    }

}

#endif //ATOMIC_PLUGIN_MAIN

#endif // ATOMIC_PLUGIN_H

