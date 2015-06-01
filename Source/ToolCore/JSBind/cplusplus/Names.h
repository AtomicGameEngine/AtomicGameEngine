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

#ifndef CPLUSPLUS_NAMES_H
#define CPLUSPLUS_NAMES_H

#include "CPlusPlusForwardDeclarations.h"
#include "Name.h"
#include "FullySpecifiedType.h"
#include <vector>

namespace CPlusPlus {

class CPLUSPLUS_EXPORT QualifiedNameId: public Name
{
public:
    QualifiedNameId(const Name *base, const Name *name)
        : _base(base), _name(name) {}

    virtual ~QualifiedNameId();

    virtual const Identifier *identifier() const;

    const Name *base() const;
    const Name *name() const;

    virtual const QualifiedNameId *asQualifiedNameId() const
    { return this; }

protected:
    virtual void accept0(NameVisitor *visitor) const;
    virtual bool match0(const Name *otherName, Matcher *matcher) const;

private:
    const Name *_base;
    const Name *_name;
};

class CPLUSPLUS_EXPORT DestructorNameId: public Name
{
public:
    DestructorNameId(const Name *name);
    virtual ~DestructorNameId();

    virtual const Name *name() const;

    virtual const Identifier *identifier() const;

    virtual const DestructorNameId *asDestructorNameId() const
    { return this; }

protected:
    virtual void accept0(NameVisitor *visitor) const;
    virtual bool match0(const Name *otherName, Matcher *matcher) const;

private:
    const Name *_name;
};

class CPLUSPLUS_EXPORT TemplateNameId: public Name
{
public:
    template <typename Iterator>
    TemplateNameId(const Identifier *identifier, bool isSpecialization, Iterator first,
                   Iterator last)
        : _identifier(identifier)
        , _templateArguments(first, last)
        , _isSpecialization(isSpecialization) {}

    virtual ~TemplateNameId();

    virtual const Identifier *identifier() const;

    // ### find a better name
    unsigned templateArgumentCount() const;
    const FullySpecifiedType &templateArgumentAt(unsigned index) const;

    virtual const TemplateNameId *asTemplateNameId() const
    { return this; }

    typedef std::vector<FullySpecifiedType>::const_iterator TemplateArgumentIterator;

    TemplateArgumentIterator firstTemplateArgument() const { return _templateArguments.begin(); }
    TemplateArgumentIterator lastTemplateArgument() const { return _templateArguments.end(); }
    bool isSpecialization() const { return _isSpecialization; }
    // this is temporary solution needed in ClassOrNamespace::nestedType
    // when we try to find correct specialization for instantiation
    void setIsSpecialization(bool isSpecialization) { _isSpecialization = isSpecialization; }

    // Comparator needed to distinguish between two different TemplateNameId(e.g.:used in std::map)
    struct Compare: std::binary_function<const TemplateNameId *, const TemplateNameId *, bool> {
        bool operator()(const TemplateNameId *name, const TemplateNameId *other) const;
    };

protected:
    virtual void accept0(NameVisitor *visitor) const;
    virtual bool match0(const Name *otherName, Matcher *matcher) const;

private:
    const Identifier *_identifier;
    std::vector<FullySpecifiedType> _templateArguments;
    // now TemplateNameId can be a specialization or an instantiation
    bool _isSpecialization;
};

class CPLUSPLUS_EXPORT OperatorNameId: public Name
{
public:
    /*
        new  delete    new[]     delete[]
        +    -    *    /    %    ^    &    |    ~
        !    =    <    >    +=   -=   *=   /=   %=
        ^=   &=   |=   <<   >>   >>=  <<=  ==   !=
        <=   >=   &&   ||   ++   --   ,    ->*  ->
        ()   []
     */
    enum Kind {
        InvalidOp,
        NewOp,
        DeleteOp,
        NewArrayOp,
        DeleteArrayOp,
        PlusOp,
        MinusOp,
        StarOp,
        SlashOp,
        PercentOp,
        CaretOp,
        AmpOp,
        PipeOp,
        TildeOp,
        ExclaimOp,
        EqualOp,
        LessOp,
        GreaterOp,
        PlusEqualOp,
        MinusEqualOp,
        StarEqualOp,
        SlashEqualOp,
        PercentEqualOp,
        CaretEqualOp,
        AmpEqualOp,
        PipeEqualOp,
        LessLessOp,
        GreaterGreaterOp,
        LessLessEqualOp,
        GreaterGreaterEqualOp,
        EqualEqualOp,
        ExclaimEqualOp,
        LessEqualOp,
        GreaterEqualOp,
        AmpAmpOp,
        PipePipeOp,
        PlusPlusOp,
        MinusMinusOp,
        CommaOp,
        ArrowStarOp,
        ArrowOp,
        FunctionCallOp,
        ArrayAccessOp
    };

public:
    OperatorNameId(Kind kind);
    virtual ~OperatorNameId();

    Kind kind() const;

    virtual const Identifier *identifier() const;

    virtual const OperatorNameId *asOperatorNameId() const
    { return this; }

protected:
    virtual void accept0(NameVisitor *visitor) const;
    virtual bool match0(const Name *otherName, Matcher *matcher) const;

private:
    Kind _kind;
};

class CPLUSPLUS_EXPORT ConversionNameId: public Name
{
public:
    ConversionNameId(const FullySpecifiedType &type);
    virtual ~ConversionNameId();

    FullySpecifiedType type() const;

    virtual const Identifier *identifier() const;

    virtual const ConversionNameId *asConversionNameId() const
    { return this; }

protected:
    virtual void accept0(NameVisitor *visitor) const;
    virtual bool match0(const Name *otherName, Matcher *matcher) const;

private:
    FullySpecifiedType _type;
};

class CPLUSPLUS_EXPORT SelectorNameId: public Name
{
public:
    template <typename Iterator>
    SelectorNameId(Iterator first, Iterator last, bool hasArguments)
        : _names(first, last), _hasArguments(hasArguments) {}

    virtual ~SelectorNameId();

    virtual const Identifier *identifier() const;

    unsigned nameCount() const;
    const Name *nameAt(unsigned index) const;
    bool hasArguments() const;

    virtual const SelectorNameId *asSelectorNameId() const
    { return this; }

    typedef std::vector<const Name *>::const_iterator NameIterator;

    NameIterator firstName() const { return _names.begin(); }
    NameIterator lastName() const { return _names.end(); }

protected:
    virtual void accept0(NameVisitor *visitor) const;
    virtual bool match0(const Name *otherName, Matcher *matcher) const;

private:
    std::vector<const Name *> _names;
    bool _hasArguments;
};

class CPLUSPLUS_EXPORT AnonymousNameId: public Name
{
public:
    AnonymousNameId(unsigned classTokenIndex);
    virtual ~AnonymousNameId();

    unsigned classTokenIndex() const;

    virtual const Identifier *identifier() const;

    virtual const AnonymousNameId *asAnonymousNameId() const
    { return this; }

protected:
    virtual void accept0(NameVisitor *visitor) const;
    virtual bool match0(const Name *otherName, Matcher *matcher) const;

private:
    unsigned _classTokenIndex;
};



} // namespace CPlusPlus

#endif // CPLUSPLUS_NAMES_H
