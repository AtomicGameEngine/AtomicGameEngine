//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
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
//

#pragma once

#include "JSASTVisitor.h"
#include "JSTheme.h"

#include <TurboBadger/tb_style_edit.h>

using namespace tb;

namespace AtomicEditor
{

// TBBlock::InsertText, handle text input, split/layout

class JSASTSyntaxColorVisitor : public JSASTTraversalVisitor
{
public:

    JSASTSyntaxColorVisitor(TBStyleEdit* styleEdit) : styleEdit_(styleEdit) {}

    using JSASTTraversalVisitor::visit;

    virtual JSASTExpression* visit(JSASTIdentifier* expr)
    {
        //SetLocColor(expr->GetLoc(), 6);
        return expr;
    }

    virtual JSASTExpression* visit(JSASTLiteral* expr)
    {
        JSThemeColor color = JSTHEME_NORMAL;
        switch (expr->GetLiteralType())
        {
        case JSASTLiteral::LITERAL_BOOLEAN:
            color = JSTHEME_LITERAL_BOOLEAN;
            break;
        case JSASTLiteral::LITERAL_STRING:
            color = JSTHEME_LITERAL_STRING;
            break;
        case JSASTLiteral::LITERAL_NULL:
            color = JSTHEME_LITERAL_NULL;
            break;
        case JSASTLiteral::LITERAL_NUMBER:
            color = JSTHEME_LITERAL_NUMBER;
            break;
        case JSASTLiteral::LITERAL_REGEX:
            color = JSTHEME_LITERAL_REGEX;
            break;

        default:
            break;
        }

        SetLocColor(expr->GetLoc(), color);
        return expr;
    }

    virtual JSASTStatement* visit(JSASTFunctionDeclaration* decl)
    {
        SetLocColor(decl->GetLoc(), JSTHEME_FUNCTION, true);

        JSASTNode* id = decl->GetID();
        if (id)
            SetLocColor(id->GetLoc(), JSTHEME_CODE, true);

        for (unsigned i = 0; i < decl->GetParamsCount(); i++)
        {
            JSASTExpression* p = decl->GetParam(i);
            SetLocColor(p->GetLoc(), JSTHEME_FUNCTIONDECLARG, true);
        }


        return JSASTTraversalVisitor::visit(decl);

    }

    virtual JSASTStatement* visit(JSASTVariableDeclaration* decl)
    {
        SetLocColor(decl->GetLoc(), JSTHEME_VAR, true);

        SetLocColor(decl->GetLoc(), "=", JSTHEME_KEYWORD, true);

        for (unsigned i = 0; i < decl->GetDeclarationsCount(); i++)
        {
            JSASTExpression* expr = decl->GetDeclaration(i);

            if (expr->GetType() == JSAST_VARIABLEDECLARATOR)
            {
                JSASTVariableDeclarator* v = (JSASTVariableDeclarator*) expr;

                if (JSASTExpression* init = v->GetInit())
                {
                    if (init->GetType() == JSAST_FUNCTIONEXPRESSION)
                    {
                        if (v->GetID())
                            SetLocColor(v->GetID()->GetLoc(), JSTHEME_CODE, true);
                    }
                }
            }

        }

        return JSASTTraversalVisitor::visit(decl);
    }

    virtual JSASTComment* visit(JSASTComment* comment)
    {
        SetLocColor(comment->GetLoc(), JSTHEME_COMMENT, false);
        return comment;
    }

    virtual JSASTStatement* visit(JSASTIfStatement* stmt)
    {
        SetLocColor(stmt->GetLoc(), JSTHEME_KEYWORD, true);
        return JSASTTraversalVisitor::visit(stmt);
    }

    virtual JSASTExpression* visit(JSASTConditionalExpression* expr)
    {
        return JSASTTraversalVisitor::visit(expr);
    }


    virtual JSASTStatement* visit(JSASTForStatement* stmt)
    {
        SetLocColor(stmt->GetLoc(), JSTHEME_KEYWORD, true);
        return JSASTTraversalVisitor::visit(stmt);
    }


    virtual JSASTExpression* visit(JSASTCallExpression* expr)
    {

        JSASTNode* locNode = expr->GetCallee();
        if (locNode->GetType() == JSAST_MEMBEREXPRESSION)
        {
            locNode = ((JSASTMemberExpression*) locNode)->GetProperty();
        }

        //SetLocColor(locNode->GetLoc(), 2);

        return JSASTTraversalVisitor::visit(expr);
    }

    virtual JSASTStatement* visit(JSASTReturnStatement *stmt)
    {
        SetLocColor(stmt->GetLoc(), JSTHEME_KEYWORD, true);

        return JSASTTraversalVisitor::visit(stmt);
    }

    virtual JSASTExpression* visit(JSASTUnaryExpression* expr)
    {
        const String& op = expr->GetOperator();

        SetLocColor(expr->GetLoc(), op, JSTHEME_KEYWORD, false);

        return JSASTTraversalVisitor::visit(expr);
    }

    virtual JSASTExpression* visit(JSASTUpdateExpression* expr)
    {
        const String& op = expr->GetOperator();

        SetLocColor(expr->GetLoc(), op, JSTHEME_KEYWORD, false);

        return JSASTTraversalVisitor::visit(expr);
    }


    virtual JSASTExpression* visit(JSASTBinaryExpression* expr)
    {
        const String& op = expr->GetOperator();

        SetLocColor(expr->GetLoc(), op, JSTHEME_KEYWORD, false);

        return JSASTTraversalVisitor::visit(expr);
    }

    virtual JSASTExpression* visit(JSASTAssignmentExpression* expr)
    {
        const String& op = expr->GetOperator();

        SetLocColor(expr->GetLoc(), op, JSTHEME_KEYWORD, false);

        return JSASTTraversalVisitor::visit(expr);
    }

    virtual JSASTExpression* visit(JSASTLogicalExpression* expr)
    {
        const String& op = expr->GetOperator();

        SetLocColor(expr->GetLoc(), op, JSTHEME_KEYWORD, false);

        return JSASTTraversalVisitor::visit(expr);
    }


    virtual JSASTExpression* visit(JSASTNewExpression* expr)
    {
        SetLocColor(expr->GetLoc(), JSTHEME_KEYWORD, true);

        JSASTNode* locNode = expr->GetCallee();

        if (locNode->GetType() == JSAST_MEMBEREXPRESSION)
        {
            locNode = ((JSASTMemberExpression*) locNode)->GetProperty();
        }

        return JSASTTraversalVisitor::visit(expr);
    }

    virtual JSASTExpression* visit(JSASTFunctionExpression* expr)
    {
        SetLocColor(expr->GetLoc(), "function", JSTHEME_FUNCTION, true);

        for (unsigned i = 0; i < expr->GetParamsCount(); i++)
        {
            JSASTExpression* p = expr->GetParam(i);
            SetLocColor(p->GetLoc(), JSTHEME_FUNCTIONDECLARG, true);
        }

        return JSASTTraversalVisitor::visit(expr);
    }


private:

    void SetLocColor(const JSASTNode::Loc& loc, unsigned color, bool single = false)
    {
        if (!loc.Valid())
            return;

        PODVector<TBTextFragment*> fragments;
        GetTextFragments(loc, fragments);
        for (unsigned i = 0; i < fragments.Size(); i++)
        {
            fragments.At(i)->themeColor = color;
            if (single)
                break;
        }
    }

    void SetLocColor(const JSASTNode::Loc& loc, const String& value, unsigned color, bool single = false)
    {
        if (!loc.Valid())
            return;

        String substring = value;

        PODVector<TBTextFragment*> fragments;
        GetTextFragments(loc, fragments);
        for (unsigned i = 0; i < fragments.Size(); i++)
        {
            TBTextFragment* fragment = fragments[i];

            if (!strncmp(substring.CString(), fragment->Str(), substring.Length()))
            {
                fragments.At(i)->themeColor = color;

                substring = substring.Substring(fragments.At(i)->len);

                if (single || !substring.Length())
                    break;
            }
        }
    }


    TBTextFragment* GetFragmentAt( const JSASTNode::Loc& loc)
    {
        if (!loc.Valid())
            return NULL;

        int line = loc.startLine_ - 1;

        TBBlock *block = styleEdit_->blocks.GetFirst();
        int count = 0;
        while (block && count < line)
        {
            block = block->GetNext();
            count++;
        }

        if (!block)
            return NULL;

        for (TBTextFragment* frag = block->fragments.GetFirst(); frag; frag = frag->GetNext())
        {
            if (frag->ofs >= loc.startColumn_)
                return frag;
        }

        return NULL;

    }


    void GetTextFragments( const JSASTNode::Loc& loc,  PODVector<TBTextFragment*>& fragments)
    {
        if (!loc.Valid())
            return;

        for (int lcounter = loc.startLine_; lcounter <= loc.endLine_; lcounter++)
        {
            int line = lcounter - 1;

            TBBlock *block = styleEdit_->blocks.GetFirst();
            int count = 0;
            while (block && count < line)
            {
                block = block->GetNext();
                count++;
            }

            if (!block)
                return;

            for (TBTextFragment* frag = block->fragments.GetFirst(); frag; frag = frag->GetNext())
            {
                if (loc.startLine_ == loc.endLine_)
                {
                    if (frag->ofs >= loc.startColumn_ && frag->ofs < loc.endColumn_)
                        fragments.Push(frag);
                }
                else
                {
                    if (lcounter == loc.startLine_)
                    {
                        if (frag->ofs >= loc.startColumn_)
                            fragments.Push(frag);

                    }
                    else if (lcounter == loc.endLine_)
                    {
                        if (frag->ofs < loc.endColumn_)
                            fragments.Push(frag);

                    }
                    else
                    {
                        fragments.Push(frag);
                    }
                }
            }
        }

    }

    TBStyleEdit* styleEdit_;

};


}
