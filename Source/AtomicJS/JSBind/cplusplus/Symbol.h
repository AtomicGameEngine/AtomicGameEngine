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

#ifndef CPLUSPLUS_SYMBOL_H
#define CPLUSPLUS_SYMBOL_H

#include "CPlusPlusForwardDeclarations.h"


namespace CPlusPlus {

class CPLUSPLUS_EXPORT Symbol
{
    Symbol(const Symbol &other);
    void operator =(const Symbol &other);

public:
    /// Storage class specifier
    enum Storage {
        NoStorage = 0,
        Friend,
        Auto,
        Register,
        Static,
        Extern,
        Mutable,
        Typedef
    };

    /// Access specifier.
    enum Visibility {
        Public,
        Protected,
        Private,
        Package
    };

public:
    /// Constructs a Symbol with the given source location, name and translation unit.
    Symbol(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name);
    Symbol(Clone *clone, Subst *subst, Symbol *original);

    /// Destroy this Symbol.
    virtual ~Symbol();

    /// Returns this Symbol's source location.
    unsigned sourceLocation() const;

    /// \returns this Symbol's line number. The line number is 1-based.
    unsigned line() const;

    /// \returns this Symbol's column number. The column number is 1-based.
    unsigned column() const;

    /// Returns this Symbol's file name.
    const StringLiteral *fileId() const;

    /// Returns this Symbol's file name.
    const char *fileName() const;

    /// Returns this Symbol's file name length.
    unsigned fileNameLength() const;

    /// Returns this Symbol's name.
    const Name *name() const;

    /// Sets this Symbol's name.
    void setName(const Name *name); // ### dangerous

    /// Returns this Symbol's (optional) identifier
    const Identifier *identifier() const;

    /// Returns this Symbol's storage class specifier.
    int storage() const;

    /// Sets this Symbol's storage class specifier.
    void setStorage(int storage);

    /// Returns this Symbol's visibility.
    int visibility() const;

    /// Sets this Symbol's visibility.
    void setVisibility(int visibility);

    /// Returns the next chained Symbol.
    Symbol *next() const;

    /// Returns true if this Symbol has friend storage specifier.
    bool isFriend() const;

    /// Returns true if this Symbol has register storage specifier.
    bool isRegister() const;

    /// Returns true if this Symbol has static storage specifier.
    bool isStatic() const;

    /// Returns true if this Symbol has extern storage specifier.
    bool isExtern() const;

    /// Returns true if this Symbol has mutable storage specifier.
    bool isMutable() const;

    /// Returns true if this Symbol has typedef storage specifier.
    bool isTypedef() const;

    /// Returns true if this Symbol's visibility is public.
    bool isPublic() const;

    /// Returns true if this Symbol's visibility is protected.
    bool isProtected() const;

    /// Returns true if this Symbol's visibility is private.
    bool isPrivate() const;

    /// Returns true if this Symbol is a Scope.
    bool isScope() const;

    /// Returns true if this Symbol is an Enum.
    bool isEnum() const;

    /// Returns true if this Symbol is an Function.
    bool isFunction() const;

    /// Returns true if this Symbol is a Namespace.
    bool isNamespace() const;

    /// Returns true if this Symbol is a Template.
    bool isTemplate() const;

    /// Returns true if this Symbol is a Class.
    bool isClass() const;

    /// Returns true if this Symbol is a Block.
    bool isBlock() const;

    /// Returns true if this Symbol is a UsingNamespaceDirective.
    bool isUsingNamespaceDirective() const;

    /// Returns true if this Symbol is a UsingDeclaration.
    bool isUsingDeclaration() const;

    /// Returns true if this Symbol is a Declaration.
    bool isDeclaration() const;

    /// Returns true if this Symbol is an Argument.
    bool isArgument() const;

    /// Returns true if this Symbol is a Typename argument.
    bool isTypenameArgument() const;

    /// Returns true if this Symbol is a BaseClass.
    bool isBaseClass() const;

    /// Returns true if this Symbol is a ForwardClassDeclaration.
    bool isForwardClassDeclaration() const;

    /// Returns true if this Symbol is a QtPropertyDeclaration.
    bool isQtPropertyDeclaration() const;

    /// Returns true if this Symbol is a QtEnum.
    bool isQtEnum() const;

    bool isObjCBaseClass() const;
    bool isObjCBaseProtocol() const;

    /// Returns true if this Symbol is an Objective-C Class declaration.
    bool isObjCClass() const;

    /// Returns true if this Symbol is an Objective-C Class forward declaration.
    bool isObjCForwardClassDeclaration() const;

    /// Returns true if this Symbol is an Objective-C Protocol declaration.
    bool isObjCProtocol() const;

    /// Returns true if this Symbol is an Objective-C Protocol forward declaration.
    bool isObjCForwardProtocolDeclaration() const;

    /// Returns true if this Symbol is an Objective-C method declaration.
    bool isObjCMethod() const;

    /// Returns true if this Symbol is an Objective-C @property declaration.
    bool isObjCPropertyDeclaration() const;

    virtual const Scope *asScope() const { return 0; }
    virtual const Enum *asEnum() const { return 0; }
    virtual const Function *asFunction() const { return 0; }
    virtual const Namespace *asNamespace() const { return 0; }
    virtual const Template *asTemplate() const { return 0; }
    virtual const NamespaceAlias *asNamespaceAlias() const { return 0; }
    virtual const Class *asClass() const { return 0; }
    virtual const Block *asBlock() const { return 0; }
    virtual const UsingNamespaceDirective *asUsingNamespaceDirective() const { return 0; }
    virtual const UsingDeclaration *asUsingDeclaration() const { return 0; }
    virtual const Declaration *asDeclaration() const { return 0; }
    virtual const Argument *asArgument() const { return 0; }
    virtual const TypenameArgument *asTypenameArgument() const { return 0; }
    virtual const BaseClass *asBaseClass() const { return 0; }
    virtual const ForwardClassDeclaration *asForwardClassDeclaration() const { return 0; }
    virtual const QtPropertyDeclaration *asQtPropertyDeclaration() const { return 0; }
    virtual const QtEnum *asQtEnum() const { return 0; }
    virtual const ObjCBaseClass *asObjCBaseClass() const { return 0; }
    virtual const ObjCBaseProtocol *asObjCBaseProtocol() const { return 0; }
    virtual const ObjCClass *asObjCClass() const { return 0; }
    virtual const ObjCForwardClassDeclaration *asObjCForwardClassDeclaration() const { return 0; }
    virtual const ObjCProtocol *asObjCProtocol() const { return 0; }
    virtual const ObjCForwardProtocolDeclaration *asObjCForwardProtocolDeclaration() const { return 0; }
    virtual const ObjCMethod *asObjCMethod() const { return 0; }
    virtual const ObjCPropertyDeclaration *asObjCPropertyDeclaration() const { return 0; }

    virtual Scope *asScope() { return 0; }
    virtual Enum *asEnum() { return 0; }
    virtual Function *asFunction() { return 0; }
    virtual Namespace *asNamespace() { return 0; }
    virtual Template *asTemplate() { return 0; }
    virtual NamespaceAlias *asNamespaceAlias() { return 0; }
    virtual Class *asClass() { return 0; }
    virtual Block *asBlock() { return 0; }
    virtual UsingNamespaceDirective *asUsingNamespaceDirective() { return 0; }
    virtual UsingDeclaration *asUsingDeclaration() { return 0; }
    virtual Declaration *asDeclaration() { return 0; }
    virtual Argument *asArgument() { return 0; }
    virtual TypenameArgument *asTypenameArgument() { return 0; }
    virtual BaseClass *asBaseClass() { return 0; }
    virtual ForwardClassDeclaration *asForwardClassDeclaration() { return 0; }
    virtual QtPropertyDeclaration *asQtPropertyDeclaration() { return 0; }
    virtual QtEnum *asQtEnum() { return 0; }
    virtual ObjCBaseClass *asObjCBaseClass() { return 0; }
    virtual ObjCBaseProtocol *asObjCBaseProtocol() { return 0; }
    virtual ObjCClass *asObjCClass() { return 0; }
    virtual ObjCForwardClassDeclaration *asObjCForwardClassDeclaration() { return 0; }
    virtual ObjCProtocol *asObjCProtocol() { return 0; }
    virtual ObjCForwardProtocolDeclaration *asObjCForwardProtocolDeclaration() { return 0; }
    virtual ObjCMethod *asObjCMethod() { return 0; }
    virtual ObjCPropertyDeclaration *asObjCPropertyDeclaration() { return 0; }

    /// Returns this Symbol's type.
    virtual FullySpecifiedType type() const = 0;

    /// Returns this Symbol's hash value.
    unsigned hashCode() const;

    /// Returns this Symbol's index.
    unsigned index() const;

    const Name *unqualifiedName() const;

    bool isGenerated() const;

    bool isDeprecated() const;
    void setDeprecated(bool isDeprecated);

    bool isUnavailable() const;
    void setUnavailable(bool isUnavailable);

    /// Returns this Symbol's eclosing scope.
    Scope *enclosingScope() const;

    /// Returns the eclosing namespace scope.
    Namespace *enclosingNamespace() const;

    /// Returns the eclosing template scope.
    Template *enclosingTemplate() const;

    /// Returns the enclosing class scope.
    Class *enclosingClass() const;

    /// Returns the enclosing enum scope.
    Enum *enclosingEnum() const;

    /// Returns the enclosing prototype scope.
    Function *enclosingFunction() const;

    /// Returns the enclosing Block scope.
    Block *enclosingBlock() const;

    void setEnclosingScope(Scope *enclosingScope); // ### make me private
    void resetEnclosingScope(); // ### make me private
    void setSourceLocation(unsigned sourceLocation, TranslationUnit *translationUnit); // ### make me private

    void visitSymbol(SymbolVisitor *visitor);
    static void visitSymbol(Symbol *symbol, SymbolVisitor *visitor);

    virtual void copy(Symbol *other);

protected:
    virtual void visitSymbol0(SymbolVisitor *visitor) = 0;

private:
    const Name *_name;
    Scope *_enclosingScope;
    Symbol *_next;
    const StringLiteral *_fileId;
    unsigned _sourceLocation;
    unsigned _hashCode;
    int _storage;
    int _visibility;
    unsigned _index;
    unsigned _line;
    unsigned _column;

    bool _isGenerated: 1;
    bool _isDeprecated: 1;
    bool _isUnavailable: 1;

    class HashCode;

    friend class SymbolTable;
};

} // namespace CPlusPlus


#endif // CPLUSPLUS_SYMBOL_H
