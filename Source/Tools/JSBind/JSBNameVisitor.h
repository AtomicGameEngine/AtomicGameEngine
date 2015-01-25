
#pragma once

#include <Atomic/Container/Str.h>

#include "cplusplus/CPlusPlus.h"
using namespace CPlusPlus;
using namespace Atomic;

class JSBNameVisitor: protected NameVisitor
{
public:
    JSBNameVisitor();
    virtual ~JSBNameVisitor();

    String operator()(const Name *name);

protected:

    String switchName(const String &name = String());

    virtual void visit(const Identifier *name);
    virtual void visit(const TemplateNameId *name);
    virtual void visit(const DestructorNameId *name);
    virtual void visit(const OperatorNameId *name);
    virtual void visit(const ConversionNameId *name);
    virtual void visit(const QualifiedNameId *name);
    virtual void visit(const SelectorNameId *name);
    virtual void visit(const AnonymousNameId *name);

private:
    String _name;
};
