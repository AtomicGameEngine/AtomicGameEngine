
#include <Atomic/Atomic.h>
#include <Atomic/IO/File.h>
#include "JSBHeader.h"
#include "JSBHeaderVisitor.h"
#include "JSBPreprocessVisitor.h"
#include "JSBind.h"

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

    File file(JSBind::context_);
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


    //if (TranslationUnitAST *ast = _translationUnit->ast()->asTranslationUnit())
    (*semantic)(translationUnit_->ast()->asTranslationUnit(), globalNamespace_);
    //else if (StatementAST *ast = _translationUnit->ast()->asStatement())
    //    semantic(ast, _globalNamespace);
    //else if (ExpressionAST *ast = _translationUnit->ast()->asExpression())
    //    semantic(ast, _globalNamespace);
    //else if (DeclarationAST *ast = translationUnit()->ast()->asDeclaration())
    //    semantic(ast, _globalNamespace);

    //JSBHeaderVisitor* visitor = new JSBHeaderVisitor(this, translationUnit,globalNamespace);
    //visitor->accept(translationUnit->ast());

}

void JSBHeader::VisitPreprocess()
{
    JSBPreprocessVisitor* visitor = new JSBPreprocessVisitor(this, translationUnit_,globalNamespace_);
}

void JSBHeader::VisitHeader()
{
    JSBHeaderVisitor* visitor = new JSBHeaderVisitor(this, translationUnit_,globalNamespace_);
}

