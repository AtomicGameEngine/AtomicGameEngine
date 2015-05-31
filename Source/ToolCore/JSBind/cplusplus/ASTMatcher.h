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

#ifndef ASTMATCHER_H
#define ASTMATCHER_H

#include "ASTfwd.h"

namespace CPlusPlus {

class CPLUSPLUS_EXPORT ASTMatcher
{
public:
    ASTMatcher();
    virtual ~ASTMatcher();

    virtual bool match(AccessDeclarationAST *node, AccessDeclarationAST *pattern);
    virtual bool match(AliasDeclarationAST *node, AliasDeclarationAST *pattern);
    virtual bool match(AlignmentSpecifierAST *node, AlignmentSpecifierAST *pattern);
    virtual bool match(AlignofExpressionAST *node, AlignofExpressionAST *pattern);
    virtual bool match(AnonymousNameAST *node, AnonymousNameAST *pattern);
    virtual bool match(ArrayAccessAST *node, ArrayAccessAST *pattern);
    virtual bool match(ArrayDeclaratorAST *node, ArrayDeclaratorAST *pattern);
    virtual bool match(ArrayInitializerAST *node, ArrayInitializerAST *pattern);
    virtual bool match(AsmDefinitionAST *node, AsmDefinitionAST *pattern);
    virtual bool match(BaseSpecifierAST *node, BaseSpecifierAST *pattern);
    virtual bool match(BinaryExpressionAST *node, BinaryExpressionAST *pattern);
    virtual bool match(BoolLiteralAST *node, BoolLiteralAST *pattern);
    virtual bool match(BracedInitializerAST *node, BracedInitializerAST *pattern);
    virtual bool match(BracketDesignatorAST *node, BracketDesignatorAST *pattern);
    virtual bool match(BreakStatementAST *node, BreakStatementAST *pattern);
    virtual bool match(CallAST *node, CallAST *pattern);
    virtual bool match(CaptureAST *node, CaptureAST *pattern);
    virtual bool match(CaseStatementAST *node, CaseStatementAST *pattern);
    virtual bool match(CastExpressionAST *node, CastExpressionAST *pattern);
    virtual bool match(CatchClauseAST *node, CatchClauseAST *pattern);
    virtual bool match(ClassSpecifierAST *node, ClassSpecifierAST *pattern);
    virtual bool match(CompoundExpressionAST *node, CompoundExpressionAST *pattern);
    virtual bool match(CompoundLiteralAST *node, CompoundLiteralAST *pattern);
    virtual bool match(CompoundStatementAST *node, CompoundStatementAST *pattern);
    virtual bool match(ConditionAST *node, ConditionAST *pattern);
    virtual bool match(ConditionalExpressionAST *node, ConditionalExpressionAST *pattern);
    virtual bool match(ContinueStatementAST *node, ContinueStatementAST *pattern);
    virtual bool match(ConversionFunctionIdAST *node, ConversionFunctionIdAST *pattern);
    virtual bool match(CppCastExpressionAST *node, CppCastExpressionAST *pattern);
    virtual bool match(CtorInitializerAST *node, CtorInitializerAST *pattern);
    virtual bool match(DeclarationStatementAST *node, DeclarationStatementAST *pattern);
    virtual bool match(DeclaratorAST *node, DeclaratorAST *pattern);
    virtual bool match(DeclaratorIdAST *node, DeclaratorIdAST *pattern);
    virtual bool match(DecltypeSpecifierAST *node, DecltypeSpecifierAST *pattern);
    virtual bool match(DeleteExpressionAST *node, DeleteExpressionAST *pattern);
    virtual bool match(DesignatedInitializerAST *node, DesignatedInitializerAST *pattern);
    virtual bool match(DestructorNameAST *node, DestructorNameAST *pattern);
    virtual bool match(DoStatementAST *node, DoStatementAST *pattern);
    virtual bool match(DotDesignatorAST *node, DotDesignatorAST *pattern);
    virtual bool match(DynamicExceptionSpecificationAST *node, DynamicExceptionSpecificationAST *pattern);
    virtual bool match(ElaboratedTypeSpecifierAST *node, ElaboratedTypeSpecifierAST *pattern);
    virtual bool match(EmptyDeclarationAST *node, EmptyDeclarationAST *pattern);
    virtual bool match(EnumSpecifierAST *node, EnumSpecifierAST *pattern);
    virtual bool match(EnumeratorAST *node, EnumeratorAST *pattern);
    virtual bool match(ExceptionDeclarationAST *node, ExceptionDeclarationAST *pattern);
    virtual bool match(ExpressionListParenAST *node, ExpressionListParenAST *pattern);
    virtual bool match(ExpressionOrDeclarationStatementAST *node, ExpressionOrDeclarationStatementAST *pattern);
    virtual bool match(ExpressionStatementAST *node, ExpressionStatementAST *pattern);
    virtual bool match(ForStatementAST *node, ForStatementAST *pattern);
    virtual bool match(ForeachStatementAST *node, ForeachStatementAST *pattern);
    virtual bool match(FunctionDeclaratorAST *node, FunctionDeclaratorAST *pattern);
    virtual bool match(FunctionDefinitionAST *node, FunctionDefinitionAST *pattern);
    virtual bool match(GnuAttributeAST *node, GnuAttributeAST *pattern);
    virtual bool match(GnuAttributeSpecifierAST *node, GnuAttributeSpecifierAST *pattern);
    virtual bool match(GotoStatementAST *node, GotoStatementAST *pattern);
    virtual bool match(IdExpressionAST *node, IdExpressionAST *pattern);
    virtual bool match(IfStatementAST *node, IfStatementAST *pattern);
    virtual bool match(LabeledStatementAST *node, LabeledStatementAST *pattern);
    virtual bool match(LambdaCaptureAST *node, LambdaCaptureAST *pattern);
    virtual bool match(LambdaDeclaratorAST *node, LambdaDeclaratorAST *pattern);
    virtual bool match(LambdaExpressionAST *node, LambdaExpressionAST *pattern);
    virtual bool match(LambdaIntroducerAST *node, LambdaIntroducerAST *pattern);
    virtual bool match(LinkageBodyAST *node, LinkageBodyAST *pattern);
    virtual bool match(LinkageSpecificationAST *node, LinkageSpecificationAST *pattern);
    virtual bool match(MemInitializerAST *node, MemInitializerAST *pattern);
    virtual bool match(MemberAccessAST *node, MemberAccessAST *pattern);
    virtual bool match(NamedTypeSpecifierAST *node, NamedTypeSpecifierAST *pattern);
    virtual bool match(NamespaceAST *node, NamespaceAST *pattern);
    virtual bool match(NamespaceAliasDefinitionAST *node, NamespaceAliasDefinitionAST *pattern);
    virtual bool match(NestedDeclaratorAST *node, NestedDeclaratorAST *pattern);
    virtual bool match(NestedExpressionAST *node, NestedExpressionAST *pattern);
    virtual bool match(NestedNameSpecifierAST *node, NestedNameSpecifierAST *pattern);
    virtual bool match(NewArrayDeclaratorAST *node, NewArrayDeclaratorAST *pattern);
    virtual bool match(NewExpressionAST *node, NewExpressionAST *pattern);
    virtual bool match(NewTypeIdAST *node, NewTypeIdAST *pattern);
    virtual bool match(NoExceptSpecificationAST *node, NoExceptSpecificationAST *pattern);
    virtual bool match(NumericLiteralAST *node, NumericLiteralAST *pattern);
    virtual bool match(ObjCClassDeclarationAST *node, ObjCClassDeclarationAST *pattern);
    virtual bool match(ObjCClassForwardDeclarationAST *node, ObjCClassForwardDeclarationAST *pattern);
    virtual bool match(ObjCDynamicPropertiesDeclarationAST *node, ObjCDynamicPropertiesDeclarationAST *pattern);
    virtual bool match(ObjCEncodeExpressionAST *node, ObjCEncodeExpressionAST *pattern);
    virtual bool match(ObjCFastEnumerationAST *node, ObjCFastEnumerationAST *pattern);
    virtual bool match(ObjCInstanceVariablesDeclarationAST *node, ObjCInstanceVariablesDeclarationAST *pattern);
    virtual bool match(ObjCMessageArgumentAST *node, ObjCMessageArgumentAST *pattern);
    virtual bool match(ObjCMessageArgumentDeclarationAST *node, ObjCMessageArgumentDeclarationAST *pattern);
    virtual bool match(ObjCMessageExpressionAST *node, ObjCMessageExpressionAST *pattern);
    virtual bool match(ObjCMethodDeclarationAST *node, ObjCMethodDeclarationAST *pattern);
    virtual bool match(ObjCMethodPrototypeAST *node, ObjCMethodPrototypeAST *pattern);
    virtual bool match(ObjCPropertyAttributeAST *node, ObjCPropertyAttributeAST *pattern);
    virtual bool match(ObjCPropertyDeclarationAST *node, ObjCPropertyDeclarationAST *pattern);
    virtual bool match(ObjCProtocolDeclarationAST *node, ObjCProtocolDeclarationAST *pattern);
    virtual bool match(ObjCProtocolExpressionAST *node, ObjCProtocolExpressionAST *pattern);
    virtual bool match(ObjCProtocolForwardDeclarationAST *node, ObjCProtocolForwardDeclarationAST *pattern);
    virtual bool match(ObjCProtocolRefsAST *node, ObjCProtocolRefsAST *pattern);
    virtual bool match(ObjCSelectorAST *node, ObjCSelectorAST *pattern);
    virtual bool match(ObjCSelectorArgumentAST *node, ObjCSelectorArgumentAST *pattern);
    virtual bool match(ObjCSelectorExpressionAST *node, ObjCSelectorExpressionAST *pattern);
    virtual bool match(ObjCSynchronizedStatementAST *node, ObjCSynchronizedStatementAST *pattern);
    virtual bool match(ObjCSynthesizedPropertiesDeclarationAST *node, ObjCSynthesizedPropertiesDeclarationAST *pattern);
    virtual bool match(ObjCSynthesizedPropertyAST *node, ObjCSynthesizedPropertyAST *pattern);
    virtual bool match(ObjCTypeNameAST *node, ObjCTypeNameAST *pattern);
    virtual bool match(ObjCVisibilityDeclarationAST *node, ObjCVisibilityDeclarationAST *pattern);
    virtual bool match(OperatorAST *node, OperatorAST *pattern);
    virtual bool match(OperatorFunctionIdAST *node, OperatorFunctionIdAST *pattern);
    virtual bool match(ParameterDeclarationAST *node, ParameterDeclarationAST *pattern);
    virtual bool match(ParameterDeclarationClauseAST *node, ParameterDeclarationClauseAST *pattern);
    virtual bool match(PointerAST *node, PointerAST *pattern);
    virtual bool match(PointerLiteralAST *node, PointerLiteralAST *pattern);
    virtual bool match(PointerToMemberAST *node, PointerToMemberAST *pattern);
    virtual bool match(PostIncrDecrAST *node, PostIncrDecrAST *pattern);
    virtual bool match(QtEnumDeclarationAST *node, QtEnumDeclarationAST *pattern);
    virtual bool match(QtFlagsDeclarationAST *node, QtFlagsDeclarationAST *pattern);
    virtual bool match(QtInterfaceNameAST *node, QtInterfaceNameAST *pattern);
    virtual bool match(QtInterfacesDeclarationAST *node, QtInterfacesDeclarationAST *pattern);
    virtual bool match(QtMemberDeclarationAST *node, QtMemberDeclarationAST *pattern);
    virtual bool match(QtMethodAST *node, QtMethodAST *pattern);
    virtual bool match(QtObjectTagAST *node, QtObjectTagAST *pattern);
    virtual bool match(QtPrivateSlotAST *node, QtPrivateSlotAST *pattern);
    virtual bool match(QtPropertyDeclarationAST *node, QtPropertyDeclarationAST *pattern);
    virtual bool match(QtPropertyDeclarationItemAST *node, QtPropertyDeclarationItemAST *pattern);
    virtual bool match(QualifiedNameAST *node, QualifiedNameAST *pattern);
    virtual bool match(RangeBasedForStatementAST *node, RangeBasedForStatementAST *pattern);
    virtual bool match(ReferenceAST *node, ReferenceAST *pattern);
    virtual bool match(ReturnStatementAST *node, ReturnStatementAST *pattern);
    virtual bool match(SimpleDeclarationAST *node, SimpleDeclarationAST *pattern);
    virtual bool match(SimpleNameAST *node, SimpleNameAST *pattern);
    virtual bool match(SimpleSpecifierAST *node, SimpleSpecifierAST *pattern);
    virtual bool match(SizeofExpressionAST *node, SizeofExpressionAST *pattern);
    virtual bool match(StaticAssertDeclarationAST *node, StaticAssertDeclarationAST *pattern);
    virtual bool match(StringLiteralAST *node, StringLiteralAST *pattern);
    virtual bool match(SwitchStatementAST *node, SwitchStatementAST *pattern);
    virtual bool match(TemplateDeclarationAST *node, TemplateDeclarationAST *pattern);
    virtual bool match(TemplateIdAST *node, TemplateIdAST *pattern);
    virtual bool match(TemplateTypeParameterAST *node, TemplateTypeParameterAST *pattern);
    virtual bool match(ThisExpressionAST *node, ThisExpressionAST *pattern);
    virtual bool match(ThrowExpressionAST *node, ThrowExpressionAST *pattern);
    virtual bool match(TrailingReturnTypeAST *node, TrailingReturnTypeAST *pattern);
    virtual bool match(TranslationUnitAST *node, TranslationUnitAST *pattern);
    virtual bool match(TryBlockStatementAST *node, TryBlockStatementAST *pattern);
    virtual bool match(TypeConstructorCallAST *node, TypeConstructorCallAST *pattern);
    virtual bool match(TypeIdAST *node, TypeIdAST *pattern);
    virtual bool match(TypeidExpressionAST *node, TypeidExpressionAST *pattern);
    virtual bool match(TypenameCallExpressionAST *node, TypenameCallExpressionAST *pattern);
    virtual bool match(TypenameTypeParameterAST *node, TypenameTypeParameterAST *pattern);
    virtual bool match(TypeofSpecifierAST *node, TypeofSpecifierAST *pattern);
    virtual bool match(UnaryExpressionAST *node, UnaryExpressionAST *pattern);
    virtual bool match(UsingAST *node, UsingAST *pattern);
    virtual bool match(UsingDirectiveAST *node, UsingDirectiveAST *pattern);
    virtual bool match(WhileStatementAST *node, WhileStatementAST *pattern);
};

} // namespace CPlusPlus

#endif // CPLUSPLUS_ASTMATCHER_H
