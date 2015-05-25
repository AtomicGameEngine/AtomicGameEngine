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

#include "Symbols.h"
#include "Names.h"
#include "TypeVisitor.h"
#include "SymbolVisitor.h"
#include "Matcher.h"
#include "Scope.h"
#include "Templates.h"

using namespace CPlusPlus;

UsingNamespaceDirective::UsingNamespaceDirective(TranslationUnit *translationUnit,
                                                 unsigned sourceLocation, const Name *name)
    : Symbol(translationUnit, sourceLocation, name)
{ }

UsingNamespaceDirective::UsingNamespaceDirective(Clone *clone, Subst *subst, UsingNamespaceDirective *original)
    : Symbol(clone, subst, original)
{ }

UsingNamespaceDirective::~UsingNamespaceDirective()
{ }

FullySpecifiedType UsingNamespaceDirective::type() const
{ return FullySpecifiedType(); }

void UsingNamespaceDirective::visitSymbol0(SymbolVisitor *visitor)
{ visitor->visit(this); }

NamespaceAlias::NamespaceAlias(TranslationUnit *translationUnit,
                               unsigned sourceLocation, const Name *name)
    : Symbol(translationUnit, sourceLocation, name), _namespaceName(0)
{ }

NamespaceAlias::NamespaceAlias(Clone *clone, Subst *subst, NamespaceAlias *original)
    : Symbol(clone, subst, original)
    , _namespaceName(clone->name(original->_namespaceName, subst))
{ }

NamespaceAlias::~NamespaceAlias()
{ }

const Name *NamespaceAlias::namespaceName() const
{ return _namespaceName; }

void NamespaceAlias::setNamespaceName(const Name *namespaceName)
{ _namespaceName = namespaceName; }

FullySpecifiedType NamespaceAlias::type() const
{ return FullySpecifiedType(); }

void NamespaceAlias::visitSymbol0(SymbolVisitor *visitor)
{ visitor->visit(this); }


UsingDeclaration::UsingDeclaration(TranslationUnit *translationUnit,
                                   unsigned sourceLocation, const Name *name)
    : Symbol(translationUnit, sourceLocation, name)
{ }

UsingDeclaration::UsingDeclaration(Clone *clone, Subst *subst, UsingDeclaration *original)
    : Symbol(clone, subst, original)
{ }

UsingDeclaration::~UsingDeclaration()
{ }

FullySpecifiedType UsingDeclaration::type() const
{ return FullySpecifiedType(); }

void UsingDeclaration::visitSymbol0(SymbolVisitor *visitor)
{ visitor->visit(this); }

Declaration::Declaration(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name)
    : Symbol(translationUnit, sourceLocation, name)
    , _initializer(0)
{ }

Declaration::Declaration(Clone *clone, Subst *subst, Declaration *original)
    : Symbol(clone, subst, original)
    , _type(clone->type(original->_type, subst))
    , _initializer(clone->stringLiteral(original->_initializer))
{ }

Declaration::~Declaration()
{ }

void Declaration::setType(const FullySpecifiedType &type)
{ _type = type; }

void Declaration::setInitializer(const StringLiteral *initializer)
{
    _initializer = initializer;
}

FullySpecifiedType Declaration::type() const
{ return _type; }

const StringLiteral *Declaration::getInitializer() const
{
    return _initializer;
}

void Declaration::visitSymbol0(SymbolVisitor *visitor)
{ visitor->visit(this); }

EnumeratorDeclaration::EnumeratorDeclaration(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name)
    : Declaration(translationUnit, sourceLocation, name)
    , _constantValue(0)
{}

EnumeratorDeclaration::~EnumeratorDeclaration()
{}

const StringLiteral *EnumeratorDeclaration::constantValue() const
{ return _constantValue; }

void EnumeratorDeclaration::setConstantValue(const StringLiteral *constantValue)
{ _constantValue = constantValue; }

Argument::Argument(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name)
    : Symbol(translationUnit, sourceLocation, name),
      _initializer(0)
{ }

Argument::Argument(Clone *clone, Subst *subst, Argument *original)
    : Symbol(clone, subst, original)
    , _initializer(clone->stringLiteral(original->_initializer))
    , _type(clone->type(original->_type, subst))
{ }

Argument::~Argument()
{ }

bool Argument::hasInitializer() const
{ return _initializer != 0; }

const StringLiteral *Argument::initializer() const
{ return _initializer; }

void Argument::setInitializer(const StringLiteral *initializer)
{ _initializer = initializer; }

void Argument::setType(const FullySpecifiedType &type)
{ _type = type; }

FullySpecifiedType Argument::type() const
{ return _type; }

void Argument::visitSymbol0(SymbolVisitor *visitor)
{ visitor->visit(this); }

TypenameArgument::TypenameArgument(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name)
    : Symbol(translationUnit, sourceLocation, name)
{ }

TypenameArgument::TypenameArgument(Clone *clone, Subst *subst, TypenameArgument *original)
    : Symbol(clone, subst, original)
    , _type(clone->type(original->_type, subst))
{ }

TypenameArgument::~TypenameArgument()
{ }

void TypenameArgument::setType(const FullySpecifiedType &type)
{ _type = type; }

FullySpecifiedType TypenameArgument::type() const
{ return _type; }

void TypenameArgument::visitSymbol0(SymbolVisitor *visitor)
{ visitor->visit(this); }

Function::Function(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name)
    : Scope(translationUnit, sourceLocation, name),
      _flags(0)
{ }

Function::Function(Clone *clone, Subst *subst, Function *original)
    : Scope(clone, subst, original)
    , _returnType(clone->type(original->_returnType, subst))
    , _flags(original->_flags)
{ }

Function::~Function()
{ }

bool Function::isNormal() const
{ return f._methodKey == NormalMethod; }

bool Function::isSignal() const
{ return f._methodKey == SignalMethod; }

bool Function::isSlot() const
{ return f._methodKey == SlotMethod; }

bool Function::isInvokable() const
{ return f._methodKey == InvokableMethod; }

int Function::methodKey() const
{ return f._methodKey; }

void Function::setMethodKey(int key)
{ f._methodKey = key; }

bool Function::isSignatureEqualTo(const Function *other, Matcher *matcher) const
{
    if (! other)
        return false;
    else if (isConst() != other->isConst())
        return false;
    else if (isVolatile() != other->isVolatile())
        return false;
    else if (! Matcher::match(unqualifiedName(), other->unqualifiedName(), matcher))
        return false;

    const unsigned argc = argumentCount();
    if (argc != other->argumentCount())
        return false;
    for (unsigned i = 0; i < argc; ++i) {
        Symbol *l = argumentAt(i);
        Symbol *r = other->argumentAt(i);
        if (! l->type().match(r->type(), matcher))
            return false;
    }
    return true;
}

void Function::accept0(TypeVisitor *visitor)
{ visitor->visit(this); }

bool Function::match0(const Type *otherType, Matcher *matcher) const
{
    if (const Function *otherTy = otherType->asFunctionType())
        return matcher->match(this, otherTy);

    return false;
}

FullySpecifiedType Function::type() const
{
    FullySpecifiedType ty(const_cast<Function *>(this));
    ty.setConst(isConst());
    ty.setVolatile(isVolatile());
    return ty;
}

FullySpecifiedType Function::returnType() const
{ return _returnType; }

void Function::setReturnType(const FullySpecifiedType &returnType)
{ _returnType = returnType; }

bool Function::hasReturnType() const
{
    const FullySpecifiedType ty = returnType();
    return ty.isValid() || ty.isSigned() || ty.isUnsigned();
}

unsigned Function::argumentCount() const
{
    const unsigned memCnt = memberCount();
    if (memCnt > 0 && memberAt(0)->type()->isVoidType())
        return 0;

    // Definitions with function-try-blocks will have more than a block, and
    // arguments with a lambda as default argument will also have more blocks.
    unsigned argc = 0;
    for (unsigned it = 0; it < memCnt; ++it)
        if (memberAt(it)->isArgument())
            ++argc;
    return argc;
}

Symbol *Function::argumentAt(unsigned index) const
{
    for (unsigned it = 0, eit = memberCount(); it < eit; ++it) {
        if (Argument *arg = memberAt(it)->asArgument()) {
            if (index == 0)
                return arg;
            else
                --index;
        }
    }

    return 0;
}

bool Function::hasArguments() const
{
    unsigned argc = argumentCount();
    return ! (argc == 0 || (argc == 1 && argumentAt(0)->type()->isVoidType()));
}

unsigned Function::minimumArgumentCount() const
{
    unsigned index = 0;

    for (unsigned ei = argumentCount(); index < ei; ++index) {
        if (Argument *arg = argumentAt(index)->asArgument()) {
            if (arg->hasInitializer())
                break;
        }
    }

    return index;
}

bool Function::isVirtual() const
{ return f._isVirtual; }

void Function::setVirtual(bool isVirtual)
{ f._isVirtual = isVirtual; }

bool Function::isOverride() const
{ return f._isOverride; }

void Function::setOverride(bool isOverride)
{ f._isOverride = isOverride; }

bool Function::isFinal() const
{ return f._isFinal; }

void Function::setFinal(bool isFinal)
{ f._isFinal = isFinal; }

bool Function::isVariadic() const
{ return f._isVariadic; }

void Function::setVariadic(bool isVariadic)
{ f._isVariadic = isVariadic; }

bool Function::isConst() const
{ return f._isConst; }

void Function::setConst(bool isConst)
{ f._isConst = isConst; }

bool Function::isVolatile() const
{ return f._isVolatile; }

void Function::setVolatile(bool isVolatile)
{ f._isVolatile = isVolatile; }

bool Function::isPureVirtual() const
{ return f._isPureVirtual; }

void Function::setPureVirtual(bool isPureVirtual)
{ f._isPureVirtual = isPureVirtual; }

bool Function::isAmbiguous() const
{ return f._isAmbiguous; }

void Function::setAmbiguous(bool isAmbiguous)
{ f._isAmbiguous = isAmbiguous; }

void Function::visitSymbol0(SymbolVisitor *visitor)
{
    if (visitor->visit(this)) {
        for (unsigned i = 0; i < memberCount(); ++i) {
            visitSymbol(memberAt(i), visitor);
        }
    }
}

bool Function::maybeValidPrototype(unsigned actualArgumentCount) const
{
    const unsigned argc = argumentCount();
    unsigned minNumberArguments = 0;

    for (; minNumberArguments < argc; ++minNumberArguments) {
        Argument *arg = argumentAt(minNumberArguments)->asArgument();

        if (! arg)
            return false;

        if (arg->hasInitializer())
            break;
    }

    if (actualArgumentCount < minNumberArguments) {
        // not enough arguments.
        return false;

    } else if (!isVariadic() && actualArgumentCount > argc) {
        // too many arguments.
        return false;
    }

    return true;
}


Block::Block(TranslationUnit *translationUnit, unsigned sourceLocation)
    : Scope(translationUnit, sourceLocation, /*name = */ 0)
{ }

Block::Block(Clone *clone, Subst *subst, Block *original)
    : Scope(clone, subst, original)
{ }

Block::~Block()
{ }

FullySpecifiedType Block::type() const
{ return FullySpecifiedType(); }

void Block::visitSymbol0(SymbolVisitor *visitor)
{
    if (visitor->visit(this)) {
        for (unsigned i = 0; i < memberCount(); ++i) {
            visitSymbol(memberAt(i), visitor);
        }
    }
}

Enum::Enum(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name)
    : Scope(translationUnit, sourceLocation, name)
    , _isScoped(false)
{ }

Enum::Enum(Clone *clone, Subst *subst, Enum *original)
    : Scope(clone, subst, original)
    , _isScoped(original->isScoped())
{ }

Enum::~Enum()
{ }

FullySpecifiedType Enum::type() const
{ return FullySpecifiedType(const_cast<Enum *>(this)); }

bool Enum::isScoped() const
{
    return _isScoped;
}

void Enum::setScoped(bool scoped)
{
    _isScoped = scoped;
}

void Enum::accept0(TypeVisitor *visitor)
{ visitor->visit(this); }

bool Enum::match0(const Type *otherType, Matcher *matcher) const
{
    if (const Enum *otherTy = otherType->asEnumType())
        return matcher->match(this, otherTy);

    return false;
}

void Enum::visitSymbol0(SymbolVisitor *visitor)
{
    if (visitor->visit(this)) {
        for (unsigned i = 0; i < memberCount(); ++i) {
            visitSymbol(memberAt(i), visitor);
        }
    }
}

Template::Template(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name)
    : Scope(translationUnit, sourceLocation, name)
{ }

Template::Template(Clone *clone, Subst *subst, Template *original)
    : Scope(clone, subst, original)
{ }

Template::~Template()
{ }

unsigned Template::templateParameterCount() const
{
    if (declaration() != 0)
        return memberCount() - 1;

    return 0;
}

Symbol *Template::templateParameterAt(unsigned index) const
{ return memberAt(index); }

Symbol *Template::declaration() const
{
    if (isEmpty())
        return 0;

    if (Symbol *s = memberAt(memberCount() - 1)) {
        if (s->isClass() || s->isForwardClassDeclaration() ||
            s->isTemplate() || s->isFunction() || s->isDeclaration())
            return s;
    }

    return 0;
}

FullySpecifiedType Template::type() const
{ return FullySpecifiedType(const_cast<Template *>(this)); }

void Template::visitSymbol0(SymbolVisitor *visitor)
{
    if (visitor->visit(this)) {
        for (unsigned i = 0; i < memberCount(); ++i) {
            visitSymbol(memberAt(i), visitor);
        }
    }
}

void Template::accept0(TypeVisitor *visitor)
{ visitor->visit(this); }

bool Template::match0(const Type *otherType, Matcher *matcher) const
{
    if (const Template *otherTy = otherType->asTemplateType())
        return matcher->match(this, otherTy);
    return false;
}

Namespace::Namespace(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name)
    : Scope(translationUnit, sourceLocation, name)
    , _isInline(false)
{ }

Namespace::Namespace(Clone *clone, Subst *subst, Namespace *original)
    : Scope(clone, subst, original)
    , _isInline(original->_isInline)
{ }

Namespace::~Namespace()
{ }

void Namespace::accept0(TypeVisitor *visitor)
{ visitor->visit(this); }

bool Namespace::match0(const Type *otherType, Matcher *matcher) const
{
    if (const Namespace *otherTy = otherType->asNamespaceType())
        return matcher->match(this, otherTy);

    return false;
}

void Namespace::visitSymbol0(SymbolVisitor *visitor)
{
    if (visitor->visit(this)) {
        for (unsigned i = 0; i < memberCount(); ++i) {
            visitSymbol(memberAt(i), visitor);
        }
    }
}

FullySpecifiedType Namespace::type() const
{ return FullySpecifiedType(const_cast<Namespace *>(this)); }

BaseClass::BaseClass(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name)
    : Symbol(translationUnit, sourceLocation, name),
      _isVirtual(false)
{ }

BaseClass::BaseClass(Clone *clone, Subst *subst, BaseClass *original)
    : Symbol(clone, subst, original)
    , _isVirtual(original->_isVirtual)
    , _type(clone->type(original->_type, subst))
{ }

BaseClass::~BaseClass()
{ }

FullySpecifiedType BaseClass::type() const
{ return _type; }

void BaseClass::setType(const FullySpecifiedType &type)
{ _type = type; }

bool BaseClass::isVirtual() const
{ return _isVirtual; }

void BaseClass::setVirtual(bool isVirtual)
{ _isVirtual = isVirtual; }

void BaseClass::visitSymbol0(SymbolVisitor *visitor)
{ visitor->visit(this); }

ForwardClassDeclaration::ForwardClassDeclaration(TranslationUnit *translationUnit,
                                                 unsigned sourceLocation, const Name *name)
    : Symbol(translationUnit, sourceLocation, name)
{ }

ForwardClassDeclaration::ForwardClassDeclaration(Clone *clone, Subst *subst, ForwardClassDeclaration *original)
    : Symbol(clone, subst, original)
{ }

ForwardClassDeclaration::~ForwardClassDeclaration()
{ }

FullySpecifiedType ForwardClassDeclaration::type() const
{ return FullySpecifiedType(const_cast<ForwardClassDeclaration *>(this)); }

void ForwardClassDeclaration::visitSymbol0(SymbolVisitor *visitor)
{ visitor->visit(this); }

void ForwardClassDeclaration::accept0(TypeVisitor *visitor)
{ visitor->visit(this); }

bool ForwardClassDeclaration::match0(const Type *otherType, Matcher *matcher) const
{
    if (const ForwardClassDeclaration *otherTy = otherType->asForwardClassDeclarationType())
        return matcher->match(this, otherTy);

    return false;
}

Class::Class(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name)
    : Scope(translationUnit, sourceLocation, name),
      _key(ClassKey)
{ }

Class::Class(Clone *clone, Subst *subst, Class *original)
    : Scope(clone, subst, original)
    , _key(original->_key)
{
    for (size_t i = 0; i < original->_baseClasses.size(); ++i)
        addBaseClass(clone->symbol(original->_baseClasses.at(i), subst)->asBaseClass());
}

Class::~Class()
{ }

bool Class::isClass() const
{ return _key == ClassKey; }

bool Class::isStruct() const
{ return _key == StructKey; }

bool Class::isUnion() const
{ return _key == UnionKey; }

Class::Key Class::classKey() const
{ return _key; }

void Class::setClassKey(Key key)
{ _key = key; }

void Class::accept0(TypeVisitor *visitor)
{ visitor->visit(this); }

bool Class::match0(const Type *otherType, Matcher *matcher) const
{
    if (const Class *otherTy = otherType->asClassType())
        return matcher->match(this, otherTy);

    return false;
}

unsigned Class::baseClassCount() const
{ return unsigned(_baseClasses.size()); }

BaseClass *Class::baseClassAt(unsigned index) const
{ return _baseClasses.at(index); }

void Class::addBaseClass(BaseClass *baseClass)
{ _baseClasses.push_back(baseClass); }

FullySpecifiedType Class::type() const
{ return FullySpecifiedType(const_cast<Class *>(this)); }

void Class::visitSymbol0(SymbolVisitor *visitor)
{
    if (visitor->visit(this)) {
        for (unsigned i = 0; i < _baseClasses.size(); ++i) {
            visitSymbol(_baseClasses.at(i), visitor);
        }
        for (unsigned i = 0; i < memberCount(); ++i) {
            visitSymbol(memberAt(i), visitor);
        }
    }
}


QtPropertyDeclaration::QtPropertyDeclaration(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name)
    : Symbol(translationUnit, sourceLocation, name)
    , _flags(NoFlags)
{ }

QtPropertyDeclaration::QtPropertyDeclaration(Clone *clone, Subst *subst, QtPropertyDeclaration *original)
    : Symbol(clone, subst, original)
    , _type(clone->type(original->_type, subst))
    , _flags(original->_flags)
{ }

QtPropertyDeclaration::~QtPropertyDeclaration()
{ }

void QtPropertyDeclaration::setType(const FullySpecifiedType &type)
{ _type = type; }

void QtPropertyDeclaration::setFlags(int flags)
{ _flags = flags; }

int QtPropertyDeclaration::flags() const
{ return _flags; }

FullySpecifiedType QtPropertyDeclaration::type() const
{ return _type; }

void QtPropertyDeclaration::visitSymbol0(SymbolVisitor *visitor)
{ visitor->visit(this); }


QtEnum::QtEnum(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name)
    : Symbol(translationUnit, sourceLocation, name)
{ }

QtEnum::QtEnum(Clone *clone, Subst *subst, QtEnum *original)
    : Symbol(clone, subst, original)
{ }

QtEnum::~QtEnum()
{ }

FullySpecifiedType QtEnum::type() const
{ return FullySpecifiedType(); }

void QtEnum::visitSymbol0(SymbolVisitor *visitor)
{ visitor->visit(this); }


ObjCBaseClass::ObjCBaseClass(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name)
    : Symbol(translationUnit, sourceLocation, name)
{ }

ObjCBaseClass::ObjCBaseClass(Clone *clone, Subst *subst, ObjCBaseClass *original)
    : Symbol(clone, subst, original)
{ }

ObjCBaseClass::~ObjCBaseClass()
{ }

FullySpecifiedType ObjCBaseClass::type() const
{ return FullySpecifiedType(); }

void ObjCBaseClass::visitSymbol0(SymbolVisitor *visitor)
{ visitor->visit(this); }

ObjCBaseProtocol::ObjCBaseProtocol(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name)
    : Symbol(translationUnit, sourceLocation, name)
{ }

ObjCBaseProtocol::ObjCBaseProtocol(Clone *clone, Subst *subst, ObjCBaseProtocol *original)
    : Symbol(clone, subst, original)
{ }

ObjCBaseProtocol::~ObjCBaseProtocol()
{ }

FullySpecifiedType ObjCBaseProtocol::type() const
{ return FullySpecifiedType(); }

void ObjCBaseProtocol::visitSymbol0(SymbolVisitor *visitor)
{ visitor->visit(this); }

ObjCClass::ObjCClass(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name):
    Scope(translationUnit, sourceLocation, name),
    _categoryName(0),
    _baseClass(0),
    _isInterface(false)
{ }

ObjCClass::ObjCClass(Clone *clone, Subst *subst, ObjCClass *original)
    : Scope(clone, subst, original)
    , _categoryName(clone->name(original->_categoryName, subst))
    , _baseClass(0)
    , _isInterface(original->_isInterface)
{
    if (original->_baseClass)
        _baseClass = clone->symbol(original->_baseClass, subst)->asObjCBaseClass();
    for (size_t i = 0; i < original->_protocols.size(); ++i)
        addProtocol(clone->symbol(original->_protocols.at(i), subst)->asObjCBaseProtocol());
}

ObjCClass::~ObjCClass()
{}

bool ObjCClass::isInterface() const
{ return _isInterface; }

void ObjCClass::setInterface(bool isInterface)
{ _isInterface = isInterface; }

bool ObjCClass::isCategory() const
{ return _categoryName != 0; }

const Name *ObjCClass::categoryName() const
{ return _categoryName; }

void ObjCClass::setCategoryName(const Name *categoryName)
{ _categoryName = categoryName; }

ObjCBaseClass *ObjCClass::baseClass() const
{ return _baseClass; }

void ObjCClass::setBaseClass(ObjCBaseClass *baseClass)
{ _baseClass = baseClass; }

unsigned ObjCClass::protocolCount() const
{ return unsigned(_protocols.size()); }

ObjCBaseProtocol *ObjCClass::protocolAt(unsigned index) const
{ return _protocols.at(index); }

void ObjCClass::addProtocol(ObjCBaseProtocol *protocol)
{ _protocols.push_back(protocol); }

FullySpecifiedType ObjCClass::type() const
{ return FullySpecifiedType(const_cast<ObjCClass *>(this)); }

void ObjCClass::visitSymbol0(SymbolVisitor *visitor)
{
    if (visitor->visit(this)) {
        if (_baseClass)
            visitSymbol(_baseClass, visitor);

        for (unsigned i = 0; i < _protocols.size(); ++i)
            visitSymbol(_protocols.at(i), visitor);

        for (unsigned i = 0; i < memberCount(); ++i)
            visitSymbol(memberAt(i), visitor);
    }
}

void ObjCClass::accept0(TypeVisitor *visitor)
{ visitor->visit(this); }

bool ObjCClass::match0(const Type *otherType, Matcher *matcher) const
{
    if (const ObjCClass *otherTy = otherType->asObjCClassType())
        return matcher->match(this, otherTy);

    return false;
}

ObjCProtocol::ObjCProtocol(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name):
        Scope(translationUnit, sourceLocation, name)
{
}

ObjCProtocol::ObjCProtocol(Clone *clone, Subst *subst, ObjCProtocol *original)
    : Scope(clone, subst, original)
{
    for (size_t i = 0; i < original->_protocols.size(); ++i)
        addProtocol(clone->symbol(original->_protocols.at(i), subst)->asObjCBaseProtocol());
}

ObjCProtocol::~ObjCProtocol()
{}

unsigned ObjCProtocol::protocolCount() const
{ return unsigned(_protocols.size()); }

ObjCBaseProtocol *ObjCProtocol::protocolAt(unsigned index) const
{ return _protocols.at(index); }

void ObjCProtocol::addProtocol(ObjCBaseProtocol *protocol)
{ _protocols.push_back(protocol); }

FullySpecifiedType ObjCProtocol::type() const
{ return FullySpecifiedType(const_cast<ObjCProtocol *>(this)); }

void ObjCProtocol::visitSymbol0(SymbolVisitor *visitor)
{
    if (visitor->visit(this)) {
        for (unsigned i = 0; i < _protocols.size(); ++i)
            visitSymbol(_protocols.at(i), visitor);
    }
}

void ObjCProtocol::accept0(TypeVisitor *visitor)
{ visitor->visit(this); }

bool ObjCProtocol::match0(const Type *otherType, Matcher *matcher) const
{
    if (const ObjCProtocol *otherTy = otherType->asObjCProtocolType())
        return matcher->match(this, otherTy);

    return false;
}

ObjCForwardClassDeclaration::ObjCForwardClassDeclaration(TranslationUnit *translationUnit, unsigned sourceLocation,
                                                         const Name *name):
        Symbol(translationUnit, sourceLocation, name)
{
}

ObjCForwardClassDeclaration::ObjCForwardClassDeclaration(Clone *clone, Subst *subst, ObjCForwardClassDeclaration *original)
    : Symbol(clone, subst, original)
{ }

ObjCForwardClassDeclaration::~ObjCForwardClassDeclaration()
{}

FullySpecifiedType ObjCForwardClassDeclaration::type() const
{ return FullySpecifiedType(); }

void ObjCForwardClassDeclaration::visitSymbol0(SymbolVisitor *visitor)
{ visitor->visit(this); }

void ObjCForwardClassDeclaration::accept0(TypeVisitor *visitor)
{ visitor->visit(this); }

bool ObjCForwardClassDeclaration::match0(const Type *otherType, Matcher *matcher) const
{
    if (const ObjCForwardClassDeclaration *otherTy = otherType->asObjCForwardClassDeclarationType())
        return matcher->match(this, otherTy);

    return false;
}

ObjCForwardProtocolDeclaration::ObjCForwardProtocolDeclaration(TranslationUnit *translationUnit, unsigned sourceLocation,
                                                               const Name *name):
        Symbol(translationUnit, sourceLocation, name)
{
}

ObjCForwardProtocolDeclaration::ObjCForwardProtocolDeclaration(Clone *clone, Subst *subst, ObjCForwardProtocolDeclaration *original)
    : Symbol(clone, subst, original)
{ }

ObjCForwardProtocolDeclaration::~ObjCForwardProtocolDeclaration()
{}

FullySpecifiedType ObjCForwardProtocolDeclaration::type() const
{ return FullySpecifiedType(); }

void ObjCForwardProtocolDeclaration::visitSymbol0(SymbolVisitor *visitor)
{ visitor->visit(this); }

void ObjCForwardProtocolDeclaration::accept0(TypeVisitor *visitor)
{ visitor->visit(this); }

bool ObjCForwardProtocolDeclaration::match0(const Type *otherType, Matcher *matcher) const
{
    if (const ObjCForwardProtocolDeclaration *otherTy = otherType->asObjCForwardProtocolDeclarationType())
        return matcher->match(this, otherTy);

    return false;
}

ObjCMethod::ObjCMethod(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name)
    : Scope(translationUnit, sourceLocation, name),
     _flags(0)
{ }

ObjCMethod::ObjCMethod(Clone *clone, Subst *subst, ObjCMethod *original)
    : Scope(clone, subst, original)
    , _returnType(clone->type(original->_returnType, subst))
    , _flags(original->_flags)
{ }

ObjCMethod::~ObjCMethod()
{ }

void ObjCMethod::accept0(TypeVisitor *visitor)
{ visitor->visit(this); }

bool ObjCMethod::match0(const Type *otherType, Matcher *matcher) const
{
    if (const ObjCMethod *otherTy = otherType->asObjCMethodType())
        return matcher->match(this, otherTy);

    return false;
}

FullySpecifiedType ObjCMethod::type() const
{ return FullySpecifiedType(const_cast<ObjCMethod *>(this)); }

FullySpecifiedType ObjCMethod::returnType() const
{ return _returnType; }

void ObjCMethod::setReturnType(const FullySpecifiedType &returnType)
{ _returnType = returnType; }

bool ObjCMethod::hasReturnType() const
{
    const FullySpecifiedType ty = returnType();
    return ty.isValid() || ty.isSigned() || ty.isUnsigned();
}

unsigned ObjCMethod::argumentCount() const
{
    const unsigned c = memberCount();
    if (c > 0 && memberAt(c - 1)->isBlock())
        return c - 1;
    return c;
}

Symbol *ObjCMethod::argumentAt(unsigned index) const
{
    return memberAt(index);
}

bool ObjCMethod::hasArguments() const
{
    return ! (argumentCount() == 0 ||
              (argumentCount() == 1 && argumentAt(0)->type()->isVoidType()));
}

bool ObjCMethod::isVariadic() const
{ return f._isVariadic; }

void ObjCMethod::setVariadic(bool isVariadic)
{ f._isVariadic = isVariadic; }

void ObjCMethod::visitSymbol0(SymbolVisitor *visitor)
{
    if (visitor->visit(this)) {
        for (unsigned i = 0; i < memberCount(); ++i) {
            visitSymbol(memberAt(i), visitor);
        }
    }
}

ObjCPropertyDeclaration::ObjCPropertyDeclaration(TranslationUnit *translationUnit,
                                                 unsigned sourceLocation,
                                                 const Name *name):
    Symbol(translationUnit, sourceLocation, name),
    _getterName(0),
    _setterName(0),
    _propertyAttributes(None)
{}

ObjCPropertyDeclaration::ObjCPropertyDeclaration(Clone *clone, Subst *subst, ObjCPropertyDeclaration *original)
    : Symbol(clone, subst, original)
    , _getterName(clone->name(original->_getterName, subst))
    , _setterName(clone->name(original->_setterName, subst))
    , _type(clone->type(original->_type, subst))
    , _propertyAttributes(original->_propertyAttributes)
{ }

ObjCPropertyDeclaration::~ObjCPropertyDeclaration()
{}

bool ObjCPropertyDeclaration::hasAttribute(int attribute) const
{ return _propertyAttributes & attribute; }

void ObjCPropertyDeclaration::setAttributes(int attributes)
{ _propertyAttributes = attributes; }

bool ObjCPropertyDeclaration::hasGetter() const
{ return hasAttribute(Getter); }

bool ObjCPropertyDeclaration::hasSetter() const
{ return hasAttribute(Setter); }

const Name *ObjCPropertyDeclaration::getterName() const
{ return _getterName; }

void ObjCPropertyDeclaration::setGetterName(const Name *getterName)
{ _getterName = getterName; }

const Name *ObjCPropertyDeclaration::setterName() const
{ return _setterName; }

void ObjCPropertyDeclaration::setSetterName(const Name *setterName)
{ _setterName = setterName; }

void ObjCPropertyDeclaration::setType(const FullySpecifiedType &type)
{ _type = type; }

FullySpecifiedType ObjCPropertyDeclaration::type() const
{ return _type; }

void ObjCPropertyDeclaration::visitSymbol0(SymbolVisitor *visitor)
{
    if (visitor->visit(this)) {
    }
}
