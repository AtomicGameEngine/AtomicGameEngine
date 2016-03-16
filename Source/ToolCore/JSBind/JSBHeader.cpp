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
