// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine


#pragma once

#include "JSBNameVisitor.h"

class JSBPreprocessVisitor : public SymbolVisitor
{
    JSBHeader* header_;
    JSBModule* module_;
    JSBindings* bindings_;

    Namespace* globalNamespace_;

public:

    JSBPreprocessVisitor(JSBHeader* header, TranslationUnit *unit, Namespace* globalNamespace) :
        header_(header),
        globalNamespace_(globalNamespace)
    {
        module_ = header_->module_;
        bindings_ = module_->bindings_;
        accept(globalNamespace_);
    }

    String getNameString(const Name* name)
    {
        JSBNameVisitor nvisitor;
        return nvisitor(name);
    }

    virtual bool visit(Enum *penum)
    {
        JSBEnum* jenum = new JSBEnum();

        jenum->name_ = getNameString(penum->name());
        jenum->header_ = header_;

        for (unsigned i = 0; i < penum->memberCount(); i++)
        {
            Symbol* symbol = penum->memberAt(i);
            jenum->values_.Push(getNameString(symbol->name()));
        }

        jenum->Preprocess();

        bindings_->RegisterEnum(jenum);

        module_->enums_.Push(jenum);

        return true;

    }


};
