
#include "../Atomic.h"
#include "JSBNameVisitor.h"

JSBNameVisitor::JSBNameVisitor()
{ }

JSBNameVisitor::~JSBNameVisitor()
{ }

String JSBNameVisitor::operator()(const Name *name)
{
    String previousName = switchName();
    accept(name);
    return switchName(previousName);
}

String JSBNameVisitor::switchName(const String &name)
{
    String previousName = _name;
    _name = name;
    return previousName;
}

void JSBNameVisitor::visit(const Identifier *name)
{
    const Identifier *id = name->identifier();
    if (id)
        _name = String(id->chars(), id->size());
    else
        _name = "anonymous";
}

void JSBNameVisitor::visit(const TemplateNameId *name)
{
    _name = "Not Implemented";
}

void JSBNameVisitor::visit(const DestructorNameId *name)
{
    const Identifier *id = name->identifier();
    _name += "~";
    _name += String(id->chars(), id->size());
}

void JSBNameVisitor::visit(const OperatorNameId *name)
{
    _name += "operator";
    if (true) //_overview->includeWhiteSpaceInOperatorName)
        _name += " ";
    switch (name->kind()) { // ### i should probably do this in OperatorNameId
    case OperatorNameId::InvalidOp:
        _name += "<invalid>";
        break;
    case OperatorNameId::NewOp:
        _name += "new";
        break;
    case OperatorNameId::DeleteOp:
        _name += "delete";
        break;
    case OperatorNameId::NewArrayOp:
        _name += "new[]";
        break;
    case OperatorNameId::DeleteArrayOp:
        _name += "delete[]";
        break;
    case OperatorNameId::PlusOp:
        _name += '+';
        break;
    case OperatorNameId::MinusOp:
        _name += '-';
        break;
    case OperatorNameId::StarOp:
        _name += '*';
        break;
    case OperatorNameId::SlashOp:
        _name += '/';
        break;
    case OperatorNameId::PercentOp:
        _name += '%';
        break;
    case OperatorNameId::CaretOp:
        _name += '^';
        break;
    case OperatorNameId::AmpOp:
        _name += '&';
        break;
    case OperatorNameId::PipeOp:
        _name += '|';
        break;
    case OperatorNameId::TildeOp:
        _name += '~';
        break;
    case OperatorNameId::ExclaimOp:
        _name += '!';
        break;
    case OperatorNameId::EqualOp:
        _name += '=';
        break;
    case OperatorNameId::LessOp:
        _name += '<';
        break;
    case OperatorNameId::GreaterOp:
        _name += '>';
        break;
    case OperatorNameId::PlusEqualOp:
        _name += "+=";
        break;
    case OperatorNameId::MinusEqualOp:
        _name += "-=";
        break;
    case OperatorNameId::StarEqualOp:
        _name += "*=";
        break;
    case OperatorNameId::SlashEqualOp:
        _name += "/=";
        break;
    case OperatorNameId::PercentEqualOp:
        _name += "%=";
        break;
    case OperatorNameId::CaretEqualOp:
        _name += "^=";
        break;
    case OperatorNameId::AmpEqualOp:
        _name += "&=";
        break;
    case OperatorNameId::PipeEqualOp:
        _name += "|=";
        break;
    case OperatorNameId::LessLessOp:
        _name += "<<";
        break;
    case OperatorNameId::GreaterGreaterOp:
        _name += ">>";
        break;
    case OperatorNameId::LessLessEqualOp:
        _name += "<<=";
        break;
    case OperatorNameId::GreaterGreaterEqualOp:
        _name += ">>=";
        break;
    case OperatorNameId::EqualEqualOp:
        _name += "==";
        break;
    case OperatorNameId::ExclaimEqualOp:
        _name += "!=";
        break;
    case OperatorNameId::LessEqualOp:
        _name += "<=";
        break;
    case OperatorNameId::GreaterEqualOp:
        _name += ">=";
        break;
    case OperatorNameId::AmpAmpOp:
        _name += "&&";
        break;
    case OperatorNameId::PipePipeOp:
        _name += "||";
        break;
    case OperatorNameId::PlusPlusOp:
        _name += "++";
        break;
    case OperatorNameId::MinusMinusOp:
        _name += "--";
        break;
    case OperatorNameId::CommaOp:
        _name += ',';
        break;
    case OperatorNameId::ArrowStarOp:
        _name += "->*";
        break;
    case OperatorNameId::ArrowOp:
        _name += "->";
        break;
    case OperatorNameId::FunctionCallOp:
        _name += "()";
        break;
    case OperatorNameId::ArrayAccessOp:
        _name += "[]";
        break;
    } // switch
}

void JSBNameVisitor::visit(const ConversionNameId *name)
{
    _name += "operator ";
    _name += "UNIMPLEMENTED";//overview()->prettyType(name->type());
}

void JSBNameVisitor::visit(const QualifiedNameId *name)
{
    if (name->base())
        _name += operator()(name->base());
    _name += "::";
    _name += operator()(name->name());
}

void JSBNameVisitor::visit(const SelectorNameId *name)
{
    for (unsigned i = 0; i < name->nameCount(); ++i) {
        const Name *n = name->nameAt(i);
        if (!n)
            continue;

        if (const Identifier *id = n->identifier()) {
            _name += String(id->chars(), id->size());

            if (name->hasArguments() || name->nameCount() > 1)
                _name += ':';
        }
    }
}

void JSBNameVisitor::visit(const AnonymousNameId *name)
{
    _name = "ANONYMOUS_UNIMPLEMENTED";//String::fromLatin1("Anonymous:%1").arg(name->classTokenIndex());
}
