//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/IO/File.h>

#include "cplusplus/CPlusPlus.h"
using namespace CPlusPlus;

#include "JSBPreprocessVisitor.h"
#include "JSBHeaderVisitor.h"

#include "JSBModule.h"
#include "JSBHeader.h"

namespace ToolCore
{

JSBHeader::JSBHeader(Context* context, JSBModule* module, const String& filepath) : Object(context),
    translationUnit_(0),
    globalNamespace_(0),
    filepath_(filepath),
    module_(module)
{

}

JSBHeader::~JSBHeader()
{

}

void JSBHeader::Parse()
{
    Control* control = new Control();
    const StringLiteral *fileId = control->stringLiteral(filepath_.CString(), filepath_.Length() );
    LanguageFeatures features;
    features.qtEnabled = false;
    features.qtMocRunEnabled = false;
    features.qtKeywordsEnabled = false;
    features.cxx11Enabled = true;
    features.objCEnabled = false;
    features.c99Enabled = false;
    translationUnit_ = new TranslationUnit(control, fileId);
    translationUnit_->setLanguageFeatures(features);
    control->switchTranslationUnit(translationUnit_);

    File file(context_);
    file.Open(filepath_);

    unsigned size = file.GetSize();

    data_ = new char[size + 1];
    data_[size] = '\0';

    file.Read(data_, size);

    // TODO: add error reporting
    translationUnit_->blockErrors(true);

    translationUnit_->setSource(data_, size);
    translationUnit_->tokenize();
    translationUnit_->parse();

    globalNamespace_ = control->newNamespace(0);
    Bind* semantic = new Bind(translationUnit_);
    semantic->setSkipFunctionBodies(true);

    (*semantic)(translationUnit_->ast()->asTranslationUnit(), globalNamespace_);
}

void JSBHeader::VisitPreprocess()
{
    JSBPreprocessVisitor(this, translationUnit_,globalNamespace_);
}

void JSBHeader::VisitHeader()
{
    JSBHeaderVisitor(this, translationUnit_, globalNamespace_);
}




}
