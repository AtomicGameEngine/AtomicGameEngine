#ifndef CPLUSPLUS_OBJC_TYPEQUALIFIERS_H
#define CPLUSPLUS_OBJC_TYPEQUALIFIERS_H

#include "CPlusPlusForwardDeclarations.h"


namespace CPlusPlus {

enum {
  Token_in,
  Token_out,
  Token_copy,
  Token_byref,
  Token_inout,
  Token_assign,
  Token_bycopy,
  Token_getter,
  Token_retain,
  Token_setter,
  Token_oneway,
  Token_readonly,
  Token_nonatomic,
  Token_readwrite,
  Token_identifier
};

CPLUSPLUS_EXPORT int classifyObjectiveCContextKeyword(const char *s, int n);

} // namespace CPlusPlus


#endif // CPLUSPLUS_OBJC_TYPEQUALIFIERS_H
