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

#ifndef CPLUSPLUS_NAMEVISITOR_H
#define CPLUSPLUS_NAMEVISITOR_H

#include "CPlusPlusForwardDeclarations.h"


namespace CPlusPlus {

class CPLUSPLUS_EXPORT NameVisitor
{
    NameVisitor(const NameVisitor &other);
    void operator =(const NameVisitor &other);

public:
    NameVisitor();
    virtual ~NameVisitor();

    void accept(const Name *name);

    virtual bool preVisit(const Name *) { return true; }
    virtual void postVisit(const Name *) {}

    virtual void visit(const AnonymousNameId *) {}
    virtual void visit(const Identifier *) {}
    virtual void visit(const TemplateNameId *) {}
    virtual void visit(const DestructorNameId *) {}
    virtual void visit(const OperatorNameId *) {}
    virtual void visit(const ConversionNameId *) {}
    virtual void visit(const QualifiedNameId *) {}
    virtual void visit(const SelectorNameId *) {}
};

} // namespace CPlusPlus


#endif // CPLUSPLUS_NAMEVISITOR_H
