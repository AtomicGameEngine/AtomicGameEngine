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

#include <Atomic/Atomic.h>

#include "JSBNameVisitor.h"

namespace ToolCore
{

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
    String previousName = name_;
    name_ = name;
    return previousName;
}

void JSBNameVisitor::visit(const Identifier *name)
{
    const Identifier *id = name->identifier();
    if (id)
        name_ = String(id->chars(), id->size());
    else
        name_ = "anonymous";
}

void JSBNameVisitor::visit(const TemplateNameId *name)
{
    name_ = "Not Implemented TemplateNameId";

    unsigned numTemplateArgs = name->templateArgumentCount();
    if (numTemplateArgs != 1)
        return;

    FullySpecifiedType fst = name->templateArgumentAt(0);

    const Identifier *id = name->identifier();

    name_ = String(id->chars(), id->size());

}

void JSBNameVisitor::visit(const DestructorNameId *name)
{
    const Identifier *id = name->identifier();
    name_ += "~";
    name_ += String(id->chars(), id->size());
}

void JSBNameVisitor::visit(const OperatorNameId *name)
{
    name_ += "operator";
    if (true) //_overview->includeWhiteSpaceInOperatorName)
        name_ += " ";
    switch (name->kind()) { // ### i should probably do this in OperatorNameId
    case OperatorNameId::InvalidOp:
        name_ += "<invalid>";
        break;
    case OperatorNameId::NewOp:
        name_ += "new";
        break;
    case OperatorNameId::DeleteOp:
        name_ += "delete";
        break;
    case OperatorNameId::NewArrayOp:
        name_ += "new[]";
        break;
    case OperatorNameId::DeleteArrayOp:
        name_ += "delete[]";
        break;
    case OperatorNameId::PlusOp:
        name_ += '+';
        break;
    case OperatorNameId::MinusOp:
        name_ += '-';
        break;
    case OperatorNameId::StarOp:
        name_ += '*';
        break;
    case OperatorNameId::SlashOp:
        name_ += '/';
        break;
    case OperatorNameId::PercentOp:
        name_ += '%';
        break;
    case OperatorNameId::CaretOp:
        name_ += '^';
        break;
    case OperatorNameId::AmpOp:
        name_ += '&';
        break;
    case OperatorNameId::PipeOp:
        name_ += '|';
        break;
    case OperatorNameId::TildeOp:
        name_ += '~';
        break;
    case OperatorNameId::ExclaimOp:
        name_ += '!';
        break;
    case OperatorNameId::EqualOp:
        name_ += '=';
        break;
    case OperatorNameId::LessOp:
        name_ += '<';
        break;
    case OperatorNameId::GreaterOp:
        name_ += '>';
        break;
    case OperatorNameId::PlusEqualOp:
        name_ += "+=";
        break;
    case OperatorNameId::MinusEqualOp:
        name_ += "-=";
        break;
    case OperatorNameId::StarEqualOp:
        name_ += "*=";
        break;
    case OperatorNameId::SlashEqualOp:
        name_ += "/=";
        break;
    case OperatorNameId::PercentEqualOp:
        name_ += "%=";
        break;
    case OperatorNameId::CaretEqualOp:
        name_ += "^=";
        break;
    case OperatorNameId::AmpEqualOp:
        name_ += "&=";
        break;
    case OperatorNameId::PipeEqualOp:
        name_ += "|=";
        break;
    case OperatorNameId::LessLessOp:
        name_ += "<<";
        break;
    case OperatorNameId::GreaterGreaterOp:
        name_ += ">>";
        break;
    case OperatorNameId::LessLessEqualOp:
        name_ += "<<=";
        break;
    case OperatorNameId::GreaterGreaterEqualOp:
        name_ += ">>=";
        break;
    case OperatorNameId::EqualEqualOp:
        name_ += "==";
        break;
    case OperatorNameId::ExclaimEqualOp:
        name_ += "!=";
        break;
    case OperatorNameId::LessEqualOp:
        name_ += "<=";
        break;
    case OperatorNameId::GreaterEqualOp:
        name_ += ">=";
        break;
    case OperatorNameId::AmpAmpOp:
        name_ += "&&";
        break;
    case OperatorNameId::PipePipeOp:
        name_ += "||";
        break;
    case OperatorNameId::PlusPlusOp:
        name_ += "++";
        break;
    case OperatorNameId::MinusMinusOp:
        name_ += "--";
        break;
    case OperatorNameId::CommaOp:
        name_ += ',';
        break;
    case OperatorNameId::ArrowStarOp:
        name_ += "->*";
        break;
    case OperatorNameId::ArrowOp:
        name_ += "->";
        break;
    case OperatorNameId::FunctionCallOp:
        name_ += "()";
        break;
    case OperatorNameId::ArrayAccessOp:
        name_ += "[]";
        break;
    } // switch
}

void JSBNameVisitor::visit(const ConversionNameId *name)
{
    name_ += "operator ";
    name_ += "UNIMPLEMENTED";//overview()->prettyType(name->type());
}

void JSBNameVisitor::visit(const QualifiedNameId *name)
{
    if (name->base())
        name_ += operator()(name->base());
    name_ += "::";
    name_ += operator()(name->name());
}

void JSBNameVisitor::visit(const SelectorNameId *name)
{
    for (unsigned i = 0; i < name->nameCount(); ++i) {
        const Name *n = name->nameAt(i);
        if (!n)
            continue;

        if (const Identifier *id = n->identifier()) {
            name_ += String(id->chars(), id->size());

            if (name->hasArguments() || name->nameCount() > 1)
                name_ += ':';
        }
    }
}

void JSBNameVisitor::visit(const AnonymousNameId *name)
{
    name_ = "ANONYMOUS_UNIMPLEMENTED";//String::fromLatin1("Anonymous:%1").arg(name->classTokenIndex());
}

}
