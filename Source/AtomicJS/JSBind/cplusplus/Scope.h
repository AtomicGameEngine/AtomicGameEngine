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

#ifndef CPLUSPLUS_SCOPE_H
#define CPLUSPLUS_SCOPE_H

#include "CPlusPlusForwardDeclarations.h"
#include "Symbol.h"
#include "Names.h"

namespace CPlusPlus {

class CPLUSPLUS_EXPORT Scope: public Symbol
{
public:
    Scope(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name);
    Scope(Clone *clone, Subst *subst, Scope *original);
    virtual ~Scope();

    /// Adds a Symbol to this Scope.
    void addMember(Symbol *symbol);

    /// Returns true if this Scope is empty; otherwise returns false.
    bool isEmpty() const;

    /// Returns the number of symbols is in the scope.
    unsigned memberCount() const;

    /// Returns the Symbol at the given position.
    Symbol *memberAt(unsigned index) const;

    typedef Symbol **iterator;

    /// Returns the first Symbol in the scope.
    iterator firstMember() const;

    /// Returns the last Symbol in the scope.
    iterator lastMember() const;

    Symbol *find(const Identifier *id) const;
    Symbol *find(OperatorNameId::Kind operatorId) const;

    /// Set the start offset of the scope
    unsigned startOffset() const;
    void setStartOffset(unsigned offset);

    /// Set the end offset of the scope
    unsigned endOffset() const;
    void setEndOffset(unsigned offset);

    virtual const Scope *asScope() const
    { return this; }

    virtual Scope *asScope()
    { return this; }

private:
    SymbolTable *_members;
    unsigned _startOffset;
    unsigned _endOffset;
};

} // namespace CPlusPlus


#endif // CPLUSPLUS_SCOPE_H
