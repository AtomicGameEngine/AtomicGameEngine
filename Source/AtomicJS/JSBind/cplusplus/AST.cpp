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

#include "AST.h"
#include "ASTVisitor.h"
#include "ASTMatcher.h"
#include "MemoryPool.h"

#include "cppassert.h"

#include <algorithm>


/*
   All firstToken/lastToken functions below which have a doxygen comment with
   \generated in it, will be re-generated when the tool "cplusplus-update-frontend" is run.

   For functions which are hand-coded, or which should not be changed, make sure that
   the comment is gone.
 */


using namespace CPlusPlus;

AST::AST()
{ }

AST::~AST()
{ CPP_CHECK(0); }

void AST::accept(ASTVisitor *visitor)
{
    if (visitor->preVisit(this))
        accept0(visitor);
    visitor->postVisit(this);
}

bool AST::match(AST *ast, AST *pattern, ASTMatcher *matcher)
{
    if (ast == pattern)
        return true;

    else if (! ast || ! pattern)
        return false;

    return ast->match(pattern, matcher);
}

bool AST::match(AST *pattern, ASTMatcher *matcher)
{
    return match0(pattern, matcher);
}

unsigned GnuAttributeSpecifierAST::firstToken() const
{
    return attribute_token;
}

unsigned BaseSpecifierAST::firstToken() const
{
    if (virtual_token && access_specifier_token)
        return std::min(virtual_token, access_specifier_token);
    if (virtual_token)
        return virtual_token;
    if (access_specifier_token)
        return access_specifier_token;
    if (name)
        return name->firstToken();
    // assert?
    return 0;
}

unsigned BaseSpecifierAST::lastToken() const
{
    if (name)
        return name->lastToken();
    else if (virtual_token && access_specifier_token)
        return std::max(virtual_token, access_specifier_token) + 1;
    else if (virtual_token)
        return virtual_token + 1;
    else if (access_specifier_token)
        return access_specifier_token + 1;
    // assert?
    return 0;
}

/** \generated */
unsigned AccessDeclarationAST::firstToken() const
{
    if (access_specifier_token)
        return access_specifier_token;
    if (slots_token)
        return slots_token;
    if (colon_token)
        return colon_token;
    return 0;
}

/** \generated */
unsigned AccessDeclarationAST::lastToken() const
{
    if (colon_token)
        return colon_token + 1;
    if (slots_token)
        return slots_token + 1;
    if (access_specifier_token)
        return access_specifier_token + 1;
    return 1;
}

/** \generated */
unsigned ArrayAccessAST::firstToken() const
{
    if (base_expression)
        if (unsigned candidate = base_expression->firstToken())
            return candidate;
    if (lbracket_token)
        return lbracket_token;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    if (rbracket_token)
        return rbracket_token;
    return 0;
}

/** \generated */
unsigned ArrayAccessAST::lastToken() const
{
    if (rbracket_token)
        return rbracket_token + 1;
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (lbracket_token)
        return lbracket_token + 1;
    if (base_expression)
        if (unsigned candidate = base_expression->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned ArrayDeclaratorAST::firstToken() const
{
    if (lbracket_token)
        return lbracket_token;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    if (rbracket_token)
        return rbracket_token;
    return 0;
}

/** \generated */
unsigned ArrayDeclaratorAST::lastToken() const
{
    if (rbracket_token)
        return rbracket_token + 1;
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (lbracket_token)
        return lbracket_token + 1;
    return 1;
}

/** \generated */
unsigned ArrayInitializerAST::firstToken() const
{
    if (lbrace_token)
        return lbrace_token;
    if (expression_list)
        if (unsigned candidate = expression_list->firstToken())
            return candidate;
    if (rbrace_token)
        return rbrace_token;
    return 0;
}

/** \generated */
unsigned ArrayInitializerAST::lastToken() const
{
    if (rbrace_token)
        return rbrace_token + 1;
    if (expression_list)
        if (unsigned candidate = expression_list->lastToken())
            return candidate;
    if (lbrace_token)
        return lbrace_token + 1;
    return 1;
}

/** \generated */
unsigned AsmDefinitionAST::firstToken() const
{
    if (asm_token)
        return asm_token;
    if (volatile_token)
        return volatile_token;
    if (lparen_token)
        return lparen_token;
    if (rparen_token)
        return rparen_token;
    if (semicolon_token)
        return semicolon_token;
    return 0;
}

/** \generated */
unsigned AsmDefinitionAST::lastToken() const
{
    if (semicolon_token)
        return semicolon_token + 1;
    if (rparen_token)
        return rparen_token + 1;
    if (lparen_token)
        return lparen_token + 1;
    if (volatile_token)
        return volatile_token + 1;
    if (asm_token)
        return asm_token + 1;
    return 1;
}

/** \generated */
unsigned GnuAttributeAST::firstToken() const
{
    if (identifier_token)
        return identifier_token;
    if (lparen_token)
        return lparen_token;
    if (tag_token)
        return tag_token;
    if (expression_list)
        if (unsigned candidate = expression_list->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    return 0;
}

/** \generated */
unsigned GnuAttributeAST::lastToken() const
{
    if (rparen_token)
        return rparen_token + 1;
    if (expression_list)
        if (unsigned candidate = expression_list->lastToken())
            return candidate;
    if (tag_token)
        return tag_token + 1;
    if (lparen_token)
        return lparen_token + 1;
    if (identifier_token)
        return identifier_token + 1;
    return 1;
}

/** \generated */
unsigned BinaryExpressionAST::firstToken() const
{
    if (left_expression)
        if (unsigned candidate = left_expression->firstToken())
            return candidate;
    if (binary_op_token)
        return binary_op_token;
    if (right_expression)
        if (unsigned candidate = right_expression->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned BinaryExpressionAST::lastToken() const
{
    if (right_expression)
        if (unsigned candidate = right_expression->lastToken())
            return candidate;
    if (binary_op_token)
        return binary_op_token + 1;
    if (left_expression)
        if (unsigned candidate = left_expression->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned BoolLiteralAST::firstToken() const
{
    if (literal_token)
        return literal_token;
    return 0;
}

/** \generated */
unsigned BoolLiteralAST::lastToken() const
{
    if (literal_token)
        return literal_token + 1;
    return 1;
}

/** \generated */
unsigned BracedInitializerAST::firstToken() const
{
    if (lbrace_token)
        return lbrace_token;
    if (expression_list)
        if (unsigned candidate = expression_list->firstToken())
            return candidate;
    if (comma_token)
        return comma_token;
    if (rbrace_token)
        return rbrace_token;
    return 0;
}

/** \generated */
unsigned BracedInitializerAST::lastToken() const
{
    if (rbrace_token)
        return rbrace_token + 1;
    if (comma_token)
        return comma_token + 1;
    if (expression_list)
        if (unsigned candidate = expression_list->lastToken())
            return candidate;
    if (lbrace_token)
        return lbrace_token + 1;
    return 1;
}

/** \generated */
unsigned BreakStatementAST::firstToken() const
{
    if (break_token)
        return break_token;
    if (semicolon_token)
        return semicolon_token;
    return 0;
}

/** \generated */
unsigned BreakStatementAST::lastToken() const
{
    if (semicolon_token)
        return semicolon_token + 1;
    if (break_token)
        return break_token + 1;
    return 1;
}

/** \generated */
unsigned CallAST::firstToken() const
{
    if (base_expression)
        if (unsigned candidate = base_expression->firstToken())
            return candidate;
    if (lparen_token)
        return lparen_token;
    if (expression_list)
        if (unsigned candidate = expression_list->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    return 0;
}

/** \generated */
unsigned CallAST::lastToken() const
{
    if (rparen_token)
        return rparen_token + 1;
    if (expression_list)
        if (unsigned candidate = expression_list->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (base_expression)
        if (unsigned candidate = base_expression->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned CaptureAST::firstToken() const
{
    if (amper_token)
        return amper_token;
    if (identifier)
        if (unsigned candidate = identifier->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned CaptureAST::lastToken() const
{
    if (identifier)
        if (unsigned candidate = identifier->lastToken())
            return candidate;
    if (amper_token)
        return amper_token + 1;
    return 1;
}

/** \generated */
unsigned CaseStatementAST::firstToken() const
{
    if (case_token)
        return case_token;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    if (colon_token)
        return colon_token;
    if (statement)
        if (unsigned candidate = statement->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned CaseStatementAST::lastToken() const
{
    if (statement)
        if (unsigned candidate = statement->lastToken())
            return candidate;
    if (colon_token)
        return colon_token + 1;
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (case_token)
        return case_token + 1;
    return 1;
}

/** \generated */
unsigned CastExpressionAST::firstToken() const
{
    if (lparen_token)
        return lparen_token;
    if (type_id)
        if (unsigned candidate = type_id->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned CastExpressionAST::lastToken() const
{
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (rparen_token)
        return rparen_token + 1;
    if (type_id)
        if (unsigned candidate = type_id->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    return 1;
}

/** \generated */
unsigned CatchClauseAST::firstToken() const
{
    if (catch_token)
        return catch_token;
    if (lparen_token)
        return lparen_token;
    if (exception_declaration)
        if (unsigned candidate = exception_declaration->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    if (statement)
        if (unsigned candidate = statement->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned CatchClauseAST::lastToken() const
{
    if (statement)
        if (unsigned candidate = statement->lastToken())
            return candidate;
    if (rparen_token)
        return rparen_token + 1;
    if (exception_declaration)
        if (unsigned candidate = exception_declaration->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (catch_token)
        return catch_token + 1;
    return 1;
}

/** \generated */
unsigned ClassSpecifierAST::firstToken() const
{
    if (classkey_token)
        return classkey_token;
    if (attribute_list)
        if (unsigned candidate = attribute_list->firstToken())
            return candidate;
    if (name)
        if (unsigned candidate = name->firstToken())
            return candidate;
    if (final_token)
        return final_token;
    if (colon_token)
        return colon_token;
    if (base_clause_list)
        if (unsigned candidate = base_clause_list->firstToken())
            return candidate;
    if (dot_dot_dot_token)
        return dot_dot_dot_token;
    if (lbrace_token)
        return lbrace_token;
    if (member_specifier_list)
        if (unsigned candidate = member_specifier_list->firstToken())
            return candidate;
    if (rbrace_token)
        return rbrace_token;
    return 0;
}

/** \generated */
unsigned ClassSpecifierAST::lastToken() const
{
    if (rbrace_token)
        return rbrace_token + 1;
    if (member_specifier_list)
        if (unsigned candidate = member_specifier_list->lastToken())
            return candidate;
    if (lbrace_token)
        return lbrace_token + 1;
    if (dot_dot_dot_token)
        return dot_dot_dot_token + 1;
    if (base_clause_list)
        if (unsigned candidate = base_clause_list->lastToken())
            return candidate;
    if (colon_token)
        return colon_token + 1;
    if (final_token)
        return final_token + 1;
    if (name)
        if (unsigned candidate = name->lastToken())
            return candidate;
    if (attribute_list)
        if (unsigned candidate = attribute_list->lastToken())
            return candidate;
    if (classkey_token)
        return classkey_token + 1;
    return 1;
}

/** \generated */
unsigned CompoundExpressionAST::firstToken() const
{
    if (lparen_token)
        return lparen_token;
    if (statement)
        if (unsigned candidate = statement->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    return 0;
}

/** \generated */
unsigned CompoundExpressionAST::lastToken() const
{
    if (rparen_token)
        return rparen_token + 1;
    if (statement)
        if (unsigned candidate = statement->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    return 1;
}

/** \generated */
unsigned CompoundLiteralAST::firstToken() const
{
    if (lparen_token)
        return lparen_token;
    if (type_id)
        if (unsigned candidate = type_id->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    if (initializer)
        if (unsigned candidate = initializer->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned CompoundLiteralAST::lastToken() const
{
    if (initializer)
        if (unsigned candidate = initializer->lastToken())
            return candidate;
    if (rparen_token)
        return rparen_token + 1;
    if (type_id)
        if (unsigned candidate = type_id->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    return 1;
}

/** \generated */
unsigned CompoundStatementAST::firstToken() const
{
    if (lbrace_token)
        return lbrace_token;
    if (statement_list)
        if (unsigned candidate = statement_list->firstToken())
            return candidate;
    if (rbrace_token)
        return rbrace_token;
    return 0;
}

/** \generated */
unsigned CompoundStatementAST::lastToken() const
{
    if (rbrace_token)
        return rbrace_token + 1;
    if (statement_list)
        if (unsigned candidate = statement_list->lastToken())
            return candidate;
    if (lbrace_token)
        return lbrace_token + 1;
    return 1;
}

/** \generated */
unsigned ConditionAST::firstToken() const
{
    if (type_specifier_list)
        if (unsigned candidate = type_specifier_list->firstToken())
            return candidate;
    if (declarator)
        if (unsigned candidate = declarator->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned ConditionAST::lastToken() const
{
    if (declarator)
        if (unsigned candidate = declarator->lastToken())
            return candidate;
    if (type_specifier_list)
        if (unsigned candidate = type_specifier_list->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned ConditionalExpressionAST::firstToken() const
{
    if (condition)
        if (unsigned candidate = condition->firstToken())
            return candidate;
    if (question_token)
        return question_token;
    if (left_expression)
        if (unsigned candidate = left_expression->firstToken())
            return candidate;
    if (colon_token)
        return colon_token;
    if (right_expression)
        if (unsigned candidate = right_expression->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned ConditionalExpressionAST::lastToken() const
{
    if (right_expression)
        if (unsigned candidate = right_expression->lastToken())
            return candidate;
    if (colon_token)
        return colon_token + 1;
    if (left_expression)
        if (unsigned candidate = left_expression->lastToken())
            return candidate;
    if (question_token)
        return question_token + 1;
    if (condition)
        if (unsigned candidate = condition->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned ContinueStatementAST::firstToken() const
{
    if (continue_token)
        return continue_token;
    if (semicolon_token)
        return semicolon_token;
    return 0;
}

/** \generated */
unsigned ContinueStatementAST::lastToken() const
{
    if (semicolon_token)
        return semicolon_token + 1;
    if (continue_token)
        return continue_token + 1;
    return 1;
}

/** \generated */
unsigned ConversionFunctionIdAST::firstToken() const
{
    if (operator_token)
        return operator_token;
    if (type_specifier_list)
        if (unsigned candidate = type_specifier_list->firstToken())
            return candidate;
    if (ptr_operator_list)
        if (unsigned candidate = ptr_operator_list->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned ConversionFunctionIdAST::lastToken() const
{
    if (ptr_operator_list)
        if (unsigned candidate = ptr_operator_list->lastToken())
            return candidate;
    if (type_specifier_list)
        if (unsigned candidate = type_specifier_list->lastToken())
            return candidate;
    if (operator_token)
        return operator_token + 1;
    return 1;
}

/** \generated */
unsigned CppCastExpressionAST::firstToken() const
{
    if (cast_token)
        return cast_token;
    if (less_token)
        return less_token;
    if (type_id)
        if (unsigned candidate = type_id->firstToken())
            return candidate;
    if (greater_token)
        return greater_token;
    if (lparen_token)
        return lparen_token;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    return 0;
}

/** \generated */
unsigned CppCastExpressionAST::lastToken() const
{
    if (rparen_token)
        return rparen_token + 1;
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (greater_token)
        return greater_token + 1;
    if (type_id)
        if (unsigned candidate = type_id->lastToken())
            return candidate;
    if (less_token)
        return less_token + 1;
    if (cast_token)
        return cast_token + 1;
    return 1;
}

/** \generated */
unsigned CtorInitializerAST::firstToken() const
{
    if (colon_token)
        return colon_token;
    if (member_initializer_list)
        if (unsigned candidate = member_initializer_list->firstToken())
            return candidate;
    if (dot_dot_dot_token)
        return dot_dot_dot_token;
    return 0;
}

/** \generated */
unsigned CtorInitializerAST::lastToken() const
{
    if (dot_dot_dot_token)
        return dot_dot_dot_token + 1;
    if (member_initializer_list)
        if (unsigned candidate = member_initializer_list->lastToken())
            return candidate;
    if (colon_token)
        return colon_token + 1;
    return 1;
}

/** \generated */
unsigned DeclarationStatementAST::firstToken() const
{
    if (declaration)
        if (unsigned candidate = declaration->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned DeclarationStatementAST::lastToken() const
{
    if (declaration)
        if (unsigned candidate = declaration->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned DeclaratorAST::firstToken() const
{
    if (attribute_list)
        if (unsigned candidate = attribute_list->firstToken())
            return candidate;
    if (ptr_operator_list)
        if (unsigned candidate = ptr_operator_list->firstToken())
            return candidate;
    if (core_declarator)
        if (unsigned candidate = core_declarator->firstToken())
            return candidate;
    if (postfix_declarator_list)
        if (unsigned candidate = postfix_declarator_list->firstToken())
            return candidate;
    if (post_attribute_list)
        if (unsigned candidate = post_attribute_list->firstToken())
            return candidate;
    if (equal_token)
        return equal_token;
    if (initializer)
        if (unsigned candidate = initializer->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned DeclaratorAST::lastToken() const
{
    if (initializer)
        if (unsigned candidate = initializer->lastToken())
            return candidate;
    if (equal_token)
        return equal_token + 1;
    if (post_attribute_list)
        if (unsigned candidate = post_attribute_list->lastToken())
            return candidate;
    if (postfix_declarator_list)
        if (unsigned candidate = postfix_declarator_list->lastToken())
            return candidate;
    if (core_declarator)
        if (unsigned candidate = core_declarator->lastToken())
            return candidate;
    if (ptr_operator_list)
        if (unsigned candidate = ptr_operator_list->lastToken())
            return candidate;
    if (attribute_list)
        if (unsigned candidate = attribute_list->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned DeclaratorIdAST::firstToken() const
{
    if (dot_dot_dot_token)
        return dot_dot_dot_token;
    if (name)
        if (unsigned candidate = name->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned DeclaratorIdAST::lastToken() const
{
    if (name)
        if (unsigned candidate = name->lastToken())
            return candidate;
    if (dot_dot_dot_token)
        return dot_dot_dot_token + 1;
    return 1;
}

/** \generated */
unsigned DeleteExpressionAST::firstToken() const
{
    if (scope_token)
        return scope_token;
    if (delete_token)
        return delete_token;
    if (lbracket_token)
        return lbracket_token;
    if (rbracket_token)
        return rbracket_token;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned DeleteExpressionAST::lastToken() const
{
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (rbracket_token)
        return rbracket_token + 1;
    if (lbracket_token)
        return lbracket_token + 1;
    if (delete_token)
        return delete_token + 1;
    if (scope_token)
        return scope_token + 1;
    return 1;
}

/** \generated */
unsigned DestructorNameAST::firstToken() const
{
    if (tilde_token)
        return tilde_token;
    if (unqualified_name)
        if (unsigned candidate = unqualified_name->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned DestructorNameAST::lastToken() const
{
    if (unqualified_name)
        if (unsigned candidate = unqualified_name->lastToken())
            return candidate;
    if (tilde_token)
        return tilde_token + 1;
    return 1;
}

/** \generated */
unsigned DoStatementAST::firstToken() const
{
    if (do_token)
        return do_token;
    if (statement)
        if (unsigned candidate = statement->firstToken())
            return candidate;
    if (while_token)
        return while_token;
    if (lparen_token)
        return lparen_token;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    if (semicolon_token)
        return semicolon_token;
    return 0;
}

/** \generated */
unsigned DoStatementAST::lastToken() const
{
    if (semicolon_token)
        return semicolon_token + 1;
    if (rparen_token)
        return rparen_token + 1;
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (while_token)
        return while_token + 1;
    if (statement)
        if (unsigned candidate = statement->lastToken())
            return candidate;
    if (do_token)
        return do_token + 1;
    return 1;
}

/** \generated */
unsigned ElaboratedTypeSpecifierAST::firstToken() const
{
    if (classkey_token)
        return classkey_token;
    if (attribute_list)
        if (unsigned candidate = attribute_list->firstToken())
            return candidate;
    if (name)
        if (unsigned candidate = name->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned ElaboratedTypeSpecifierAST::lastToken() const
{
    if (name)
        if (unsigned candidate = name->lastToken())
            return candidate;
    if (attribute_list)
        if (unsigned candidate = attribute_list->lastToken())
            return candidate;
    if (classkey_token)
        return classkey_token + 1;
    return 1;
}

/** \generated */
unsigned EmptyDeclarationAST::firstToken() const
{
    if (semicolon_token)
        return semicolon_token;
    return 0;
}

/** \generated */
unsigned EmptyDeclarationAST::lastToken() const
{
    if (semicolon_token)
        return semicolon_token + 1;
    return 1;
}

/** \generated */
unsigned EnumSpecifierAST::firstToken() const
{
    if (enum_token)
        return enum_token;
    if (key_token)
        return key_token;
    if (name)
        if (unsigned candidate = name->firstToken())
            return candidate;
    if (colon_token)
        return colon_token;
    if (type_specifier_list)
        if (unsigned candidate = type_specifier_list->firstToken())
            return candidate;
    if (lbrace_token)
        return lbrace_token;
    if (enumerator_list)
        if (unsigned candidate = enumerator_list->firstToken())
            return candidate;
    if (stray_comma_token)
        return stray_comma_token;
    if (rbrace_token)
        return rbrace_token;
    return 0;
}

/** \generated */
unsigned EnumSpecifierAST::lastToken() const
{
    if (rbrace_token)
        return rbrace_token + 1;
    if (stray_comma_token)
        return stray_comma_token + 1;
    if (enumerator_list)
        if (unsigned candidate = enumerator_list->lastToken())
            return candidate;
    if (lbrace_token)
        return lbrace_token + 1;
    if (type_specifier_list)
        if (unsigned candidate = type_specifier_list->lastToken())
            return candidate;
    if (colon_token)
        return colon_token + 1;
    if (name)
        if (unsigned candidate = name->lastToken())
            return candidate;
    if (key_token)
        return key_token + 1;
    if (enum_token)
        return enum_token + 1;
    return 1;
}

/** \generated */
unsigned EnumeratorAST::firstToken() const
{
    if (identifier_token)
        return identifier_token;
    if (equal_token)
        return equal_token;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned EnumeratorAST::lastToken() const
{
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (equal_token)
        return equal_token + 1;
    if (identifier_token)
        return identifier_token + 1;
    return 1;
}

/** \generated */
unsigned ExceptionDeclarationAST::firstToken() const
{
    if (type_specifier_list)
        if (unsigned candidate = type_specifier_list->firstToken())
            return candidate;
    if (declarator)
        if (unsigned candidate = declarator->firstToken())
            return candidate;
    if (dot_dot_dot_token)
        return dot_dot_dot_token;
    return 0;
}

/** \generated */
unsigned ExceptionDeclarationAST::lastToken() const
{
    if (dot_dot_dot_token)
        return dot_dot_dot_token + 1;
    if (declarator)
        if (unsigned candidate = declarator->lastToken())
            return candidate;
    if (type_specifier_list)
        if (unsigned candidate = type_specifier_list->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned DynamicExceptionSpecificationAST::firstToken() const
{
    if (throw_token)
        return throw_token;
    if (lparen_token)
        return lparen_token;
    if (dot_dot_dot_token)
        return dot_dot_dot_token;
    if (type_id_list)
        if (unsigned candidate = type_id_list->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    return 0;
}

/** \generated */
unsigned DynamicExceptionSpecificationAST::lastToken() const
{
    if (rparen_token)
        return rparen_token + 1;
    if (type_id_list)
        if (unsigned candidate = type_id_list->lastToken())
            return candidate;
    if (dot_dot_dot_token)
        return dot_dot_dot_token + 1;
    if (lparen_token)
        return lparen_token + 1;
    if (throw_token)
        return throw_token + 1;
    return 1;
}

/** \generated */
unsigned ExpressionOrDeclarationStatementAST::firstToken() const
{
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    if (declaration)
        if (unsigned candidate = declaration->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned ExpressionOrDeclarationStatementAST::lastToken() const
{
    if (declaration)
        if (unsigned candidate = declaration->lastToken())
            return candidate;
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned ExpressionStatementAST::firstToken() const
{
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    if (semicolon_token)
        return semicolon_token;
    return 0;
}

/** \generated */
unsigned ExpressionStatementAST::lastToken() const
{
    if (semicolon_token)
        return semicolon_token + 1;
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned ForStatementAST::firstToken() const
{
    if (for_token)
        return for_token;
    if (lparen_token)
        return lparen_token;
    if (initializer)
        if (unsigned candidate = initializer->firstToken())
            return candidate;
    if (condition)
        if (unsigned candidate = condition->firstToken())
            return candidate;
    if (semicolon_token)
        return semicolon_token;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    if (statement)
        if (unsigned candidate = statement->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned ForStatementAST::lastToken() const
{
    if (statement)
        if (unsigned candidate = statement->lastToken())
            return candidate;
    if (rparen_token)
        return rparen_token + 1;
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (semicolon_token)
        return semicolon_token + 1;
    if (condition)
        if (unsigned candidate = condition->lastToken())
            return candidate;
    if (initializer)
        if (unsigned candidate = initializer->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (for_token)
        return for_token + 1;
    return 1;
}

/** \generated */
unsigned ForeachStatementAST::firstToken() const
{
    if (foreach_token)
        return foreach_token;
    if (lparen_token)
        return lparen_token;
    if (type_specifier_list)
        if (unsigned candidate = type_specifier_list->firstToken())
            return candidate;
    if (declarator)
        if (unsigned candidate = declarator->firstToken())
            return candidate;
    if (initializer)
        if (unsigned candidate = initializer->firstToken())
            return candidate;
    if (comma_token)
        return comma_token;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    if (statement)
        if (unsigned candidate = statement->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned ForeachStatementAST::lastToken() const
{
    if (statement)
        if (unsigned candidate = statement->lastToken())
            return candidate;
    if (rparen_token)
        return rparen_token + 1;
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (comma_token)
        return comma_token + 1;
    if (initializer)
        if (unsigned candidate = initializer->lastToken())
            return candidate;
    if (declarator)
        if (unsigned candidate = declarator->lastToken())
            return candidate;
    if (type_specifier_list)
        if (unsigned candidate = type_specifier_list->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (foreach_token)
        return foreach_token + 1;
    return 1;
}

/** \generated */
unsigned FunctionDeclaratorAST::firstToken() const
{
    if (lparen_token)
        return lparen_token;
    if (parameter_declaration_clause)
        if (unsigned candidate = parameter_declaration_clause->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    if (cv_qualifier_list)
        if (unsigned candidate = cv_qualifier_list->firstToken())
            return candidate;
    if (ref_qualifier_token)
        return ref_qualifier_token;
    if (exception_specification)
        if (unsigned candidate = exception_specification->firstToken())
            return candidate;
    if (trailing_return_type)
        if (unsigned candidate = trailing_return_type->firstToken())
            return candidate;
    if (as_cpp_initializer)
        if (unsigned candidate = as_cpp_initializer->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned FunctionDeclaratorAST::lastToken() const
{
    if (as_cpp_initializer)
        if (unsigned candidate = as_cpp_initializer->lastToken())
            return candidate;
    if (trailing_return_type)
        if (unsigned candidate = trailing_return_type->lastToken())
            return candidate;
    if (exception_specification)
        if (unsigned candidate = exception_specification->lastToken())
            return candidate;
    if (ref_qualifier_token)
        return ref_qualifier_token + 1;
    if (cv_qualifier_list)
        if (unsigned candidate = cv_qualifier_list->lastToken())
            return candidate;
    if (rparen_token)
        return rparen_token + 1;
    if (parameter_declaration_clause)
        if (unsigned candidate = parameter_declaration_clause->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    return 1;
}

/** \generated */
unsigned FunctionDefinitionAST::firstToken() const
{
    if (qt_invokable_token)
        return qt_invokable_token;
    if (decl_specifier_list)
        if (unsigned candidate = decl_specifier_list->firstToken())
            return candidate;
    if (declarator)
        if (unsigned candidate = declarator->firstToken())
            return candidate;
    if (ctor_initializer)
        if (unsigned candidate = ctor_initializer->firstToken())
            return candidate;
    if (function_body)
        if (unsigned candidate = function_body->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned FunctionDefinitionAST::lastToken() const
{
    if (function_body)
        if (unsigned candidate = function_body->lastToken())
            return candidate;
    if (ctor_initializer)
        if (unsigned candidate = ctor_initializer->lastToken())
            return candidate;
    if (declarator)
        if (unsigned candidate = declarator->lastToken())
            return candidate;
    if (decl_specifier_list)
        if (unsigned candidate = decl_specifier_list->lastToken())
            return candidate;
    if (qt_invokable_token)
        return qt_invokable_token + 1;
    return 1;
}

/** \generated */
unsigned GotoStatementAST::firstToken() const
{
    if (goto_token)
        return goto_token;
    if (identifier_token)
        return identifier_token;
    if (semicolon_token)
        return semicolon_token;
    return 0;
}

/** \generated */
unsigned GotoStatementAST::lastToken() const
{
    if (semicolon_token)
        return semicolon_token + 1;
    if (identifier_token)
        return identifier_token + 1;
    if (goto_token)
        return goto_token + 1;
    return 1;
}

/** \generated */
unsigned IdExpressionAST::firstToken() const
{
    if (name)
        if (unsigned candidate = name->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned IdExpressionAST::lastToken() const
{
    if (name)
        if (unsigned candidate = name->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned IfStatementAST::firstToken() const
{
    if (if_token)
        return if_token;
    if (lparen_token)
        return lparen_token;
    if (condition)
        if (unsigned candidate = condition->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    if (statement)
        if (unsigned candidate = statement->firstToken())
            return candidate;
    if (else_token)
        return else_token;
    if (else_statement)
        if (unsigned candidate = else_statement->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned IfStatementAST::lastToken() const
{
    if (else_statement)
        if (unsigned candidate = else_statement->lastToken())
            return candidate;
    if (else_token)
        return else_token + 1;
    if (statement)
        if (unsigned candidate = statement->lastToken())
            return candidate;
    if (rparen_token)
        return rparen_token + 1;
    if (condition)
        if (unsigned candidate = condition->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (if_token)
        return if_token + 1;
    return 1;
}

/** \generated */
unsigned LabeledStatementAST::firstToken() const
{
    if (label_token)
        return label_token;
    if (colon_token)
        return colon_token;
    if (statement)
        if (unsigned candidate = statement->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned LabeledStatementAST::lastToken() const
{
    if (statement)
        if (unsigned candidate = statement->lastToken())
            return candidate;
    if (colon_token)
        return colon_token + 1;
    if (label_token)
        return label_token + 1;
    return 1;
}

/** \generated */
unsigned LambdaCaptureAST::firstToken() const
{
    if (default_capture_token)
        return default_capture_token;
    if (capture_list)
        if (unsigned candidate = capture_list->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned LambdaCaptureAST::lastToken() const
{
    if (capture_list)
        if (unsigned candidate = capture_list->lastToken())
            return candidate;
    if (default_capture_token)
        return default_capture_token + 1;
    return 1;
}

/** \generated */
unsigned LambdaDeclaratorAST::firstToken() const
{
    if (lparen_token)
        return lparen_token;
    if (parameter_declaration_clause)
        if (unsigned candidate = parameter_declaration_clause->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    if (attributes)
        if (unsigned candidate = attributes->firstToken())
            return candidate;
    if (mutable_token)
        return mutable_token;
    if (exception_specification)
        if (unsigned candidate = exception_specification->firstToken())
            return candidate;
    if (trailing_return_type)
        if (unsigned candidate = trailing_return_type->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned LambdaDeclaratorAST::lastToken() const
{
    if (trailing_return_type)
        if (unsigned candidate = trailing_return_type->lastToken())
            return candidate;
    if (exception_specification)
        if (unsigned candidate = exception_specification->lastToken())
            return candidate;
    if (mutable_token)
        return mutable_token + 1;
    if (attributes)
        if (unsigned candidate = attributes->lastToken())
            return candidate;
    if (rparen_token)
        return rparen_token + 1;
    if (parameter_declaration_clause)
        if (unsigned candidate = parameter_declaration_clause->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    return 1;
}

/** \generated */
unsigned LambdaExpressionAST::firstToken() const
{
    if (lambda_introducer)
        if (unsigned candidate = lambda_introducer->firstToken())
            return candidate;
    if (lambda_declarator)
        if (unsigned candidate = lambda_declarator->firstToken())
            return candidate;
    if (statement)
        if (unsigned candidate = statement->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned LambdaExpressionAST::lastToken() const
{
    if (statement)
        if (unsigned candidate = statement->lastToken())
            return candidate;
    if (lambda_declarator)
        if (unsigned candidate = lambda_declarator->lastToken())
            return candidate;
    if (lambda_introducer)
        if (unsigned candidate = lambda_introducer->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned LambdaIntroducerAST::firstToken() const
{
    if (lbracket_token)
        return lbracket_token;
    if (lambda_capture)
        if (unsigned candidate = lambda_capture->firstToken())
            return candidate;
    if (rbracket_token)
        return rbracket_token;
    return 0;
}

/** \generated */
unsigned LambdaIntroducerAST::lastToken() const
{
    if (rbracket_token)
        return rbracket_token + 1;
    if (lambda_capture)
        if (unsigned candidate = lambda_capture->lastToken())
            return candidate;
    if (lbracket_token)
        return lbracket_token + 1;
    return 1;
}

/** \generated */
unsigned LinkageBodyAST::firstToken() const
{
    if (lbrace_token)
        return lbrace_token;
    if (declaration_list)
        if (unsigned candidate = declaration_list->firstToken())
            return candidate;
    if (rbrace_token)
        return rbrace_token;
    return 0;
}

/** \generated */
unsigned LinkageBodyAST::lastToken() const
{
    if (rbrace_token)
        return rbrace_token + 1;
    if (declaration_list)
        if (unsigned candidate = declaration_list->lastToken())
            return candidate;
    if (lbrace_token)
        return lbrace_token + 1;
    return 1;
}

/** \generated */
unsigned LinkageSpecificationAST::firstToken() const
{
    if (extern_token)
        return extern_token;
    if (extern_type_token)
        return extern_type_token;
    if (declaration)
        if (unsigned candidate = declaration->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned LinkageSpecificationAST::lastToken() const
{
    if (declaration)
        if (unsigned candidate = declaration->lastToken())
            return candidate;
    if (extern_type_token)
        return extern_type_token + 1;
    if (extern_token)
        return extern_token + 1;
    return 1;
}

/** \generated */
unsigned MemInitializerAST::firstToken() const
{
    if (name)
        if (unsigned candidate = name->firstToken())
            return candidate;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned MemInitializerAST::lastToken() const
{
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (name)
        if (unsigned candidate = name->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned MemberAccessAST::firstToken() const
{
    if (base_expression)
        if (unsigned candidate = base_expression->firstToken())
            return candidate;
    if (access_token)
        return access_token;
    if (template_token)
        return template_token;
    if (member_name)
        if (unsigned candidate = member_name->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned MemberAccessAST::lastToken() const
{
    if (member_name)
        if (unsigned candidate = member_name->lastToken())
            return candidate;
    if (template_token)
        return template_token + 1;
    if (access_token)
        return access_token + 1;
    if (base_expression)
        if (unsigned candidate = base_expression->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned NamedTypeSpecifierAST::firstToken() const
{
    if (name)
        if (unsigned candidate = name->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned NamedTypeSpecifierAST::lastToken() const
{
    if (name)
        if (unsigned candidate = name->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned NamespaceAST::firstToken() const
{
    if (inline_token)
        return inline_token;
    if (namespace_token)
        return namespace_token;
    if (identifier_token)
        return identifier_token;
    if (attribute_list)
        if (unsigned candidate = attribute_list->firstToken())
            return candidate;
    if (linkage_body)
        if (unsigned candidate = linkage_body->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned NamespaceAST::lastToken() const
{
    if (linkage_body)
        if (unsigned candidate = linkage_body->lastToken())
            return candidate;
    if (attribute_list)
        if (unsigned candidate = attribute_list->lastToken())
            return candidate;
    if (identifier_token)
        return identifier_token + 1;
    if (namespace_token)
        return namespace_token + 1;
    if (inline_token)
        return inline_token + 1;
    return 1;
}

/** \generated */
unsigned NamespaceAliasDefinitionAST::firstToken() const
{
    if (namespace_token)
        return namespace_token;
    if (namespace_name_token)
        return namespace_name_token;
    if (equal_token)
        return equal_token;
    if (name)
        if (unsigned candidate = name->firstToken())
            return candidate;
    if (semicolon_token)
        return semicolon_token;
    return 0;
}

/** \generated */
unsigned NamespaceAliasDefinitionAST::lastToken() const
{
    if (semicolon_token)
        return semicolon_token + 1;
    if (name)
        if (unsigned candidate = name->lastToken())
            return candidate;
    if (equal_token)
        return equal_token + 1;
    if (namespace_name_token)
        return namespace_name_token + 1;
    if (namespace_token)
        return namespace_token + 1;
    return 1;
}

/** \generated */
unsigned NestedDeclaratorAST::firstToken() const
{
    if (lparen_token)
        return lparen_token;
    if (declarator)
        if (unsigned candidate = declarator->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    return 0;
}

/** \generated */
unsigned NestedDeclaratorAST::lastToken() const
{
    if (rparen_token)
        return rparen_token + 1;
    if (declarator)
        if (unsigned candidate = declarator->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    return 1;
}

/** \generated */
unsigned NestedExpressionAST::firstToken() const
{
    if (lparen_token)
        return lparen_token;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    return 0;
}

/** \generated */
unsigned NestedExpressionAST::lastToken() const
{
    if (rparen_token)
        return rparen_token + 1;
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    return 1;
}

/** \generated */
unsigned NestedNameSpecifierAST::firstToken() const
{
    if (class_or_namespace_name)
        if (unsigned candidate = class_or_namespace_name->firstToken())
            return candidate;
    if (scope_token)
        return scope_token;
    return 0;
}

/** \generated */
unsigned NestedNameSpecifierAST::lastToken() const
{
    if (scope_token)
        return scope_token + 1;
    if (class_or_namespace_name)
        if (unsigned candidate = class_or_namespace_name->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned NewArrayDeclaratorAST::firstToken() const
{
    if (lbracket_token)
        return lbracket_token;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    if (rbracket_token)
        return rbracket_token;
    return 0;
}

/** \generated */
unsigned NewArrayDeclaratorAST::lastToken() const
{
    if (rbracket_token)
        return rbracket_token + 1;
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (lbracket_token)
        return lbracket_token + 1;
    return 1;
}

/** \generated */
unsigned NewExpressionAST::firstToken() const
{
    if (scope_token)
        return scope_token;
    if (new_token)
        return new_token;
    if (new_placement)
        if (unsigned candidate = new_placement->firstToken())
            return candidate;
    if (lparen_token)
        return lparen_token;
    if (type_id)
        if (unsigned candidate = type_id->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    if (new_type_id)
        if (unsigned candidate = new_type_id->firstToken())
            return candidate;
    if (new_initializer)
        if (unsigned candidate = new_initializer->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned NewExpressionAST::lastToken() const
{
    if (new_initializer)
        if (unsigned candidate = new_initializer->lastToken())
            return candidate;
    if (new_type_id)
        if (unsigned candidate = new_type_id->lastToken())
            return candidate;
    if (rparen_token)
        return rparen_token + 1;
    if (type_id)
        if (unsigned candidate = type_id->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (new_placement)
        if (unsigned candidate = new_placement->lastToken())
            return candidate;
    if (new_token)
        return new_token + 1;
    if (scope_token)
        return scope_token + 1;
    return 1;
}

/** \generated */
unsigned ExpressionListParenAST::firstToken() const
{
    if (lparen_token)
        return lparen_token;
    if (expression_list)
        if (unsigned candidate = expression_list->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    return 0;
}

/** \generated */
unsigned ExpressionListParenAST::lastToken() const
{
    if (rparen_token)
        return rparen_token + 1;
    if (expression_list)
        if (unsigned candidate = expression_list->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    return 1;
}

/** \generated */
unsigned NewTypeIdAST::firstToken() const
{
    if (type_specifier_list)
        if (unsigned candidate = type_specifier_list->firstToken())
            return candidate;
    if (ptr_operator_list)
        if (unsigned candidate = ptr_operator_list->firstToken())
            return candidate;
    if (new_array_declarator_list)
        if (unsigned candidate = new_array_declarator_list->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned NewTypeIdAST::lastToken() const
{
    if (new_array_declarator_list)
        if (unsigned candidate = new_array_declarator_list->lastToken())
            return candidate;
    if (ptr_operator_list)
        if (unsigned candidate = ptr_operator_list->lastToken())
            return candidate;
    if (type_specifier_list)
        if (unsigned candidate = type_specifier_list->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned NumericLiteralAST::firstToken() const
{
    if (literal_token)
        return literal_token;
    return 0;
}

/** \generated */
unsigned NumericLiteralAST::lastToken() const
{
    if (literal_token)
        return literal_token + 1;
    return 1;
}

/** \generated */
unsigned ObjCClassDeclarationAST::firstToken() const
{
    if (attribute_list)
        if (unsigned candidate = attribute_list->firstToken())
            return candidate;
    if (interface_token)
        return interface_token;
    if (implementation_token)
        return implementation_token;
    if (class_name)
        if (unsigned candidate = class_name->firstToken())
            return candidate;
    if (lparen_token)
        return lparen_token;
    if (category_name)
        if (unsigned candidate = category_name->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    if (colon_token)
        return colon_token;
    if (superclass)
        if (unsigned candidate = superclass->firstToken())
            return candidate;
    if (protocol_refs)
        if (unsigned candidate = protocol_refs->firstToken())
            return candidate;
    if (inst_vars_decl)
        if (unsigned candidate = inst_vars_decl->firstToken())
            return candidate;
    if (member_declaration_list)
        if (unsigned candidate = member_declaration_list->firstToken())
            return candidate;
    if (end_token)
        return end_token;
    return 0;
}

/** \generated */
unsigned ObjCClassDeclarationAST::lastToken() const
{
    if (end_token)
        return end_token + 1;
    if (member_declaration_list)
        if (unsigned candidate = member_declaration_list->lastToken())
            return candidate;
    if (inst_vars_decl)
        if (unsigned candidate = inst_vars_decl->lastToken())
            return candidate;
    if (protocol_refs)
        if (unsigned candidate = protocol_refs->lastToken())
            return candidate;
    if (superclass)
        if (unsigned candidate = superclass->lastToken())
            return candidate;
    if (colon_token)
        return colon_token + 1;
    if (rparen_token)
        return rparen_token + 1;
    if (category_name)
        if (unsigned candidate = category_name->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (class_name)
        if (unsigned candidate = class_name->lastToken())
            return candidate;
    if (implementation_token)
        return implementation_token + 1;
    if (interface_token)
        return interface_token + 1;
    if (attribute_list)
        if (unsigned candidate = attribute_list->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned ObjCClassForwardDeclarationAST::firstToken() const
{
    if (attribute_list)
        if (unsigned candidate = attribute_list->firstToken())
            return candidate;
    if (class_token)
        return class_token;
    if (identifier_list)
        if (unsigned candidate = identifier_list->firstToken())
            return candidate;
    if (semicolon_token)
        return semicolon_token;
    return 0;
}

/** \generated */
unsigned ObjCClassForwardDeclarationAST::lastToken() const
{
    if (semicolon_token)
        return semicolon_token + 1;
    if (identifier_list)
        if (unsigned candidate = identifier_list->lastToken())
            return candidate;
    if (class_token)
        return class_token + 1;
    if (attribute_list)
        if (unsigned candidate = attribute_list->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned ObjCDynamicPropertiesDeclarationAST::firstToken() const
{
    if (dynamic_token)
        return dynamic_token;
    if (property_identifier_list)
        if (unsigned candidate = property_identifier_list->firstToken())
            return candidate;
    if (semicolon_token)
        return semicolon_token;
    return 0;
}

/** \generated */
unsigned ObjCDynamicPropertiesDeclarationAST::lastToken() const
{
    if (semicolon_token)
        return semicolon_token + 1;
    if (property_identifier_list)
        if (unsigned candidate = property_identifier_list->lastToken())
            return candidate;
    if (dynamic_token)
        return dynamic_token + 1;
    return 1;
}

/** \generated */
unsigned ObjCEncodeExpressionAST::firstToken() const
{
    if (encode_token)
        return encode_token;
    if (type_name)
        if (unsigned candidate = type_name->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned ObjCEncodeExpressionAST::lastToken() const
{
    if (type_name)
        if (unsigned candidate = type_name->lastToken())
            return candidate;
    if (encode_token)
        return encode_token + 1;
    return 1;
}

/** \generated */
unsigned ObjCFastEnumerationAST::firstToken() const
{
    if (for_token)
        return for_token;
    if (lparen_token)
        return lparen_token;
    if (type_specifier_list)
        if (unsigned candidate = type_specifier_list->firstToken())
            return candidate;
    if (declarator)
        if (unsigned candidate = declarator->firstToken())
            return candidate;
    if (initializer)
        if (unsigned candidate = initializer->firstToken())
            return candidate;
    if (in_token)
        return in_token;
    if (fast_enumeratable_expression)
        if (unsigned candidate = fast_enumeratable_expression->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    if (statement)
        if (unsigned candidate = statement->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned ObjCFastEnumerationAST::lastToken() const
{
    if (statement)
        if (unsigned candidate = statement->lastToken())
            return candidate;
    if (rparen_token)
        return rparen_token + 1;
    if (fast_enumeratable_expression)
        if (unsigned candidate = fast_enumeratable_expression->lastToken())
            return candidate;
    if (in_token)
        return in_token + 1;
    if (initializer)
        if (unsigned candidate = initializer->lastToken())
            return candidate;
    if (declarator)
        if (unsigned candidate = declarator->lastToken())
            return candidate;
    if (type_specifier_list)
        if (unsigned candidate = type_specifier_list->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (for_token)
        return for_token + 1;
    return 1;
}

/** \generated */
unsigned ObjCInstanceVariablesDeclarationAST::firstToken() const
{
    if (lbrace_token)
        return lbrace_token;
    if (instance_variable_list)
        if (unsigned candidate = instance_variable_list->firstToken())
            return candidate;
    if (rbrace_token)
        return rbrace_token;
    return 0;
}

/** \generated */
unsigned ObjCInstanceVariablesDeclarationAST::lastToken() const
{
    if (rbrace_token)
        return rbrace_token + 1;
    if (instance_variable_list)
        if (unsigned candidate = instance_variable_list->lastToken())
            return candidate;
    if (lbrace_token)
        return lbrace_token + 1;
    return 1;
}

/** \generated */
unsigned ObjCMessageArgumentAST::firstToken() const
{
    if (parameter_value_expression)
        if (unsigned candidate = parameter_value_expression->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned ObjCMessageArgumentAST::lastToken() const
{
    if (parameter_value_expression)
        if (unsigned candidate = parameter_value_expression->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned ObjCMessageArgumentDeclarationAST::firstToken() const
{
    if (type_name)
        if (unsigned candidate = type_name->firstToken())
            return candidate;
    if (attribute_list)
        if (unsigned candidate = attribute_list->firstToken())
            return candidate;
    if (param_name)
        if (unsigned candidate = param_name->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned ObjCMessageArgumentDeclarationAST::lastToken() const
{
    if (param_name)
        if (unsigned candidate = param_name->lastToken())
            return candidate;
    if (attribute_list)
        if (unsigned candidate = attribute_list->lastToken())
            return candidate;
    if (type_name)
        if (unsigned candidate = type_name->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned ObjCMessageExpressionAST::firstToken() const
{
    if (lbracket_token)
        return lbracket_token;
    if (receiver_expression)
        if (unsigned candidate = receiver_expression->firstToken())
            return candidate;
    if (selector)
        if (unsigned candidate = selector->firstToken())
            return candidate;
    if (argument_list)
        if (unsigned candidate = argument_list->firstToken())
            return candidate;
    if (rbracket_token)
        return rbracket_token;
    return 0;
}

/** \generated */
unsigned ObjCMessageExpressionAST::lastToken() const
{
    if (rbracket_token)
        return rbracket_token + 1;
    if (argument_list)
        if (unsigned candidate = argument_list->lastToken())
            return candidate;
    if (selector)
        if (unsigned candidate = selector->lastToken())
            return candidate;
    if (receiver_expression)
        if (unsigned candidate = receiver_expression->lastToken())
            return candidate;
    if (lbracket_token)
        return lbracket_token + 1;
    return 1;
}

/** \generated */
unsigned ObjCMethodDeclarationAST::firstToken() const
{
    if (method_prototype)
        if (unsigned candidate = method_prototype->firstToken())
            return candidate;
    if (function_body)
        if (unsigned candidate = function_body->firstToken())
            return candidate;
    if (semicolon_token)
        return semicolon_token;
    return 0;
}

/** \generated */
unsigned ObjCMethodDeclarationAST::lastToken() const
{
    if (semicolon_token)
        return semicolon_token + 1;
    if (function_body)
        if (unsigned candidate = function_body->lastToken())
            return candidate;
    if (method_prototype)
        if (unsigned candidate = method_prototype->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned ObjCMethodPrototypeAST::firstToken() const
{
    if (method_type_token)
        return method_type_token;
    if (type_name)
        if (unsigned candidate = type_name->firstToken())
            return candidate;
    if (selector)
        if (unsigned candidate = selector->firstToken())
            return candidate;
    if (argument_list)
        if (unsigned candidate = argument_list->firstToken())
            return candidate;
    if (dot_dot_dot_token)
        return dot_dot_dot_token;
    if (attribute_list)
        if (unsigned candidate = attribute_list->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned ObjCMethodPrototypeAST::lastToken() const
{
    if (attribute_list)
        if (unsigned candidate = attribute_list->lastToken())
            return candidate;
    if (dot_dot_dot_token)
        return dot_dot_dot_token + 1;
    if (argument_list)
        if (unsigned candidate = argument_list->lastToken())
            return candidate;
    if (selector)
        if (unsigned candidate = selector->lastToken())
            return candidate;
    if (type_name)
        if (unsigned candidate = type_name->lastToken())
            return candidate;
    if (method_type_token)
        return method_type_token + 1;
    return 1;
}

/** \generated */
unsigned ObjCPropertyAttributeAST::firstToken() const
{
    if (attribute_identifier_token)
        return attribute_identifier_token;
    if (equals_token)
        return equals_token;
    if (method_selector)
        if (unsigned candidate = method_selector->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned ObjCPropertyAttributeAST::lastToken() const
{
    if (method_selector)
        if (unsigned candidate = method_selector->lastToken())
            return candidate;
    if (equals_token)
        return equals_token + 1;
    if (attribute_identifier_token)
        return attribute_identifier_token + 1;
    return 1;
}

/** \generated */
unsigned ObjCPropertyDeclarationAST::firstToken() const
{
    if (attribute_list)
        if (unsigned candidate = attribute_list->firstToken())
            return candidate;
    if (property_token)
        return property_token;
    if (lparen_token)
        return lparen_token;
    if (property_attribute_list)
        if (unsigned candidate = property_attribute_list->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    if (simple_declaration)
        if (unsigned candidate = simple_declaration->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned ObjCPropertyDeclarationAST::lastToken() const
{
    if (simple_declaration)
        if (unsigned candidate = simple_declaration->lastToken())
            return candidate;
    if (rparen_token)
        return rparen_token + 1;
    if (property_attribute_list)
        if (unsigned candidate = property_attribute_list->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (property_token)
        return property_token + 1;
    if (attribute_list)
        if (unsigned candidate = attribute_list->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned ObjCProtocolDeclarationAST::firstToken() const
{
    if (attribute_list)
        if (unsigned candidate = attribute_list->firstToken())
            return candidate;
    if (protocol_token)
        return protocol_token;
    if (name)
        if (unsigned candidate = name->firstToken())
            return candidate;
    if (protocol_refs)
        if (unsigned candidate = protocol_refs->firstToken())
            return candidate;
    if (member_declaration_list)
        if (unsigned candidate = member_declaration_list->firstToken())
            return candidate;
    if (end_token)
        return end_token;
    return 0;
}

/** \generated */
unsigned ObjCProtocolDeclarationAST::lastToken() const
{
    if (end_token)
        return end_token + 1;
    if (member_declaration_list)
        if (unsigned candidate = member_declaration_list->lastToken())
            return candidate;
    if (protocol_refs)
        if (unsigned candidate = protocol_refs->lastToken())
            return candidate;
    if (name)
        if (unsigned candidate = name->lastToken())
            return candidate;
    if (protocol_token)
        return protocol_token + 1;
    if (attribute_list)
        if (unsigned candidate = attribute_list->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned ObjCProtocolExpressionAST::firstToken() const
{
    if (protocol_token)
        return protocol_token;
    if (lparen_token)
        return lparen_token;
    if (identifier_token)
        return identifier_token;
    if (rparen_token)
        return rparen_token;
    return 0;
}

/** \generated */
unsigned ObjCProtocolExpressionAST::lastToken() const
{
    if (rparen_token)
        return rparen_token + 1;
    if (identifier_token)
        return identifier_token + 1;
    if (lparen_token)
        return lparen_token + 1;
    if (protocol_token)
        return protocol_token + 1;
    return 1;
}

/** \generated */
unsigned ObjCProtocolForwardDeclarationAST::firstToken() const
{
    if (attribute_list)
        if (unsigned candidate = attribute_list->firstToken())
            return candidate;
    if (protocol_token)
        return protocol_token;
    if (identifier_list)
        if (unsigned candidate = identifier_list->firstToken())
            return candidate;
    if (semicolon_token)
        return semicolon_token;
    return 0;
}

/** \generated */
unsigned ObjCProtocolForwardDeclarationAST::lastToken() const
{
    if (semicolon_token)
        return semicolon_token + 1;
    if (identifier_list)
        if (unsigned candidate = identifier_list->lastToken())
            return candidate;
    if (protocol_token)
        return protocol_token + 1;
    if (attribute_list)
        if (unsigned candidate = attribute_list->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned ObjCProtocolRefsAST::firstToken() const
{
    if (less_token)
        return less_token;
    if (identifier_list)
        if (unsigned candidate = identifier_list->firstToken())
            return candidate;
    if (greater_token)
        return greater_token;
    return 0;
}

/** \generated */
unsigned ObjCProtocolRefsAST::lastToken() const
{
    if (greater_token)
        return greater_token + 1;
    if (identifier_list)
        if (unsigned candidate = identifier_list->lastToken())
            return candidate;
    if (less_token)
        return less_token + 1;
    return 1;
}

/** \generated */
unsigned ObjCSelectorAST::firstToken() const
{
    if (selector_argument_list)
        if (unsigned candidate = selector_argument_list->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned ObjCSelectorAST::lastToken() const
{
    if (selector_argument_list)
        if (unsigned candidate = selector_argument_list->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned ObjCSelectorArgumentAST::firstToken() const
{
    if (name_token)
        return name_token;
    if (colon_token)
        return colon_token;
    return 0;
}

/** \generated */
unsigned ObjCSelectorArgumentAST::lastToken() const
{
    if (colon_token)
        return colon_token + 1;
    if (name_token)
        return name_token + 1;
    return 1;
}

/** \generated */
unsigned ObjCSelectorExpressionAST::firstToken() const
{
    if (selector_token)
        return selector_token;
    if (lparen_token)
        return lparen_token;
    if (selector)
        if (unsigned candidate = selector->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    return 0;
}

/** \generated */
unsigned ObjCSelectorExpressionAST::lastToken() const
{
    if (rparen_token)
        return rparen_token + 1;
    if (selector)
        if (unsigned candidate = selector->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (selector_token)
        return selector_token + 1;
    return 1;
}

/** \generated */
unsigned ObjCSynchronizedStatementAST::firstToken() const
{
    if (synchronized_token)
        return synchronized_token;
    if (lparen_token)
        return lparen_token;
    if (synchronized_object)
        if (unsigned candidate = synchronized_object->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    if (statement)
        if (unsigned candidate = statement->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned ObjCSynchronizedStatementAST::lastToken() const
{
    if (statement)
        if (unsigned candidate = statement->lastToken())
            return candidate;
    if (rparen_token)
        return rparen_token + 1;
    if (synchronized_object)
        if (unsigned candidate = synchronized_object->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (synchronized_token)
        return synchronized_token + 1;
    return 1;
}

/** \generated */
unsigned ObjCSynthesizedPropertiesDeclarationAST::firstToken() const
{
    if (synthesized_token)
        return synthesized_token;
    if (property_identifier_list)
        if (unsigned candidate = property_identifier_list->firstToken())
            return candidate;
    if (semicolon_token)
        return semicolon_token;
    return 0;
}

/** \generated */
unsigned ObjCSynthesizedPropertiesDeclarationAST::lastToken() const
{
    if (semicolon_token)
        return semicolon_token + 1;
    if (property_identifier_list)
        if (unsigned candidate = property_identifier_list->lastToken())
            return candidate;
    if (synthesized_token)
        return synthesized_token + 1;
    return 1;
}

/** \generated */
unsigned ObjCSynthesizedPropertyAST::firstToken() const
{
    if (property_identifier_token)
        return property_identifier_token;
    if (equals_token)
        return equals_token;
    if (alias_identifier_token)
        return alias_identifier_token;
    return 0;
}

/** \generated */
unsigned ObjCSynthesizedPropertyAST::lastToken() const
{
    if (alias_identifier_token)
        return alias_identifier_token + 1;
    if (equals_token)
        return equals_token + 1;
    if (property_identifier_token)
        return property_identifier_token + 1;
    return 1;
}

/** \generated */
unsigned ObjCTypeNameAST::firstToken() const
{
    if (lparen_token)
        return lparen_token;
    if (type_qualifier_token)
        return type_qualifier_token;
    if (type_id)
        if (unsigned candidate = type_id->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    return 0;
}

/** \generated */
unsigned ObjCTypeNameAST::lastToken() const
{
    if (rparen_token)
        return rparen_token + 1;
    if (type_id)
        if (unsigned candidate = type_id->lastToken())
            return candidate;
    if (type_qualifier_token)
        return type_qualifier_token + 1;
    if (lparen_token)
        return lparen_token + 1;
    return 1;
}

/** \generated */
unsigned ObjCVisibilityDeclarationAST::firstToken() const
{
    if (visibility_token)
        return visibility_token;
    return 0;
}

/** \generated */
unsigned ObjCVisibilityDeclarationAST::lastToken() const
{
    if (visibility_token)
        return visibility_token + 1;
    return 1;
}

/** \generated */
unsigned OperatorAST::firstToken() const
{
    if (op_token)
        return op_token;
    if (open_token)
        return open_token;
    if (close_token)
        return close_token;
    return 0;
}

/** \generated */
unsigned OperatorAST::lastToken() const
{
    if (close_token)
        return close_token + 1;
    if (open_token)
        return open_token + 1;
    if (op_token)
        return op_token + 1;
    return 1;
}

/** \generated */
unsigned OperatorFunctionIdAST::firstToken() const
{
    if (operator_token)
        return operator_token;
    if (op)
        if (unsigned candidate = op->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned OperatorFunctionIdAST::lastToken() const
{
    if (op)
        if (unsigned candidate = op->lastToken())
            return candidate;
    if (operator_token)
        return operator_token + 1;
    return 1;
}

/** \generated */
unsigned ParameterDeclarationAST::firstToken() const
{
    if (type_specifier_list)
        if (unsigned candidate = type_specifier_list->firstToken())
            return candidate;
    if (declarator)
        if (unsigned candidate = declarator->firstToken())
            return candidate;
    if (equal_token)
        return equal_token;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned ParameterDeclarationAST::lastToken() const
{
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (equal_token)
        return equal_token + 1;
    if (declarator)
        if (unsigned candidate = declarator->lastToken())
            return candidate;
    if (type_specifier_list)
        if (unsigned candidate = type_specifier_list->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned ParameterDeclarationClauseAST::firstToken() const
{
    if (parameter_declaration_list)
        if (unsigned candidate = parameter_declaration_list->firstToken())
            return candidate;
    if (dot_dot_dot_token)
        return dot_dot_dot_token;
    return 0;
}

/** \generated */
unsigned ParameterDeclarationClauseAST::lastToken() const
{
    if (dot_dot_dot_token)
        return dot_dot_dot_token + 1;
    if (parameter_declaration_list)
        if (unsigned candidate = parameter_declaration_list->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned PointerAST::firstToken() const
{
    if (star_token)
        return star_token;
    if (cv_qualifier_list)
        if (unsigned candidate = cv_qualifier_list->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned PointerAST::lastToken() const
{
    if (cv_qualifier_list)
        if (unsigned candidate = cv_qualifier_list->lastToken())
            return candidate;
    if (star_token)
        return star_token + 1;
    return 1;
}

/** \generated */
unsigned PointerToMemberAST::firstToken() const
{
    if (global_scope_token)
        return global_scope_token;
    if (nested_name_specifier_list)
        if (unsigned candidate = nested_name_specifier_list->firstToken())
            return candidate;
    if (star_token)
        return star_token;
    if (cv_qualifier_list)
        if (unsigned candidate = cv_qualifier_list->firstToken())
            return candidate;
    if (ref_qualifier_token)
        return ref_qualifier_token;
    return 0;
}

/** \generated */
unsigned PointerToMemberAST::lastToken() const
{
    if (ref_qualifier_token)
        return ref_qualifier_token + 1;
    if (cv_qualifier_list)
        if (unsigned candidate = cv_qualifier_list->lastToken())
            return candidate;
    if (star_token)
        return star_token + 1;
    if (nested_name_specifier_list)
        if (unsigned candidate = nested_name_specifier_list->lastToken())
            return candidate;
    if (global_scope_token)
        return global_scope_token + 1;
    return 1;
}

/** \generated */
unsigned PostIncrDecrAST::firstToken() const
{
    if (base_expression)
        if (unsigned candidate = base_expression->firstToken())
            return candidate;
    if (incr_decr_token)
        return incr_decr_token;
    return 0;
}

/** \generated */
unsigned PostIncrDecrAST::lastToken() const
{
    if (incr_decr_token)
        return incr_decr_token + 1;
    if (base_expression)
        if (unsigned candidate = base_expression->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned QtEnumDeclarationAST::firstToken() const
{
    if (enum_specifier_token)
        return enum_specifier_token;
    if (lparen_token)
        return lparen_token;
    if (enumerator_list)
        if (unsigned candidate = enumerator_list->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    return 0;
}

/** \generated */
unsigned QtEnumDeclarationAST::lastToken() const
{
    if (rparen_token)
        return rparen_token + 1;
    if (enumerator_list)
        if (unsigned candidate = enumerator_list->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (enum_specifier_token)
        return enum_specifier_token + 1;
    return 1;
}

/** \generated */
unsigned QtFlagsDeclarationAST::firstToken() const
{
    if (flags_specifier_token)
        return flags_specifier_token;
    if (lparen_token)
        return lparen_token;
    if (flag_enums_list)
        if (unsigned candidate = flag_enums_list->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    return 0;
}

/** \generated */
unsigned QtFlagsDeclarationAST::lastToken() const
{
    if (rparen_token)
        return rparen_token + 1;
    if (flag_enums_list)
        if (unsigned candidate = flag_enums_list->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (flags_specifier_token)
        return flags_specifier_token + 1;
    return 1;
}

/** \generated */
unsigned QtInterfaceNameAST::firstToken() const
{
    if (interface_name)
        if (unsigned candidate = interface_name->firstToken())
            return candidate;
    if (constraint_list)
        if (unsigned candidate = constraint_list->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned QtInterfaceNameAST::lastToken() const
{
    if (constraint_list)
        if (unsigned candidate = constraint_list->lastToken())
            return candidate;
    if (interface_name)
        if (unsigned candidate = interface_name->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned QtInterfacesDeclarationAST::firstToken() const
{
    if (interfaces_token)
        return interfaces_token;
    if (lparen_token)
        return lparen_token;
    if (interface_name_list)
        if (unsigned candidate = interface_name_list->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    return 0;
}

/** \generated */
unsigned QtInterfacesDeclarationAST::lastToken() const
{
    if (rparen_token)
        return rparen_token + 1;
    if (interface_name_list)
        if (unsigned candidate = interface_name_list->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (interfaces_token)
        return interfaces_token + 1;
    return 1;
}

/** \generated */
unsigned QtMemberDeclarationAST::firstToken() const
{
    if (q_token)
        return q_token;
    if (lparen_token)
        return lparen_token;
    if (type_id)
        if (unsigned candidate = type_id->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    return 0;
}

/** \generated */
unsigned QtMemberDeclarationAST::lastToken() const
{
    if (rparen_token)
        return rparen_token + 1;
    if (type_id)
        if (unsigned candidate = type_id->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (q_token)
        return q_token + 1;
    return 1;
}

/** \generated */
unsigned QtMethodAST::firstToken() const
{
    if (method_token)
        return method_token;
    if (lparen_token)
        return lparen_token;
    if (declarator)
        if (unsigned candidate = declarator->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    return 0;
}

/** \generated */
unsigned QtMethodAST::lastToken() const
{
    if (rparen_token)
        return rparen_token + 1;
    if (declarator)
        if (unsigned candidate = declarator->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (method_token)
        return method_token + 1;
    return 1;
}

/** \generated */
unsigned QtObjectTagAST::firstToken() const
{
    if (q_object_token)
        return q_object_token;
    return 0;
}

/** \generated */
unsigned QtObjectTagAST::lastToken() const
{
    if (q_object_token)
        return q_object_token + 1;
    return 1;
}

/** \generated */
unsigned QtPrivateSlotAST::firstToken() const
{
    if (q_private_slot_token)
        return q_private_slot_token;
    if (lparen_token)
        return lparen_token;
    if (dptr_token)
        return dptr_token;
    if (dptr_lparen_token)
        return dptr_lparen_token;
    if (dptr_rparen_token)
        return dptr_rparen_token;
    if (comma_token)
        return comma_token;
    if (type_specifier_list)
        if (unsigned candidate = type_specifier_list->firstToken())
            return candidate;
    if (declarator)
        if (unsigned candidate = declarator->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    return 0;
}

/** \generated */
unsigned QtPrivateSlotAST::lastToken() const
{
    if (rparen_token)
        return rparen_token + 1;
    if (declarator)
        if (unsigned candidate = declarator->lastToken())
            return candidate;
    if (type_specifier_list)
        if (unsigned candidate = type_specifier_list->lastToken())
            return candidate;
    if (comma_token)
        return comma_token + 1;
    if (dptr_rparen_token)
        return dptr_rparen_token + 1;
    if (dptr_lparen_token)
        return dptr_lparen_token + 1;
    if (dptr_token)
        return dptr_token + 1;
    if (lparen_token)
        return lparen_token + 1;
    if (q_private_slot_token)
        return q_private_slot_token + 1;
    return 1;
}

/** \generated */
unsigned QtPropertyDeclarationAST::firstToken() const
{
    if (property_specifier_token)
        return property_specifier_token;
    if (lparen_token)
        return lparen_token;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    if (comma_token)
        return comma_token;
    if (type_id)
        if (unsigned candidate = type_id->firstToken())
            return candidate;
    if (property_name)
        if (unsigned candidate = property_name->firstToken())
            return candidate;
    if (property_declaration_item_list)
        if (unsigned candidate = property_declaration_item_list->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    return 0;
}

/** \generated */
unsigned QtPropertyDeclarationAST::lastToken() const
{
    if (rparen_token)
        return rparen_token + 1;
    if (property_declaration_item_list)
        if (unsigned candidate = property_declaration_item_list->lastToken())
            return candidate;
    if (property_name)
        if (unsigned candidate = property_name->lastToken())
            return candidate;
    if (type_id)
        if (unsigned candidate = type_id->lastToken())
            return candidate;
    if (comma_token)
        return comma_token + 1;
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (property_specifier_token)
        return property_specifier_token + 1;
    return 1;
}

/** \generated */
unsigned QtPropertyDeclarationItemAST::firstToken() const
{
    if (item_name_token)
        return item_name_token;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned QtPropertyDeclarationItemAST::lastToken() const
{
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (item_name_token)
        return item_name_token + 1;
    return 1;
}

/** \generated */
unsigned QualifiedNameAST::firstToken() const
{
    if (global_scope_token)
        return global_scope_token;
    if (nested_name_specifier_list)
        if (unsigned candidate = nested_name_specifier_list->firstToken())
            return candidate;
    if (unqualified_name)
        if (unsigned candidate = unqualified_name->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned QualifiedNameAST::lastToken() const
{
    if (unqualified_name)
        if (unsigned candidate = unqualified_name->lastToken())
            return candidate;
    if (nested_name_specifier_list)
        if (unsigned candidate = nested_name_specifier_list->lastToken())
            return candidate;
    if (global_scope_token)
        return global_scope_token + 1;
    return 1;
}

/** \generated */
unsigned ReferenceAST::firstToken() const
{
    if (reference_token)
        return reference_token;
    return 0;
}

/** \generated */
unsigned ReferenceAST::lastToken() const
{
    if (reference_token)
        return reference_token + 1;
    return 1;
}

/** \generated */
unsigned ReturnStatementAST::firstToken() const
{
    if (return_token)
        return return_token;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    if (semicolon_token)
        return semicolon_token;
    return 0;
}

/** \generated */
unsigned ReturnStatementAST::lastToken() const
{
    if (semicolon_token)
        return semicolon_token + 1;
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (return_token)
        return return_token + 1;
    return 1;
}

/** \generated */
unsigned SimpleDeclarationAST::firstToken() const
{
    if (qt_invokable_token)
        return qt_invokable_token;
    if (decl_specifier_list)
        if (unsigned candidate = decl_specifier_list->firstToken())
            return candidate;
    if (declarator_list)
        if (unsigned candidate = declarator_list->firstToken())
            return candidate;
    if (semicolon_token)
        return semicolon_token;
    return 0;
}

/** \generated */
unsigned SimpleDeclarationAST::lastToken() const
{
    if (semicolon_token)
        return semicolon_token + 1;
    if (declarator_list)
        if (unsigned candidate = declarator_list->lastToken())
            return candidate;
    if (decl_specifier_list)
        if (unsigned candidate = decl_specifier_list->lastToken())
            return candidate;
    if (qt_invokable_token)
        return qt_invokable_token + 1;
    return 1;
}

/** \generated */
unsigned SimpleNameAST::firstToken() const
{
    if (identifier_token)
        return identifier_token;
    return 0;
}

/** \generated */
unsigned SimpleNameAST::lastToken() const
{
    if (identifier_token)
        return identifier_token + 1;
    return 1;
}

/** \generated */
unsigned SimpleSpecifierAST::firstToken() const
{
    if (specifier_token)
        return specifier_token;
    return 0;
}

/** \generated */
unsigned SimpleSpecifierAST::lastToken() const
{
    if (specifier_token)
        return specifier_token + 1;
    return 1;
}

/** \generated */
unsigned SizeofExpressionAST::firstToken() const
{
    if (sizeof_token)
        return sizeof_token;
    if (dot_dot_dot_token)
        return dot_dot_dot_token;
    if (lparen_token)
        return lparen_token;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    return 0;
}

/** \generated */
unsigned SizeofExpressionAST::lastToken() const
{
    if (rparen_token)
        return rparen_token + 1;
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (dot_dot_dot_token)
        return dot_dot_dot_token + 1;
    if (sizeof_token)
        return sizeof_token + 1;
    return 1;
}

/** \generated */
unsigned StringLiteralAST::firstToken() const
{
    if (literal_token)
        return literal_token;
    if (next)
        if (unsigned candidate = next->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned StringLiteralAST::lastToken() const
{
    if (next)
        if (unsigned candidate = next->lastToken())
            return candidate;
    if (literal_token)
        return literal_token + 1;
    return 1;
}

/** \generated */
unsigned SwitchStatementAST::firstToken() const
{
    if (switch_token)
        return switch_token;
    if (lparen_token)
        return lparen_token;
    if (condition)
        if (unsigned candidate = condition->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    if (statement)
        if (unsigned candidate = statement->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned SwitchStatementAST::lastToken() const
{
    if (statement)
        if (unsigned candidate = statement->lastToken())
            return candidate;
    if (rparen_token)
        return rparen_token + 1;
    if (condition)
        if (unsigned candidate = condition->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (switch_token)
        return switch_token + 1;
    return 1;
}

/** \generated */
unsigned TemplateDeclarationAST::firstToken() const
{
    if (export_token)
        return export_token;
    if (template_token)
        return template_token;
    if (less_token)
        return less_token;
    if (template_parameter_list)
        if (unsigned candidate = template_parameter_list->firstToken())
            return candidate;
    if (greater_token)
        return greater_token;
    if (declaration)
        if (unsigned candidate = declaration->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned TemplateDeclarationAST::lastToken() const
{
    if (declaration)
        if (unsigned candidate = declaration->lastToken())
            return candidate;
    if (greater_token)
        return greater_token + 1;
    if (template_parameter_list)
        if (unsigned candidate = template_parameter_list->lastToken())
            return candidate;
    if (less_token)
        return less_token + 1;
    if (template_token)
        return template_token + 1;
    if (export_token)
        return export_token + 1;
    return 1;
}

/** \generated */
unsigned TemplateIdAST::firstToken() const
{
    if (template_token)
        return template_token;
    if (identifier_token)
        return identifier_token;
    if (less_token)
        return less_token;
    if (template_argument_list)
        if (unsigned candidate = template_argument_list->firstToken())
            return candidate;
    if (greater_token)
        return greater_token;
    return 0;
}

/** \generated */
unsigned TemplateIdAST::lastToken() const
{
    if (greater_token)
        return greater_token + 1;
    if (template_argument_list)
        if (unsigned candidate = template_argument_list->lastToken())
            return candidate;
    if (less_token)
        return less_token + 1;
    if (identifier_token)
        return identifier_token + 1;
    if (template_token)
        return template_token + 1;
    return 1;
}

/** \generated */
unsigned TemplateTypeParameterAST::firstToken() const
{
    if (template_token)
        return template_token;
    if (less_token)
        return less_token;
    if (template_parameter_list)
        if (unsigned candidate = template_parameter_list->firstToken())
            return candidate;
    if (greater_token)
        return greater_token;
    if (class_token)
        return class_token;
    if (dot_dot_dot_token)
        return dot_dot_dot_token;
    if (name)
        if (unsigned candidate = name->firstToken())
            return candidate;
    if (equal_token)
        return equal_token;
    if (type_id)
        if (unsigned candidate = type_id->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned TemplateTypeParameterAST::lastToken() const
{
    if (type_id)
        if (unsigned candidate = type_id->lastToken())
            return candidate;
    if (equal_token)
        return equal_token + 1;
    if (name)
        if (unsigned candidate = name->lastToken())
            return candidate;
    if (dot_dot_dot_token)
        return dot_dot_dot_token + 1;
    if (class_token)
        return class_token + 1;
    if (greater_token)
        return greater_token + 1;
    if (template_parameter_list)
        if (unsigned candidate = template_parameter_list->lastToken())
            return candidate;
    if (less_token)
        return less_token + 1;
    if (template_token)
        return template_token + 1;
    return 1;
}

/** \generated */
unsigned ThisExpressionAST::firstToken() const
{
    if (this_token)
        return this_token;
    return 0;
}

/** \generated */
unsigned ThisExpressionAST::lastToken() const
{
    if (this_token)
        return this_token + 1;
    return 1;
}

/** \generated */
unsigned ThrowExpressionAST::firstToken() const
{
    if (throw_token)
        return throw_token;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned ThrowExpressionAST::lastToken() const
{
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (throw_token)
        return throw_token + 1;
    return 1;
}

/** \generated */
unsigned TrailingReturnTypeAST::firstToken() const
{
    if (arrow_token)
        return arrow_token;
    if (attributes)
        if (unsigned candidate = attributes->firstToken())
            return candidate;
    if (type_specifier_list)
        if (unsigned candidate = type_specifier_list->firstToken())
            return candidate;
    if (declarator)
        if (unsigned candidate = declarator->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned TrailingReturnTypeAST::lastToken() const
{
    if (declarator)
        if (unsigned candidate = declarator->lastToken())
            return candidate;
    if (type_specifier_list)
        if (unsigned candidate = type_specifier_list->lastToken())
            return candidate;
    if (attributes)
        if (unsigned candidate = attributes->lastToken())
            return candidate;
    if (arrow_token)
        return arrow_token + 1;
    return 1;
}

/** \generated */
unsigned TranslationUnitAST::firstToken() const
{
    if (declaration_list)
        if (unsigned candidate = declaration_list->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned TranslationUnitAST::lastToken() const
{
    if (declaration_list)
        if (unsigned candidate = declaration_list->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned TryBlockStatementAST::firstToken() const
{
    if (try_token)
        return try_token;
    if (statement)
        if (unsigned candidate = statement->firstToken())
            return candidate;
    if (catch_clause_list)
        if (unsigned candidate = catch_clause_list->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned TryBlockStatementAST::lastToken() const
{
    if (catch_clause_list)
        if (unsigned candidate = catch_clause_list->lastToken())
            return candidate;
    if (statement)
        if (unsigned candidate = statement->lastToken())
            return candidate;
    if (try_token)
        return try_token + 1;
    return 1;
}

/** \generated */
unsigned TypeConstructorCallAST::firstToken() const
{
    if (type_specifier_list)
        if (unsigned candidate = type_specifier_list->firstToken())
            return candidate;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned TypeConstructorCallAST::lastToken() const
{
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (type_specifier_list)
        if (unsigned candidate = type_specifier_list->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned TypeIdAST::firstToken() const
{
    if (type_specifier_list)
        if (unsigned candidate = type_specifier_list->firstToken())
            return candidate;
    if (declarator)
        if (unsigned candidate = declarator->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned TypeIdAST::lastToken() const
{
    if (declarator)
        if (unsigned candidate = declarator->lastToken())
            return candidate;
    if (type_specifier_list)
        if (unsigned candidate = type_specifier_list->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned TypeidExpressionAST::firstToken() const
{
    if (typeid_token)
        return typeid_token;
    if (lparen_token)
        return lparen_token;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    return 0;
}

/** \generated */
unsigned TypeidExpressionAST::lastToken() const
{
    if (rparen_token)
        return rparen_token + 1;
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (typeid_token)
        return typeid_token + 1;
    return 1;
}

/** \generated */
unsigned TypenameCallExpressionAST::firstToken() const
{
    if (typename_token)
        return typename_token;
    if (name)
        if (unsigned candidate = name->firstToken())
            return candidate;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned TypenameCallExpressionAST::lastToken() const
{
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (name)
        if (unsigned candidate = name->lastToken())
            return candidate;
    if (typename_token)
        return typename_token + 1;
    return 1;
}

/** \generated */
unsigned TypenameTypeParameterAST::firstToken() const
{
    if (classkey_token)
        return classkey_token;
    if (dot_dot_dot_token)
        return dot_dot_dot_token;
    if (name)
        if (unsigned candidate = name->firstToken())
            return candidate;
    if (equal_token)
        return equal_token;
    if (type_id)
        if (unsigned candidate = type_id->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned TypenameTypeParameterAST::lastToken() const
{
    if (type_id)
        if (unsigned candidate = type_id->lastToken())
            return candidate;
    if (equal_token)
        return equal_token + 1;
    if (name)
        if (unsigned candidate = name->lastToken())
            return candidate;
    if (dot_dot_dot_token)
        return dot_dot_dot_token + 1;
    if (classkey_token)
        return classkey_token + 1;
    return 1;
}

/** \generated */
unsigned TypeofSpecifierAST::firstToken() const
{
    if (typeof_token)
        return typeof_token;
    if (lparen_token)
        return lparen_token;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    return 0;
}

/** \generated */
unsigned TypeofSpecifierAST::lastToken() const
{
    if (rparen_token)
        return rparen_token + 1;
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (typeof_token)
        return typeof_token + 1;
    return 1;
}

/** \generated */
unsigned UnaryExpressionAST::firstToken() const
{
    if (unary_op_token)
        return unary_op_token;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned UnaryExpressionAST::lastToken() const
{
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (unary_op_token)
        return unary_op_token + 1;
    return 1;
}

/** \generated */
unsigned UsingAST::firstToken() const
{
    if (using_token)
        return using_token;
    if (typename_token)
        return typename_token;
    if (name)
        if (unsigned candidate = name->firstToken())
            return candidate;
    if (semicolon_token)
        return semicolon_token;
    return 0;
}

/** \generated */
unsigned UsingAST::lastToken() const
{
    if (semicolon_token)
        return semicolon_token + 1;
    if (name)
        if (unsigned candidate = name->lastToken())
            return candidate;
    if (typename_token)
        return typename_token + 1;
    if (using_token)
        return using_token + 1;
    return 1;
}

/** \generated */
unsigned UsingDirectiveAST::firstToken() const
{
    if (using_token)
        return using_token;
    if (namespace_token)
        return namespace_token;
    if (name)
        if (unsigned candidate = name->firstToken())
            return candidate;
    if (semicolon_token)
        return semicolon_token;
    return 0;
}

/** \generated */
unsigned UsingDirectiveAST::lastToken() const
{
    if (semicolon_token)
        return semicolon_token + 1;
    if (name)
        if (unsigned candidate = name->lastToken())
            return candidate;
    if (namespace_token)
        return namespace_token + 1;
    if (using_token)
        return using_token + 1;
    return 1;
}

/** \generated */
unsigned WhileStatementAST::firstToken() const
{
    if (while_token)
        return while_token;
    if (lparen_token)
        return lparen_token;
    if (condition)
        if (unsigned candidate = condition->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    if (statement)
        if (unsigned candidate = statement->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned WhileStatementAST::lastToken() const
{
    if (statement)
        if (unsigned candidate = statement->lastToken())
            return candidate;
    if (rparen_token)
        return rparen_token + 1;
    if (condition)
        if (unsigned candidate = condition->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (while_token)
        return while_token + 1;
    return 1;
}

/** \generated */
unsigned GnuAttributeSpecifierAST::lastToken() const
{
    if (second_rparen_token)
        return second_rparen_token + 1;
    if (first_rparen_token)
        return first_rparen_token + 1;
    if (attribute_list)
        if (unsigned candidate = attribute_list->lastToken())
            return candidate;
    if (second_lparen_token)
        return second_lparen_token + 1;
    if (first_lparen_token)
        return first_lparen_token + 1;
    if (attribute_token)
        return attribute_token + 1;
    return 1;
}

/** \generated */
unsigned PointerLiteralAST::firstToken() const
{
    if (literal_token)
        return literal_token;
    return 0;
}

/** \generated */
unsigned PointerLiteralAST::lastToken() const
{
    if (literal_token)
        return literal_token + 1;
    return 1;
}

/** \generated */
unsigned NoExceptSpecificationAST::firstToken() const
{
    if (noexcept_token)
        return noexcept_token;
    if (lparen_token)
        return lparen_token;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    return 0;
}

/** \generated */
unsigned NoExceptSpecificationAST::lastToken() const
{
    if (rparen_token)
        return rparen_token + 1;
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (noexcept_token)
        return noexcept_token + 1;
    return 1;
}

/** \generated */
unsigned StaticAssertDeclarationAST::firstToken() const
{
    if (static_assert_token)
        return static_assert_token;
    if (lparen_token)
        return lparen_token;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    if (comma_token)
        return comma_token;
    if (string_literal)
        if (unsigned candidate = string_literal->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    if (semicolon_token)
        return semicolon_token;
    return 0;
}

/** \generated */
unsigned StaticAssertDeclarationAST::lastToken() const
{
    if (semicolon_token)
        return semicolon_token + 1;
    if (rparen_token)
        return rparen_token + 1;
    if (string_literal)
        if (unsigned candidate = string_literal->lastToken())
            return candidate;
    if (comma_token)
        return comma_token + 1;
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (static_assert_token)
        return static_assert_token + 1;
    return 1;
}

/** \generated */
unsigned DecltypeSpecifierAST::firstToken() const
{
    if (decltype_token)
        return decltype_token;
    if (lparen_token)
        return lparen_token;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    return 0;
}

/** \generated */
unsigned DecltypeSpecifierAST::lastToken() const
{
    if (rparen_token)
        return rparen_token + 1;
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (decltype_token)
        return decltype_token + 1;
    return 1;
}

/** \generated */
unsigned RangeBasedForStatementAST::firstToken() const
{
    if (for_token)
        return for_token;
    if (lparen_token)
        return lparen_token;
    if (type_specifier_list)
        if (unsigned candidate = type_specifier_list->firstToken())
            return candidate;
    if (declarator)
        if (unsigned candidate = declarator->firstToken())
            return candidate;
    if (colon_token)
        return colon_token;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    if (statement)
        if (unsigned candidate = statement->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned RangeBasedForStatementAST::lastToken() const
{
    if (statement)
        if (unsigned candidate = statement->lastToken())
            return candidate;
    if (rparen_token)
        return rparen_token + 1;
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (colon_token)
        return colon_token + 1;
    if (declarator)
        if (unsigned candidate = declarator->lastToken())
            return candidate;
    if (type_specifier_list)
        if (unsigned candidate = type_specifier_list->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (for_token)
        return for_token + 1;
    return 1;
}

/** \generated */
unsigned AlignofExpressionAST::firstToken() const
{
    if (alignof_token)
        return alignof_token;
    if (lparen_token)
        return lparen_token;
    if (typeId)
        if (unsigned candidate = typeId->firstToken())
            return candidate;
    if (rparen_token)
        return rparen_token;
    return 0;
}

/** \generated */
unsigned AlignofExpressionAST::lastToken() const
{
    if (rparen_token)
        return rparen_token + 1;
    if (typeId)
        if (unsigned candidate = typeId->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (alignof_token)
        return alignof_token + 1;
    return 1;
}

/** \generated */
unsigned AliasDeclarationAST::firstToken() const
{
    if (using_token)
        return using_token;
    if (name)
        if (unsigned candidate = name->firstToken())
            return candidate;
    if (equal_token)
        return equal_token;
    if (typeId)
        if (unsigned candidate = typeId->firstToken())
            return candidate;
    if (semicolon_token)
        return semicolon_token;
    return 0;
}

/** \generated */
unsigned AliasDeclarationAST::lastToken() const
{
    if (semicolon_token)
        return semicolon_token + 1;
    if (typeId)
        if (unsigned candidate = typeId->lastToken())
            return candidate;
    if (equal_token)
        return equal_token + 1;
    if (name)
        if (unsigned candidate = name->lastToken())
            return candidate;
    if (using_token)
        return using_token + 1;
    return 1;
}

/** \generated */
unsigned DesignatedInitializerAST::firstToken() const
{
    if (designator_list)
        if (unsigned candidate = designator_list->firstToken())
            return candidate;
    if (equal_token)
        return equal_token;
    if (initializer)
        if (unsigned candidate = initializer->firstToken())
            return candidate;
    return 0;
}

/** \generated */
unsigned DesignatedInitializerAST::lastToken() const
{
    if (initializer)
        if (unsigned candidate = initializer->lastToken())
            return candidate;
    if (equal_token)
        return equal_token + 1;
    if (designator_list)
        if (unsigned candidate = designator_list->lastToken())
            return candidate;
    return 1;
}

/** \generated */
unsigned BracketDesignatorAST::firstToken() const
{
    if (lbracket_token)
        return lbracket_token;
    if (expression)
        if (unsigned candidate = expression->firstToken())
            return candidate;
    if (rbracket_token)
        return rbracket_token;
    return 0;
}

/** \generated */
unsigned BracketDesignatorAST::lastToken() const
{
    if (rbracket_token)
        return rbracket_token + 1;
    if (expression)
        if (unsigned candidate = expression->lastToken())
            return candidate;
    if (lbracket_token)
        return lbracket_token + 1;
    return 1;
}

/** \generated */
unsigned DotDesignatorAST::firstToken() const
{
    if (dot_token)
        return dot_token;
    if (identifier_token)
        return identifier_token;
    return 0;
}

/** \generated */
unsigned DotDesignatorAST::lastToken() const
{
    if (identifier_token)
        return identifier_token + 1;
    if (dot_token)
        return dot_token + 1;
    return 1;
}

/** \generated */
unsigned AlignmentSpecifierAST::firstToken() const
{
    if (align_token)
        return align_token;
    if (lparen_token)
        return lparen_token;
    if (typeIdExprOrAlignmentExpr)
        if (unsigned candidate = typeIdExprOrAlignmentExpr->firstToken())
            return candidate;
    if (ellipses_token)
        return ellipses_token;
    if (rparen_token)
        return rparen_token;
    return 0;
}

/** \generated */
unsigned AlignmentSpecifierAST::lastToken() const
{
    if (rparen_token)
        return rparen_token + 1;
    if (ellipses_token)
        return ellipses_token + 1;
    if (typeIdExprOrAlignmentExpr)
        if (unsigned candidate = typeIdExprOrAlignmentExpr->lastToken())
            return candidate;
    if (lparen_token)
        return lparen_token + 1;
    if (align_token)
        return align_token + 1;
    return 1;
}

