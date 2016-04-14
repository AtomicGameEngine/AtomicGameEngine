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

#include <Atomic/IO/Log.h>

#include "JSBNameVisitor.h"

#include "JSBEnum.h"
#include "JSBModule.h"
#include "JSBHeader.h"

namespace ToolCore
{

class JSBPreprocessVisitor : public SymbolVisitor
{

public:

    JSBPreprocessVisitor(JSBHeader* header, TranslationUnit *unit, Namespace* globalNamespace) :
        header_(header),
        globalNamespace_(globalNamespace)
    {
        module_ = header_->GetModule();
        accept(globalNamespace_);
    }

    String getNameString(const Name* name)
    {
        JSBNameVisitor nvisitor;
        return nvisitor(name);
    }

    // reject template types
    virtual bool visit(Template *t)
    {
        return false;
    }

    virtual bool visit(Enum *penum)
    {
        // don't want enum's in classes
        if (classes_.Size())
            return true;

        JSBModule* module = header_->GetModule();

        JSBEnum* jenum = new JSBEnum(header_->GetContext(), module_, getNameString(penum->name()));
        jenum->SetHeader(header_);

        for (unsigned i = 0; i < penum->memberCount(); i++)
        {
            Symbol* symbol = penum->memberAt(i);

            String name = getNameString(symbol->name());
            String value = "";

            Declaration* decl = symbol->asDeclaration();
            if (decl)
            {
                EnumeratorDeclaration* enumDecl = decl->asEnumeratorDeclarator();
                const StringLiteral* constantValue = enumDecl->constantValue();
                if (constantValue)
                {
                    value = constantValue->chars();
                }
            }

            jenum->AddValue(name);

        }

        jenum->Preprocess();

        module->RegisterEnum(jenum);

        return true;
    }

    virtual bool visit(Class *klass)
    {
        classes_.Push(klass);

        String name = getNameString(klass->name());

        JSBModule* module = header_->GetModule();

        module->RegisterClass(name);

        return true;
    }

    void postVisit(Symbol *symbol)
    {
        if (symbol->asClass())
        {
            classes_.Remove((Class*) symbol);
        }
    }

private:

    SharedPtr<JSBHeader> header_;
    SharedPtr<JSBModule> module_;

    PODVector<Class*> classes_;

    Namespace* globalNamespace_;

};

}
