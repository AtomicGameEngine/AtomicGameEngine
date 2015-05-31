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

#ifndef CPLUSPLUS_SYMBOLS_H
#define CPLUSPLUS_SYMBOLS_H

#include "CPlusPlusForwardDeclarations.h"
#include "Symbol.h"
#include "Type.h"
#include "FullySpecifiedType.h"
#include "Scope.h"
#include <vector>

namespace CPlusPlus {

class CPLUSPLUS_EXPORT UsingNamespaceDirective: public Symbol
{
public:
    UsingNamespaceDirective(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name);
    UsingNamespaceDirective(Clone *clone, Subst *subst, UsingNamespaceDirective *original);
    virtual ~UsingNamespaceDirective();

    // Symbol's interface
    virtual FullySpecifiedType type() const;

    virtual const UsingNamespaceDirective *asUsingNamespaceDirective() const
    { return this; }

    virtual UsingNamespaceDirective *asUsingNamespaceDirective()
    { return this; }

protected:
    virtual void visitSymbol0(SymbolVisitor *visitor);
};

class CPLUSPLUS_EXPORT UsingDeclaration: public Symbol
{
public:
    UsingDeclaration(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name);
    UsingDeclaration(Clone *clone, Subst *subst, UsingDeclaration *original);
    virtual ~UsingDeclaration();

    // Symbol's interface
    virtual FullySpecifiedType type() const;

    virtual const UsingDeclaration *asUsingDeclaration() const
    { return this; }

    virtual UsingDeclaration *asUsingDeclaration()
    { return this; }

protected:
    virtual void visitSymbol0(SymbolVisitor *visitor);
};

class CPLUSPLUS_EXPORT NamespaceAlias: public Symbol
{
public:
    NamespaceAlias(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name);
    NamespaceAlias(Clone *clone, Subst *subst, NamespaceAlias *original);
    virtual ~NamespaceAlias();

    const Name *namespaceName() const;
    void setNamespaceName(const Name *namespaceName);

    // Symbol's interface
    virtual FullySpecifiedType type() const;

    virtual const NamespaceAlias *asNamespaceAlias() const
    { return this; }

    virtual NamespaceAlias *asNamespaceAlias()
    { return this; }

protected:
    virtual void visitSymbol0(SymbolVisitor *visitor);

private:
    const Name *_namespaceName;
};

class CPLUSPLUS_EXPORT Declaration: public Symbol
{
public:
    Declaration(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name);
    Declaration(Clone *clone, Subst *subst, Declaration *original);
    virtual ~Declaration();

    void setType(const FullySpecifiedType &type);
    void setInitializer(StringLiteral const* initializer);

    // Symbol's interface
    virtual FullySpecifiedType type() const;
    const StringLiteral *getInitializer() const;

    virtual const Declaration *asDeclaration() const
    { return this; }

    virtual Declaration *asDeclaration()
    { return this; }

    virtual EnumeratorDeclaration *asEnumeratorDeclarator()
    { return 0; }

    virtual const EnumeratorDeclaration *asEnumeratorDeclarator() const
    { return 0; }

protected:
    virtual void visitSymbol0(SymbolVisitor *visitor);

private:
    FullySpecifiedType _type;
    const StringLiteral *_initializer;
};

class CPLUSPLUS_EXPORT EnumeratorDeclaration: public Declaration
{
public:
    EnumeratorDeclaration(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name);
    virtual ~EnumeratorDeclaration();

    const StringLiteral *constantValue() const;
    void setConstantValue(const StringLiteral *constantValue);

    virtual EnumeratorDeclaration *asEnumeratorDeclarator()
    { return this; }

    virtual const EnumeratorDeclaration *asEnumeratorDeclarator() const
    { return this; }

private:
    const StringLiteral *_constantValue;
};

class CPLUSPLUS_EXPORT Argument: public Symbol
{
public:
    Argument(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name);
    Argument(Clone *clone, Subst *subst, Argument *original);
    virtual ~Argument();

    void setType(const FullySpecifiedType &type);

    bool hasInitializer() const;

    const StringLiteral *initializer() const;
    void setInitializer(const StringLiteral *initializer);

    // Symbol's interface
    virtual FullySpecifiedType type() const;

    virtual const Argument *asArgument() const
    { return this; }

    virtual Argument *asArgument()
    { return this; }

protected:
    virtual void visitSymbol0(SymbolVisitor *visitor);

private:
    const StringLiteral *_initializer;
    FullySpecifiedType _type;
};

class CPLUSPLUS_EXPORT TypenameArgument: public Symbol
{
public:
    TypenameArgument(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name);
    TypenameArgument(Clone *clone, Subst *subst, TypenameArgument *original);
    virtual ~TypenameArgument();

    void setType(const FullySpecifiedType &type);

    // Symbol's interface
    virtual FullySpecifiedType type() const;

    virtual const TypenameArgument *asTypenameArgument() const
    { return this; }

    virtual TypenameArgument *asTypenameArgument()
    { return this; }

protected:
    virtual void visitSymbol0(SymbolVisitor *visitor);

private:
    FullySpecifiedType _type;
};

class CPLUSPLUS_EXPORT Block: public Scope
{
public:
    Block(TranslationUnit *translationUnit, unsigned sourceLocation);
    Block(Clone *clone, Subst *subst, Block *original);
    virtual ~Block();

    // Symbol's interface
    virtual FullySpecifiedType type() const;

    virtual const Block *asBlock() const
    { return this; }

    virtual Block *asBlock()
    { return this; }

protected:
    virtual void visitSymbol0(SymbolVisitor *visitor);
};

class CPLUSPLUS_EXPORT ForwardClassDeclaration: public Symbol, public Type
{
public:
    ForwardClassDeclaration(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name);
    ForwardClassDeclaration(Clone *clone, Subst *subst, ForwardClassDeclaration *original);
    virtual ~ForwardClassDeclaration();

    // Symbol's interface
    virtual FullySpecifiedType type() const;

    virtual const ForwardClassDeclaration *asForwardClassDeclaration() const
    { return this; }

    virtual ForwardClassDeclaration *asForwardClassDeclaration()
    { return this; }

    // Type's interface
    virtual const ForwardClassDeclaration *asForwardClassDeclarationType() const
    { return this; }

    virtual ForwardClassDeclaration *asForwardClassDeclarationType()
    { return this; }

protected:
    virtual void visitSymbol0(SymbolVisitor *visitor);
    virtual void accept0(TypeVisitor *visitor);
    virtual bool match0(const Type *otherType, Matcher *matcher) const;
};

class CPLUSPLUS_EXPORT Enum: public Scope, public Type
{
public:
    Enum(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name);
    Enum(Clone *clone, Subst *subst, Enum *original);
    virtual ~Enum();

    bool isScoped() const;
    void setScoped(bool scoped);

    // Symbol's interface
    virtual FullySpecifiedType type() const;

    virtual const Enum *asEnum() const
    { return this; }

    virtual Enum *asEnum()
    { return this; }

    // Type's interface
    virtual const Enum *asEnumType() const
    { return this; }

    virtual Enum *asEnumType()
    { return this; }

protected:
    virtual void visitSymbol0(SymbolVisitor *visitor);
    virtual void accept0(TypeVisitor *visitor);
    virtual bool match0(const Type *otherType, Matcher *matcher) const;

private:
    bool _isScoped;
};

class CPLUSPLUS_EXPORT Function: public Scope, public Type
{
public:
    enum MethodKey {
        NormalMethod,
        SlotMethod,
        SignalMethod,
        InvokableMethod
    };

public:
    Function(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name);
    Function(Clone *clone, Subst *subst, Function *original);
    virtual ~Function();

    bool isNormal() const;
    bool isSignal() const;
    bool isSlot() const;
    bool isInvokable() const;
    int methodKey() const;
    void setMethodKey(int key);

    FullySpecifiedType returnType() const;
    void setReturnType(const FullySpecifiedType &returnType);

    /** Convenience function that returns whether the function returns something (including void). */
    bool hasReturnType() const;

    unsigned argumentCount() const;
    Symbol *argumentAt(unsigned index) const;

    /** Convenience function that returns whether the function receives any arguments. */
    bool hasArguments() const;
    unsigned minimumArgumentCount() const;

    bool isVirtual() const;
    void setVirtual(bool isVirtual);

    bool isOverride() const;
    void setOverride(bool isOverride);

    bool isFinal() const;
    void setFinal(bool isFinal);

    bool isVariadic() const;
    void setVariadic(bool isVariadic);

    bool isConst() const;
    void setConst(bool isConst);

    bool isVolatile() const;
    void setVolatile(bool isVolatile);

    bool isPureVirtual() const;
    void setPureVirtual(bool isPureVirtual);

    bool isSignatureEqualTo(const Function *other, Matcher *matcher = 0) const;

    bool isAmbiguous() const; // internal
    void setAmbiguous(bool isAmbiguous); // internal

    bool maybeValidPrototype(unsigned actualArgumentCount) const;

    // Symbol's interface
    virtual FullySpecifiedType type() const;

    virtual const Function *asFunction() const
    { return this; }

    virtual Function *asFunction()
    { return this; }

    // Type's interface
    virtual const Function *asFunctionType() const
    { return this; }

    virtual Function *asFunctionType()
    { return this; }

protected:
    virtual void visitSymbol0(SymbolVisitor *visitor);
    virtual void accept0(TypeVisitor *visitor);
    virtual bool match0(const Type *otherType, Matcher *matcher) const;

private:
    FullySpecifiedType _returnType;
    struct Flags {
        unsigned _isVirtual: 1;
        unsigned _isOverride: 1;
        unsigned _isFinal: 1;
        unsigned _isVariadic: 1;
        unsigned _isPureVirtual: 1;
        unsigned _isConst: 1;
        unsigned _isVolatile: 1;
        unsigned _isAmbiguous: 1;
        unsigned _methodKey: 3;
    };
    union {
        unsigned _flags;
        Flags f;
    };
};

class CPLUSPLUS_EXPORT Template: public Scope, public Type
{
public:
    Template(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name);
    Template(Clone *clone, Subst *subst, Template *original);
    virtual ~Template();

    unsigned templateParameterCount() const;
    Symbol *templateParameterAt(unsigned index) const;
    Symbol *declaration() const;

    // Symbol's interface
    virtual FullySpecifiedType type() const;

    virtual const Template *asTemplate() const
    { return this; }

    virtual Template *asTemplate()
    { return this; }

    // Type's interface
    virtual const Template *asTemplateType() const
    { return this; }

    virtual Template *asTemplateType()
    { return this; }

protected:
    virtual void visitSymbol0(SymbolVisitor *visitor);
    virtual void accept0(TypeVisitor *visitor);
    virtual bool match0(const Type *otherType, Matcher *matcher) const;
};


class CPLUSPLUS_EXPORT Namespace: public Scope, public Type
{
public:
    Namespace(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name);
    Namespace(Clone *clone, Subst *subst, Namespace *original);
    virtual ~Namespace();

    // Symbol's interface
    virtual FullySpecifiedType type() const;

    virtual const Namespace *asNamespace() const
    { return this; }

    virtual Namespace *asNamespace()
    { return this; }

    // Type's interface
    virtual const Namespace *asNamespaceType() const
    { return this; }

    virtual Namespace *asNamespaceType()
    { return this; }

    bool isInline() const
    { return _isInline; }

    void setInline(bool onoff)
    { _isInline = onoff; }

protected:
    virtual void visitSymbol0(SymbolVisitor *visitor);
    virtual void accept0(TypeVisitor *visitor);
    virtual bool match0(const Type *otherType, Matcher *matcher) const;

private:
    bool _isInline;
};

class CPLUSPLUS_EXPORT BaseClass: public Symbol
{
public:
    BaseClass(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name);
    BaseClass(Clone *clone, Subst *subst, BaseClass *original);
    virtual ~BaseClass();

    bool isVirtual() const;
    void setVirtual(bool isVirtual);

    // Symbol's interface
    virtual FullySpecifiedType type() const;
    void setType(const FullySpecifiedType &type);

    virtual const BaseClass *asBaseClass() const
    { return this; }

    virtual BaseClass *asBaseClass()
    { return this; }

protected:
    virtual void visitSymbol0(SymbolVisitor *visitor);

private:
    bool _isVirtual;
    FullySpecifiedType _type;
};

class CPLUSPLUS_EXPORT Class: public Scope, public Type
{
public:
    Class(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name);
    Class(Clone *clone, Subst *subst, Class *original);
    virtual ~Class();

    enum Key {
        ClassKey,
        StructKey,
        UnionKey
    };

    bool isClass() const;
    bool isStruct() const;
    bool isUnion() const;
    Key classKey() const;
    void setClassKey(Key key);

    unsigned baseClassCount() const;
    BaseClass *baseClassAt(unsigned index) const;
    void addBaseClass(BaseClass *baseClass);

    // Symbol's interface
    virtual FullySpecifiedType type() const;

    virtual const Class *asClass() const
    { return this; }

    virtual Class *asClass()
    { return this; }

    // Type's interface
    virtual const Class *asClassType() const
    { return this; }

    virtual Class *asClassType()
    { return this; }

protected:
    virtual void visitSymbol0(SymbolVisitor *visitor);
    virtual void accept0(TypeVisitor *visitor);
    virtual bool match0(const Type *otherType, Matcher *matcher) const;

private:
    Key _key;
    std::vector<BaseClass *> _baseClasses;
};

class CPLUSPLUS_EXPORT QtPropertyDeclaration: public Symbol
{
public:
    enum Flag {
        NoFlags = 0,
        ReadFunction = 1 << 0,
        WriteFunction = 1 << 1,
        MemberVariable = 1 << 2,
        ResetFunction = 1 << 3,
        NotifyFunction = 1 << 4,
        DesignableFlag = 1 << 5,
        DesignableFunction = 1 << 6,
        ScriptableFlag = 1 << 7,
        ScriptableFunction = 1 << 8,
        StoredFlag = 1 << 9,
        StoredFunction = 1 << 10,
        UserFlag = 1 << 11,
        UserFunction = 1 << 12,
        ConstantFlag = 1 << 13,
        FinalFlag = 1 << 14
    };

public:
    QtPropertyDeclaration(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name);
    QtPropertyDeclaration(Clone *clone, Subst *subst, QtPropertyDeclaration *original);
    virtual ~QtPropertyDeclaration();

    void setType(const FullySpecifiedType &type);

    void setFlags(int flags);
    int flags() const;

    // Symbol's interface
    virtual FullySpecifiedType type() const;

    virtual const QtPropertyDeclaration *asQtPropertyDeclaration() const
    { return this; }

    virtual QtPropertyDeclaration *asQtPropertyDeclaration()
    { return this; }

protected:
    virtual void visitSymbol0(SymbolVisitor *visitor);

private:
    FullySpecifiedType _type;
    int _flags;
};

class CPLUSPLUS_EXPORT QtEnum: public Symbol
{
public:
    QtEnum(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name);
    QtEnum(Clone *clone, Subst *subst, QtEnum *original);
    virtual ~QtEnum();

    // Symbol's interface
    virtual FullySpecifiedType type() const;

    virtual const QtEnum *asQtEnum() const
    { return this; }

    virtual QtEnum *asQtEnum()
    { return this; }

protected:
    virtual void visitSymbol0(SymbolVisitor *visitor);
};

class CPLUSPLUS_EXPORT ObjCBaseClass: public Symbol
{
public:
    ObjCBaseClass(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name);
    ObjCBaseClass(Clone *clone, Subst *subst, ObjCBaseClass *original);
    virtual ~ObjCBaseClass();

    // Symbol's interface
    virtual FullySpecifiedType type() const;

    virtual const ObjCBaseClass *asObjCBaseClass() const
    { return this; }

    virtual ObjCBaseClass *asObjCBaseClass()
    { return this; }

protected:
    virtual void visitSymbol0(SymbolVisitor *visitor);
};

class CPLUSPLUS_EXPORT ObjCBaseProtocol: public Symbol
{
public:
    ObjCBaseProtocol(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name);
    ObjCBaseProtocol(Clone *clone, Subst *subst, ObjCBaseProtocol *original);
    virtual ~ObjCBaseProtocol();

    // Symbol's interface
    virtual FullySpecifiedType type() const;

    virtual const ObjCBaseProtocol *asObjCBaseProtocol() const
    { return this; }

    virtual ObjCBaseProtocol *asObjCBaseProtocol()
    { return this; }

protected:
    virtual void visitSymbol0(SymbolVisitor *visitor);
};

class CPLUSPLUS_EXPORT ObjCForwardProtocolDeclaration: public Symbol, public Type
{
public:
    ObjCForwardProtocolDeclaration(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name);
    ObjCForwardProtocolDeclaration(Clone *clone, Subst *subst, ObjCForwardProtocolDeclaration *original);
    virtual ~ObjCForwardProtocolDeclaration();

    // Symbol's interface
    virtual FullySpecifiedType type() const;

    virtual const ObjCForwardProtocolDeclaration *asObjCForwardProtocolDeclaration() const
    { return this; }

    virtual ObjCForwardProtocolDeclaration *asObjCForwardProtocolDeclaration()
    { return this; }

    // Type's interface
    virtual const ObjCForwardProtocolDeclaration *asObjCForwardProtocolDeclarationType() const
    { return this; }

    virtual ObjCForwardProtocolDeclaration *asObjCForwardProtocolDeclarationType()
    { return this; }

protected:
    virtual void visitSymbol0(SymbolVisitor *visitor);
    virtual void accept0(TypeVisitor *visitor);
    virtual bool match0(const Type *otherType, Matcher *matcher) const;
};

class CPLUSPLUS_EXPORT ObjCProtocol: public Scope, public Type
{
public:
    ObjCProtocol(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name);
    ObjCProtocol(Clone *clone, Subst *subst, ObjCProtocol *original);
    virtual ~ObjCProtocol();

    unsigned protocolCount() const;
    ObjCBaseProtocol *protocolAt(unsigned index) const;
    void addProtocol(ObjCBaseProtocol *protocol);

    // Symbol's interface
    virtual FullySpecifiedType type() const;

    virtual const ObjCProtocol *asObjCProtocol() const
    { return this; }

    virtual ObjCProtocol *asObjCProtocol()
    { return this; }

    // Type's interface
    virtual const ObjCProtocol *asObjCProtocolType() const
    { return this; }

    virtual ObjCProtocol *asObjCProtocolType()
    { return this; }

protected:
    virtual void visitSymbol0(SymbolVisitor *visitor);
    virtual void accept0(TypeVisitor *visitor);
    virtual bool match0(const Type *otherType, Matcher *matcher) const;

private:
    std::vector<ObjCBaseProtocol *> _protocols;
};

class CPLUSPLUS_EXPORT ObjCForwardClassDeclaration: public Symbol, public Type
{
public:
    ObjCForwardClassDeclaration(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name);
    ObjCForwardClassDeclaration(Clone *clone, Subst *subst, ObjCForwardClassDeclaration *original);
    virtual ~ObjCForwardClassDeclaration();

    // Symbol's interface
    virtual FullySpecifiedType type() const;

    virtual const ObjCForwardClassDeclaration *asObjCForwardClassDeclaration() const
    { return this; }

    virtual ObjCForwardClassDeclaration *asObjCForwardClassDeclaration()
    { return this; }

    // Type's interface
    virtual const ObjCForwardClassDeclaration *asObjCForwardClassDeclarationType() const
    { return this; }

    virtual ObjCForwardClassDeclaration *asObjCForwardClassDeclarationType()
    { return this; }

protected:
    virtual void visitSymbol0(SymbolVisitor *visitor);
    virtual void accept0(TypeVisitor *visitor);
    virtual bool match0(const Type *otherType, Matcher *matcher) const;
};

class CPLUSPLUS_EXPORT ObjCClass: public Scope, public Type
{
public:
    ObjCClass(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name);
    ObjCClass(Clone *clone, Subst *subst, ObjCClass *original);
    virtual ~ObjCClass();

    bool isInterface() const;
    void setInterface(bool isInterface);

    bool isCategory() const;
    const Name *categoryName() const;
    void setCategoryName(const Name *categoryName);

    ObjCBaseClass *baseClass() const;
    void setBaseClass(ObjCBaseClass *baseClass);

    unsigned protocolCount() const;
    ObjCBaseProtocol *protocolAt(unsigned index) const;
    void addProtocol(ObjCBaseProtocol *protocol);

    // Symbol's interface
    virtual FullySpecifiedType type() const;

    virtual const ObjCClass *asObjCClass() const
    { return this; }

    virtual ObjCClass *asObjCClass()
    { return this; }

    // Type's interface
    virtual const ObjCClass *asObjCClassType() const
    { return this; }

    virtual ObjCClass *asObjCClassType()
    { return this; }

protected:
    virtual void visitSymbol0(SymbolVisitor *visitor);
    virtual void accept0(TypeVisitor *visitor);
    virtual bool match0(const Type *otherType, Matcher *matcher) const;

private:
    const Name *_categoryName;
    ObjCBaseClass * _baseClass;
    std::vector<ObjCBaseProtocol *> _protocols;
    bool _isInterface;
};

class CPLUSPLUS_EXPORT ObjCMethod: public Scope, public Type
{
public:
    ObjCMethod(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name);
    ObjCMethod(Clone *clone, Subst *subst, ObjCMethod *original);
    virtual ~ObjCMethod();

    FullySpecifiedType returnType() const;
    void setReturnType(const FullySpecifiedType &returnType);

    /** Convenience function that returns whether the function returns something (including void). */
    bool hasReturnType() const;

    unsigned argumentCount() const;
    Symbol *argumentAt(unsigned index) const;

    /** Convenience function that returns whether the function receives any arguments. */
    bool hasArguments() const;

    bool isVariadic() const;
    void setVariadic(bool isVariadic);

    // Symbol's interface
    virtual FullySpecifiedType type() const;

    virtual const ObjCMethod *asObjCMethod() const
    { return this; }

    virtual ObjCMethod *asObjCMethod()
    { return this; }

    // Type's interface
    virtual const ObjCMethod *asObjCMethodType() const
    { return this; }

    virtual ObjCMethod *asObjCMethodType()
    { return this; }

protected:
    virtual void visitSymbol0(SymbolVisitor *visitor);
    virtual void accept0(TypeVisitor *visitor);
    virtual bool match0(const Type *otherType, Matcher *matcher) const;

private:
    FullySpecifiedType _returnType;
    struct Flags {
        unsigned _isVariadic: 1;
    };
    union {
        unsigned _flags;
        Flags f;
    };
};

class CPLUSPLUS_EXPORT ObjCPropertyDeclaration: public Symbol
{
public:
    enum PropertyAttributes {
        None = 0,
        Assign = 1 << 0,
        Retain = 1 << 1,
        Copy = 1 << 2,
        ReadOnly = 1 << 3,
        ReadWrite = 1 << 4,
        Getter = 1 << 5,
        Setter = 1 << 6,
        NonAtomic = 1 << 7,

        WritabilityMask = ReadOnly | ReadWrite,
        SetterSemanticsMask = Assign | Retain | Copy
    };

public:
    ObjCPropertyDeclaration(TranslationUnit *translationUnit,
                            unsigned sourceLocation,
                            const Name *name);
    ObjCPropertyDeclaration(Clone *clone, Subst *subst, ObjCPropertyDeclaration *original);
    virtual ~ObjCPropertyDeclaration();

    bool hasAttribute(int attribute) const;
    void setAttributes(int attributes);

    bool hasGetter() const;
    bool hasSetter() const;

    const Name *getterName() const;

    void setGetterName(const Name *getterName);

    const Name *setterName() const;
    void setSetterName(const Name *setterName);

    void setType(const FullySpecifiedType &type);

    // Symbol's interface
    virtual FullySpecifiedType type() const;

    virtual const ObjCPropertyDeclaration *asObjCPropertyDeclaration() const
    { return this; }

    virtual ObjCPropertyDeclaration *asObjCPropertyDeclaration()
    { return this; }

protected:
    virtual void visitSymbol0(SymbolVisitor *visitor);

private:
    const Name *_getterName;
    const Name *_setterName;
    FullySpecifiedType _type;
    int _propertyAttributes;
};

} // namespace CPlusPlus


#endif // CPLUSPLUS_SYMBOLS_H
