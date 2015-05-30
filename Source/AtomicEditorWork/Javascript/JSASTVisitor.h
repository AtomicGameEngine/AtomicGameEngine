// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include "JSAST.h"

namespace AtomicEditor
{

class JSASTVisitor
{
public:

    virtual JSASTProgram* visit(JSASTProgram *program) = 0;

    virtual JSASTStatement* visit(JSASTBlockStatement *stmt) = 0;
    virtual JSASTStatement* visit(JSASTExpressionStatement *stmt) = 0;
    virtual JSASTStatement* visit(JSASTIfStatement *stmt) = 0;
    virtual JSASTStatement* visit(JSASTFunctionDeclaration *stmt) = 0;
    virtual JSASTStatement* visit(JSASTVariableDeclaration *stmt) = 0;
    virtual JSASTStatement* visit(JSASTReturnStatement *stmt) = 0;
    virtual JSASTStatement* visit(JSASTEmptyStatement *stmt) = 0;
    virtual JSASTStatement* visit(JSASTForStatement *stmt) = 0;

    virtual JSASTExpression* visit(JSASTAssignmentExpression* expr) = 0;
    virtual JSASTExpression* visit(JSASTLogicalExpression* expr) = 0;
    virtual JSASTExpression* visit(JSASTConditionalExpression* expr) = 0;
    virtual JSASTExpression* visit(JSASTThisExpression* expr) = 0;

    virtual JSASTExpression* visit(JSASTIdentifier* expr) = 0;
    virtual JSASTExpression* visit(JSASTCallExpression* expr) = 0;
    virtual JSASTExpression* visit(JSASTBinaryExpression* expr) = 0;
    virtual JSASTExpression* visit(JSASTMemberExpression* expr) = 0;
    virtual JSASTExpression* visit(JSASTLiteral* expr) = 0;
    virtual JSASTExpression* visit(JSASTArrayExpression* expr) = 0;
    virtual JSASTExpression* visit(JSASTObjectExpression* expr) = 0;
    virtual JSASTExpression* visit(JSASTFunctionExpression* expr) = 0;
    virtual JSASTExpression* visit(JSASTNewExpression* expr) = 0;
    virtual JSASTExpression* visit(JSASTUnaryExpression* expr) = 0;
    virtual JSASTExpression* visit(JSASTUpdateExpression* expr) = 0;
    virtual JSASTExpression* visit(JSASTVariableDeclarator* expr) = 0;    

    virtual JSASTProperty* visit(JSASTProperty* property) = 0;
    virtual JSASTStatement*  visit(JSASTLabeledStatement* property) = 0;

    virtual JSASTComment* visit(JSASTComment* expr) = 0;

};

class JSASTTraversalVisitor : public JSASTVisitor
{
public:

    virtual JSASTProgram* visit(JSASTProgram *program)
    {
        // this would cause recursion
        // program->Accept(this);

        unsigned count = program->GetCommentCount();
        for (unsigned i = 0; i < count; i++)
        {
            JSASTComment* cmt = program->GetComment(i);
            if (cmt)
                cmt->Accept(this);
        }


        count = program->GetStatementCount();
        for (unsigned i = 0; i < count; i++)
        {
            JSASTStatement* stmt = program->GetStatement(i);
            if (stmt)
                stmt->Accept(this);
        }

        return program;

    }

    virtual JSASTStatement* visit(JSASTForStatement *stmt)
    {
        if (stmt->GetInitVariable())
            stmt->GetInitVariable()->Accept(this);
        if (stmt->GetInitExpression())
            stmt->GetInitExpression()->Accept(this);

        if (stmt->GetTest())
            stmt->GetTest()->Accept(this);

        if (stmt->GetUpdate())
            stmt->GetUpdate()->Accept(this);

        if (stmt->GetBody())
            stmt->GetBody()->Accept(this);

        return stmt;

    }

    virtual JSASTStatement* visit(JSASTBlockStatement *block)
    {
        unsigned count = block->GetStatementCount();

        for (unsigned i = 0; i < count; i++)
        {
            JSASTStatement* stmt = block->GetStatement(i);
            if (stmt)
                stmt->Accept(this);
        }

        return block;

    }

    virtual JSASTStatement* visit(JSASTExpressionStatement *stmt)
    {
        if (stmt->GetExpression())
            stmt->GetExpression()->Accept(this);

        return stmt;
    }

    virtual JSASTStatement* visit(JSASTIfStatement *stmt)
    {
        if (stmt->GetTest())
            stmt->GetTest()->Accept(this);
        if (stmt->GetConsequent())
            stmt->GetConsequent()->Accept(this);
        if (stmt->GetAlternate())
            stmt->GetAlternate()->Accept(this);

        return stmt;
    }

    virtual JSASTStatement* visit(JSASTFunctionDeclaration *stmt)
    {
        if (stmt->GetID())
            stmt->GetID()->Accept(this);

        for (unsigned i = 0; i < stmt->GetParamsCount(); i++)
            stmt->GetParam(i)->Accept(this);

        JSASTBlockStatement* body = stmt->GetBodyStatement();

        if (body)
            body->Accept(this);

        return stmt;

    }

    virtual JSASTStatement* visit(JSASTVariableDeclaration *stmt)
    {
        for (unsigned i = 0; i < stmt->GetDeclarationsCount(); i++)
            stmt->GetDeclaration(i)->Accept(this);

        return stmt;
    }

    virtual JSASTStatement* visit(JSASTReturnStatement *stmt)
    {
        if (stmt->GetArgument())
            stmt->GetArgument()->Accept(this);

        return stmt;
    }

    virtual JSASTStatement* visit(JSASTEmptyStatement *stmt)
    {
        return stmt;
    }

    virtual JSASTExpression* visit(JSASTAssignmentExpression* expr)
    {
        if (expr->GetLeft())
            expr->GetLeft()->Accept(this);

        if (expr->GetRight())
            expr->GetRight()->Accept(this);

        return expr;

    }

    virtual JSASTExpression* visit(JSASTConditionalExpression *expr)
    {
        if (expr->GetTest())
            expr->GetTest()->Accept(this);
        if (expr->GetConsequent())
            expr->GetConsequent()->Accept(this);
        if (expr->GetAlternate())
            expr->GetAlternate()->Accept(this);

        return expr;
    }


    virtual JSASTExpression* visit(JSASTLogicalExpression* expr)
    {
        if (expr->GetLeft())
            expr->GetLeft()->Accept(this);

        if (expr->GetRight())
            expr->GetRight()->Accept(this);

        return expr;

    }

    virtual JSASTExpression* visit(JSASTIdentifier* expr)
    {
        return expr;
    }

    virtual JSASTExpression* visit(JSASTCallExpression* expr)
    {
        if (expr->GetCallee())
            expr->GetCallee()->Accept(this);

        for (unsigned i = 0; i < expr->GetArgumentCount(); i++)
        {   if (expr->GetArgument(i))
                expr->GetArgument(i)->Accept(this);
        }

        return expr;
    }

    virtual JSASTExpression* visit(JSASTBinaryExpression* expr)
    {
        if (expr->GetLeft())
            expr->GetLeft()->Accept(this);

        if (expr->GetRight())
            expr->GetRight()->Accept(this);

        return expr;

    }

    virtual JSASTExpression* visit(JSASTMemberExpression* expr)
    {
        if (expr->GetObject())
            expr->GetObject()->Accept(this);

        if (expr->GetProperty())
            expr->GetProperty()->Accept(this);

        return expr;

    }

    virtual JSASTExpression* visit(JSASTLiteral* expr)
    {
        return expr;
    }

    virtual JSASTExpression* visit(JSASTArrayExpression* expr)
    {
        for (unsigned i = 0; i < expr->GetElementCount(); i++)
        {
            JSASTExpression* arrayElement = expr->GetElement(i);
            if (arrayElement)
                arrayElement->Accept(this);
        }

        return expr;
    }

    virtual JSASTExpression* visit(JSASTObjectExpression* expr)
    {
        for (unsigned i = 0; i < expr->GetPropertyCount(); i++)
        {
            JSASTProperty* property = expr->GetProperty(i);

            if (property)
                property->Accept(this);
        }

        return expr;
    }

    virtual JSASTExpression* visit(JSASTFunctionExpression* expr)
    {
        if (expr->GetID())
            expr->GetID()->Accept(this);

        for (unsigned i = 0; i < expr->GetParamsCount(); i++)
            expr->GetParam(i)->Accept(this);

        JSASTBlockStatement* body = expr->GetBodyStatement();

        if (body)
            body->Accept(this);

        return expr;

    }

    virtual JSASTExpression* visit(JSASTNewExpression* expr)
    {
        if (expr->GetCallee())
            expr->GetCallee()->Accept(this);

        for (unsigned i = 0; i < expr->GetArgumentCount(); i++)
        {
            if (expr->GetArgument(i))
                expr->GetArgument(i)->Accept(this);
        }

        return expr;

    }

    virtual JSASTExpression* visit(JSASTUnaryExpression* expr)
    {
        if (expr->GetArgument())
            expr->GetArgument()->Accept(this);

        return expr;
    }

    virtual JSASTExpression* visit(JSASTUpdateExpression* expr)
    {
        if (expr->GetArgument())
            expr->GetArgument()->Accept(this);

        return expr;
    }

    virtual JSASTExpression* visit(JSASTVariableDeclarator* expr)
    {
        if (expr->GetID())
            expr->GetID()->Accept(this);

        if (expr->GetInit())
            expr->GetInit()->Accept(this);


        return expr;
    }

    virtual JSASTComment* visit(JSASTComment* comment)
    {
        return comment;
    }

    virtual JSASTProperty* visit(JSASTProperty* property)
    {
        if (property->GetKey())
            property->GetKey()->Accept(this);
        if (property->GetValue())
            property->GetValue()->Accept(this);

        return property;
    }

    virtual JSASTStatement* visit(JSASTLabeledStatement* labeledStatement)
    {
        if (labeledStatement->GetLabel())
            labeledStatement->GetLabel()->Accept(this);
        if (labeledStatement->GetBody())
            labeledStatement->GetBody()->Accept(this);

        return labeledStatement;
    }


    virtual JSASTExpression* visit(JSASTThisExpression* expr)
    {
        return expr;
    }


};



}


