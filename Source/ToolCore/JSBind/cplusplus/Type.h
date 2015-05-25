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

#ifndef CPLUSPLUS_TYPE_H
#define CPLUSPLUS_TYPE_H

#include "CPlusPlusForwardDeclarations.h"

namespace CPlusPlus {

class CPLUSPLUS_EXPORT Type
{
public:
    Type();
    virtual ~Type();

    bool isUndefinedType() const;
    bool isVoidType() const;
    bool isIntegerType() const;
    bool isFloatType() const;
    bool isPointerType() const;
    bool isPointerToMemberType() const;
    bool isReferenceType() const;
    bool isArrayType() const;
    bool isNamedType() const;
    bool isFunctionType() const;
    bool isNamespaceType() const;
    bool isTemplateType() const;
    bool isClassType() const;
    bool isEnumType() const;
    bool isForwardClassDeclarationType() const;
    bool isObjCClassType() const;
    bool isObjCProtocolType() const;
    bool isObjCMethodType() const;
    bool isObjCForwardClassDeclarationType() const;
    bool isObjCForwardProtocolDeclarationType() const;

    virtual const UndefinedType *asUndefinedType() const { return 0; }
    virtual const VoidType *asVoidType() const { return 0; }
    virtual const IntegerType *asIntegerType() const { return 0; }
    virtual const FloatType *asFloatType() const { return 0; }
    virtual const PointerType *asPointerType() const { return 0; }
    virtual const PointerToMemberType *asPointerToMemberType() const { return 0; }
    virtual const ReferenceType *asReferenceType() const { return 0; }
    virtual const ArrayType *asArrayType() const { return 0; }
    virtual const NamedType *asNamedType() const { return 0; }
    virtual const Function *asFunctionType() const { return 0; }
    virtual const Namespace *asNamespaceType() const { return 0; }
    virtual const Template *asTemplateType() const { return 0; }
    virtual const Class *asClassType() const { return 0; }
    virtual const Enum *asEnumType() const { return 0; }
    virtual const ForwardClassDeclaration *asForwardClassDeclarationType() const { return 0; }
    virtual const ObjCClass *asObjCClassType() const { return 0; }
    virtual const ObjCProtocol *asObjCProtocolType() const { return 0; }
    virtual const ObjCMethod *asObjCMethodType() const { return 0; }
    virtual const ObjCForwardClassDeclaration *asObjCForwardClassDeclarationType() const { return 0; }
    virtual const ObjCForwardProtocolDeclaration *asObjCForwardProtocolDeclarationType() const { return 0; }

    virtual UndefinedType *asUndefinedType() { return 0; }
    virtual VoidType *asVoidType() { return 0; }
    virtual IntegerType *asIntegerType() { return 0; }
    virtual FloatType *asFloatType() { return 0; }
    virtual PointerType *asPointerType() { return 0; }
    virtual PointerToMemberType *asPointerToMemberType() { return 0; }
    virtual ReferenceType *asReferenceType() { return 0; }
    virtual ArrayType *asArrayType() { return 0; }
    virtual NamedType *asNamedType() { return 0; }
    virtual Function *asFunctionType() { return 0; }
    virtual Namespace *asNamespaceType() { return 0; }
    virtual Template *asTemplateType() { return 0; }
    virtual Class *asClassType() { return 0; }
    virtual Enum *asEnumType() { return 0; }
    virtual ForwardClassDeclaration *asForwardClassDeclarationType() { return 0; }
    virtual ObjCClass *asObjCClassType() { return 0; }
    virtual ObjCProtocol *asObjCProtocolType() { return 0; }
    virtual ObjCMethod *asObjCMethodType() { return 0; }
    virtual ObjCForwardClassDeclaration *asObjCForwardClassDeclarationType() { return 0; }
    virtual ObjCForwardProtocolDeclaration *asObjCForwardProtocolDeclarationType() { return 0; }

    void accept(TypeVisitor *visitor);
    static void accept(Type *type, TypeVisitor *visitor);

    bool match(const Type *other, Matcher *matcher = 0) const;

protected:
    virtual void accept0(TypeVisitor *visitor) = 0;

protected: // for Matcher
    friend class Matcher;
    virtual bool match0(const Type *otherType, Matcher *matcher) const = 0;
};

} // namespace CPlusPlus

#endif // CPLUSPLUS_TYPE_H
