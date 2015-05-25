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

#ifndef CPLUSPLUS_BIND_H
#define CPLUSPLUS_BIND_H

#include "ASTVisitor.h"
#include "FullySpecifiedType.h"
#include "Names.h"

namespace CPlusPlus {

class CPLUSPLUS_EXPORT Bind: protected ASTVisitor
{
public:
    Bind(TranslationUnit *unit);

    void operator()(TranslationUnitAST *ast, Namespace *globalNamespace);
    void operator()(DeclarationAST *ast, Scope *scope);
    void operator()(StatementAST *ast, Scope *scope);
    FullySpecifiedType operator()(ExpressionAST *ast, Scope *scope);
    FullySpecifiedType operator()(NewTypeIdAST *ast, Scope *scope);

    bool skipFunctionBodies() const;
    void setSkipFunctionBodies(bool skipFunctionBodies);

protected:
    using ASTVisitor::translationUnit;

    unsigned location(DeclaratorAST *ast, unsigned defaultLocation) const;
    unsigned location(CoreDeclaratorAST *ast, unsigned defaultLocation) const;
    unsigned location(NameAST *name, unsigned defaultLocation) const;

    static int visibilityForAccessSpecifier(int tokenKind);
    static int visibilityForClassKey(int tokenKind);
    static int visibilityForObjCAccessSpecifier(int tokenKind);
    static bool isObjCClassMethod(int tokenKind);

    void setDeclSpecifiers(Symbol *symbol, const FullySpecifiedType &declSpecifiers);

    typedef FullySpecifiedType ExpressionTy;
    ExpressionTy expression(ExpressionAST *ast);

    const Name *name(NameAST *ast);

    void statement(StatementAST *ast);
    void declaration(DeclarationAST *ast);

    FullySpecifiedType specifier(SpecifierAST *ast, const FullySpecifiedType &init);
    FullySpecifiedType ptrOperator(PtrOperatorAST *ast, const FullySpecifiedType &init);
    FullySpecifiedType coreDeclarator(CoreDeclaratorAST *ast, const FullySpecifiedType &init);
    FullySpecifiedType postfixDeclarator(PostfixDeclaratorAST *ast, const FullySpecifiedType &init);

    Scope *switchScope(Scope *scope);
    int switchVisibility(int visibility);
    int switchMethodKey(int methodKey);
    int switchObjCVisibility(int visibility);

    unsigned calculateScopeStart(ObjCClassDeclarationAST *ast) const;
    unsigned calculateScopeStart(ObjCProtocolDeclarationAST *ast) const;

    const Name *objCSelectorArgument(ObjCSelectorArgumentAST *ast, bool *hasArg);
    void attribute(GnuAttributeAST *ast);
    FullySpecifiedType declarator(DeclaratorAST *ast, const FullySpecifiedType &init, DeclaratorIdAST **declaratorId);
    void qtInterfaceName(QtInterfaceNameAST *ast);
    void baseSpecifier(BaseSpecifierAST *ast, unsigned colon_token, Class *klass);
    void ctorInitializer(CtorInitializerAST *ast, Function *fun);
    void enumerator(EnumeratorAST *ast, Enum *symbol);
    FullySpecifiedType exceptionSpecification(ExceptionSpecificationAST *ast, const FullySpecifiedType &init);
    void memInitializer(MemInitializerAST *ast, Function *fun);
    const Name *nestedNameSpecifier(NestedNameSpecifierAST *ast);
    void newPlacement(ExpressionListParenAST *ast);
    FullySpecifiedType newArrayDeclarator(NewArrayDeclaratorAST *ast, const FullySpecifiedType &init);
    FullySpecifiedType newTypeId(NewTypeIdAST *ast);
    OperatorNameId::Kind cppOperator(OperatorAST *ast);
    void parameterDeclarationClause(ParameterDeclarationClauseAST *ast, unsigned lparen_token, Function *fun);
    void translationUnit(TranslationUnitAST *ast);
    void objCProtocolRefs(ObjCProtocolRefsAST *ast, Symbol *objcClassOrProtocol);
    void objCMessageArgument(ObjCMessageArgumentAST *ast);
    FullySpecifiedType objCTypeName(ObjCTypeNameAST *ast);
    void objCInstanceVariablesDeclaration(ObjCInstanceVariablesDeclarationAST *ast, ObjCClass *klass);
    void objCPropertyAttribute(ObjCPropertyAttributeAST *ast);
    void objCMessageArgumentDeclaration(ObjCMessageArgumentDeclarationAST *ast, ObjCMethod *method);
    ObjCMethod *objCMethodPrototype(ObjCMethodPrototypeAST *ast);
    void objCSynthesizedProperty(ObjCSynthesizedPropertyAST *ast);
    void lambdaIntroducer(LambdaIntroducerAST *ast);
    void lambdaCapture(LambdaCaptureAST *ast);
    void capture(CaptureAST *ast);
    Function *lambdaDeclarator(LambdaDeclaratorAST *ast);
    FullySpecifiedType trailingReturnType(TrailingReturnTypeAST *ast, const FullySpecifiedType &init);
    const StringLiteral *asStringLiteral(unsigned firstToken, unsigned lastToken);

    virtual bool preVisit(AST *);
    virtual void postVisit(AST *);

    // AST
    virtual bool visit(ObjCSelectorArgumentAST *ast);
    virtual bool visit(GnuAttributeAST *ast);
    virtual bool visit(DeclaratorAST *ast);
    virtual bool visit(QtPropertyDeclarationItemAST *ast);
    virtual bool visit(QtInterfaceNameAST *ast);
    virtual bool visit(BaseSpecifierAST *ast);
    virtual bool visit(CtorInitializerAST *ast);
    virtual bool visit(EnumeratorAST *ast);
    virtual bool visit(DynamicExceptionSpecificationAST *ast);
    virtual bool visit(MemInitializerAST *ast);
    virtual bool visit(NestedNameSpecifierAST *ast);
    virtual bool visit(NewArrayDeclaratorAST *ast);
    virtual bool visit(NewTypeIdAST *ast);
    virtual bool visit(OperatorAST *ast);
    virtual bool visit(ParameterDeclarationClauseAST *ast);
    virtual bool visit(TranslationUnitAST *ast);
    virtual bool visit(ObjCProtocolRefsAST *ast);
    virtual bool visit(ObjCMessageArgumentAST *ast);
    virtual bool visit(ObjCTypeNameAST *ast);
    virtual bool visit(ObjCInstanceVariablesDeclarationAST *ast);
    virtual bool visit(ObjCPropertyAttributeAST *ast);
    virtual bool visit(ObjCMessageArgumentDeclarationAST *ast);
    virtual bool visit(ObjCMethodPrototypeAST *ast);
    virtual bool visit(ObjCSynthesizedPropertyAST *ast);
    virtual bool visit(LambdaIntroducerAST *ast);
    virtual bool visit(LambdaCaptureAST *ast);
    virtual bool visit(CaptureAST *ast);
    virtual bool visit(LambdaDeclaratorAST *ast);
    virtual bool visit(TrailingReturnTypeAST *ast);

    // StatementAST
    virtual bool visit(QtMemberDeclarationAST *ast);
    virtual bool visit(CaseStatementAST *ast);
    virtual bool visit(CompoundStatementAST *ast);
    virtual bool visit(DeclarationStatementAST *ast);
    virtual bool visit(DoStatementAST *ast);
    virtual bool visit(ExpressionOrDeclarationStatementAST *ast);
    virtual bool visit(ExpressionStatementAST *ast);
    virtual bool visit(ForeachStatementAST *ast);
    virtual bool visit(RangeBasedForStatementAST *ast);
    virtual bool visit(ForStatementAST *ast);
    virtual bool visit(IfStatementAST *ast);
    virtual bool visit(LabeledStatementAST *ast);
    virtual bool visit(BreakStatementAST *ast);
    virtual bool visit(ContinueStatementAST *ast);
    virtual bool visit(GotoStatementAST *ast);
    virtual bool visit(ReturnStatementAST *ast);
    virtual bool visit(SwitchStatementAST *ast);
    virtual bool visit(TryBlockStatementAST *ast);
    virtual bool visit(CatchClauseAST *ast);
    virtual bool visit(WhileStatementAST *ast);
    virtual bool visit(ObjCFastEnumerationAST *ast);
    virtual bool visit(ObjCSynchronizedStatementAST *ast);

    // ExpressionAST
    virtual bool visit(IdExpressionAST *ast);
    virtual bool visit(CompoundExpressionAST *ast);
    virtual bool visit(CompoundLiteralAST *ast);
    virtual bool visit(QtMethodAST *ast);
    virtual bool visit(BinaryExpressionAST *ast);
    virtual bool visit(CastExpressionAST *ast);
    virtual bool visit(ConditionAST *ast);
    virtual bool visit(ConditionalExpressionAST *ast);
    virtual bool visit(CppCastExpressionAST *ast);
    virtual bool visit(DeleteExpressionAST *ast);
    virtual bool visit(ArrayInitializerAST *ast);
    virtual bool visit(NewExpressionAST *ast);
    virtual bool visit(TypeidExpressionAST *ast);
    virtual bool visit(TypenameCallExpressionAST *ast);
    virtual bool visit(TypeConstructorCallAST *ast);
    virtual bool visit(SizeofExpressionAST *ast);
    virtual bool visit(PointerLiteralAST *ast);
    virtual bool visit(NumericLiteralAST *ast);
    virtual bool visit(BoolLiteralAST *ast);
    virtual bool visit(ThisExpressionAST *ast);
    virtual bool visit(NestedExpressionAST *ast);
    virtual bool visit(StringLiteralAST *ast);
    virtual bool visit(ThrowExpressionAST *ast);
    virtual bool visit(TypeIdAST *ast);
    virtual bool visit(UnaryExpressionAST *ast);
    virtual bool visit(ObjCMessageExpressionAST *ast);
    virtual bool visit(ObjCProtocolExpressionAST *ast);
    virtual bool visit(ObjCEncodeExpressionAST *ast);
    virtual bool visit(ObjCSelectorExpressionAST *ast);
    virtual bool visit(LambdaExpressionAST *ast);
    virtual bool visit(BracedInitializerAST *ast);
    virtual bool visit(ExpressionListParenAST *ast);

    // DeclarationAST
    virtual bool visit(SimpleDeclarationAST *ast);
    virtual bool visit(EmptyDeclarationAST *ast);
    virtual bool visit(AccessDeclarationAST *ast);
    virtual bool visit(QtObjectTagAST *ast);
    virtual bool visit(QtPrivateSlotAST *ast);
    virtual bool visit(QtPropertyDeclarationAST *ast);
    virtual bool visit(QtEnumDeclarationAST *ast);
    virtual bool visit(QtFlagsDeclarationAST *ast);
    virtual bool visit(QtInterfacesDeclarationAST *ast);
    virtual bool visit(AliasDeclarationAST *ast);
    virtual bool visit(AsmDefinitionAST *ast);
    virtual bool visit(ExceptionDeclarationAST *ast);
    virtual bool visit(FunctionDefinitionAST *ast);
    virtual bool visit(LinkageBodyAST *ast);
    virtual bool visit(LinkageSpecificationAST *ast);
    virtual bool visit(NamespaceAST *ast);
    virtual bool visit(NamespaceAliasDefinitionAST *ast);
    virtual bool visit(ParameterDeclarationAST *ast);
    virtual bool visit(TemplateDeclarationAST *ast);
    virtual bool visit(TypenameTypeParameterAST *ast);
    virtual bool visit(TemplateTypeParameterAST *ast);
    virtual bool visit(UsingAST *ast);
    virtual bool visit(UsingDirectiveAST *ast);
    virtual bool visit(ObjCClassForwardDeclarationAST *ast);
    virtual bool visit(ObjCClassDeclarationAST *ast);
    virtual bool visit(ObjCProtocolForwardDeclarationAST *ast);
    virtual bool visit(ObjCProtocolDeclarationAST *ast);
    virtual bool visit(ObjCVisibilityDeclarationAST *ast);
    virtual bool visit(ObjCPropertyDeclarationAST *ast);
    virtual bool visit(ObjCMethodDeclarationAST *ast);
    virtual bool visit(ObjCSynthesizedPropertiesDeclarationAST *ast);
    virtual bool visit(ObjCDynamicPropertiesDeclarationAST *ast);

    // NameAST
    virtual bool visit(ObjCSelectorAST *ast);
    virtual bool visit(QualifiedNameAST *ast);
    virtual bool visit(OperatorFunctionIdAST *ast);
    virtual bool visit(ConversionFunctionIdAST *ast);
    virtual bool visit(AnonymousNameAST *ast);
    virtual bool visit(SimpleNameAST *ast);
    virtual bool visit(DestructorNameAST *ast);
    virtual bool visit(TemplateIdAST *ast);

    // SpecifierAST
    virtual bool visit(SimpleSpecifierAST *ast);
    virtual bool visit(AlignmentSpecifierAST *ast);
    virtual bool visit(GnuAttributeSpecifierAST *ast);
    virtual bool visit(TypeofSpecifierAST *ast);
    virtual bool visit(DecltypeSpecifierAST *ast);
    virtual bool visit(ClassSpecifierAST *ast);
    virtual bool visit(NamedTypeSpecifierAST *ast);
    virtual bool visit(ElaboratedTypeSpecifierAST *ast);
    virtual bool visit(EnumSpecifierAST *ast);

    // PtrOperatorAST
    virtual bool visit(PointerToMemberAST *ast);
    virtual bool visit(PointerAST *ast);
    virtual bool visit(ReferenceAST *ast);

    // PostfixAST
    virtual bool visit(CallAST *ast);
    virtual bool visit(ArrayAccessAST *ast);
    virtual bool visit(PostIncrDecrAST *ast);
    virtual bool visit(MemberAccessAST *ast);

    // CoreDeclaratorAST
    virtual bool visit(DeclaratorIdAST *ast);
    virtual bool visit(NestedDeclaratorAST *ast);

    // PostfixDeclaratorAST
    virtual bool visit(FunctionDeclaratorAST *ast);
    virtual bool visit(ArrayDeclaratorAST *ast);

private:
    static const int kMaxDepth;

    void ensureValidClassName(const Name **name, unsigned sourceLocation);

    Scope *_scope;
    ExpressionTy _expression;
    const Name *_name;
    FullySpecifiedType _type;
    DeclaratorIdAST **_declaratorId;
    int _visibility;
    int _objcVisibility;
    int _methodKey;
    bool _skipFunctionBodies;
    int _depth;
};

} // namespace CPlusPlus

#endif // CPLUSPLUS_BIND_H
