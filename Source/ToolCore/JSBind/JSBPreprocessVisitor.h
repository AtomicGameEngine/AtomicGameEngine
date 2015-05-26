// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine


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
        JSBModule* module = header_->GetModule();

        JSBEnum* jenum = new JSBEnum(header_->GetContext(), module_, getNameString(penum->name()));
        jenum->SetHeader(header_);

        for (unsigned i = 0; i < penum->memberCount(); i++)
        {
            Symbol* symbol = penum->memberAt(i);
            jenum->AddValue(getNameString(symbol->name()));
        }

        jenum->Preprocess();

        module->RegisterEnum(jenum);

        return true;
    }

    virtual bool visit(Class *klass)
    {
        String name = getNameString(klass->name());

        JSBModule* module = header_->GetModule();

        module->RegisterClass(name);

        return true;
    }

private:

    SharedPtr<JSBHeader> header_;
    SharedPtr<JSBModule> module_;

    Namespace* globalNamespace_;

};

}
