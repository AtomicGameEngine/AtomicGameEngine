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

#ifndef CPLUSPLUS_TEMPLATES_H
#define CPLUSPLUS_TEMPLATES_H

#include "CPlusPlusForwardDeclarations.h"
#include "TypeVisitor.h"
#include "FullySpecifiedType.h"
#include "Name.h"
#include "NameVisitor.h"
#include "SymbolVisitor.h"

#include <map>
#include <utility>

namespace CPlusPlus {

class Clone;

class CPLUSPLUS_EXPORT Subst
{
    Subst(const Subst &other);
    Subst &operator = (const Subst &other);

public:
    Subst(Control *control, Subst *previous = 0)
        : _control(control)
        , _previous(previous)
    { }

    Control *control() const { return _control; }
    Subst *previous() const { return _previous; }

    FullySpecifiedType apply(const Name *name) const;

    void bind(const Name *name, const FullySpecifiedType &ty)
    { _map.insert(std::make_pair(name, ty)); }

    FullySpecifiedType &operator[](const Name *name) { return _map[name]; }

    bool contains(const Name *name) const { return _map.find(name) != _map.end(); }

private:
    Control *_control;
    Subst *_previous;
    std::map<const Name *, FullySpecifiedType, Name::Compare> _map;
};

class CPLUSPLUS_EXPORT CloneType: protected TypeVisitor
{
public:
    CloneType(Clone *clone);

    FullySpecifiedType operator()(const FullySpecifiedType &type, Subst *subst) { return cloneType(type, subst); }
    FullySpecifiedType cloneType(const FullySpecifiedType &type, Subst *subst);

protected:
    virtual void visit(UndefinedType *type);
    virtual void visit(VoidType *type);
    virtual void visit(IntegerType *type);
    virtual void visit(FloatType *type);
    virtual void visit(PointerToMemberType *type);
    virtual void visit(PointerType *type);
    virtual void visit(ReferenceType *type);
    virtual void visit(ArrayType *type);
    virtual void visit(NamedType *type);
    virtual void visit(Function *type);
    virtual void visit(Namespace *type);
    virtual void visit(Template *type);
    virtual void visit(Class *type);
    virtual void visit(Enum *type);
    virtual void visit(ForwardClassDeclaration *type);
    virtual void visit(ObjCClass *type);
    virtual void visit(ObjCProtocol *type);
    virtual void visit(ObjCMethod *type);
    virtual void visit(ObjCForwardClassDeclaration *type);
    virtual void visit(ObjCForwardProtocolDeclaration *type);

protected:
    typedef std::pair <const FullySpecifiedType, Subst *> TypeSubstPair;
    std::map<TypeSubstPair, FullySpecifiedType> _cache;

    Clone *_clone;
    Control *_control;
    Subst *_subst;
    FullySpecifiedType _type;
};

class CPLUSPLUS_EXPORT CloneName: protected NameVisitor
{
public:
    CloneName(Clone *clone);

    const Name *operator()(const Name *name, Subst *subst) { return cloneName(name, subst); }
    const Name *cloneName(const Name *name, Subst *subst);

protected:
    virtual void visit(const Identifier *name);
    virtual void visit(const AnonymousNameId *name);
    virtual void visit(const TemplateNameId *name);
    virtual void visit(const DestructorNameId *name);
    virtual void visit(const OperatorNameId *name);
    virtual void visit(const ConversionNameId *name);
    virtual void visit(const QualifiedNameId *name);
    virtual void visit(const SelectorNameId *name);

protected:
    typedef std::pair <const Name *, Subst *> NameSubstPair;
    std::map<NameSubstPair, const Name *> _cache;

    Clone *_clone;
    Control *_control;
    Subst *_subst;
    const Name *_name;
};

class CPLUSPLUS_EXPORT CloneSymbol: protected SymbolVisitor
{
public:
    CloneSymbol(Clone *clone);

    Symbol *operator()(Symbol *symbol, Subst *subst) { return cloneSymbol(symbol, subst); }
    Symbol *cloneSymbol(Symbol *symbol, Subst *subst);

protected:
    virtual bool visit(UsingNamespaceDirective *symbol);
    virtual bool visit(UsingDeclaration *symbol);
    virtual bool visit(NamespaceAlias *symbol);
    virtual bool visit(Declaration *symbol);
    virtual bool visit(Argument *symbol);
    virtual bool visit(TypenameArgument *symbol);
    virtual bool visit(BaseClass *symbol);
    virtual bool visit(Enum *symbol);
    virtual bool visit(Function *symbol);
    virtual bool visit(Namespace *symbol);
    virtual bool visit(Template *symbol);
    virtual bool visit(Class *symbol);
    virtual bool visit(Block *symbol);
    virtual bool visit(ForwardClassDeclaration *symbol);

    // Qt
    virtual bool visit(QtPropertyDeclaration *symbol);
    virtual bool visit(QtEnum *symbol);

    // Objective-C
    virtual bool visit(ObjCBaseClass *symbol);
    virtual bool visit(ObjCBaseProtocol *symbol);
    virtual bool visit(ObjCClass *symbol);
    virtual bool visit(ObjCForwardClassDeclaration *symbol);
    virtual bool visit(ObjCProtocol *symbol);
    virtual bool visit(ObjCForwardProtocolDeclaration *symbol);
    virtual bool visit(ObjCMethod *symbol);
    virtual bool visit(ObjCPropertyDeclaration *symbol);

protected:
    typedef std::pair <Symbol *, Subst *> SymbolSubstPair;
    std::map<SymbolSubstPair, Symbol *> _cache;

    Clone *_clone;
    Control *_control;
    Subst *_subst;
    Symbol *_symbol;
};

class CPLUSPLUS_EXPORT Clone
{
    Control *_control;

public:
    Clone(Control *control);

    Control *control() const { return _control; }
    const StringLiteral *stringLiteral(const StringLiteral *literal);
    const NumericLiteral *numericLiteral(const NumericLiteral *literal);
    const Identifier *identifier(const Identifier *id);

    FullySpecifiedType type(const FullySpecifiedType &type, Subst *subst);
    const Name *name(const Name *name, Subst *subst);
    Symbol *symbol(Symbol *symbol, Subst *subst);

    Symbol *instantiate(Template *templ,
                        const FullySpecifiedType *const args, unsigned argc,
                        Subst *subst = 0);

private:
    CloneType _type;
    CloneName _name;
    CloneSymbol _symbol;
};

} // end of namespace CPlusPlus

#endif // CPLUSPLUS_TEMPLATES_H
