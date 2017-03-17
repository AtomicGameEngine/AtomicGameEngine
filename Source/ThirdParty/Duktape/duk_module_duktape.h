#if !defined(DUK_MODULE_DUKTAPE_H_INCLUDED)
#define DUK_MODULE_DUKTAPE_H_INCLUDED

#include "duktape.h"

/* Maximum length of CommonJS module identifier to resolve.  Length includes
 * both current module ID, requested (possibly relative) module ID, and a
 * slash in between.
 */
#define  DUK_COMMONJS_MODULE_ID_LIMIT  256

#if defined(__cplusplus)
extern "C" {
#endif

extern void duk_module_duktape_init(duk_context *ctx);

#if defined(__cplusplus)
} // extern "C"
#endif

#endif  /* DUK_MODULE_DUKTAPE_H_INCLUDED */
