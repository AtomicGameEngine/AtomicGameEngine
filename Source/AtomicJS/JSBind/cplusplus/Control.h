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

#ifndef CPLUSPLUS_CONTROL_H
#define CPLUSPLUS_CONTROL_H

#include "CPlusPlusForwardDeclarations.h"
#include "ASTfwd.h"
#include "Names.h"

namespace CPlusPlus {

class CPLUSPLUS_EXPORT TopLevelDeclarationProcessor
{
public:
    virtual ~TopLevelDeclarationProcessor() {}
    virtual bool processDeclaration(DeclarationAST *ast) = 0;
};

class CPLUSPLUS_EXPORT Control
{
public:
    Control();
    ~Control();

    TranslationUnit *translationUnit() const;
    TranslationUnit *switchTranslationUnit(TranslationUnit *unit);

    TopLevelDeclarationProcessor *topLevelDeclarationProcessor() const;
    void setTopLevelDeclarationProcessor(TopLevelDeclarationProcessor *processor);

    DiagnosticClient *diagnosticClient() const;
    void setDiagnosticClient(DiagnosticClient *diagnosticClient);

    /// Returns the canonical anonymous name id
    const AnonymousNameId *anonymousNameId(unsigned classTokenIndex);

    /// Returns the canonical template name id.
    const TemplateNameId *templateNameId(const Identifier *id,
                                         bool isSpecialization,
                                         const FullySpecifiedType *const args = 0,
                                         unsigned argc = 0);

    /// Returns the canonical destructor name id.
    const DestructorNameId *destructorNameId(const Name *name);

    /// Returns the canonical operator name id.
    const OperatorNameId *operatorNameId(OperatorNameId::Kind operatorId);

    /// Returns the canonical conversion name id.
    const ConversionNameId *conversionNameId(const FullySpecifiedType &type);

    /// Returns the canonical qualified name id.
    const QualifiedNameId *qualifiedNameId(const Name *base, const Name *name);

    const SelectorNameId *selectorNameId(const Name *const *names,
                                         unsigned nameCount,
                                         bool hasArguments);

    /// Returns a Type object of type VoidType.
    VoidType *voidType();

    /// Returns a Type object of type IntegerType.
    IntegerType *integerType(int integerId);

    /// Returns a Type object of type FloatType.
    FloatType *floatType(int floatId);

    /// Returns a Type object of type PointertoMemberType.
    PointerToMemberType *pointerToMemberType(const Name *memberName,
                                             const FullySpecifiedType &elementType);

    /// Returns a Type object of type PointerType.
    PointerType *pointerType(const FullySpecifiedType &elementType);

    /// Returns a Type object of type ReferenceType.
    ReferenceType *referenceType(const FullySpecifiedType &elementType, bool rvalueRef);

    /// Retruns a Type object of type ArrayType.
    ArrayType *arrayType(const FullySpecifiedType &elementType, unsigned size = 0);

    /// Returns a Type object of type NamedType.
    NamedType *namedType(const Name *name);

    /// Creates a new Declaration symbol.
    Declaration *newDeclaration(unsigned sourceLocation, const Name *name);

    /// Creates a new EnumeratorDeclaration symbol.
    EnumeratorDeclaration *newEnumeratorDeclaration(unsigned sourceLocation, const Name *name);

    /// Creates a new Argument symbol.
    Argument *newArgument(unsigned sourceLocation, const Name *name = 0);

    /// Creates a new Argument symbol.
    TypenameArgument *newTypenameArgument(unsigned sourceLocation, const Name *name = 0);

    /// Creates a new Function symbol.
    Function *newFunction(unsigned sourceLocation, const Name *name = 0);

    /// Creates a new Namespace symbol.
    Namespace *newNamespace(unsigned sourceLocation, const Name *name = 0);

    /// Creates a new Template symbol.
    Template *newTemplate(unsigned sourceLocation, const Name *name = 0);

    /// Creates a new Namespace symbol.
    NamespaceAlias *newNamespaceAlias(unsigned sourceLocation, const Name *name = 0);

    /// Creates a new BaseClass symbol.
    BaseClass *newBaseClass(unsigned sourceLocation, const Name *name = 0);

    /// Creates a new Class symbol.
    Class *newClass(unsigned sourceLocation, const Name *name = 0);

    /// Creates a new Enum symbol.
    Enum *newEnum(unsigned sourceLocation, const Name *name = 0);

    /// Creates a new Block symbol.
    Block *newBlock(unsigned sourceLocation);

    /// Creates a new UsingNamespaceDirective symbol.
    UsingNamespaceDirective *newUsingNamespaceDirective(unsigned sourceLocation, const Name *name = 0);

    /// Creates a new UsingDeclaration symbol.
    UsingDeclaration *newUsingDeclaration(unsigned sourceLocation, const Name *name = 0);

    /// Creates a new ForwardClassDeclaration symbol.
    ForwardClassDeclaration *newForwardClassDeclaration(unsigned sourceLocation, const Name *name = 0);

    /// Creates a new QtPropertyDeclaration symbol.
    QtPropertyDeclaration *newQtPropertyDeclaration(unsigned sourceLocation, const Name *name = 0);

    /// Creates a new QtEnum symbol.
    QtEnum *newQtEnum(unsigned sourceLocation, const Name *name = 0);

    ObjCBaseClass *newObjCBaseClass(unsigned sourceLocation, const Name *name);
    ObjCBaseProtocol *newObjCBaseProtocol(unsigned sourceLocation, const Name *name);

    /// Creates a new Objective-C class symbol.
    ObjCClass *newObjCClass(unsigned sourceLocation, const Name *name = 0);

    /// Creates a new Objective-C class forward declaration symbol.
    ObjCForwardClassDeclaration *newObjCForwardClassDeclaration(unsigned sourceLocation, const Name *name = 0);

    /// Creates a new Objective-C protocol symbol.
    ObjCProtocol *newObjCProtocol(unsigned sourceLocation, const Name *name = 0);

    /// Creates a new Objective-C protocol forward declaration symbol.
    ObjCForwardProtocolDeclaration *newObjCForwardProtocolDeclaration(unsigned sourceLocation, const Name *name = 0);

    /// Creates a new Objective-C method symbol.
    ObjCMethod *newObjCMethod(unsigned sourceLocation, const Name *name = 0);

    /// Creates a new Objective-C @property declaration symbol.
    ObjCPropertyDeclaration *newObjCPropertyDeclaration(unsigned sourceLocation, const Name *name);

    const Identifier *deprecatedId() const;
    const Identifier *unavailableId() const;
    // Objective-C specific context keywords.
    const Identifier *objcGetterId() const;
    const Identifier *objcSetterId() const;
    const Identifier *objcReadwriteId() const;
    const Identifier *objcReadonlyId() const;
    const Identifier *objcAssignId() const;
    const Identifier *objcRetainId() const;
    const Identifier *objcCopyId() const;
    const Identifier *objcNonatomicId() const;
    // C++11 context keywords
    const Identifier *cpp11Override() const;
    const Identifier *cpp11Final() const;

    const OperatorNameId *findOperatorNameId(OperatorNameId::Kind operatorId) const;

    const Identifier *findIdentifier(const char *chars, unsigned size) const;
    const Identifier *identifier(const char *chars, unsigned size);
    const Identifier *identifier(const char *chars);

    typedef const Identifier *const *IdentifierIterator;
    typedef const StringLiteral *const *StringLiteralIterator;
    typedef const NumericLiteral *const *NumericLiteralIterator;

    IdentifierIterator firstIdentifier() const;
    IdentifierIterator lastIdentifier() const;

    StringLiteralIterator firstStringLiteral() const;
    StringLiteralIterator lastStringLiteral() const;

    NumericLiteralIterator firstNumericLiteral() const;
    NumericLiteralIterator lastNumericLiteral() const;

    const StringLiteral *stringLiteral(const char *chars, unsigned size);
    const StringLiteral *stringLiteral(const char *chars);

    const NumericLiteral *numericLiteral(const char *chars, unsigned size);
    const NumericLiteral *numericLiteral(const char *chars);

    Symbol **firstSymbol() const;
    Symbol **lastSymbol() const;
    unsigned symbolCount() const;

    bool hasSymbol(Symbol *symbol) const;
    void addSymbol(Symbol *symbol);

    void squeeze();

private:
    class Data;
    friend class Data;
    Data *d;
};

} // namespace CPlusPlus


#endif // CPLUSPLUS_CONTROL_H
