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

#include "ASTVisitor.h"
#include "AST.h"
#include "TranslationUnit.h"
#include "Control.h"

using namespace CPlusPlus;

ASTVisitor::ASTVisitor(TranslationUnit *translationUnit)
    : _translationUnit(translationUnit)
{ }

ASTVisitor::~ASTVisitor()
{ }

void ASTVisitor::accept(AST *ast)
{ AST::accept(ast, this); }

Control *ASTVisitor::control() const
{
    if (_translationUnit)
        return _translationUnit->control();

    return 0;
}

TranslationUnit *ASTVisitor::translationUnit() const
{ return _translationUnit; }

void ASTVisitor::setTranslationUnit(TranslationUnit *translationUnit)
{ _translationUnit = translationUnit; }

unsigned ASTVisitor::tokenCount() const
{ return translationUnit()->tokenCount(); }

const Token &ASTVisitor::tokenAt(unsigned index) const
{ return translationUnit()->tokenAt(index); }

int ASTVisitor::tokenKind(unsigned index) const
{ return translationUnit()->tokenKind(index); }

const char *ASTVisitor::spell(unsigned index) const
{ return translationUnit()->spell(index); }

const Identifier *ASTVisitor::identifier(unsigned index) const
{ return translationUnit()->identifier(index); }

const Literal *ASTVisitor::literal(unsigned index) const
{ return translationUnit()->literal(index); }

const NumericLiteral *ASTVisitor::numericLiteral(unsigned index) const
{ return translationUnit()->numericLiteral(index); }

const StringLiteral *ASTVisitor::stringLiteral(unsigned index) const
{ return translationUnit()->stringLiteral(index); }

void ASTVisitor::getPosition(unsigned offset,
                             unsigned *line,
                             unsigned *column,
                             const StringLiteral **fileName) const
{ translationUnit()->getPosition(offset, line, column, fileName); }

void ASTVisitor::getTokenPosition(unsigned index,
                                  unsigned *line,
                                  unsigned *column,
                                  const StringLiteral **fileName) const
{ translationUnit()->getTokenPosition(index, line, column, fileName); }

void ASTVisitor::getTokenStartPosition(unsigned index, unsigned *line, unsigned *column) const
{ getPosition(tokenAt(index).utf16charsBegin(), line, column); }

void ASTVisitor::getTokenEndPosition(unsigned index, unsigned *line, unsigned *column) const
{ getPosition(tokenAt(index).utf16charsEnd(), line, column); }
