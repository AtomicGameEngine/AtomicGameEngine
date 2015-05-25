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

#ifndef CPLUSPLUS_ASTVISITOR_H
#define CPLUSPLUS_ASTVISITOR_H

#include "CPlusPlusForwardDeclarations.h"
#include "ASTfwd.h"

namespace CPlusPlus {

class CPLUSPLUS_EXPORT ASTVisitor
{
    ASTVisitor(const ASTVisitor &other);
    void operator =(const ASTVisitor &other);

public:
    ASTVisitor(TranslationUnit *unit);
    virtual ~ASTVisitor();

    TranslationUnit *translationUnit() const;
    void setTranslationUnit(TranslationUnit *translationUnit);

    Control *control() const;
    unsigned tokenCount() const;
    const Token &tokenAt(unsigned index) const;
    int tokenKind(unsigned index) const;
    const char *spell(unsigned index) const;
    const Identifier *identifier(unsigned index) const;
    const Literal *literal(unsigned index) const;
    const NumericLiteral *numericLiteral(unsigned index) const;
    const StringLiteral *stringLiteral(unsigned index) const;

    void getPosition(unsigned offset,
                     unsigned *line,
                     unsigned *column = 0,
                     const StringLiteral **fileName = 0) const;

    void getTokenPosition(unsigned index,
                          unsigned *line,
                          unsigned *column = 0,
                          const StringLiteral **fileName = 0) const;

    void getTokenStartPosition(unsigned index, unsigned *line, unsigned *column) const;
    void getTokenEndPosition(unsigned index, unsigned *line, unsigned *column) const;

    void accept(AST *ast);

    template <typename Tptr>
    void accept(List<Tptr> *it)
    {
        for (; it; it = it->next)
            accept(it->value);
    }

    virtual bool preVisit(AST *) { return true; }
    virtual void postVisit(AST *) {}

    virtual bool visit(AccessDeclarationAST *) { return true; }
    virtual bool visit(AliasDeclarationAST *) { return true; }
    virtual bool visit(AlignmentSpecifierAST *) { return true; }
    virtual bool visit(AlignofExpressionAST *) { return true; }
    virtual bool visit(AnonymousNameAST *) { return true; }
    virtual bool visit(ArrayAccessAST *) { return true; }
    virtual bool visit(ArrayDeclaratorAST *) { return true; }
    virtual bool visit(ArrayInitializerAST *) { return true; }
    virtual bool visit(AsmDefinitionAST *) { return true; }
    virtual bool visit(BaseSpecifierAST *) { return true; }
    virtual bool visit(BinaryExpressionAST *) { return true; }
    virtual bool visit(BoolLiteralAST *) { return true; }
    virtual bool visit(BracedInitializerAST *) { return true; }
    virtual bool visit(BracketDesignatorAST *) { return true; }
    virtual bool visit(BreakStatementAST *) { return true; }
    virtual bool visit(CallAST *) { return true; }
    virtual bool visit(CaptureAST *) { return true; }
    virtual bool visit(CaseStatementAST *) { return true; }
    virtual bool visit(CastExpressionAST *) { return true; }
    virtual bool visit(CatchClauseAST *) { return true; }
    virtual bool visit(ClassSpecifierAST *) { return true; }
    virtual bool visit(CompoundExpressionAST *) { return true; }
    virtual bool visit(CompoundLiteralAST *) { return true; }
    virtual bool visit(CompoundStatementAST *) { return true; }
    virtual bool visit(ConditionAST *) { return true; }
    virtual bool visit(ConditionalExpressionAST *) { return true; }
    virtual bool visit(ContinueStatementAST *) { return true; }
    virtual bool visit(ConversionFunctionIdAST *) { return true; }
    virtual bool visit(CppCastExpressionAST *) { return true; }
    virtual bool visit(CtorInitializerAST *) { return true; }
    virtual bool visit(DeclarationStatementAST *) { return true; }
    virtual bool visit(DeclaratorAST *) { return true; }
    virtual bool visit(DeclaratorIdAST *) { return true; }
    virtual bool visit(DecltypeSpecifierAST *) { return true; }
    virtual bool visit(DeleteExpressionAST *) { return true; }
    virtual bool visit(DesignatedInitializerAST *) { return true; }
    virtual bool visit(DestructorNameAST *) { return true; }
    virtual bool visit(DoStatementAST *) { return true; }
    virtual bool visit(DotDesignatorAST *) { return true; }
    virtual bool visit(DynamicExceptionSpecificationAST *) { return true; }
    virtual bool visit(ElaboratedTypeSpecifierAST *) { return true; }
    virtual bool visit(EmptyDeclarationAST *) { return true; }
    virtual bool visit(EnumSpecifierAST *) { return true; }
    virtual bool visit(EnumeratorAST *) { return true; }
    virtual bool visit(ExceptionDeclarationAST *) { return true; }
    virtual bool visit(ExpressionListParenAST *) { return true; }
    virtual bool visit(ExpressionOrDeclarationStatementAST *) { return true; }
    virtual bool visit(ExpressionStatementAST *) { return true; }
    virtual bool visit(ForStatementAST *) { return true; }
    virtual bool visit(ForeachStatementAST *) { return true; }
    virtual bool visit(FunctionDeclaratorAST *) { return true; }
    virtual bool visit(FunctionDefinitionAST *) { return true; }
    virtual bool visit(GnuAttributeAST *) { return true; }
    virtual bool visit(GnuAttributeSpecifierAST *) { return true; }
    virtual bool visit(GotoStatementAST *) { return true; }
    virtual bool visit(IdExpressionAST *) { return true; }
    virtual bool visit(IfStatementAST *) { return true; }
    virtual bool visit(LabeledStatementAST *) { return true; }
    virtual bool visit(LambdaCaptureAST *) { return true; }
    virtual bool visit(LambdaDeclaratorAST *) { return true; }
    virtual bool visit(LambdaExpressionAST *) { return true; }
    virtual bool visit(LambdaIntroducerAST *) { return true; }
    virtual bool visit(LinkageBodyAST *) { return true; }
    virtual bool visit(LinkageSpecificationAST *) { return true; }
    virtual bool visit(MemInitializerAST *) { return true; }
    virtual bool visit(MemberAccessAST *) { return true; }
    virtual bool visit(NamedTypeSpecifierAST *) { return true; }
    virtual bool visit(NamespaceAST *) { return true; }
    virtual bool visit(NamespaceAliasDefinitionAST *) { return true; }
    virtual bool visit(NestedDeclaratorAST *) { return true; }
    virtual bool visit(NestedExpressionAST *) { return true; }
    virtual bool visit(NestedNameSpecifierAST *) { return true; }
    virtual bool visit(NewArrayDeclaratorAST *) { return true; }
    virtual bool visit(NewExpressionAST *) { return true; }
    virtual bool visit(NewTypeIdAST *) { return true; }
    virtual bool visit(NoExceptSpecificationAST *) { return true; }
    virtual bool visit(NumericLiteralAST *) { return true; }
    virtual bool visit(ObjCClassDeclarationAST *) { return true; }
    virtual bool visit(ObjCClassForwardDeclarationAST *) { return true; }
    virtual bool visit(ObjCDynamicPropertiesDeclarationAST *) { return true; }
    virtual bool visit(ObjCEncodeExpressionAST *) { return true; }
    virtual bool visit(ObjCFastEnumerationAST *) { return true; }
    virtual bool visit(ObjCInstanceVariablesDeclarationAST *) { return true; }
    virtual bool visit(ObjCMessageArgumentAST *) { return true; }
    virtual bool visit(ObjCMessageArgumentDeclarationAST *) { return true; }
    virtual bool visit(ObjCMessageExpressionAST *) { return true; }
    virtual bool visit(ObjCMethodDeclarationAST *) { return true; }
    virtual bool visit(ObjCMethodPrototypeAST *) { return true; }
    virtual bool visit(ObjCPropertyAttributeAST *) { return true; }
    virtual bool visit(ObjCPropertyDeclarationAST *) { return true; }
    virtual bool visit(ObjCProtocolDeclarationAST *) { return true; }
    virtual bool visit(ObjCProtocolExpressionAST *) { return true; }
    virtual bool visit(ObjCProtocolForwardDeclarationAST *) { return true; }
    virtual bool visit(ObjCProtocolRefsAST *) { return true; }
    virtual bool visit(ObjCSelectorAST *) { return true; }
    virtual bool visit(ObjCSelectorArgumentAST *) { return true; }
    virtual bool visit(ObjCSelectorExpressionAST *) { return true; }
    virtual bool visit(ObjCSynchronizedStatementAST *) { return true; }
    virtual bool visit(ObjCSynthesizedPropertiesDeclarationAST *) { return true; }
    virtual bool visit(ObjCSynthesizedPropertyAST *) { return true; }
    virtual bool visit(ObjCTypeNameAST *) { return true; }
    virtual bool visit(ObjCVisibilityDeclarationAST *) { return true; }
    virtual bool visit(OperatorAST *) { return true; }
    virtual bool visit(OperatorFunctionIdAST *) { return true; }
    virtual bool visit(ParameterDeclarationAST *) { return true; }
    virtual bool visit(ParameterDeclarationClauseAST *) { return true; }
    virtual bool visit(PointerAST *) { return true; }
    virtual bool visit(PointerLiteralAST *) { return true; }
    virtual bool visit(PointerToMemberAST *) { return true; }
    virtual bool visit(PostIncrDecrAST *) { return true; }
    virtual bool visit(QtEnumDeclarationAST *) { return true; }
    virtual bool visit(QtFlagsDeclarationAST *) { return true; }
    virtual bool visit(QtInterfaceNameAST *) { return true; }
    virtual bool visit(QtInterfacesDeclarationAST *) { return true; }
    virtual bool visit(QtMemberDeclarationAST *) { return true; }
    virtual bool visit(QtMethodAST *) { return true; }
    virtual bool visit(QtObjectTagAST *) { return true; }
    virtual bool visit(QtPrivateSlotAST *) { return true; }
    virtual bool visit(QtPropertyDeclarationAST *) { return true; }
    virtual bool visit(QtPropertyDeclarationItemAST *) { return true; }
    virtual bool visit(QualifiedNameAST *) { return true; }
    virtual bool visit(RangeBasedForStatementAST *) { return true; }
    virtual bool visit(ReferenceAST *) { return true; }
    virtual bool visit(ReturnStatementAST *) { return true; }
    virtual bool visit(SimpleDeclarationAST *) { return true; }
    virtual bool visit(SimpleNameAST *) { return true; }
    virtual bool visit(SimpleSpecifierAST *) { return true; }
    virtual bool visit(SizeofExpressionAST *) { return true; }
    virtual bool visit(StaticAssertDeclarationAST *) { return true; }
    virtual bool visit(StringLiteralAST *) { return true; }
    virtual bool visit(SwitchStatementAST *) { return true; }
    virtual bool visit(TemplateDeclarationAST *) { return true; }
    virtual bool visit(TemplateIdAST *) { return true; }
    virtual bool visit(TemplateTypeParameterAST *) { return true; }
    virtual bool visit(ThisExpressionAST *) { return true; }
    virtual bool visit(ThrowExpressionAST *) { return true; }
    virtual bool visit(TrailingReturnTypeAST *) { return true; }
    virtual bool visit(TranslationUnitAST *) { return true; }
    virtual bool visit(TryBlockStatementAST *) { return true; }
    virtual bool visit(TypeConstructorCallAST *) { return true; }
    virtual bool visit(TypeIdAST *) { return true; }
    virtual bool visit(TypeidExpressionAST *) { return true; }
    virtual bool visit(TypenameCallExpressionAST *) { return true; }
    virtual bool visit(TypenameTypeParameterAST *) { return true; }
    virtual bool visit(TypeofSpecifierAST *) { return true; }
    virtual bool visit(UnaryExpressionAST *) { return true; }
    virtual bool visit(UsingAST *) { return true; }
    virtual bool visit(UsingDirectiveAST *) { return true; }
    virtual bool visit(WhileStatementAST *) { return true; }

    virtual void endVisit(AccessDeclarationAST *) {}
    virtual void endVisit(AliasDeclarationAST *) {}
    virtual void endVisit(AlignmentSpecifierAST *) {}
    virtual void endVisit(AlignofExpressionAST *) {}
    virtual void endVisit(AnonymousNameAST *) {}
    virtual void endVisit(ArrayAccessAST *) {}
    virtual void endVisit(ArrayDeclaratorAST *) {}
    virtual void endVisit(ArrayInitializerAST *) {}
    virtual void endVisit(AsmDefinitionAST *) {}
    virtual void endVisit(BaseSpecifierAST *) {}
    virtual void endVisit(BinaryExpressionAST *) {}
    virtual void endVisit(BoolLiteralAST *) {}
    virtual void endVisit(BracedInitializerAST *) {}
    virtual void endVisit(BracketDesignatorAST *) {}
    virtual void endVisit(BreakStatementAST *) {}
    virtual void endVisit(CallAST *) {}
    virtual void endVisit(CaptureAST *) {}
    virtual void endVisit(CaseStatementAST *) {}
    virtual void endVisit(CastExpressionAST *) {}
    virtual void endVisit(CatchClauseAST *) {}
    virtual void endVisit(ClassSpecifierAST *) {}
    virtual void endVisit(CompoundExpressionAST *) {}
    virtual void endVisit(CompoundLiteralAST *) {}
    virtual void endVisit(CompoundStatementAST *) {}
    virtual void endVisit(ConditionAST *) {}
    virtual void endVisit(ConditionalExpressionAST *) {}
    virtual void endVisit(ContinueStatementAST *) {}
    virtual void endVisit(ConversionFunctionIdAST *) {}
    virtual void endVisit(CppCastExpressionAST *) {}
    virtual void endVisit(CtorInitializerAST *) {}
    virtual void endVisit(DeclarationStatementAST *) {}
    virtual void endVisit(DeclaratorAST *) {}
    virtual void endVisit(DeclaratorIdAST *) {}
    virtual void endVisit(DecltypeSpecifierAST *) {}
    virtual void endVisit(DeleteExpressionAST *) {}
    virtual void endVisit(DesignatedInitializerAST *) {}
    virtual void endVisit(DestructorNameAST *) {}
    virtual void endVisit(DoStatementAST *) {}
    virtual void endVisit(DotDesignatorAST *) {}
    virtual void endVisit(DynamicExceptionSpecificationAST *) {}
    virtual void endVisit(ElaboratedTypeSpecifierAST *) {}
    virtual void endVisit(EmptyDeclarationAST *) {}
    virtual void endVisit(EnumSpecifierAST *) {}
    virtual void endVisit(EnumeratorAST *) {}
    virtual void endVisit(ExceptionDeclarationAST *) {}
    virtual void endVisit(ExpressionListParenAST *) {}
    virtual void endVisit(ExpressionOrDeclarationStatementAST *) {}
    virtual void endVisit(ExpressionStatementAST *) {}
    virtual void endVisit(ForStatementAST *) {}
    virtual void endVisit(ForeachStatementAST *) {}
    virtual void endVisit(FunctionDeclaratorAST *) {}
    virtual void endVisit(FunctionDefinitionAST *) {}
    virtual void endVisit(GnuAttributeAST *) {}
    virtual void endVisit(GnuAttributeSpecifierAST *) {}
    virtual void endVisit(GotoStatementAST *) {}
    virtual void endVisit(IdExpressionAST *) {}
    virtual void endVisit(IfStatementAST *) {}
    virtual void endVisit(LabeledStatementAST *) {}
    virtual void endVisit(LambdaCaptureAST *) {}
    virtual void endVisit(LambdaDeclaratorAST *) {}
    virtual void endVisit(LambdaExpressionAST *) {}
    virtual void endVisit(LambdaIntroducerAST *) {}
    virtual void endVisit(LinkageBodyAST *) {}
    virtual void endVisit(LinkageSpecificationAST *) {}
    virtual void endVisit(MemInitializerAST *) {}
    virtual void endVisit(MemberAccessAST *) {}
    virtual void endVisit(NamedTypeSpecifierAST *) {}
    virtual void endVisit(NamespaceAST *) {}
    virtual void endVisit(NamespaceAliasDefinitionAST *) {}
    virtual void endVisit(NestedDeclaratorAST *) {}
    virtual void endVisit(NestedExpressionAST *) {}
    virtual void endVisit(NestedNameSpecifierAST *) {}
    virtual void endVisit(NewArrayDeclaratorAST *) {}
    virtual void endVisit(NewExpressionAST *) {}
    virtual void endVisit(NewTypeIdAST *) {}
    virtual void endVisit(NoExceptSpecificationAST *) {}
    virtual void endVisit(NumericLiteralAST *) {}
    virtual void endVisit(ObjCClassDeclarationAST *) {}
    virtual void endVisit(ObjCClassForwardDeclarationAST *) {}
    virtual void endVisit(ObjCDynamicPropertiesDeclarationAST *) {}
    virtual void endVisit(ObjCEncodeExpressionAST *) {}
    virtual void endVisit(ObjCFastEnumerationAST *) {}
    virtual void endVisit(ObjCInstanceVariablesDeclarationAST *) {}
    virtual void endVisit(ObjCMessageArgumentAST *) {}
    virtual void endVisit(ObjCMessageArgumentDeclarationAST *) {}
    virtual void endVisit(ObjCMessageExpressionAST *) {}
    virtual void endVisit(ObjCMethodDeclarationAST *) {}
    virtual void endVisit(ObjCMethodPrototypeAST *) {}
    virtual void endVisit(ObjCPropertyAttributeAST *) {}
    virtual void endVisit(ObjCPropertyDeclarationAST *) {}
    virtual void endVisit(ObjCProtocolDeclarationAST *) {}
    virtual void endVisit(ObjCProtocolExpressionAST *) {}
    virtual void endVisit(ObjCProtocolForwardDeclarationAST *) {}
    virtual void endVisit(ObjCProtocolRefsAST *) {}
    virtual void endVisit(ObjCSelectorAST *) {}
    virtual void endVisit(ObjCSelectorArgumentAST *) {}
    virtual void endVisit(ObjCSelectorExpressionAST *) {}
    virtual void endVisit(ObjCSynchronizedStatementAST *) {}
    virtual void endVisit(ObjCSynthesizedPropertiesDeclarationAST *) {}
    virtual void endVisit(ObjCSynthesizedPropertyAST *) {}
    virtual void endVisit(ObjCTypeNameAST *) {}
    virtual void endVisit(ObjCVisibilityDeclarationAST *) {}
    virtual void endVisit(OperatorAST *) {}
    virtual void endVisit(OperatorFunctionIdAST *) {}
    virtual void endVisit(ParameterDeclarationAST *) {}
    virtual void endVisit(ParameterDeclarationClauseAST *) {}
    virtual void endVisit(PointerAST *) {}
    virtual void endVisit(PointerLiteralAST *) {}
    virtual void endVisit(PointerToMemberAST *) {}
    virtual void endVisit(PostIncrDecrAST *) {}
    virtual void endVisit(QtEnumDeclarationAST *) {}
    virtual void endVisit(QtFlagsDeclarationAST *) {}
    virtual void endVisit(QtInterfaceNameAST *) {}
    virtual void endVisit(QtInterfacesDeclarationAST *) {}
    virtual void endVisit(QtMemberDeclarationAST *) {}
    virtual void endVisit(QtMethodAST *) {}
    virtual void endVisit(QtObjectTagAST *) {}
    virtual void endVisit(QtPrivateSlotAST *) {}
    virtual void endVisit(QtPropertyDeclarationAST *) {}
    virtual void endVisit(QtPropertyDeclarationItemAST *) {}
    virtual void endVisit(QualifiedNameAST *) {}
    virtual void endVisit(RangeBasedForStatementAST *) {}
    virtual void endVisit(ReferenceAST *) {}
    virtual void endVisit(ReturnStatementAST *) {}
    virtual void endVisit(SimpleDeclarationAST *) {}
    virtual void endVisit(SimpleNameAST *) {}
    virtual void endVisit(SimpleSpecifierAST *) {}
    virtual void endVisit(SizeofExpressionAST *) {}
    virtual void endVisit(StaticAssertDeclarationAST *) {}
    virtual void endVisit(StringLiteralAST *) {}
    virtual void endVisit(SwitchStatementAST *) {}
    virtual void endVisit(TemplateDeclarationAST *) {}
    virtual void endVisit(TemplateIdAST *) {}
    virtual void endVisit(TemplateTypeParameterAST *) {}
    virtual void endVisit(ThisExpressionAST *) {}
    virtual void endVisit(ThrowExpressionAST *) {}
    virtual void endVisit(TrailingReturnTypeAST *) {}
    virtual void endVisit(TranslationUnitAST *) {}
    virtual void endVisit(TryBlockStatementAST *) {}
    virtual void endVisit(TypeConstructorCallAST *) {}
    virtual void endVisit(TypeIdAST *) {}
    virtual void endVisit(TypeidExpressionAST *) {}
    virtual void endVisit(TypenameCallExpressionAST *) {}
    virtual void endVisit(TypenameTypeParameterAST *) {}
    virtual void endVisit(TypeofSpecifierAST *) {}
    virtual void endVisit(UnaryExpressionAST *) {}
    virtual void endVisit(UsingAST *) {}
    virtual void endVisit(UsingDirectiveAST *) {}
    virtual void endVisit(WhileStatementAST *) {}

private:
   TranslationUnit *_translationUnit;
};

} // namespace CPlusPlus

#endif // CPLUSPLUS_ASTVISITOR_H
