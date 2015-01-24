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

#include "Templates.h"
#include "CoreTypes.h"
#include "Symbols.h"
#include "Control.h"
#include "Names.h"
#include "Literals.h"

#include "cppassert.h"


using namespace CPlusPlus;

CloneType::CloneType(Clone *clone)
    : _clone(clone)
    , _control(clone->control())
    , _subst(0)
{
}

FullySpecifiedType CloneType::cloneType(const FullySpecifiedType &type, Subst *subst)
{
    TypeSubstPair typeSubstPair = std::make_pair(type, subst);
    if (_cache.find(typeSubstPair) != _cache.end())
        return _cache[typeSubstPair];

    std::swap(_subst, subst);
    FullySpecifiedType ty(type);
    std::swap(_type, ty);
    accept(_type.type());
    std::swap(_type, ty);
    std::swap(_subst, subst);

    _cache[typeSubstPair] = ty;
    return ty;
}

void CloneType::visit(UndefinedType *type)
{
    _type.setType(type);
}

void CloneType::visit(VoidType *)
{
    _type.setType(_control->voidType());
}

void CloneType::visit(IntegerType *type)
{
    _type.setType(_control->integerType(type->kind()));
}

void CloneType::visit(FloatType *type)
{
    _type.setType(_control->floatType(type->kind()));
}

void CloneType::visit(PointerToMemberType *type)
{
    _type.setType(_control->pointerToMemberType(_clone->name(type->memberName(), _subst),
                                                _clone->type(type->elementType(), _subst)));
}

void CloneType::visit(PointerType *type)
{
    _type.setType(_control->pointerType(_clone->type(type->elementType(), _subst)));
}

void CloneType::visit(ReferenceType *type)
{
    _type.setType(_control->referenceType(_clone->type(type->elementType(), _subst), type->isRvalueReference()));
}

void CloneType::visit(ArrayType *type)
{
    _type.setType(_control->arrayType(_clone->type(type->elementType(), _subst), type->size()));
}

void CloneType::visit(NamedType *type)
{
    const Name *name = _clone->name(type->name(), _subst);
    FullySpecifiedType ty;
    if (_subst)
        ty = _subst->apply(name);
    if (! ty.isValid())
        ty = _control->namedType(name);
    _type.setType(ty.type());
}

void CloneType::visit(Function *type)
{
    Function *f = _clone->symbol(type, _subst)->asFunction();
    _type = f;
}

void CloneType::visit(Namespace *type)
{
    Namespace *ns = _clone->symbol(type, _subst)->asNamespace();
    _type = ns;
}

void CloneType::visit(Template *type)
{
    Template *templ = _clone->symbol(type, _subst)->asTemplate();
    _type = templ;
}

void CloneType::visit(Class *type)
{
    Class *klass = _clone->symbol(type, _subst)->asClass();
    _type = klass;
}

void CloneType::visit(Enum *type)
{
    Enum *e = _clone->symbol(type, _subst)->asEnum();
    _type = e;
}

void CloneType::visit(ForwardClassDeclaration *type)
{
    ForwardClassDeclaration *fwd = _clone->symbol(type, _subst)->asForwardClassDeclaration();
    _type = fwd;
}

void CloneType::visit(ObjCClass *type)
{
    ObjCClass *klass = _clone->symbol(type, _subst)->asObjCClass();
    _type = klass;
}

void CloneType::visit(ObjCProtocol *type)
{
    ObjCProtocol *proto = _clone->symbol(type, _subst)->asObjCProtocol();
    _type = proto;
}

void CloneType::visit(ObjCMethod *type)
{
    ObjCMethod *meth = _clone->symbol(type, _subst)->asObjCMethod();
    _type = meth;
}

void CloneType::visit(ObjCForwardClassDeclaration *type)
{
    ObjCForwardClassDeclaration *fwd = _clone->symbol(type, _subst)->asObjCForwardClassDeclaration();
    _type = fwd;
}

void CloneType::visit(ObjCForwardProtocolDeclaration *type)
{
    ObjCForwardProtocolDeclaration *fwd = _clone->symbol(type, _subst)->asObjCForwardProtocolDeclaration();
    _type = fwd;
}

CloneSymbol::CloneSymbol(Clone *clone)
    : _clone(clone)
    , _control(clone->control())
    , _subst(0)
    , _symbol(0)
{
}

Symbol *CloneSymbol::cloneSymbol(Symbol *symbol, Subst *subst)
{
    if (! symbol)
        return 0;

    SymbolSubstPair symbolSubstPair = std::make_pair(symbol, subst);
    if (_cache.find(symbolSubstPair) != _cache.end()) {
        Symbol *cachedSymbol = _cache[symbolSubstPair];
        if (cachedSymbol->enclosingScope() == symbol->enclosingScope())
            return cachedSymbol;
    }

    Symbol *r = 0;
    std::swap(_subst, subst);
    std::swap(_symbol, r);
    accept(symbol);
    std::swap(_symbol, r);
    std::swap(_subst, subst);

    CPP_CHECK(r != 0);
    _cache[symbolSubstPair] = r;
    return r;
}

bool CloneSymbol::visit(UsingNamespaceDirective *symbol)
{
    UsingNamespaceDirective *u = new UsingNamespaceDirective(_clone, _subst, symbol);
    _symbol = u;
    _control->addSymbol(u);
    return false;
}

bool CloneSymbol::visit(UsingDeclaration *symbol)
{
    UsingDeclaration *u = new UsingDeclaration(_clone, _subst, symbol);
    _symbol = u;
    _control->addSymbol(u);
    return false;
}

bool CloneSymbol::visit(NamespaceAlias *symbol)
{
    NamespaceAlias *ns = new NamespaceAlias(_clone, _subst, symbol);
    _symbol = ns;
    _control->addSymbol(ns);
    return false;
}

bool CloneSymbol::visit(Declaration *symbol)
{
    Declaration *decl = new Declaration(_clone, _subst, symbol);
    _symbol = decl;
    _control->addSymbol(decl);
    return false;
}

bool CloneSymbol::visit(Argument *symbol)
{
    Argument *arg = new Argument(_clone, _subst, symbol);
    _symbol = arg;
    _control->addSymbol(arg);
    return false;
}

bool CloneSymbol::visit(TypenameArgument *symbol)
{
    TypenameArgument *arg = new TypenameArgument(_clone, _subst, symbol);
    _symbol = arg;
    _control->addSymbol(arg);
    return false;
}

bool CloneSymbol::visit(BaseClass *symbol)
{
    BaseClass *bc = new BaseClass(_clone, _subst, symbol);
    _symbol = bc;
    _control->addSymbol(bc);
    return false;
}

bool CloneSymbol::visit(Enum *symbol)
{
    Enum *e = new Enum(_clone, _subst, symbol);
    _symbol = e;
    _control->addSymbol(e);
    return false;
}

bool CloneSymbol::visit(Function *symbol)
{
    Function *fun = new Function(_clone, _subst, symbol);
    _symbol = fun;
    _control->addSymbol(fun);
    return false;
}

bool CloneSymbol::visit(Namespace *symbol)
{
    Namespace *ns = new Namespace(_clone, _subst, symbol);
    _symbol = ns;
    _control->addSymbol(ns);
    return false;
}

bool CloneSymbol::visit(Template *symbol)
{
    Template *templ = new Template(_clone, _subst, symbol);
    _symbol = templ;
    _control->addSymbol(templ);
    return false;
}

bool CloneSymbol::visit(Class *symbol)
{
    Class *klass = new Class(_clone, _subst, symbol);
    _symbol = klass;
    _control->addSymbol(klass);
    return false;
}

bool CloneSymbol::visit(Block *symbol)
{
    Block *block = new Block(_clone, _subst, symbol);
    _symbol = block;
    _control->addSymbol(block);
    return false;
}

bool CloneSymbol::visit(ForwardClassDeclaration *symbol)
{
    ForwardClassDeclaration *fwd = new ForwardClassDeclaration(_clone, _subst, symbol);
    _symbol = fwd;
    _control->addSymbol(fwd);
    return false;
}

bool CloneSymbol::visit(QtPropertyDeclaration *symbol)
{
    QtPropertyDeclaration *decl = new QtPropertyDeclaration(_clone, _subst, symbol);
    _symbol = decl;
    _control->addSymbol(decl);
    return false;
}

bool CloneSymbol::visit(QtEnum *symbol)
{
    QtEnum *e = new QtEnum(_clone, _subst, symbol);
    _symbol = e;
    _control->addSymbol(e);
    return false;
}

bool CloneSymbol::visit(ObjCBaseClass *symbol)
{
    ObjCBaseClass *bc = new ObjCBaseClass(_clone, _subst, symbol);
    _symbol = bc;
    _control->addSymbol(bc);
    return false;
}

bool CloneSymbol::visit(ObjCBaseProtocol *symbol)
{
    ObjCBaseProtocol *bc = new ObjCBaseProtocol(_clone, _subst, symbol);
    _symbol = bc;
    _control->addSymbol(bc);
    return false;
}

bool CloneSymbol::visit(ObjCClass *symbol)
{
    ObjCClass *klass = new ObjCClass(_clone, _subst, symbol);
    _symbol = klass;
    _control->addSymbol(klass);
    return false;
}

bool CloneSymbol::visit(ObjCForwardClassDeclaration *symbol)
{
    ObjCForwardClassDeclaration *fwd = new ObjCForwardClassDeclaration(_clone, _subst, symbol);
    _symbol = fwd;
    _control->addSymbol(fwd);
    return false;
}

bool CloneSymbol::visit(ObjCProtocol *symbol)
{
    ObjCProtocol *proto = new ObjCProtocol(_clone, _subst, symbol);
    _symbol = proto;
    _control->addSymbol(proto);
    return false;
}

bool CloneSymbol::visit(ObjCForwardProtocolDeclaration *symbol)
{
    ObjCForwardProtocolDeclaration *fwd = new ObjCForwardProtocolDeclaration(_clone, _subst, symbol);
    _symbol = fwd;
    _control->addSymbol(fwd);
    return false;
}

bool CloneSymbol::visit(ObjCMethod *symbol)
{
    ObjCMethod *meth = new ObjCMethod(_clone, _subst, symbol);
    _symbol = meth;
    _control->addSymbol(meth);
    return false;
}

bool CloneSymbol::visit(ObjCPropertyDeclaration *symbol)
{
    ObjCPropertyDeclaration *decl = new ObjCPropertyDeclaration(_clone, _subst, symbol);
    _symbol = decl;
    _control->addSymbol(decl);
    return false;
}

CloneName::CloneName(Clone *clone)
    : _clone(clone)
    , _control(clone->control())
    , _subst(0)
    , _name(0)
{
}

const Name *CloneName::cloneName(const Name *name, Subst *subst)
{
    if (! name)
        return 0;

    NameSubstPair nameSubstPair = std::make_pair(name, subst);
    if (_cache.find(nameSubstPair) != _cache.end())
        return _cache[nameSubstPair];

    const Name *r = 0;
    std::swap(_subst, subst);
    std::swap(_name, r);
    accept(name);
    std::swap(_name, r);
    std::swap(_subst, subst);
    CPP_CHECK(r != 0);
    _cache[nameSubstPair] = r;
    return r;
}

void CloneName::visit(const Identifier *name)
{
    _name = _control->identifier(name->chars(), name->size());
}

void CloneName::visit(const AnonymousNameId *name)
{
    _name = _control->anonymousNameId(name->classTokenIndex());
}

void CloneName::visit(const TemplateNameId *name)
{
    std::vector<FullySpecifiedType> args(name->templateArgumentCount());
    for (unsigned i = 0; i < args.size(); ++i)
        args[i] = _clone->type(name->templateArgumentAt(i), _subst);
    if (args.empty())
        _name = _control->templateNameId(_clone->identifier(name->identifier()), name->isSpecialization());
    else
        _name = _control->templateNameId(_clone->identifier(name->identifier()), name->isSpecialization(),
                                         &args[0], unsigned(args.size()));
}

void CloneName::visit(const DestructorNameId *name)
{
    _name = _control->destructorNameId(_clone->identifier(name->identifier()));
}

void CloneName::visit(const OperatorNameId *name)
{
    _name = _control->operatorNameId(name->kind());
}

void CloneName::visit(const ConversionNameId *name)
{
    _name = _control->conversionNameId(_clone->type(name->type(), _subst));
}

void CloneName::visit(const QualifiedNameId *name)
{
    _name = _control->qualifiedNameId(_clone->name(name->base(), _subst),
                                      _clone->name(name->name(), _subst));
}

void CloneName::visit(const SelectorNameId *name)
{
    CPP_CHECK(name->nameCount() > 0);
    std::vector<const Name *> names(name->nameCount());
    for (unsigned i = 0; i < names.size(); ++i)
        names[i] = _clone->name(name->nameAt(i), _subst);
    _name = _control->selectorNameId(&names[0], unsigned(names.size()), name->hasArguments());
}


Clone::Clone(Control *control)
    : _control(control)
    , _type(this)
    , _name(this)
    , _symbol(this)
{
}

const StringLiteral *Clone::stringLiteral(const StringLiteral *literal)
{
    return literal ? _control->stringLiteral(literal->chars(), literal->size()) : 0;
}

const NumericLiteral *Clone::numericLiteral(const NumericLiteral *literal)
{
    return literal ? _control->numericLiteral(literal->chars(), literal->size()) : 0;
}

const Identifier *Clone::identifier(const Identifier *id)
{
    return id ? _control->identifier(id->chars(), id->size()) : 0;
}

FullySpecifiedType Clone::type(const FullySpecifiedType &type, Subst *subst)
{
    return _type(type, subst);
}

const Name *Clone::name(const Name *name, Subst *subst)
{
    return _name(name, subst);
}

Symbol *Clone::symbol(Symbol *symbol, Subst *subst)
{
    return _symbol(symbol, subst);
}

Symbol *Clone::instantiate(Template *templ, const FullySpecifiedType *const args, unsigned argc, Subst *s)
{
    Subst subst(_control, s);
    for (unsigned i = 0, e = std::min(templ->templateParameterCount(), argc); i < e; ++i) {
        Symbol *formal = templ->templateParameterAt(i);
        FullySpecifiedType actual = args[i];
        subst.bind(name(formal->name(), 0), actual);
    }
    if (argc < templ->templateParameterCount()) {
        for (unsigned i = argc; i < templ->templateParameterCount(); ++i) {
            Symbol *formal = templ->templateParameterAt(i);
            if (TypenameArgument *tn = formal->asTypenameArgument())
                subst.bind(name(formal->name(), &subst), type(tn->type(), &subst));
        }
    }
    if (Symbol *inst = symbol(templ->declaration(), &subst)) {
        inst->setEnclosingScope(templ->enclosingScope());
        return inst;
    }
    return 0;
}

//
// substitutions
//


FullySpecifiedType Subst::apply(const Name *name) const
{
    if (name) {
        std::map<const Name *, FullySpecifiedType, Name::Compare>::const_iterator it = _map.find(name);
        if (it != _map.end())
            return it->second;
        else if (_previous)
            return _previous->apply(name);

        else if (const QualifiedNameId *q = name->asQualifiedNameId()) {
            const NamedType *baseNamedType = apply(q->base())->asNamedType();
            const NamedType *unqualified = apply(q->name())->asNamedType();
            if (baseNamedType) {
                if (! unqualified) {
                    const Name *qualifiedBase = baseNamedType->name();
                    const Name *qualifiedName = q->name();
                    return control()->namedType(control()->qualifiedNameId(qualifiedBase,
                                                                           qualifiedName));
                }
                else if(baseNamedType->name()->identifier() != 0) {
                    const QualifiedNameId *clonedQualifiedNameId
                            = control()->qualifiedNameId(baseNamedType->name()->identifier(),
                                                         unqualified->name());
                    NamedType *clonedNamedType = control()->namedType(clonedQualifiedNameId);
                    return clonedNamedType;
                }
            }
        }

    }
    return FullySpecifiedType();
}
