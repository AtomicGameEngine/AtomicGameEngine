/*
 *  Duktape public API for Duktape 2.0.1.
 *
 *  See the API reference for documentation on call semantics.
 *  The exposed API is inside the DUK_API_PUBLIC_H_INCLUDED
 *  include guard.  Other parts of the header are Duktape
 *  internal and related to platform/compiler/feature detection.
 *
 *  Git commit external (external).
 *  Git branch external.
 *
 *  See Duktape AUTHORS.rst and LICENSE.txt for copyright and
 *  licensing information.
 */

/* LICENSE.txt */
/*
 *  ===============
 *  Duktape license
 *  ===============
 *  
 *  (http://opensource.org/licenses/MIT)
 *  
 *  Copyright (c) 2013-2017 by Duktape authors (see AUTHORS.rst)
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 */

/* AUTHORS.rst */
/*
 *  ===============
 *  Duktape authors
 *  ===============
 *  
 *  Copyright
 *  =========
 *  
 *  Duktape copyrights are held by its authors.  Each author has a copyright
 *  to their contribution, and agrees to irrevocably license the contribution
 *  under the Duktape ``LICENSE.txt``.
 *  
 *  Authors
 *  =======
 *  
 *  Please include an e-mail address, a link to your GitHub profile, or something
 *  similar to allow your contribution to be identified accurately.
 *  
 *  The following people have contributed code, website contents, or Wiki contents,
 *  and agreed to irrevocably license their contributions under the Duktape
 *  ``LICENSE.txt`` (in order of appearance):
 *  
 *  * Sami Vaarala <sami.vaarala@iki.fi>
 *  * Niki Dobrev
 *  * Andreas \u00d6man <andreas@lonelycoder.com>
 *  * L\u00e1szl\u00f3 Lang\u00f3 <llango.u-szeged@partner.samsung.com>
 *  * Legimet <legimet.calc@gmail.com>
 *  * Karl Skomski <karl@skomski.com>
 *  * Bruce Pascoe <fatcerberus1@gmail.com>
 *  * Ren\u00e9 Hollander <rene@rene8888.at>
 *  * Julien Hamaide (https://github.com/crazyjul)
 *  * Sebastian G\u00f6tte (https://github.com/jaseg)
 *  * Tomasz Magulski (https://github.com/magul)
 *  * \D. Bohdan (https://github.com/dbohdan)
 *  * Ond\u0159ej Jirman (https://github.com/megous)
 *  * Sa\u00fal Ibarra Corretg\u00e9 <saghul@gmail.com>
 *  * Jeremy HU <huxingyi@msn.com>
 *  * Ole Andr\u00e9 Vadla Ravn\u00e5s (https://github.com/oleavr)
 *  * Harold Brenes (https://github.com/harold-b)
 *  * Oliver Crow (https://github.com/ocrow)
 *  * Jakub Ch\u0142api\u0144ski (https://github.com/jchlapinski)
 *  * Brett Vickers (https://github.com/beevik)
 *  * Dominik Okwieka (https://github.com/okitec)
 *  * Remko Tron\u00e7on (https://el-tramo.be)
 *  
 *  Other contributions
 *  ===================
 *  
 *  The following people have contributed something other than code (e.g. reported
 *  bugs, provided ideas, etc; roughly in order of appearance):
 *  
 *  * Greg Burns
 *  * Anthony Rabine
 *  * Carlos Costa
 *  * Aur\u00e9lien Bouilland
 *  * Preet Desai (Pris Matic)
 *  * judofyr (http://www.reddit.com/user/judofyr)
 *  * Jason Woofenden
 *  * Micha\u0142 Przyby\u015b
 *  * Anthony Howe
 *  * Conrad Pankoff
 *  * Jim Schimpf
 *  * Rajaran Gaunker (https://github.com/zimbabao)
 *  * Andreas \u00d6man
 *  * Doug Sanden
 *  * Josh Engebretson (https://github.com/JoshEngebretson)
 *  * Remo Eichenberger (https://github.com/remoe)
 *  * Mamod Mehyar (https://github.com/mamod)
 *  * David Demelier (https://github.com/markand)
 *  * Tim Caswell (https://github.com/creationix)
 *  * Mitchell Blank Jr (https://github.com/mitchblank)
 *  * https://github.com/yushli
 *  * Seo Sanghyeon (https://github.com/sanxiyn)
 *  * Han ChoongWoo (https://github.com/tunz)
 *  * Joshua Peek (https://github.com/josh)
 *  * Bruce E. Pascoe (https://github.com/fatcerberus)
 *  * https://github.com/Kelledin
 *  * https://github.com/sstruchtrup
 *  * Michael Drake (https://github.com/tlsa)
 *  * https://github.com/chris-y
 *  * Laurent Zubiaur (https://github.com/lzubiaur)
 *  * Neil Kolban (https://github.com/nkolban)
 *  
 *  If you are accidentally missing from this list, send me an e-mail
 *  (``sami.vaarala@iki.fi``) and I'll fix the omission.
 */

#if !defined(DUKTAPE_H_INCLUDED)
#define DUKTAPE_H_INCLUDED

#define DUK_SINGLE_FILE

/* External duk_config.h provides platform/compiler/OS dependent
 * typedefs and macros, and DUK_USE_xxx config options so that
 * the rest of Duktape doesn't need to do any feature detection.
 */
#include "duk_config.h"

/*
 *  BEGIN PUBLIC API
 */

#if !defined(DUK_API_PUBLIC_H_INCLUDED)
#define DUK_API_PUBLIC_H_INCLUDED

/*
 *  Avoid C++ name mangling
 */

#if defined(__cplusplus)
extern "C" {
#endif

/*
 *  Some defines forwarded from feature detection
 */

#undef DUK_API_VARIADIC_MACROS
#if defined(DUK_USE_VARIADIC_MACROS)
#define DUK_API_VARIADIC_MACROS
#endif

#define DUK_API_NORETURN(decl) DUK_NORETURN(decl)

/*
 *  Public API specific typedefs
 *
 *  Many types are wrapped by Duktape for portability to rare platforms
 *  where e.g. 'int' is a 16-bit type.  See practical typing discussion
 *  in Duktape web documentation.
 */

struct duk_thread_state;
struct duk_memory_functions;
struct duk_function_list_entry;
struct duk_number_list_entry;
struct duk_time_components;

/* duk_context is now defined in duk_config.h because it may also be
 * referenced there by prototypes.
 */
typedef struct duk_thread_state duk_thread_state;
typedef struct duk_memory_functions duk_memory_functions;
typedef struct duk_function_list_entry duk_function_list_entry;
typedef struct duk_number_list_entry duk_number_list_entry;
typedef struct duk_time_components duk_time_components;

typedef duk_ret_t (*duk_c_function)(duk_context *ctx);
typedef void *(*duk_alloc_function) (void *udata, duk_size_t size);
typedef void *(*duk_realloc_function) (void *udata, void *ptr, duk_size_t size);
typedef void (*duk_free_function) (void *udata, void *ptr);
typedef void (*duk_fatal_function) (void *udata, const char *msg);
typedef void (*duk_decode_char_function) (void *udata, duk_codepoint_t codepoint);
typedef duk_codepoint_t (*duk_map_char_function) (void *udata, duk_codepoint_t codepoint);
typedef duk_ret_t (*duk_safe_call_function) (duk_context *ctx, void *udata);
typedef duk_size_t (*duk_debug_read_function) (void *udata, char *buffer, duk_size_t length);
typedef duk_size_t (*duk_debug_write_function) (void *udata, const char *buffer, duk_size_t length);
typedef duk_size_t (*duk_debug_peek_function) (void *udata);
typedef void (*duk_debug_read_flush_function) (void *udata);
typedef void (*duk_debug_write_flush_function) (void *udata);
typedef duk_idx_t (*duk_debug_request_function) (duk_context *ctx, void *udata, duk_idx_t nvalues);
typedef void (*duk_debug_detached_function) (duk_context *ctx, void *udata);

struct duk_thread_state {
	/* XXX: Enough space to hold internal suspend/resume structure.
	 * This is rather awkward and to be fixed when the internal
	 * structure is visible for the public API header.
	 */
	char data[128];
};

struct duk_memory_functions {
	duk_alloc_function alloc_func;
	duk_realloc_function realloc_func;
	duk_free_function free_func;
	void *udata;
};

struct duk_function_list_entry {
	const char *key;
	duk_c_function value;
	duk_idx_t nargs;
};

struct duk_number_list_entry {
	const char *key;
	duk_double_t value;
};

struct duk_time_components {
	duk_double_t year;          /* year, e.g. 2016, Ecmascript year range */
	duk_double_t month;         /* month: 1-12 */
	duk_double_t day;           /* day: 1-31 */
	duk_double_t hours;         /* hour: 0-59 */
	duk_double_t minutes;       /* minute: 0-59 */
	duk_double_t seconds;       /* second: 0-59 (in POSIX time no leap second) */
	duk_double_t milliseconds;  /* may contain sub-millisecond fractions */
	duk_double_t weekday;       /* weekday: 0-6, 0=Sunday, 1=Monday, ..., 6=Saturday */
};

/*
 *  Constants
 */

/* Duktape version, (major * 10000) + (minor * 100) + patch.  Allows C code
 * to #if (DUK_VERSION >= NNN) against Duktape API version.  The same value
 * is also available to Ecmascript code in Duktape.version.  Unofficial
 * development snapshots have 99 for patch level (e.g. 0.10.99 would be a
 * development version after 0.10.0 but before the next official release).
 */
#define DUK_VERSION                       20001L

/* Git commit, describe, and branch for Duktape build.  Useful for
 * non-official snapshot builds so that application code can easily log
 * which Duktape snapshot was used.  Not available in the Ecmascript
 * environment.
 */
#define DUK_GIT_COMMIT                    "external"
#define DUK_GIT_DESCRIBE                  "external"
#define DUK_GIT_BRANCH                    "external"

/* Duktape debug protocol version used by this build. */
#define DUK_DEBUG_PROTOCOL_VERSION        2

/* Used to represent invalid index; if caller uses this without checking,
 * this index will map to a non-existent stack entry.  Also used in some
 * API calls as a marker to denote "no value".
 */
#define DUK_INVALID_INDEX                 DUK_IDX_MIN

/* Indicates that a native function does not have a fixed number of args,
 * and the argument stack should not be capped/extended at all.
 */
#define DUK_VARARGS                       ((duk_int_t) (-1))

/* Number of value stack entries (in addition to actual call arguments)
 * guaranteed to be allocated on entry to a Duktape/C function.
 */
#define DUK_API_ENTRY_STACK               64

/* Value types, used by e.g. duk_get_type() */
#define DUK_TYPE_MIN                      0
#define DUK_TYPE_NONE                     0    /* no value, e.g. invalid index */
#define DUK_TYPE_UNDEFINED                1    /* Ecmascript undefined */
#define DUK_TYPE_NULL                     2    /* Ecmascript null */
#define DUK_TYPE_BOOLEAN                  3    /* Ecmascript boolean: 0 or 1 */
#define DUK_TYPE_NUMBER                   4    /* Ecmascript number: double */
#define DUK_TYPE_STRING                   5    /* Ecmascript string: CESU-8 / extended UTF-8 encoded */
#define DUK_TYPE_OBJECT                   6    /* Ecmascript object: includes objects, arrays, functions, threads */
#define DUK_TYPE_BUFFER                   7    /* fixed or dynamic, garbage collected byte buffer */
#define DUK_TYPE_POINTER                  8    /* raw void pointer */
#define DUK_TYPE_LIGHTFUNC                9    /* lightweight function pointer */
#define DUK_TYPE_MAX                      9

/* Value mask types, used by e.g. duk_get_type_mask() */
#define DUK_TYPE_MASK_NONE                (1 << DUK_TYPE_NONE)
#define DUK_TYPE_MASK_UNDEFINED           (1 << DUK_TYPE_UNDEFINED)
#define DUK_TYPE_MASK_NULL                (1 << DUK_TYPE_NULL)
#define DUK_TYPE_MASK_BOOLEAN             (1 << DUK_TYPE_BOOLEAN)
#define DUK_TYPE_MASK_NUMBER              (1 << DUK_TYPE_NUMBER)
#define DUK_TYPE_MASK_STRING              (1 << DUK_TYPE_STRING)
#define DUK_TYPE_MASK_OBJECT              (1 << DUK_TYPE_OBJECT)
#define DUK_TYPE_MASK_BUFFER              (1 << DUK_TYPE_BUFFER)
#define DUK_TYPE_MASK_POINTER             (1 << DUK_TYPE_POINTER)
#define DUK_TYPE_MASK_LIGHTFUNC           (1 << DUK_TYPE_LIGHTFUNC)
#define DUK_TYPE_MASK_THROW               (1 << 10)  /* internal flag value: throw if mask doesn't match */
#define DUK_TYPE_MASK_PROMOTE             (1 << 11)  /* internal flag value: promote to object if mask matches */

/* Coercion hints */
#define DUK_HINT_NONE                     0    /* prefer number, unless input is a Date, in which
                                                * case prefer string (E5 Section 8.12.8)
                                                */
#define DUK_HINT_STRING                   1    /* prefer string */
#define DUK_HINT_NUMBER                   2    /* prefer number */

/* Enumeration flags for duk_enum() */
#define DUK_ENUM_INCLUDE_NONENUMERABLE    (1 << 0)    /* enumerate non-numerable properties in addition to enumerable */
#define DUK_ENUM_INCLUDE_HIDDEN           (1 << 1)    /* enumerate hidden symbols too (in Duktape 1.x called internal properties) */
#define DUK_ENUM_INCLUDE_SYMBOLS          (1 << 2)    /* enumerate symbols */
#define DUK_ENUM_EXCLUDE_STRINGS          (1 << 3)    /* exclude strings */
#define DUK_ENUM_OWN_PROPERTIES_ONLY      (1 << 4)    /* don't walk prototype chain, only check own properties */
#define DUK_ENUM_ARRAY_INDICES_ONLY       (1 << 5)    /* only enumerate array indices */
#define DUK_ENUM_SORT_ARRAY_INDICES       (1 << 6)    /* sort array indices (applied to full enumeration result, including inherited array indices) */
#define DUK_ENUM_NO_PROXY_BEHAVIOR        (1 << 7)    /* enumerate a proxy object itself without invoking proxy behavior */

/* Compilation flags for duk_compile() and duk_eval() */
/* DUK_COMPILE_xxx bits 0-2 are reserved for an internal 'nargs' argument.
 */
#define DUK_COMPILE_EVAL                  (1 << 3)    /* compile eval code (instead of global code) */
#define DUK_COMPILE_FUNCTION              (1 << 4)    /* compile function code (instead of global code) */
#define DUK_COMPILE_STRICT                (1 << 5)    /* use strict (outer) context for global, eval, or function code */
#define DUK_COMPILE_SAFE                  (1 << 6)    /* (internal) catch compilation errors */
#define DUK_COMPILE_NORESULT              (1 << 7)    /* (internal) omit eval result */
#define DUK_COMPILE_NOSOURCE              (1 << 8)    /* (internal) no source string on stack */
#define DUK_COMPILE_STRLEN                (1 << 9)    /* (internal) take strlen() of src_buffer (avoids double evaluation in macro) */
#define DUK_COMPILE_NOFILENAME            (1 << 10)    /* (internal) no filename on stack */

/* Flags for duk_def_prop() and its variants */
#define DUK_DEFPROP_WRITABLE              (1 << 0)    /* set writable (effective if DUK_DEFPROP_HAVE_WRITABLE set) */
#define DUK_DEFPROP_ENUMERABLE            (1 << 1)    /* set enumerable (effective if DUK_DEFPROP_HAVE_ENUMERABLE set) */
#define DUK_DEFPROP_CONFIGURABLE          (1 << 2)    /* set configurable (effective if DUK_DEFPROP_HAVE_CONFIGURABLE set) */
#define DUK_DEFPROP_HAVE_WRITABLE         (1 << 3)    /* set/clear writable */
#define DUK_DEFPROP_HAVE_ENUMERABLE       (1 << 4)    /* set/clear enumerable */
#define DUK_DEFPROP_HAVE_CONFIGURABLE     (1 << 5)    /* set/clear configurable */
#define DUK_DEFPROP_HAVE_VALUE            (1 << 6)    /* set value (given on value stack) */
#define DUK_DEFPROP_HAVE_GETTER           (1 << 7)    /* set getter (given on value stack) */
#define DUK_DEFPROP_HAVE_SETTER           (1 << 8)    /* set setter (given on value stack) */
#define DUK_DEFPROP_FORCE                 (1 << 9)    /* force change if possible, may still fail for e.g. virtual properties */
#define DUK_DEFPROP_SET_WRITABLE          (DUK_DEFPROP_HAVE_WRITABLE | DUK_DEFPROP_WRITABLE)
#define DUK_DEFPROP_CLEAR_WRITABLE        DUK_DEFPROP_HAVE_WRITABLE
#define DUK_DEFPROP_SET_ENUMERABLE        (DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE)
#define DUK_DEFPROP_CLEAR_ENUMERABLE      DUK_DEFPROP_HAVE_ENUMERABLE
#define DUK_DEFPROP_SET_CONFIGURABLE      (DUK_DEFPROP_HAVE_CONFIGURABLE | DUK_DEFPROP_CONFIGURABLE)
#define DUK_DEFPROP_CLEAR_CONFIGURABLE    DUK_DEFPROP_HAVE_CONFIGURABLE

/* Flags for duk_push_thread_raw() */
#define DUK_THREAD_NEW_GLOBAL_ENV         (1 << 0)    /* create a new global environment */

/* Flags for duk_gc() */
#define DUK_GC_COMPACT                    (1 << 0)    /* compact heap objects */

/* Error codes (must be 8 bits at most, see duk_error.h) */
#define DUK_ERR_NONE                      0    /* no error (e.g. from duk_get_error_code()) */
#define DUK_ERR_ERROR                     1    /* Error */
#define DUK_ERR_EVAL_ERROR                2    /* EvalError */
#define DUK_ERR_RANGE_ERROR               3    /* RangeError */
#define DUK_ERR_REFERENCE_ERROR           4    /* ReferenceError */
#define DUK_ERR_SYNTAX_ERROR              5    /* SyntaxError */
#define DUK_ERR_TYPE_ERROR                6    /* TypeError */
#define DUK_ERR_URI_ERROR                 7    /* URIError */

/* Return codes for C functions (shortcut for throwing an error) */
#define DUK_RET_ERROR                     (-DUK_ERR_ERROR)
#define DUK_RET_EVAL_ERROR                (-DUK_ERR_EVAL_ERROR)
#define DUK_RET_RANGE_ERROR               (-DUK_ERR_RANGE_ERROR)
#define DUK_RET_REFERENCE_ERROR           (-DUK_ERR_REFERENCE_ERROR)
#define DUK_RET_SYNTAX_ERROR              (-DUK_ERR_SYNTAX_ERROR)
#define DUK_RET_TYPE_ERROR                (-DUK_ERR_TYPE_ERROR)
#define DUK_RET_URI_ERROR                 (-DUK_ERR_URI_ERROR)

/* Return codes for protected calls (duk_safe_call(), duk_pcall()) */
#define DUK_EXEC_SUCCESS                  0
#define DUK_EXEC_ERROR                    1

/* Debug levels for DUK_USE_DEBUG_WRITE(). */
#define DUK_LEVEL_DEBUG                   0
#define DUK_LEVEL_DDEBUG                  1
#define DUK_LEVEL_DDDEBUG                 2

/*
 *  If no variadic macros, __FILE__ and __LINE__ are passed through globals
 *  which is ugly and not thread safe.
 */

#if !defined(DUK_API_VARIADIC_MACROS)
DUK_EXTERNAL_DECL const char *duk_api_global_filename;
DUK_EXTERNAL_DECL duk_int_t duk_api_global_line;
#endif

/*
 *  Context management
 */

DUK_EXTERNAL_DECL
duk_context *duk_create_heap(duk_alloc_function alloc_func,
                             duk_realloc_function realloc_func,
                             duk_free_function free_func,
                             void *heap_udata,
                             duk_fatal_function fatal_handler);
DUK_EXTERNAL_DECL void duk_destroy_heap(duk_context *ctx);

DUK_EXTERNAL_DECL void duk_suspend(duk_context *ctx, duk_thread_state *state);
DUK_EXTERNAL_DECL void duk_resume(duk_context *ctx, const duk_thread_state *state);

#define duk_create_heap_default() \
	duk_create_heap(NULL, NULL, NULL, NULL, NULL)

/*
 *  Memory management
 *
 *  Raw functions have no side effects (cannot trigger GC).
 */

DUK_EXTERNAL_DECL void *duk_alloc_raw(duk_context *ctx, duk_size_t size);
DUK_EXTERNAL_DECL void duk_free_raw(duk_context *ctx, void *ptr);
DUK_EXTERNAL_DECL void *duk_realloc_raw(duk_context *ctx, void *ptr, duk_size_t size);
DUK_EXTERNAL_DECL void *duk_alloc(duk_context *ctx, duk_size_t size);
DUK_EXTERNAL_DECL void duk_free(duk_context *ctx, void *ptr);
DUK_EXTERNAL_DECL void *duk_realloc(duk_context *ctx, void *ptr, duk_size_t size);
DUK_EXTERNAL_DECL void duk_get_memory_functions(duk_context *ctx, duk_memory_functions *out_funcs);
DUK_EXTERNAL_DECL void duk_gc(duk_context *ctx, duk_uint_t flags);

/*
 *  Error handling
 */

DUK_API_NORETURN(DUK_EXTERNAL_DECL void duk_throw_raw(duk_context *ctx));
#define duk_throw(ctx) \
	(duk_throw_raw((ctx)), (duk_ret_t) 0)
DUK_API_NORETURN(DUK_EXTERNAL_DECL void duk_fatal_raw(duk_context *ctx, const char *err_msg));
#define duk_fatal(ctx,err_msg) \
	(duk_fatal_raw((ctx), (err_msg)), (duk_ret_t) 0)

#if defined(DUK_API_VARIADIC_MACROS)
DUK_API_NORETURN(DUK_EXTERNAL_DECL void duk_error_raw(duk_context *ctx, duk_errcode_t err_code, const char *filename, duk_int_t line, const char *fmt, ...));
#define duk_error(ctx,err_code,...)  \
	(duk_error_raw((ctx), (duk_errcode_t) (err_code), (const char *) (DUK_FILE_MACRO), (duk_int_t) (DUK_LINE_MACRO), __VA_ARGS__), (duk_ret_t) 0)
#define duk_generic_error(ctx,...)  \
	(duk_error_raw((ctx), (duk_errcode_t) DUK_ERR_ERROR, (const char *) (DUK_FILE_MACRO), (duk_int_t) (DUK_LINE_MACRO), __VA_ARGS__), (duk_ret_t) 0)
#define duk_eval_error(ctx,...)  \
	(duk_error_raw((ctx), (duk_errcode_t) DUK_ERR_EVAL_ERROR, (const char *) (DUK_FILE_MACRO), (duk_int_t) (DUK_LINE_MACRO), __VA_ARGS__), (duk_ret_t) 0)
#define duk_range_error(ctx,...)  \
	(duk_error_raw((ctx), (duk_errcode_t) DUK_ERR_RANGE_ERROR, (const char *) (DUK_FILE_MACRO), (duk_int_t) (DUK_LINE_MACRO), __VA_ARGS__), (duk_ret_t) 0)
#define duk_reference_error(ctx,...)  \
	(duk_error_raw((ctx), (duk_errcode_t) DUK_ERR_REFERENCE_ERROR, (const char *) (DUK_FILE_MACRO), (duk_int_t) (DUK_LINE_MACRO), __VA_ARGS__), (duk_ret_t) 0)
#define duk_syntax_error(ctx,...)  \
	(duk_error_raw((ctx), (duk_errcode_t) DUK_ERR_SYNTAX_ERROR, (const char *) (DUK_FILE_MACRO), (duk_int_t) (DUK_LINE_MACRO), __VA_ARGS__), (duk_ret_t) 0)
#define duk_type_error(ctx,...)  \
	(duk_error_raw((ctx), (duk_errcode_t) DUK_ERR_TYPE_ERROR, (const char *) (DUK_FILE_MACRO), (duk_int_t) (DUK_LINE_MACRO), __VA_ARGS__), (duk_ret_t) 0)
#define duk_uri_error(ctx,...)  \
	(duk_error_raw((ctx), (duk_errcode_t) DUK_ERR_URI_ERROR, (const char *) (DUK_FILE_MACRO), (duk_int_t) (DUK_LINE_MACRO), __VA_ARGS__), (duk_ret_t) 0)
#else  /* DUK_API_VARIADIC_MACROS */
/* For legacy compilers without variadic macros a macro hack is used to allow
 * variable arguments.  While the macro allows "return duk_error(...)", it
 * will fail with e.g. "(void) duk_error(...)".  The calls are noreturn but
 * with a return value to allow the "return duk_error(...)" idiom.  This may
 * cause some compiler warnings, but without noreturn the generated code is
 * often worse.  The same approach as with variadic macros (using
 * "(duk_error(...), 0)") won't work due to the macro hack structure.
 */
DUK_API_NORETURN(DUK_EXTERNAL_DECL duk_ret_t duk_error_stash(duk_context *ctx, duk_errcode_t err_code, const char *fmt, ...));
DUK_API_NORETURN(DUK_EXTERNAL_DECL duk_ret_t duk_generic_error_stash(duk_context *ctx, const char *fmt, ...));
DUK_API_NORETURN(DUK_EXTERNAL_DECL duk_ret_t duk_eval_error_stash(duk_context *ctx, const char *fmt, ...));
DUK_API_NORETURN(DUK_EXTERNAL_DECL duk_ret_t duk_range_error_stash(duk_context *ctx, const char *fmt, ...));
DUK_API_NORETURN(DUK_EXTERNAL_DECL duk_ret_t duk_reference_error_stash(duk_context *ctx, const char *fmt, ...));
DUK_API_NORETURN(DUK_EXTERNAL_DECL duk_ret_t duk_syntax_error_stash(duk_context *ctx, const char *fmt, ...));
DUK_API_NORETURN(DUK_EXTERNAL_DECL duk_ret_t duk_type_error_stash(duk_context *ctx, const char *fmt, ...));
DUK_API_NORETURN(DUK_EXTERNAL_DECL duk_ret_t duk_uri_error_stash(duk_context *ctx, const char *fmt, ...));
#define duk_error  \
	(duk_api_global_filename = (const char *) (DUK_FILE_MACRO), \
	 duk_api_global_line = (duk_int_t) (DUK_LINE_MACRO), \
	 duk_error_stash)  /* last value is func pointer, arguments follow in parens */
#define duk_generic_error  \
	(duk_api_global_filename = (const char *) (DUK_FILE_MACRO), \
	 duk_api_global_line = (duk_int_t) (DUK_LINE_MACRO), \
	 duk_generic_error_stash)
#define duk_eval_error  \
	(duk_api_global_filename = (const char *) (DUK_FILE_MACRO), \
	 duk_api_global_line = (duk_int_t) (DUK_LINE_MACRO), \
	 duk_eval_error_stash)
#define duk_range_error  \
	(duk_api_global_filename = (const char *) (DUK_FILE_MACRO), \
	 duk_api_global_line = (duk_int_t) (DUK_LINE_MACRO), \
	 duk_range_error_stash)
#define duk_reference_error  \
	(duk_api_global_filename = (const char *) (DUK_FILE_MACRO), \
	 duk_api_global_line = (duk_int_t) (DUK_LINE_MACRO), \
	 duk_reference_error_stash)
#define duk_syntax_error  \
	(duk_api_global_filename = (const char *) (DUK_FILE_MACRO), \
	 duk_api_global_line = (duk_int_t) (DUK_LINE_MACRO), \
	 duk_syntax_error_stash)
#define duk_type_error  \
	(duk_api_global_filename = (const char *) (DUK_FILE_MACRO), \
	 duk_api_global_line = (duk_int_t) (DUK_LINE_MACRO), \
	 duk_type_error_stash)
#define duk_uri_error  \
	(duk_api_global_filename = (const char *) (DUK_FILE_MACRO), \
	 duk_api_global_line = (duk_int_t) (DUK_LINE_MACRO), \
	 duk_uri_error_stash)
#endif  /* DUK_API_VARIADIC_MACROS */

DUK_API_NORETURN(DUK_EXTERNAL_DECL void duk_error_va_raw(duk_context *ctx, duk_errcode_t err_code, const char *filename, duk_int_t line, const char *fmt, va_list ap));
#define duk_error_va(ctx,err_code,fmt,ap)  \
	(duk_error_va_raw((ctx), (duk_errcode_t) (err_code), (const char *) (DUK_FILE_MACRO), (duk_int_t) (DUK_LINE_MACRO), (fmt), (ap)), (duk_ret_t) 0)
#define duk_generic_error_va(ctx,fmt,ap)  \
	(duk_error_va_raw((ctx), (duk_errcode_t) DUK_ERR_ERROR, (const char *) (DUK_FILE_MACRO), (duk_int_t) (DUK_LINE_MACRO), (fmt), (ap)), (duk_ret_t) 0)
#define duk_eval_error_va(ctx,fmt,ap)  \
	(duk_error_va_raw((ctx), (duk_errcode_t) DUK_ERR_EVAL_ERROR, (const char *) (DUK_FILE_MACRO), (duk_int_t) (DUK_LINE_MACRO), (fmt), (ap)), (duk_ret_t) 0)
#define duk_range_error_va(ctx,fmt,ap)  \
	(duk_error_va_raw((ctx), (duk_errcode_t) DUK_ERR_RANGE_ERROR, (const char *) (DUK_FILE_MACRO), (duk_int_t) (DUK_LINE_MACRO), (fmt), (ap)), (duk_ret_t) 0)
#define duk_reference_error_va(ctx,fmt,ap)  \
	(duk_error_va_raw((ctx), (duk_errcode_t) DUK_ERR_REFERENCE_ERROR, (const char *) (DUK_FILE_MACRO), (duk_int_t) (DUK_LINE_MACRO), (fmt), (ap)), (duk_ret_t) 0)
#define duk_syntax_error_va(ctx,fmt,ap)  \
	(duk_error_va_raw((ctx), (duk_errcode_t) DUK_ERR_SYNTAX_ERROR, (const char *) (DUK_FILE_MACRO), (duk_int_t) (DUK_LINE_MACRO), (fmt), (ap)), (duk_ret_t) 0)
#define duk_type_error_va(ctx,fmt,ap)  \
	(duk_error_va_raw((ctx), (duk_errcode_t) DUK_ERR_TYPE_ERROR, (const char *) (DUK_FILE_MACRO), (duk_int_t) (DUK_LINE_MACRO), (fmt), (ap)), (duk_ret_t) 0)
#define duk_uri_error_va(ctx,fmt,ap)  \
	(duk_error_va_raw((ctx), (duk_errcode_t) DUK_ERR_URI_ERROR, (const char *) (DUK_FILE_MACRO), (duk_int_t) (DUK_LINE_MACRO), (fmt), (ap)), (duk_ret_t) 0)

/*
 *  Other state related functions
 */

DUK_EXTERNAL_DECL duk_bool_t duk_is_strict_call(duk_context *ctx);
DUK_EXTERNAL_DECL duk_bool_t duk_is_constructor_call(duk_context *ctx);

/*
 *  Stack management
 */

DUK_EXTERNAL_DECL duk_idx_t duk_normalize_index(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_idx_t duk_require_normalize_index(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_bool_t duk_is_valid_index(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL void duk_require_valid_index(duk_context *ctx, duk_idx_t idx);

DUK_EXTERNAL_DECL duk_idx_t duk_get_top(duk_context *ctx);
DUK_EXTERNAL_DECL void duk_set_top(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_idx_t duk_get_top_index(duk_context *ctx);
DUK_EXTERNAL_DECL duk_idx_t duk_require_top_index(duk_context *ctx);

/* Although extra/top could be an unsigned type here, using a signed type
 * makes the API more robust to calling code calculation errors or corner
 * cases (where caller might occasionally come up with negative values).
 * Negative values are treated as zero, which is better than casting them
 * to a large unsigned number.  (This principle is used elsewhere in the
 * API too.)
 */
DUK_EXTERNAL_DECL duk_bool_t duk_check_stack(duk_context *ctx, duk_idx_t extra);
DUK_EXTERNAL_DECL void duk_require_stack(duk_context *ctx, duk_idx_t extra);
DUK_EXTERNAL_DECL duk_bool_t duk_check_stack_top(duk_context *ctx, duk_idx_t top);
DUK_EXTERNAL_DECL void duk_require_stack_top(duk_context *ctx, duk_idx_t top);

/*
 *  Stack manipulation (other than push/pop)
 */

DUK_EXTERNAL_DECL void duk_swap(duk_context *ctx, duk_idx_t idx1, duk_idx_t idx2);
DUK_EXTERNAL_DECL void duk_swap_top(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL void duk_dup(duk_context *ctx, duk_idx_t from_idx);
DUK_EXTERNAL_DECL void duk_dup_top(duk_context *ctx);
DUK_EXTERNAL_DECL void duk_insert(duk_context *ctx, duk_idx_t to_idx);
DUK_EXTERNAL_DECL void duk_replace(duk_context *ctx, duk_idx_t to_idx);
DUK_EXTERNAL_DECL void duk_copy(duk_context *ctx, duk_idx_t from_idx, duk_idx_t to_idx);
DUK_EXTERNAL_DECL void duk_remove(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL void duk_xcopymove_raw(duk_context *to_ctx, duk_context *from_ctx, duk_idx_t count, duk_bool_t is_copy);

#define duk_xmove_top(to_ctx,from_ctx,count) \
	duk_xcopymove_raw((to_ctx), (from_ctx), (count), 0 /*is_copy*/)
#define duk_xcopy_top(to_ctx,from_ctx,count) \
	duk_xcopymove_raw((to_ctx), (from_ctx), (count), 1 /*is_copy*/)

/*
 *  Push operations
 *
 *  Push functions return the absolute (relative to bottom of frame)
 *  position of the pushed value for convenience.
 *
 *  Note: duk_dup() is technically a push.
 */

DUK_EXTERNAL_DECL void duk_push_undefined(duk_context *ctx);
DUK_EXTERNAL_DECL void duk_push_null(duk_context *ctx);
DUK_EXTERNAL_DECL void duk_push_boolean(duk_context *ctx, duk_bool_t val);
DUK_EXTERNAL_DECL void duk_push_true(duk_context *ctx);
DUK_EXTERNAL_DECL void duk_push_false(duk_context *ctx);
DUK_EXTERNAL_DECL void duk_push_number(duk_context *ctx, duk_double_t val);
DUK_EXTERNAL_DECL void duk_push_nan(duk_context *ctx);
DUK_EXTERNAL_DECL void duk_push_int(duk_context *ctx, duk_int_t val);
DUK_EXTERNAL_DECL void duk_push_uint(duk_context *ctx, duk_uint_t val);
DUK_EXTERNAL_DECL const char *duk_push_string(duk_context *ctx, const char *str);
DUK_EXTERNAL_DECL const char *duk_push_lstring(duk_context *ctx, const char *str, duk_size_t len);
DUK_EXTERNAL_DECL void duk_push_pointer(duk_context *ctx, void *p);
DUK_EXTERNAL_DECL const char *duk_push_sprintf(duk_context *ctx, const char *fmt, ...);
DUK_EXTERNAL_DECL const char *duk_push_vsprintf(duk_context *ctx, const char *fmt, va_list ap);

DUK_EXTERNAL_DECL void duk_push_this(duk_context *ctx);
DUK_EXTERNAL_DECL void duk_push_current_function(duk_context *ctx);
DUK_EXTERNAL_DECL void duk_push_current_thread(duk_context *ctx);
DUK_EXTERNAL_DECL void duk_push_global_object(duk_context *ctx);
DUK_EXTERNAL_DECL void duk_push_heap_stash(duk_context *ctx);
DUK_EXTERNAL_DECL void duk_push_global_stash(duk_context *ctx);
DUK_EXTERNAL_DECL void duk_push_thread_stash(duk_context *ctx, duk_context *target_ctx);

DUK_EXTERNAL_DECL duk_idx_t duk_push_object(duk_context *ctx);
DUK_EXTERNAL_DECL duk_idx_t duk_push_bare_object(duk_context *ctx);
DUK_EXTERNAL_DECL duk_idx_t duk_push_array(duk_context *ctx);
DUK_EXTERNAL_DECL duk_idx_t duk_push_c_function(duk_context *ctx, duk_c_function func, duk_idx_t nargs);
DUK_EXTERNAL_DECL duk_idx_t duk_push_c_lightfunc(duk_context *ctx, duk_c_function func, duk_idx_t nargs, duk_idx_t length, duk_int_t magic);
DUK_EXTERNAL_DECL duk_idx_t duk_push_thread_raw(duk_context *ctx, duk_uint_t flags);

#define duk_push_thread(ctx) \
	duk_push_thread_raw((ctx), 0 /*flags*/)

#define duk_push_thread_new_globalenv(ctx) \
	duk_push_thread_raw((ctx), DUK_THREAD_NEW_GLOBAL_ENV /*flags*/)

DUK_EXTERNAL_DECL duk_idx_t duk_push_error_object_raw(duk_context *ctx, duk_errcode_t err_code, const char *filename, duk_int_t line, const char *fmt, ...);

#if defined(DUK_API_VARIADIC_MACROS)
#define duk_push_error_object(ctx,err_code,...)  \
	duk_push_error_object_raw((ctx), (err_code), (const char *) (DUK_FILE_MACRO), (duk_int_t) (DUK_LINE_MACRO), __VA_ARGS__)
#else
DUK_EXTERNAL_DECL duk_idx_t duk_push_error_object_stash(duk_context *ctx, duk_errcode_t err_code, const char *fmt, ...);
/* Note: parentheses are required so that the comma expression works in assignments. */
#define duk_push_error_object  \
	(duk_api_global_filename = (const char *) (DUK_FILE_MACRO), \
	 duk_api_global_line = (duk_int_t) (DUK_LINE_MACRO), \
	 duk_push_error_object_stash)  /* last value is func pointer, arguments follow in parens */
#endif

DUK_EXTERNAL_DECL duk_idx_t duk_push_error_object_va_raw(duk_context *ctx, duk_errcode_t err_code, const char *filename, duk_int_t line, const char *fmt, va_list ap);
#define duk_push_error_object_va(ctx,err_code,fmt,ap)  \
	duk_push_error_object_va_raw((ctx), (err_code), (const char *) (DUK_FILE_MACRO), (duk_int_t) (DUK_LINE_MACRO), (fmt), (ap))

#define DUK_BUF_FLAG_DYNAMIC   (1 << 0)    /* internal flag: dynamic buffer */
#define DUK_BUF_FLAG_EXTERNAL  (1 << 1)    /* internal flag: external buffer */
#define DUK_BUF_FLAG_NOZERO    (1 << 2)    /* internal flag: don't zero allocated buffer */

DUK_EXTERNAL_DECL void *duk_push_buffer_raw(duk_context *ctx, duk_size_t size, duk_small_uint_t flags);

#define duk_push_buffer(ctx,size,dynamic) \
	duk_push_buffer_raw((ctx), (size), (dynamic) ? DUK_BUF_FLAG_DYNAMIC : 0)
#define duk_push_fixed_buffer(ctx,size) \
	duk_push_buffer_raw((ctx), (size), 0 /*flags*/)
#define duk_push_dynamic_buffer(ctx,size) \
	duk_push_buffer_raw((ctx), (size), DUK_BUF_FLAG_DYNAMIC /*flags*/)
#define duk_push_external_buffer(ctx) \
	((void) duk_push_buffer_raw((ctx), 0, DUK_BUF_FLAG_DYNAMIC | DUK_BUF_FLAG_EXTERNAL))

#define DUK_BUFOBJ_CREATE_ARRBUF       (1 << 4)  /* internal flag: create backing ArrayBuffer; keep in one byte */
#define DUK_BUFOBJ_ARRAYBUFFER         0
#define DUK_BUFOBJ_NODEJS_BUFFER       (1 | DUK_BUFOBJ_CREATE_ARRBUF)
#define DUK_BUFOBJ_DATAVIEW            (2 | DUK_BUFOBJ_CREATE_ARRBUF)
#define DUK_BUFOBJ_INT8ARRAY           (3 | DUK_BUFOBJ_CREATE_ARRBUF)
#define DUK_BUFOBJ_UINT8ARRAY          (4 | DUK_BUFOBJ_CREATE_ARRBUF)
#define DUK_BUFOBJ_UINT8CLAMPEDARRAY   (5 | DUK_BUFOBJ_CREATE_ARRBUF)
#define DUK_BUFOBJ_INT16ARRAY          (6 | DUK_BUFOBJ_CREATE_ARRBUF)
#define DUK_BUFOBJ_UINT16ARRAY         (7 | DUK_BUFOBJ_CREATE_ARRBUF)
#define DUK_BUFOBJ_INT32ARRAY          (8 | DUK_BUFOBJ_CREATE_ARRBUF)
#define DUK_BUFOBJ_UINT32ARRAY         (9 | DUK_BUFOBJ_CREATE_ARRBUF)
#define DUK_BUFOBJ_FLOAT32ARRAY        (10 | DUK_BUFOBJ_CREATE_ARRBUF)
#define DUK_BUFOBJ_FLOAT64ARRAY        (11 | DUK_BUFOBJ_CREATE_ARRBUF)

DUK_EXTERNAL_DECL void duk_push_buffer_object(duk_context *ctx, duk_idx_t idx_buffer, duk_size_t byte_offset, duk_size_t byte_length, duk_uint_t flags);

DUK_EXTERNAL_DECL duk_idx_t duk_push_heapptr(duk_context *ctx, void *ptr);

/*
 *  Pop operations
 */

DUK_EXTERNAL_DECL void duk_pop(duk_context *ctx);
DUK_EXTERNAL_DECL void duk_pop_n(duk_context *ctx, duk_idx_t count);
DUK_EXTERNAL_DECL void duk_pop_2(duk_context *ctx);
DUK_EXTERNAL_DECL void duk_pop_3(duk_context *ctx);

/*
 *  Type checks
 *
 *  duk_is_none(), which would indicate whether index it outside of stack,
 *  is not needed; duk_is_valid_index() gives the same information.
 */

DUK_EXTERNAL_DECL duk_int_t duk_get_type(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_bool_t duk_check_type(duk_context *ctx, duk_idx_t idx, duk_int_t type);
DUK_EXTERNAL_DECL duk_uint_t duk_get_type_mask(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_bool_t duk_check_type_mask(duk_context *ctx, duk_idx_t idx, duk_uint_t mask);

DUK_EXTERNAL_DECL duk_bool_t duk_is_undefined(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_bool_t duk_is_null(duk_context *ctx, duk_idx_t idx);
#define duk_is_null_or_undefined(ctx, idx) \
	((duk_get_type_mask((ctx), (idx)) & (DUK_TYPE_MASK_NULL | DUK_TYPE_MASK_UNDEFINED)) ? 1 : 0)

DUK_EXTERNAL_DECL duk_bool_t duk_is_boolean(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_bool_t duk_is_number(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_bool_t duk_is_nan(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_bool_t duk_is_string(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_bool_t duk_is_object(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_bool_t duk_is_buffer(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_bool_t duk_is_buffer_data(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_bool_t duk_is_pointer(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_bool_t duk_is_lightfunc(duk_context *ctx, duk_idx_t idx);

DUK_EXTERNAL_DECL duk_bool_t duk_is_symbol(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_bool_t duk_is_array(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_bool_t duk_is_function(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_bool_t duk_is_c_function(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_bool_t duk_is_ecmascript_function(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_bool_t duk_is_bound_function(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_bool_t duk_is_thread(duk_context *ctx, duk_idx_t idx);

#define duk_is_callable(ctx,idx) \
	duk_is_function((ctx), (idx))
DUK_EXTERNAL_DECL duk_bool_t duk_is_dynamic_buffer(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_bool_t duk_is_fixed_buffer(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_bool_t duk_is_external_buffer(duk_context *ctx, duk_idx_t idx);

/* Buffers and lightfuncs are not considered primitive because they mimic
 * objects and e.g. duk_to_primitive() will coerce them instead of returning
 * them as is.  Symbols are represented as strings internally.
 */
#define duk_is_primitive(ctx,idx) \
	duk_check_type_mask((ctx), (idx), DUK_TYPE_MASK_UNDEFINED | \
	                                  DUK_TYPE_MASK_NULL | \
	                                  DUK_TYPE_MASK_BOOLEAN | \
	                                  DUK_TYPE_MASK_NUMBER | \
	                                  DUK_TYPE_MASK_STRING | \
	                                  DUK_TYPE_MASK_POINTER)

/* Symbols are object coercible, covered by DUK_TYPE_MASK_STRING. */
#define duk_is_object_coercible(ctx,idx) \
	duk_check_type_mask((ctx), (idx), DUK_TYPE_MASK_BOOLEAN | \
	                                  DUK_TYPE_MASK_NUMBER | \
	                                  DUK_TYPE_MASK_STRING | \
	                                  DUK_TYPE_MASK_OBJECT | \
	                                  DUK_TYPE_MASK_BUFFER | \
	                                  DUK_TYPE_MASK_POINTER | \
	                                  DUK_TYPE_MASK_LIGHTFUNC)

DUK_EXTERNAL_DECL duk_errcode_t duk_get_error_code(duk_context *ctx, duk_idx_t idx);
#define duk_is_error(ctx,idx) \
	(duk_get_error_code((ctx), (idx)) != 0)
#define duk_is_eval_error(ctx,idx) \
	(duk_get_error_code((ctx), (idx)) == DUK_ERR_EVAL_ERROR)
#define duk_is_range_error(ctx,idx) \
	(duk_get_error_code((ctx), (idx)) == DUK_ERR_RANGE_ERROR)
#define duk_is_reference_error(ctx,idx) \
	(duk_get_error_code((ctx), (idx)) == DUK_ERR_REFERENCE_ERROR)
#define duk_is_syntax_error(ctx,idx) \
	(duk_get_error_code((ctx), (idx)) == DUK_ERR_SYNTAX_ERROR)
#define duk_is_type_error(ctx,idx) \
	(duk_get_error_code((ctx), (idx)) == DUK_ERR_TYPE_ERROR)
#define duk_is_uri_error(ctx,idx) \
	(duk_get_error_code((ctx), (idx)) == DUK_ERR_URI_ERROR)

/*
 *  Get operations: no coercion, returns default value for invalid
 *  indices and invalid value types.
 *
 *  duk_get_undefined() and duk_get_null() would be pointless and
 *  are not included.
 */

DUK_EXTERNAL_DECL duk_bool_t duk_get_boolean(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_double_t duk_get_number(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_int_t duk_get_int(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_uint_t duk_get_uint(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL const char *duk_get_string(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL const char *duk_get_lstring(duk_context *ctx, duk_idx_t idx, duk_size_t *out_len);
DUK_EXTERNAL_DECL void *duk_get_buffer(duk_context *ctx, duk_idx_t idx, duk_size_t *out_size);
DUK_EXTERNAL_DECL void *duk_get_buffer_data(duk_context *ctx, duk_idx_t idx, duk_size_t *out_size);
DUK_EXTERNAL_DECL void *duk_get_pointer(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_c_function duk_get_c_function(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_context *duk_get_context(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL void *duk_get_heapptr(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_size_t duk_get_length(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL void duk_set_length(duk_context *ctx, duk_idx_t idx, duk_size_t len);

/*
 *  Require operations: no coercion, throw error if index or type
 *  is incorrect.  No defaulting.
 */

#define duk_require_type_mask(ctx,idx,mask) \
	((void) duk_check_type_mask((ctx), (idx), (mask) | DUK_TYPE_MASK_THROW))

DUK_EXTERNAL_DECL void duk_require_undefined(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL void duk_require_null(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_bool_t duk_require_boolean(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_double_t duk_require_number(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_int_t duk_require_int(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_uint_t duk_require_uint(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL const char *duk_require_string(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL const char *duk_require_lstring(duk_context *ctx, duk_idx_t idx, duk_size_t *out_len);
DUK_EXTERNAL_DECL void *duk_require_buffer(duk_context *ctx, duk_idx_t idx, duk_size_t *out_size);
DUK_EXTERNAL_DECL void *duk_require_buffer_data(duk_context *ctx, duk_idx_t idx, duk_size_t *out_size);
DUK_EXTERNAL_DECL void *duk_require_pointer(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_c_function duk_require_c_function(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_context *duk_require_context(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL void duk_require_function(duk_context *ctx, duk_idx_t idx);
#define duk_require_callable(ctx,idx) \
	duk_require_function((ctx), (idx))
DUK_EXTERNAL_DECL void *duk_require_heapptr(duk_context *ctx, duk_idx_t idx);

/* Symbols are object coercible and covered by DUK_TYPE_MASK_STRING. */
#define duk_require_object_coercible(ctx,idx) \
	((void) duk_check_type_mask((ctx), (idx), DUK_TYPE_MASK_BOOLEAN | \
	                                            DUK_TYPE_MASK_NUMBER | \
	                                            DUK_TYPE_MASK_STRING | \
	                                            DUK_TYPE_MASK_OBJECT | \
	                                            DUK_TYPE_MASK_BUFFER | \
	                                            DUK_TYPE_MASK_POINTER | \
	                                            DUK_TYPE_MASK_LIGHTFUNC | \
	                                            DUK_TYPE_MASK_THROW))

/*
 *  Coercion operations: in-place coercion, return coerced value where
 *  applicable.  If index is invalid, throw error.  Some coercions may
 *  throw an expected error (e.g. from a toString() or valueOf() call)
 *  or an internal error (e.g. from out of memory).
 */

DUK_EXTERNAL_DECL void duk_to_undefined(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL void duk_to_null(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_bool_t duk_to_boolean(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_double_t duk_to_number(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_int_t duk_to_int(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_uint_t duk_to_uint(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_int32_t duk_to_int32(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_uint32_t duk_to_uint32(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_uint16_t duk_to_uint16(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL const char *duk_to_string(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL const char *duk_to_lstring(duk_context *ctx, duk_idx_t idx, duk_size_t *out_len);
DUK_EXTERNAL_DECL void *duk_to_buffer_raw(duk_context *ctx, duk_idx_t idx, duk_size_t *out_size, duk_uint_t flags);
DUK_EXTERNAL_DECL void *duk_to_pointer(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL void duk_to_object(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL void duk_to_primitive(duk_context *ctx, duk_idx_t idx, duk_int_t hint);

#define DUK_BUF_MODE_FIXED      0   /* internal: request fixed buffer result */
#define DUK_BUF_MODE_DYNAMIC    1   /* internal: request dynamic buffer result */
#define DUK_BUF_MODE_DONTCARE   2   /* internal: don't care about fixed/dynamic nature */

#define duk_to_buffer(ctx,idx,out_size) \
	duk_to_buffer_raw((ctx), (idx), (out_size), DUK_BUF_MODE_DONTCARE)
#define duk_to_fixed_buffer(ctx,idx,out_size) \
	duk_to_buffer_raw((ctx), (idx), (out_size), DUK_BUF_MODE_FIXED)
#define duk_to_dynamic_buffer(ctx,idx,out_size) \
	duk_to_buffer_raw((ctx), (idx), (out_size), DUK_BUF_MODE_DYNAMIC)

/* safe variants of a few coercion operations */
DUK_EXTERNAL_DECL const char *duk_safe_to_lstring(duk_context *ctx, duk_idx_t idx, duk_size_t *out_len);
#define duk_safe_to_string(ctx,idx) \
	duk_safe_to_lstring((ctx), (idx), NULL)

/*
 *  Misc conversion
 */

DUK_EXTERNAL_DECL const char *duk_base64_encode(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL void duk_base64_decode(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL const char *duk_hex_encode(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL void duk_hex_decode(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL const char *duk_json_encode(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL void duk_json_decode(duk_context *ctx, duk_idx_t idx);

DUK_EXTERNAL_DECL const char *duk_buffer_to_string(duk_context *ctx, duk_idx_t idx);

/*
 *  Buffer
 */

DUK_EXTERNAL_DECL void *duk_resize_buffer(duk_context *ctx, duk_idx_t idx, duk_size_t new_size);
DUK_EXTERNAL_DECL void *duk_steal_buffer(duk_context *ctx, duk_idx_t idx, duk_size_t *out_size);
DUK_EXTERNAL_DECL void duk_config_buffer(duk_context *ctx, duk_idx_t idx, void *ptr, duk_size_t len);

/*
 *  Property access
 *
 *  The basic function assumes key is on stack.  The _string variant takes
 *  a C string as a property name, while the _index variant takes an array
 *  index as a property name (e.g. 123 is equivalent to the key "123").
 */

DUK_EXTERNAL_DECL duk_bool_t duk_get_prop(duk_context *ctx, duk_idx_t obj_idx);
DUK_EXTERNAL_DECL duk_bool_t duk_get_prop_string(duk_context *ctx, duk_idx_t obj_idx, const char *key);
DUK_EXTERNAL_DECL duk_bool_t duk_get_prop_lstring(duk_context *ctx, duk_idx_t obj_idx, const char *key, duk_size_t key_len);
DUK_EXTERNAL_DECL duk_bool_t duk_get_prop_index(duk_context *ctx, duk_idx_t obj_idx, duk_uarridx_t arr_idx);
DUK_EXTERNAL_DECL duk_bool_t duk_put_prop(duk_context *ctx, duk_idx_t obj_idx);
DUK_EXTERNAL_DECL duk_bool_t duk_put_prop_string(duk_context *ctx, duk_idx_t obj_idx, const char *key);
DUK_EXTERNAL_DECL duk_bool_t duk_put_prop_lstring(duk_context *ctx, duk_idx_t obj_idx, const char *key, duk_size_t key_len);
DUK_EXTERNAL_DECL duk_bool_t duk_put_prop_index(duk_context *ctx, duk_idx_t obj_idx, duk_uarridx_t arr_idx);
DUK_EXTERNAL_DECL duk_bool_t duk_del_prop(duk_context *ctx, duk_idx_t obj_idx);
DUK_EXTERNAL_DECL duk_bool_t duk_del_prop_string(duk_context *ctx, duk_idx_t obj_idx, const char *key);
DUK_EXTERNAL_DECL duk_bool_t duk_del_prop_lstring(duk_context *ctx, duk_idx_t obj_idx, const char *key, duk_size_t key_len);
DUK_EXTERNAL_DECL duk_bool_t duk_del_prop_index(duk_context *ctx, duk_idx_t obj_idx, duk_uarridx_t arr_idx);
DUK_EXTERNAL_DECL duk_bool_t duk_has_prop(duk_context *ctx, duk_idx_t obj_idx);
DUK_EXTERNAL_DECL duk_bool_t duk_has_prop_string(duk_context *ctx, duk_idx_t obj_idx, const char *key);
DUK_EXTERNAL_DECL duk_bool_t duk_has_prop_lstring(duk_context *ctx, duk_idx_t obj_idx, const char *key, duk_size_t key_len);
DUK_EXTERNAL_DECL duk_bool_t duk_has_prop_index(duk_context *ctx, duk_idx_t obj_idx, duk_uarridx_t arr_idx);

DUK_EXTERNAL_DECL void duk_get_prop_desc(duk_context *ctx, duk_idx_t obj_idx, duk_uint_t flags);
DUK_EXTERNAL_DECL void duk_def_prop(duk_context *ctx, duk_idx_t obj_idx, duk_uint_t flags);

DUK_EXTERNAL_DECL duk_bool_t duk_get_global_string(duk_context *ctx, const char *key);
DUK_EXTERNAL_DECL duk_bool_t duk_get_global_lstring(duk_context *ctx, const char *key, duk_size_t key_len);
DUK_EXTERNAL_DECL duk_bool_t duk_put_global_string(duk_context *ctx, const char *key);
DUK_EXTERNAL_DECL duk_bool_t duk_put_global_lstring(duk_context *ctx, const char *key, duk_size_t key_len);

/*
 *  Inspection
 */

DUK_EXTERNAL_DECL void duk_inspect_value(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL void duk_inspect_callstack_entry(duk_context *ctx, duk_int_t level);

/*
 *  Object prototype
 */

DUK_EXTERNAL_DECL void duk_get_prototype(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL void duk_set_prototype(duk_context *ctx, duk_idx_t idx);

/*
 *  Object finalizer
 */

DUK_EXTERNAL_DECL void duk_get_finalizer(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL void duk_set_finalizer(duk_context *ctx, duk_idx_t idx);

/*
 *  Global object
 */

DUK_EXTERNAL_DECL void duk_set_global_object(duk_context *ctx);

/*
 *  Duktape/C function magic value
 */

DUK_EXTERNAL_DECL duk_int_t duk_get_magic(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL void duk_set_magic(duk_context *ctx, duk_idx_t idx, duk_int_t magic);
DUK_EXTERNAL_DECL duk_int_t duk_get_current_magic(duk_context *ctx);

/*
 *  Module helpers: put multiple function or constant properties
 */

DUK_EXTERNAL_DECL void duk_put_function_list(duk_context *ctx, duk_idx_t obj_idx, const duk_function_list_entry *funcs);
DUK_EXTERNAL_DECL void duk_put_number_list(duk_context *ctx, duk_idx_t obj_idx, const duk_number_list_entry *numbers);

/*
 *  Object operations
 */

DUK_EXTERNAL_DECL void duk_compact(duk_context *ctx, duk_idx_t obj_idx);
DUK_EXTERNAL_DECL void duk_enum(duk_context *ctx, duk_idx_t obj_idx, duk_uint_t enum_flags);
DUK_EXTERNAL_DECL duk_bool_t duk_next(duk_context *ctx, duk_idx_t enum_idx, duk_bool_t get_value);

/*
 *  String manipulation
 */

DUK_EXTERNAL_DECL void duk_concat(duk_context *ctx, duk_idx_t count);
DUK_EXTERNAL_DECL void duk_join(duk_context *ctx, duk_idx_t count);
DUK_EXTERNAL_DECL void duk_decode_string(duk_context *ctx, duk_idx_t idx, duk_decode_char_function callback, void *udata);
DUK_EXTERNAL_DECL void duk_map_string(duk_context *ctx, duk_idx_t idx, duk_map_char_function callback, void *udata);
DUK_EXTERNAL_DECL void duk_substring(duk_context *ctx, duk_idx_t idx, duk_size_t start_char_offset, duk_size_t end_char_offset);
DUK_EXTERNAL_DECL void duk_trim(duk_context *ctx, duk_idx_t idx);
DUK_EXTERNAL_DECL duk_codepoint_t duk_char_code_at(duk_context *ctx, duk_idx_t idx, duk_size_t char_offset);

/*
 *  Ecmascript operators
 */

DUK_EXTERNAL_DECL duk_bool_t duk_equals(duk_context *ctx, duk_idx_t idx1, duk_idx_t idx2);
DUK_EXTERNAL_DECL duk_bool_t duk_strict_equals(duk_context *ctx, duk_idx_t idx1, duk_idx_t idx2);
DUK_EXTERNAL_DECL duk_bool_t duk_samevalue(duk_context *ctx, duk_idx_t idx1, duk_idx_t idx2);
DUK_EXTERNAL_DECL duk_bool_t duk_instanceof(duk_context *ctx, duk_idx_t idx1, duk_idx_t idx2);

/*
 *  Function (method) calls
 */

DUK_EXTERNAL_DECL void duk_call(duk_context *ctx, duk_idx_t nargs);
DUK_EXTERNAL_DECL void duk_call_method(duk_context *ctx, duk_idx_t nargs);
DUK_EXTERNAL_DECL void duk_call_prop(duk_context *ctx, duk_idx_t obj_idx, duk_idx_t nargs);
DUK_EXTERNAL_DECL duk_int_t duk_pcall(duk_context *ctx, duk_idx_t nargs);
DUK_EXTERNAL_DECL duk_int_t duk_pcall_method(duk_context *ctx, duk_idx_t nargs);
DUK_EXTERNAL_DECL duk_int_t duk_pcall_prop(duk_context *ctx, duk_idx_t obj_idx, duk_idx_t nargs);
DUK_EXTERNAL_DECL void duk_new(duk_context *ctx, duk_idx_t nargs);
DUK_EXTERNAL_DECL duk_int_t duk_pnew(duk_context *ctx, duk_idx_t nargs);
DUK_EXTERNAL_DECL duk_int_t duk_safe_call(duk_context *ctx, duk_safe_call_function func, void *udata, duk_idx_t nargs, duk_idx_t nrets);

/*
 *  Thread management
 */

/* There are currently no native functions to yield/resume, due to the internal
 * limitations on coroutine handling.  These will be added later.
 */

/*
 *  Compilation and evaluation
 */

DUK_EXTERNAL_DECL duk_int_t duk_eval_raw(duk_context *ctx, const char *src_buffer, duk_size_t src_length, duk_uint_t flags);
DUK_EXTERNAL_DECL duk_int_t duk_compile_raw(duk_context *ctx, const char *src_buffer, duk_size_t src_length, duk_uint_t flags);

/* plain */
#define duk_eval(ctx)  \
	((void) duk_eval_raw((ctx), NULL, 0, 1 /*args*/ | DUK_COMPILE_EVAL | DUK_COMPILE_NOFILENAME))

#define duk_eval_noresult(ctx)  \
	((void) duk_eval_raw((ctx), NULL, 0, 1 /*args*/ | DUK_COMPILE_EVAL | DUK_COMPILE_NORESULT | DUK_COMPILE_NOFILENAME))

#define duk_peval(ctx)  \
	(duk_eval_raw((ctx), NULL, 0, 1 /*args*/ | DUK_COMPILE_EVAL | DUK_COMPILE_SAFE | DUK_COMPILE_NOFILENAME))

#define duk_peval_noresult(ctx)  \
	(duk_eval_raw((ctx), NULL, 0, 1 /*args*/ | DUK_COMPILE_EVAL | DUK_COMPILE_SAFE | DUK_COMPILE_NORESULT | DUK_COMPILE_NOFILENAME))

#define duk_compile(ctx,flags)  \
	((void) duk_compile_raw((ctx), NULL, 0, 2 /*args*/ | (flags)))

#define duk_pcompile(ctx,flags)  \
	(duk_compile_raw((ctx), NULL, 0, 2 /*args*/ | (flags) | DUK_COMPILE_SAFE))

/* string */
#define duk_eval_string(ctx,src)  \
	((void) duk_eval_raw((ctx), (src), 0, 0 /*args*/ | DUK_COMPILE_EVAL | DUK_COMPILE_NOSOURCE | DUK_COMPILE_STRLEN | DUK_COMPILE_NOFILENAME))

#define duk_eval_string_noresult(ctx,src)  \
	((void) duk_eval_raw((ctx), (src), 0, 0 /*args*/ | DUK_COMPILE_EVAL | DUK_COMPILE_NOSOURCE | DUK_COMPILE_STRLEN | DUK_COMPILE_NORESULT | DUK_COMPILE_NOFILENAME))

#define duk_peval_string(ctx,src)  \
	(duk_eval_raw((ctx), (src), 0, 0 /*args*/ | DUK_COMPILE_EVAL | DUK_COMPILE_SAFE | DUK_COMPILE_NOSOURCE | DUK_COMPILE_STRLEN | DUK_COMPILE_NOFILENAME))

#define duk_peval_string_noresult(ctx,src)  \
	(duk_eval_raw((ctx), (src), 0, 0 /*args*/ | DUK_COMPILE_EVAL | DUK_COMPILE_SAFE | DUK_COMPILE_NOSOURCE | DUK_COMPILE_STRLEN | DUK_COMPILE_NORESULT | DUK_COMPILE_NOFILENAME))

#define duk_compile_string(ctx,flags,src)  \
	((void) duk_compile_raw((ctx), (src), 0, 0 /*args*/ | (flags) | DUK_COMPILE_NOSOURCE | DUK_COMPILE_STRLEN | DUK_COMPILE_NOFILENAME))

#define duk_compile_string_filename(ctx,flags,src)  \
	((void) duk_compile_raw((ctx), (src), 0, 1 /*args*/ | (flags) | DUK_COMPILE_NOSOURCE | DUK_COMPILE_STRLEN))

#define duk_pcompile_string(ctx,flags,src)  \
	(duk_compile_raw((ctx), (src), 0, 0 /*args*/ | (flags) | DUK_COMPILE_SAFE | DUK_COMPILE_NOSOURCE | DUK_COMPILE_STRLEN | DUK_COMPILE_NOFILENAME))

#define duk_pcompile_string_filename(ctx,flags,src)  \
	(duk_compile_raw((ctx), (src), 0, 1 /*args*/ | (flags) | DUK_COMPILE_SAFE | DUK_COMPILE_NOSOURCE | DUK_COMPILE_STRLEN))

/* lstring */
#define duk_eval_lstring(ctx,buf,len)  \
	((void) duk_eval_raw((ctx), buf, len, 0 /*args*/ | DUK_COMPILE_EVAL | DUK_COMPILE_NOSOURCE | DUK_COMPILE_NOFILENAME))

#define duk_eval_lstring_noresult(ctx,buf,len)  \
	((void) duk_eval_raw((ctx), buf, len, 0 /*args*/ | DUK_COMPILE_EVAL | DUK_COMPILE_NOSOURCE | DUK_COMPILE_NORESULT | DUK_COMPILE_NOFILENAME))

#define duk_peval_lstring(ctx,buf,len)  \
	(duk_eval_raw((ctx), buf, len, 0 /*args*/ | DUK_COMPILE_EVAL | DUK_COMPILE_NOSOURCE | DUK_COMPILE_SAFE | DUK_COMPILE_NOFILENAME))

#define duk_peval_lstring_noresult(ctx,buf,len)  \
	(duk_eval_raw((ctx), buf, len, 0 /*args*/ | DUK_COMPILE_EVAL | DUK_COMPILE_SAFE | DUK_COMPILE_NOSOURCE | DUK_COMPILE_NORESULT | DUK_COMPILE_NOFILENAME))

#define duk_compile_lstring(ctx,flags,buf,len)  \
	((void) duk_compile_raw((ctx), buf, len, 0 /*args*/ | (flags) | DUK_COMPILE_NOSOURCE | DUK_COMPILE_NOFILENAME))

#define duk_compile_lstring_filename(ctx,flags,buf,len)  \
	((void) duk_compile_raw((ctx), buf, len, 1 /*args*/ | (flags) | DUK_COMPILE_NOSOURCE))

#define duk_pcompile_lstring(ctx,flags,buf,len)  \
	(duk_compile_raw((ctx), buf, len, 0 /*args*/ | (flags) | DUK_COMPILE_SAFE | DUK_COMPILE_NOSOURCE | DUK_COMPILE_NOFILENAME))

#define duk_pcompile_lstring_filename(ctx,flags,buf,len)  \
	(duk_compile_raw((ctx), buf, len, 1 /*args*/ | (flags) | DUK_COMPILE_SAFE | DUK_COMPILE_NOSOURCE))

/*
 *  Bytecode load/dump
 */

DUK_EXTERNAL_DECL void duk_dump_function(duk_context *ctx);
DUK_EXTERNAL_DECL void duk_load_function(duk_context *ctx);

/*
 *  Debugging
 */

DUK_EXTERNAL_DECL void duk_push_context_dump(duk_context *ctx);

/*
 *  Debugger (debug protocol)
 */

DUK_EXTERNAL_DECL void duk_debugger_attach(duk_context *ctx,
                                           duk_debug_read_function read_cb,
                                           duk_debug_write_function write_cb,
                                           duk_debug_peek_function peek_cb,
                                           duk_debug_read_flush_function read_flush_cb,
                                           duk_debug_write_flush_function write_flush_cb,
                                           duk_debug_request_function request_cb,
                                           duk_debug_detached_function detached_cb,
                                           void *udata);
DUK_EXTERNAL_DECL void duk_debugger_detach(duk_context *ctx);
DUK_EXTERNAL_DECL void duk_debugger_cooperate(duk_context *ctx);
DUK_EXTERNAL_DECL duk_bool_t duk_debugger_notify(duk_context *ctx, duk_idx_t nvalues);
DUK_EXTERNAL_DECL void duk_debugger_pause(duk_context *ctx);

/*
 *  Time handling
 */

DUK_EXTERNAL_DECL duk_double_t duk_get_now(duk_context *ctx);
DUK_EXTERNAL_DECL void duk_time_to_components(duk_context *ctx, duk_double_t timeval, duk_time_components *comp);
DUK_EXTERNAL_DECL duk_double_t duk_components_to_time(duk_context *ctx, duk_time_components *comp);

/*
 *  Date provider related constants
 *
 *  NOTE: These are "semi public" - you should only use these if you write
 *  your own platform specific Date provider, see doc/datetime.rst.
 */

/* Millisecond count constants. */
#define DUK_DATE_MSEC_SECOND          1000L
#define DUK_DATE_MSEC_MINUTE          (60L * 1000L)
#define DUK_DATE_MSEC_HOUR            (60L * 60L * 1000L)
#define DUK_DATE_MSEC_DAY             (24L * 60L * 60L * 1000L)

/* Ecmascript date range is 100 million days from Epoch:
 * > 100e6 * 24 * 60 * 60 * 1000  // 100M days in millisecs
 * 8640000000000000
 * (= 8.64e15)
 */
#define DUK_DATE_MSEC_100M_DAYS         (8.64e15)
#define DUK_DATE_MSEC_100M_DAYS_LEEWAY  (8.64e15 + 24 * 3600e3)

/* Ecmascript year range:
 * > new Date(100e6 * 24 * 3600e3).toISOString()
 * '+275760-09-13T00:00:00.000Z'
 * > new Date(-100e6 * 24 * 3600e3).toISOString()
 * '-271821-04-20T00:00:00.000Z'
 */
#define DUK_DATE_MIN_ECMA_YEAR     (-271821L)
#define DUK_DATE_MAX_ECMA_YEAR     275760L

/* Part indices for internal breakdowns.  Part order from DUK_DATE_IDX_YEAR
 * to DUK_DATE_IDX_MILLISECOND matches argument ordering of Ecmascript API
 * calls (like Date constructor call).  Some functions in duk_bi_date.c
 * depend on the specific ordering, so change with care.  16 bits are not
 * enough for all parts (year, specifically).
 *
 * Must be in-sync with genbuiltins.py.
 */
#define DUK_DATE_IDX_YEAR           0  /* year */
#define DUK_DATE_IDX_MONTH          1  /* month: 0 to 11 */
#define DUK_DATE_IDX_DAY            2  /* day within month: 0 to 30 */
#define DUK_DATE_IDX_HOUR           3
#define DUK_DATE_IDX_MINUTE         4
#define DUK_DATE_IDX_SECOND         5
#define DUK_DATE_IDX_MILLISECOND    6
#define DUK_DATE_IDX_WEEKDAY        7  /* weekday: 0 to 6, 0=sunday, 1=monday, etc */
#define DUK_DATE_IDX_NUM_PARTS      8

/* Internal API call flags, used for various functions in duk_bi_date.c.
 * Certain flags are used by only certain functions, but since the flags
 * don't overlap, a single flags value can be passed around to multiple
 * functions.
 *
 * The unused top bits of the flags field are also used to pass values
 * to helpers (duk__get_part_helper() and duk__set_part_helper()).
 *
 * Must be in-sync with genbuiltins.py.
 */

/* NOTE: when writing a Date provider you only need a few specific
 * flags from here, the rest are internal.  Avoid using anything you
 * don't need.
 */

#define DUK_DATE_FLAG_NAN_TO_ZERO          (1 << 0)  /* timeval breakdown: internal time value NaN -> zero */
#define DUK_DATE_FLAG_NAN_TO_RANGE_ERROR   (1 << 1)  /* timeval breakdown: internal time value NaN -> RangeError (toISOString) */
#define DUK_DATE_FLAG_ONEBASED             (1 << 2)  /* timeval breakdown: convert month and day-of-month parts to one-based (default is zero-based) */
#define DUK_DATE_FLAG_EQUIVYEAR            (1 << 3)  /* timeval breakdown: replace year with equivalent year in the [1971,2037] range for DST calculations */
#define DUK_DATE_FLAG_LOCALTIME            (1 << 4)  /* convert time value to local time */
#define DUK_DATE_FLAG_SUB1900              (1 << 5)  /* getter: subtract 1900 from year when getting year part */
#define DUK_DATE_FLAG_TOSTRING_DATE        (1 << 6)  /* include date part in string conversion result */
#define DUK_DATE_FLAG_TOSTRING_TIME        (1 << 7)  /* include time part in string conversion result */
#define DUK_DATE_FLAG_TOSTRING_LOCALE      (1 << 8)  /* use locale specific formatting if available */
#define DUK_DATE_FLAG_TIMESETTER           (1 << 9)  /* setter: call is a time setter (affects hour, min, sec, ms); otherwise date setter (affects year, month, day-in-month) */
#define DUK_DATE_FLAG_YEAR_FIXUP           (1 << 10) /* setter: perform 2-digit year fixup (00...99 -> 1900...1999) */
#define DUK_DATE_FLAG_SEP_T                (1 << 11) /* string conversion: use 'T' instead of ' ' as a separator */
#define DUK_DATE_FLAG_VALUE_SHIFT          12        /* additional values begin at bit 12 */

/*
 *  ROM pointer compression
 */

/* Support array for ROM pointer compression.  Only declared when ROM
 * pointer compression is active.
 */
#if defined(DUK_USE_ROM_OBJECTS) && defined(DUK_USE_HEAPPTR16)
DUK_EXTERNAL_DECL const void * const duk_rom_compressed_pointers[];
#endif

/*
 *  C++ name mangling
 */

#if defined(__cplusplus)
/* end 'extern "C"' wrapper */
}
#endif

#endif  /* DUK_API_PUBLIC_H_INCLUDED */

/*
 *  END PUBLIC API
 */

/*
 *  Union to access IEEE double memory representation, indexes for double
 *  memory representation, and some macros for double manipulation.
 *
 *  Also used by packed duk_tval.  Use a union for bit manipulation to
 *  minimize aliasing issues in practice.  The C99 standard does not
 *  guarantee that this should work, but it's a very widely supported
 *  practice for low level manipulation.
 *
 *  IEEE double format summary:
 *
 *    seeeeeee eeeeffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff
 *       A        B        C        D        E        F        G        H
 *
 *    s       sign bit
 *    eee...  exponent field
 *    fff...  fraction
 *
 *  See http://en.wikipedia.org/wiki/Double_precision_floating-point_format.
 *
 *  NaNs are represented as exponent 0x7ff and mantissa != 0.  The NaN is a
 *  signaling NaN when the highest bit of the mantissa is zero, and a quiet
 *  NaN when the highest bit is set.
 *
 *  At least three memory layouts are relevant here:
 *
 *    A B C D E F G H    Big endian (e.g. 68k)           DUK_USE_DOUBLE_BE
 *    H G F E D C B A    Little endian (e.g. x86)        DUK_USE_DOUBLE_LE
 *    D C B A H G F E    Mixed/cross endian (e.g. ARM)   DUK_USE_DOUBLE_ME
 *
 *  ARM is a special case: ARM double values are in mixed/cross endian
 *  format while ARM duk_uint64_t values are in standard little endian
 *  format (H G F E D C B A).  When a double is read as a duk_uint64_t
 *  from memory, the register will contain the (logical) value
 *  E F G H A B C D.  This requires some special handling below.
 *
 *  Indexes of various types (8-bit, 16-bit, 32-bit) in memory relative to
 *  the logical (big endian) order:
 *
 *  byte order      duk_uint8_t    duk_uint16_t     duk_uint32_t
 *    BE             01234567         0123               01
 *    LE             76543210         3210               10
 *    ME (ARM)       32107654         1032               01
 *
 *  Some processors may alter NaN values in a floating point load+store.
 *  For instance, on X86 a FLD + FSTP may convert a signaling NaN to a
 *  quiet one.  This is catastrophic when NaN space is used in packed
 *  duk_tval values.  See: misc/clang_aliasing.c.
 */

#if !defined(DUK_DBLUNION_H_INCLUDED)
#define DUK_DBLUNION_H_INCLUDED

/*
 *  Union for accessing double parts, also serves as packed duk_tval
 */

union duk_double_union {
	double d;
	float f[2];
#if defined(DUK_USE_64BIT_OPS)
	duk_uint64_t ull[1];
#endif
	duk_uint32_t ui[2];
	duk_uint16_t us[4];
	duk_uint8_t uc[8];
#if defined(DUK_USE_PACKED_TVAL)
	void *vp[2];  /* used by packed duk_tval, assumes sizeof(void *) == 4 */
#endif
};

typedef union duk_double_union duk_double_union;

/*
 *  Indexes of various types with respect to big endian (logical) layout
 */

#if defined(DUK_USE_DOUBLE_LE)
#if defined(DUK_USE_64BIT_OPS)
#define DUK_DBL_IDX_ULL0   0
#endif
#define DUK_DBL_IDX_UI0    1
#define DUK_DBL_IDX_UI1    0
#define DUK_DBL_IDX_US0    3
#define DUK_DBL_IDX_US1    2
#define DUK_DBL_IDX_US2    1
#define DUK_DBL_IDX_US3    0
#define DUK_DBL_IDX_UC0    7
#define DUK_DBL_IDX_UC1    6
#define DUK_DBL_IDX_UC2    5
#define DUK_DBL_IDX_UC3    4
#define DUK_DBL_IDX_UC4    3
#define DUK_DBL_IDX_UC5    2
#define DUK_DBL_IDX_UC6    1
#define DUK_DBL_IDX_UC7    0
#define DUK_DBL_IDX_VP0    DUK_DBL_IDX_UI0  /* packed tval */
#define DUK_DBL_IDX_VP1    DUK_DBL_IDX_UI1  /* packed tval */
#elif defined(DUK_USE_DOUBLE_BE)
#if defined(DUK_USE_64BIT_OPS)
#define DUK_DBL_IDX_ULL0   0
#endif
#define DUK_DBL_IDX_UI0    0
#define DUK_DBL_IDX_UI1    1
#define DUK_DBL_IDX_US0    0
#define DUK_DBL_IDX_US1    1
#define DUK_DBL_IDX_US2    2
#define DUK_DBL_IDX_US3    3
#define DUK_DBL_IDX_UC0    0
#define DUK_DBL_IDX_UC1    1
#define DUK_DBL_IDX_UC2    2
#define DUK_DBL_IDX_UC3    3
#define DUK_DBL_IDX_UC4    4
#define DUK_DBL_IDX_UC5    5
#define DUK_DBL_IDX_UC6    6
#define DUK_DBL_IDX_UC7    7
#define DUK_DBL_IDX_VP0    DUK_DBL_IDX_UI0  /* packed tval */
#define DUK_DBL_IDX_VP1    DUK_DBL_IDX_UI1  /* packed tval */
#elif defined(DUK_USE_DOUBLE_ME)
#if defined(DUK_USE_64BIT_OPS)
#define DUK_DBL_IDX_ULL0   0  /* not directly applicable, byte order differs from a double */
#endif
#define DUK_DBL_IDX_UI0    0
#define DUK_DBL_IDX_UI1    1
#define DUK_DBL_IDX_US0    1
#define DUK_DBL_IDX_US1    0
#define DUK_DBL_IDX_US2    3
#define DUK_DBL_IDX_US3    2
#define DUK_DBL_IDX_UC0    3
#define DUK_DBL_IDX_UC1    2
#define DUK_DBL_IDX_UC2    1
#define DUK_DBL_IDX_UC3    0
#define DUK_DBL_IDX_UC4    7
#define DUK_DBL_IDX_UC5    6
#define DUK_DBL_IDX_UC6    5
#define DUK_DBL_IDX_UC7    4
#define DUK_DBL_IDX_VP0    DUK_DBL_IDX_UI0  /* packed tval */
#define DUK_DBL_IDX_VP1    DUK_DBL_IDX_UI1  /* packed tval */
#else
#error internal error
#endif

/*
 *  Helper macros for reading/writing memory representation parts, used
 *  by duk_numconv.c and duk_tval.h.
 */

#define DUK_DBLUNION_SET_DOUBLE(u,v)  do {  \
		(u)->d = (v); \
	} while (0)

#define DUK_DBLUNION_SET_HIGH32(u,v)  do {  \
		(u)->ui[DUK_DBL_IDX_UI0] = (duk_uint32_t) (v); \
	} while (0)

#if defined(DUK_USE_64BIT_OPS)
#if defined(DUK_USE_DOUBLE_ME)
#define DUK_DBLUNION_SET_HIGH32_ZERO_LOW32(u,v)  do { \
		(u)->ull[DUK_DBL_IDX_ULL0] = (duk_uint64_t) (v); \
	} while (0)
#else
#define DUK_DBLUNION_SET_HIGH32_ZERO_LOW32(u,v)  do { \
		(u)->ull[DUK_DBL_IDX_ULL0] = ((duk_uint64_t) (v)) << 32; \
	} while (0)
#endif
#else  /* DUK_USE_64BIT_OPS */
#define DUK_DBLUNION_SET_HIGH32_ZERO_LOW32(u,v)  do { \
		(u)->ui[DUK_DBL_IDX_UI0] = (duk_uint32_t) (v); \
		(u)->ui[DUK_DBL_IDX_UI1] = (duk_uint32_t) 0; \
	} while (0)
#endif  /* DUK_USE_64BIT_OPS */

#define DUK_DBLUNION_SET_LOW32(u,v)  do {  \
		(u)->ui[DUK_DBL_IDX_UI1] = (duk_uint32_t) (v); \
	} while (0)

#define DUK_DBLUNION_GET_DOUBLE(u)  ((u)->d)
#define DUK_DBLUNION_GET_HIGH32(u)  ((u)->ui[DUK_DBL_IDX_UI0])
#define DUK_DBLUNION_GET_LOW32(u)   ((u)->ui[DUK_DBL_IDX_UI1])

#if defined(DUK_USE_64BIT_OPS)
#if defined(DUK_USE_DOUBLE_ME)
#define DUK_DBLUNION_SET_UINT64(u,v)  do { \
		(u)->ui[DUK_DBL_IDX_UI0] = (duk_uint32_t) ((v) >> 32); \
		(u)->ui[DUK_DBL_IDX_UI1] = (duk_uint32_t) (v); \
	} while (0)
#define DUK_DBLUNION_GET_UINT64(u) \
	((((duk_uint64_t) (u)->ui[DUK_DBL_IDX_UI0]) << 32) | \
	 ((duk_uint64_t) (u)->ui[DUK_DBL_IDX_UI1]))
#else
#define DUK_DBLUNION_SET_UINT64(u,v)  do { \
		(u)->ull[DUK_DBL_IDX_ULL0] = (duk_uint64_t) (v); \
	} while (0)
#define DUK_DBLUNION_GET_UINT64(u)  ((u)->ull[DUK_DBL_IDX_ULL0])
#endif
#define DUK_DBLUNION_SET_INT64(u,v) DUK_DBLUNION_SET_UINT64((u), (duk_uint64_t) (v))
#define DUK_DBLUNION_GET_INT64(u)   ((duk_int64_t) DUK_DBLUNION_GET_UINT64((u)))
#endif  /* DUK_USE_64BIT_OPS */

/*
 *  Double NaN manipulation macros related to NaN normalization needed when
 *  using the packed duk_tval representation.  NaN normalization is necessary
 *  to keep double values compatible with the duk_tval format.
 *
 *  When packed duk_tval is used, the NaN space is used to store pointers
 *  and other tagged values in addition to NaNs.  Actual NaNs are normalized
 *  to a specific quiet NaN.  The macros below are used by the implementation
 *  to check and normalize NaN values when they might be created.  The macros
 *  are essentially NOPs when the non-packed duk_tval representation is used.
 *
 *  A FULL check is exact and checks all bits.  A NOTFULL check is used by
 *  the packed duk_tval and works correctly for all NaNs except those that
 *  begin with 0x7ff0.  Since the 'normalized NaN' values used with packed
 *  duk_tval begin with 0x7ff8, the partial check is reliable when packed
 *  duk_tval is used.  The 0x7ff8 prefix means the normalized NaN will be a
 *  quiet NaN regardless of its remaining lower bits.
 *
 *  The ME variant below is specifically for ARM byte order, which has the
 *  feature that while doubles have a mixed byte order (32107654), unsigned
 *  long long values has a little endian byte order (76543210).  When writing
 *  a logical double value through a ULL pointer, the 32-bit words need to be
 *  swapped; hence the #if defined()s below for ULL writes with DUK_USE_DOUBLE_ME.
 *  This is not full ARM support but suffices for some environments.
 */

#if defined(DUK_USE_64BIT_OPS)
#if defined(DUK_USE_DOUBLE_ME)
/* Macros for 64-bit ops + mixed endian doubles. */
#define DUK__DBLUNION_SET_NAN_FULL(u)  do { \
		(u)->ull[DUK_DBL_IDX_ULL0] = 0x000000007ff80000ULL; \
	} while (0)
#define DUK__DBLUNION_IS_NAN_FULL(u) \
	((((u)->ull[DUK_DBL_IDX_ULL0] & 0x000000007ff00000ULL) == 0x000000007ff00000ULL) && \
	 ((((u)->ull[DUK_DBL_IDX_ULL0]) & 0xffffffff000fffffULL) != 0))
#define DUK__DBLUNION_IS_NORMALIZED_NAN_FULL(u) \
	((u)->ull[DUK_DBL_IDX_ULL0] == 0x000000007ff80000ULL)
#define DUK__DBLUNION_IS_ANYINF(u) \
	(((u)->ull[DUK_DBL_IDX_ULL0] & 0xffffffff7fffffffULL) == 0x000000007ff00000ULL)
#define DUK__DBLUNION_IS_POSINF(u) \
	((u)->ull[DUK_DBL_IDX_ULL0] == 0x000000007ff00000ULL)
#define DUK__DBLUNION_IS_NEGINF(u) \
	((u)->ull[DUK_DBL_IDX_ULL0] == 0x00000000fff00000ULL)
#define DUK__DBLUNION_IS_ANYZERO(u) \
	(((u)->ull[DUK_DBL_IDX_ULL0] & 0xffffffff7fffffffULL) == 0x0000000000000000ULL)
#define DUK__DBLUNION_IS_POSZERO(u) \
	((u)->ull[DUK_DBL_IDX_ULL0] == 0x0000000000000000ULL)
#define DUK__DBLUNION_IS_NEGZERO(u) \
	((u)->ull[DUK_DBL_IDX_ULL0] == 0x0000000080000000ULL)
#else
/* Macros for 64-bit ops + big/little endian doubles. */
#define DUK__DBLUNION_SET_NAN_FULL(u)  do { \
		(u)->ull[DUK_DBL_IDX_ULL0] = 0x7ff8000000000000ULL; \
	} while (0)
#define DUK__DBLUNION_IS_NAN_FULL(u) \
	((((u)->ull[DUK_DBL_IDX_ULL0] & 0x7ff0000000000000ULL) == 0x7ff0000000000000UL) && \
	 ((((u)->ull[DUK_DBL_IDX_ULL0]) & 0x000fffffffffffffULL) != 0))
#define DUK__DBLUNION_IS_NORMALIZED_NAN_FULL(u) \
	((u)->ull[DUK_DBL_IDX_ULL0] == 0x7ff8000000000000ULL)
#define DUK__DBLUNION_IS_ANYINF(u) \
	(((u)->ull[DUK_DBL_IDX_ULL0] & 0x7fffffffffffffffULL) == 0x7ff0000000000000ULL)
#define DUK__DBLUNION_IS_POSINF(u) \
	((u)->ull[DUK_DBL_IDX_ULL0] == 0x7ff0000000000000ULL)
#define DUK__DBLUNION_IS_NEGINF(u) \
	((u)->ull[DUK_DBL_IDX_ULL0] == 0xfff0000000000000ULL)
#define DUK__DBLUNION_IS_ANYZERO(u) \
	(((u)->ull[DUK_DBL_IDX_ULL0] & 0x7fffffffffffffffULL) == 0x0000000000000000ULL)
#define DUK__DBLUNION_IS_POSZERO(u) \
	((u)->ull[DUK_DBL_IDX_ULL0] == 0x0000000000000000ULL)
#define DUK__DBLUNION_IS_NEGZERO(u) \
	((u)->ull[DUK_DBL_IDX_ULL0] == 0x8000000000000000ULL)
#endif
#else  /* DUK_USE_64BIT_OPS */
/* Macros for no 64-bit ops, any endianness. */
#define DUK__DBLUNION_SET_NAN_FULL(u)  do { \
		(u)->ui[DUK_DBL_IDX_UI0] = (duk_uint32_t) 0x7ff80000UL; \
		(u)->ui[DUK_DBL_IDX_UI1] = (duk_uint32_t) 0x00000000UL; \
	} while (0)
#define DUK__DBLUNION_IS_NAN_FULL(u) \
	((((u)->ui[DUK_DBL_IDX_UI0] & 0x7ff00000UL) == 0x7ff00000UL) && \
	 (((u)->ui[DUK_DBL_IDX_UI0] & 0x000fffffUL) != 0 || \
          (u)->ui[DUK_DBL_IDX_UI1] != 0))
#define DUK__DBLUNION_IS_NORMALIZED_NAN_FULL(u) \
	(((u)->ui[DUK_DBL_IDX_UI0] == 0x7ff80000UL) && \
	 ((u)->ui[DUK_DBL_IDX_UI1] == 0x00000000UL))
#define DUK__DBLUNION_IS_ANYINF(u) \
	((((u)->ui[DUK_DBL_IDX_UI0] & 0x7fffffffUL) == 0x7ff00000UL) && \
	 ((u)->ui[DUK_DBL_IDX_UI1] == 0x00000000UL))
#define DUK__DBLUNION_IS_POSINF(u) \
	(((u)->ui[DUK_DBL_IDX_UI0] == 0x7ff00000UL) && \
	 ((u)->ui[DUK_DBL_IDX_UI1] == 0x00000000UL))
#define DUK__DBLUNION_IS_NEGINF(u) \
	(((u)->ui[DUK_DBL_IDX_UI0] == 0xfff00000UL) && \
	 ((u)->ui[DUK_DBL_IDX_UI1] == 0x00000000UL))
#define DUK__DBLUNION_IS_ANYZERO(u) \
	((((u)->ui[DUK_DBL_IDX_UI0] & 0x7fffffffUL) == 0x00000000UL) && \
	 ((u)->ui[DUK_DBL_IDX_UI1] == 0x00000000UL))
#define DUK__DBLUNION_IS_POSZERO(u) \
	(((u)->ui[DUK_DBL_IDX_UI0] == 0x00000000UL) && \
	 ((u)->ui[DUK_DBL_IDX_UI1] == 0x00000000UL))
#define DUK__DBLUNION_IS_NEGZERO(u) \
	(((u)->ui[DUK_DBL_IDX_UI0] == 0x80000000UL) && \
	 ((u)->ui[DUK_DBL_IDX_UI1] == 0x00000000UL))
#endif  /* DUK_USE_64BIT_OPS */

#define DUK__DBLUNION_SET_NAN_NOTFULL(u)  do { \
		(u)->us[DUK_DBL_IDX_US0] = 0x7ff8UL; \
	} while (0)

#define DUK__DBLUNION_IS_NAN_NOTFULL(u) \
	/* E == 0x7ff, topmost four bits of F != 0 => assume NaN */ \
	((((u)->us[DUK_DBL_IDX_US0] & 0x7ff0UL) == 0x7ff0UL) && \
	 (((u)->us[DUK_DBL_IDX_US0] & 0x000fUL) != 0x0000UL))

#define DUK__DBLUNION_IS_NORMALIZED_NAN_NOTFULL(u) \
	/* E == 0x7ff, F == 8 => normalized NaN */ \
	((u)->us[DUK_DBL_IDX_US0] == 0x7ff8UL)

#define DUK__DBLUNION_NORMALIZE_NAN_CHECK_FULL(u)  do { \
		if (DUK__DBLUNION_IS_NAN_FULL((u))) { \
			DUK__DBLUNION_SET_NAN_FULL((u)); \
		} \
	} while (0)

#define DUK__DBLUNION_NORMALIZE_NAN_CHECK_NOTFULL(u)  do { \
		if (DUK__DBLUNION_IS_NAN_NOTFULL((u))) { \
			DUK__DBLUNION_SET_NAN_NOTFULL((u)); \
		} \
	} while (0)

/* Concrete macros for NaN handling used by the implementation internals.
 * Chosen so that they match the duk_tval representation: with a packed
 * duk_tval, ensure NaNs are properly normalized; with a non-packed duk_tval
 * these are essentially NOPs.
 */

#if defined(DUK_USE_PACKED_TVAL)
#if defined(DUK_USE_FULL_TVAL)
#define DUK_DBLUNION_NORMALIZE_NAN_CHECK(u)  DUK__DBLUNION_NORMALIZE_NAN_CHECK_FULL((u))
#define DUK_DBLUNION_IS_NAN(u)               DUK__DBLUNION_IS_NAN_FULL((u))
#define DUK_DBLUNION_IS_NORMALIZED_NAN(u)    DUK__DBLUNION_IS_NORMALIZED_NAN_FULL((u))
#define DUK_DBLUNION_SET_NAN(d)              DUK__DBLUNION_SET_NAN_FULL((d))
#else
#define DUK_DBLUNION_NORMALIZE_NAN_CHECK(u)  DUK__DBLUNION_NORMALIZE_NAN_CHECK_NOTFULL((u))
#define DUK_DBLUNION_IS_NAN(u)               DUK__DBLUNION_IS_NAN_NOTFULL((u))
#define DUK_DBLUNION_IS_NORMALIZED_NAN(u)    DUK__DBLUNION_IS_NORMALIZED_NAN_NOTFULL((u))
#define DUK_DBLUNION_SET_NAN(d)              DUK__DBLUNION_SET_NAN_NOTFULL((d))
#endif
#define DUK_DBLUNION_IS_NORMALIZED(u) \
	(!DUK_DBLUNION_IS_NAN((u)) ||  /* either not a NaN */ \
	 DUK_DBLUNION_IS_NORMALIZED_NAN((u)))  /* or is a normalized NaN */
#else  /* DUK_USE_PACKED_TVAL */
#define DUK_DBLUNION_NORMALIZE_NAN_CHECK(u)  /* nop: no need to normalize */
#define DUK_DBLUNION_IS_NAN(u)               DUK__DBLUNION_IS_NAN_FULL((u))  /* (DUK_ISNAN((u)->d)) */
#define DUK_DBLUNION_IS_NORMALIZED_NAN(u)    DUK__DBLUNION_IS_NAN_FULL((u))  /* (DUK_ISNAN((u)->d)) */
#define DUK_DBLUNION_IS_NORMALIZED(u)        1  /* all doubles are considered normalized */
#define DUK_DBLUNION_SET_NAN(u)  do { \
		/* in non-packed representation we don't care about which NaN is used */ \
		(u)->d = DUK_DOUBLE_NAN; \
	} while (0)
#endif  /* DUK_USE_PACKED_TVAL */

#define DUK_DBLUNION_IS_ANYINF(u) DUK__DBLUNION_IS_ANYINF((u))
#define DUK_DBLUNION_IS_POSINF(u) DUK__DBLUNION_IS_POSINF((u))
#define DUK_DBLUNION_IS_NEGINF(u) DUK__DBLUNION_IS_NEGINF((u))

#define DUK_DBLUNION_IS_ANYZERO(u) DUK__DBLUNION_IS_ANYZERO((u))
#define DUK_DBLUNION_IS_POSZERO(u) DUK__DBLUNION_IS_POSZERO((u))
#define DUK_DBLUNION_IS_NEGZERO(u) DUK__DBLUNION_IS_NEGZERO((u))

/* XXX: native 64-bit byteswaps when available */

/* 64-bit byteswap, same operation independent of target endianness. */
#define DUK_DBLUNION_BSWAP64(u) do { \
		duk_uint32_t duk__bswaptmp1, duk__bswaptmp2; \
		duk__bswaptmp1 = (u)->ui[0]; \
		duk__bswaptmp2 = (u)->ui[1]; \
		duk__bswaptmp1 = DUK_BSWAP32(duk__bswaptmp1); \
		duk__bswaptmp2 = DUK_BSWAP32(duk__bswaptmp2); \
		(u)->ui[0] = duk__bswaptmp2; \
		(u)->ui[1] = duk__bswaptmp1; \
	} while (0)

/* Byteswap an IEEE double in the duk_double_union from host to network
 * order.  For a big endian target this is a no-op.
 */
#if defined(DUK_USE_DOUBLE_LE)
#define DUK_DBLUNION_DOUBLE_HTON(u) do { \
		duk_uint32_t duk__bswaptmp1, duk__bswaptmp2; \
		duk__bswaptmp1 = (u)->ui[0]; \
		duk__bswaptmp2 = (u)->ui[1]; \
		duk__bswaptmp1 = DUK_BSWAP32(duk__bswaptmp1); \
		duk__bswaptmp2 = DUK_BSWAP32(duk__bswaptmp2); \
		(u)->ui[0] = duk__bswaptmp2; \
		(u)->ui[1] = duk__bswaptmp1; \
	} while (0)
#elif defined(DUK_USE_DOUBLE_ME)
#define DUK_DBLUNION_DOUBLE_HTON(u) do { \
		duk_uint32_t duk__bswaptmp1, duk__bswaptmp2; \
		duk__bswaptmp1 = (u)->ui[0]; \
		duk__bswaptmp2 = (u)->ui[1]; \
		duk__bswaptmp1 = DUK_BSWAP32(duk__bswaptmp1); \
		duk__bswaptmp2 = DUK_BSWAP32(duk__bswaptmp2); \
		(u)->ui[0] = duk__bswaptmp1; \
		(u)->ui[1] = duk__bswaptmp2; \
	} while (0)
#elif defined(DUK_USE_DOUBLE_BE)
#define DUK_DBLUNION_DOUBLE_HTON(u) do { } while (0)
#else
#error internal error, double endianness insane
#endif

/* Reverse operation is the same. */
#define DUK_DBLUNION_DOUBLE_NTOH(u) DUK_DBLUNION_DOUBLE_HTON((u))

/* Some sign bit helpers. */
#if defined(DUK_USE_64BIT_OPS)
#define DUK_DBLUNION_HAS_SIGNBIT(u) (((u)->ull[DUK_DBL_IDX_ULL0] & 0x8000000000000000ULL) != 0)
#define DUK_DBLUNION_GET_SIGNBIT(u) (((u)->ull[DUK_DBL_IDX_ULL0] >> 63U))
#else
#define DUK_DBLUNION_HAS_SIGNBIT(u) (((u)->ui[DUK_DBL_IDX_UI0] & 0x80000000UL) != 0)
#define DUK_DBLUNION_GET_SIGNBIT(u) (((u)->ui[DUK_DBL_IDX_UI0] >> 31U))
#endif

#endif  /* DUK_DBLUNION_H_INCLUDED */

#endif  /* DUKTAPE_H_INCLUDED */
