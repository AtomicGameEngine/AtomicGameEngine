// Copyright (c) 2008 Roberto Raggi <roberto.raggi@gmail.com>
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

#ifndef CPLUSPLUS_CPLUSPLUSFORWARDDECLARATIONS_H
#define CPLUSPLUS_CPLUSPLUSFORWARDDECLARATIONS_H

#include <cstdlib>
#include <cstddef>

#ifdef CPLUSPLUS_WITHOUT_QT
#  ifndef CPLUSPLUS_UNLIKELY
#    ifdef __GNUC__
#      define CPLUSPLUS_UNLIKELY(expr) __builtin_expect(!!(expr), false)
#    else
#      define CPLUSPLUS_UNLIKELY(expr) (expr)
#    endif
#  endif
#  define CPLUSPLUS_EXPORT
#else
#  include <qglobal.h>
#  ifndef CPLUSPLUS_UNLIKELY
#    ifdef Q_UNLIKELY
#      define CPLUSPLUS_UNLIKELY(expr) Q_UNLIKELY(expr)
#    else // pre 4.8.something
#      ifdef __GCC__
#        define CPLUSPLUS_UNLIKELY(expr) __builtin_expect(!!(expr), false)
#      else
#        define CPLUSPLUS_UNLIKELY(expr) (expr)
#      endif
#    endif
#  endif
#  if defined(CPLUSPLUS_BUILD_LIB)
#    define CPLUSPLUS_EXPORT Q_DECL_EXPORT
#  elif defined(CPLUSPLUS_BUILD_STATIC_LIB)
#    define CPLUSPLUS_EXPORT
#  else
#    define CPLUSPLUS_EXPORT Q_DECL_IMPORT
#  endif
#endif

namespace CPlusPlus {

class TranslationUnit;
class Control;
class MemoryPool;
class DiagnosticClient;

class Identifier;
class Literal;
class StringLiteral;
class NumericLiteral;

class SymbolTable;

// names
class NameVisitor;
class Name;
class Identifier;
class AnonymousNameId;
class TemplateNameId;
class DestructorNameId;
class OperatorNameId;
class ConversionNameId;
class QualifiedNameId;
class SelectorNameId;

// types
class Matcher;
class FullySpecifiedType;
class TypeVisitor;
class Type;
class UndefinedType;
class VoidType;
class IntegerType;
class FloatType;
class PointerToMemberType;
class PointerType;
class ReferenceType;
class ArrayType;
class NamedType;

// symbols
class Clone;
class Subst;

class SymbolVisitor;
class Symbol;
class Scope;
class UsingNamespaceDirective;
class UsingDeclaration;
class Declaration;
class Argument;
class TypenameArgument;
class Function;
class Namespace;
class NamespaceAlias;
class Template;
class BaseClass;
class Block;
class Class;
class Enum;
class EnumeratorDeclaration;
class ForwardClassDeclaration;

class Token;

// Qt symbols
class QtPropertyDeclaration;
class QtEnum;

// Objective-C symbols
class ObjCBaseClass;
class ObjCBaseProtocol;
class ObjCClass;
class ObjCForwardClassDeclaration;
class ObjCProtocol;
class ObjCForwardProtocolDeclaration;
class ObjCMethod;
class ObjCPropertyDeclaration;

} // namespace CPlusPlus

#endif // CPLUSPLUS_CPLUSPLUSFORWARDDECLARATIONS_H
