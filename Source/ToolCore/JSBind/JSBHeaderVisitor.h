// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/IO/Log.h>
#include <Atomic/Core/ProcessUtils.h>

using namespace Atomic;


#include "JSBHeader.h"
#include "JSBModule.h"
#include "JSBClass.h"
#include "JSBPackage.h"
#include "JSBFunction.h"

#include "JSBNameVisitor.h"


namespace ToolCore
{

class JSBHeader;


class JSBHeaderVisitor : public SymbolVisitor
{
    JSBHeader* header_;
    JSBModule* module_;
    TranslationUnit* unit_;

    Namespace* globalNamespace_;

public:

    JSBHeaderVisitor(JSBHeader* header, TranslationUnit *unit, Namespace* globalNamespace) :
        header_(header),
        unit_(unit),
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

    JSBType* processTypeConversion(Type* type)
    {
        JSBType* jtype = NULL;

        if (type->isIntegerType())
        {
            IntegerType* itype = type->asIntegerType();
            jtype = new JSBPrimitiveType(itype->kind());
        }
        else if (type->isFloatType())
        {
            jtype = new JSBPrimitiveType(JSBPrimitiveType::Float);
        }
        else if (type->isNamedType())
        {
            NamedType* ntype = type->asNamedType();
            String classname = getNameString(ntype->name());

            if (classname.StartsWith("Atomic::"))
                classname.Replace("Atomic::", "");

            if (classname == "Vector")
            {
                if (ntype->name()->asTemplateNameId())
                {
                    const TemplateNameId* tnid = ntype->name()->asTemplateNameId();
                    FullySpecifiedType pfst = tnid->templateArgumentAt(0);
                    JSBType* vtype = processTypeConversion(pfst.type());
                    if (vtype)
                    {
                        jtype = new JSBVectorType(vtype);
                    }
                }
            }
            else if (classname == "String")
            {
                jtype = new JSBStringType();
            }
            else if (classname == "StringHash")
            {
                jtype = new JSBStringHashType();
            }
            else if (classname == "JS_HEAP_PTR")
            {
                jtype = new JSBHeapPtrType();
            }
            else
            {
                JSBClass* jclass = JSBPackage::GetClassAllPackages(classname);

                if (jclass)
                    jtype = new JSBClassType(jclass);
                else
                {
                    // this might be an enum
                    JSBEnum* jenum = JSBPackage::GetEnumAllPackages(classname);
                    if (jenum)
                        jtype = new JSBEnumType(jenum);

                }
            }
        }
        else if (type->asUndefinedType())
        {
            UndefinedType* utype = type->asUndefinedType();
            //ErrorExit("Undefined type");
        }

        return jtype;

    }

    JSBFunctionType* processFunctionType(FullySpecifiedType fst, bool retType = false)
    {
        JSBType* jtype = NULL;
        Type* type = fst.type();

        bool isPointer = false;
        bool isSharedPtr = false;
        bool isReference = false;
        bool isTemplate = false;

        bool isConst = false;

        if (type->isPointerType())
        {
            isPointer=true;
            FullySpecifiedType pfst = type->asPointerType()->elementType();
            type = pfst.type();
        }
        if (type->isReferenceType())
        {
            isReference=true;
            FullySpecifiedType pfst = type->asReferenceType()->elementType();
            type = pfst.type();
            isConst = pfst.isConst();
        }
        if (!isPointer && retType)
        {
            if (type->isNamedType())
            {
                NamedType* ntype = type->asNamedType();
                if (ntype->name()->asTemplateNameId())
                {
                    const TemplateNameId* tnid = ntype->name()->asTemplateNameId();
                    String classname = getNameString(tnid->identifier()->asNameId());
                    if (classname == "SharedPtr")
                    {
                        FullySpecifiedType pfst = tnid->templateArgumentAt(0);
                        type = pfst.type();
                        isTemplate = true;
                    }
                }
            }
        }

        if (fst.isUnsigned())
        {
            if (type->isUndefinedType())
            {
                // this happens when just using "unsigned" in code
                jtype = new JSBPrimitiveType(JSBPrimitiveType::Int, true);
            }
        }


        if (!jtype)
            jtype = processTypeConversion(type);

        if (!jtype)
            return NULL;

        // read only vectors atm
        if (!isConst && jtype->asVectorType())
            return NULL;

        bool skip = false;

        // no pointers to prim atm
        if (isPointer || isReference)
        {
            if (jtype->asPrimitiveType())
                skip = true;
            else if (!retType && !isConst && (jtype->asStringType() || jtype->asStringHashType()))
            {
                skip = true;
            }

            if (skip)
                return NULL;
        }

        JSBFunctionType* ftype = new JSBFunctionType(jtype);
        ftype->isPointer_ = isPointer;
        ftype->isSharedPtr_ = isSharedPtr;
        ftype->isReference_ = isReference;
        ftype->isTemplate_ = isTemplate;
        ftype->isConst_ = isConst;

        return ftype;

    }

    JSBFunctionType* processFunctionArgType(Argument* arg)
    {

        JSBFunctionType* jtype = processFunctionType(arg->type());

        if (!jtype)
            return NULL;

        jtype->name_ = getNameString(arg->name());

        return jtype;

    }


    JSBFunctionType* processFunctionReturnType(Function* function)
    {
        if (!function->hasReturnType())
        {
            return NULL;
        }

        JSBFunctionType* jtype = processFunctionType(function->returnType(), true);

        if (!jtype)
            return NULL;

        return jtype;

    }

    JSBFunction* processFunction(JSBClass* klass, Function* function)
    {
        JSBFunction* jfunction = new JSBFunction(klass);

        // don't ... atm
        if (function->isVariadic())
            return NULL;

        String name = getNameString(function->name());

        jfunction->SetName(name);

        // don't support operators atm
        if (name.StartsWith("operator "))
            return NULL;

        if (name == klass->GetNativeName())
            jfunction->SetConstructor();

        if (name.StartsWith("~"))
            jfunction->SetDestructor();

        if (function->isOverride())
            jfunction->SetVirtual(true);

        // see if we support return type
        if (function->hasReturnType() && !function->returnType().type()->isVoidType())
        {
            JSBFunctionType* returnType = processFunctionReturnType(function);

            if (!returnType)
                return NULL;

            jfunction->SetReturnType(returnType);
        }

        if (function->hasArguments())
        {
            for (unsigned i = 0; i < function->argumentCount(); i++)
            {
                Symbol* symbol = function->argumentAt(i);
                if (symbol->isArgument())
                {
                    Argument* arg = symbol->asArgument();

                    JSBFunctionType* ftype = processFunctionArgType(arg);
                    if (!ftype)
                        return NULL;

                    if (arg->hasInitializer())
                    {
                        ftype->initializer_ = arg->initializer()->chars();
                        if (ftype->initializer_.StartsWith("\\"))
                            ftype->initializer_ = "\"" + ftype->initializer_ + "\"";
                    }

                    jfunction->AddParameter(ftype);
                }
                else
                {
                    return NULL;
                }

            }
        }

        jfunction->sourceLocation_ = function->sourceLocation();
        jfunction->fileName_ = function->fileName();
        jfunction->sourceLine_ = function->line();
        jfunction->sourceColumn_ = function->column();
        //const Token &token = unit_->tokenAt(function->sourceLocation());
        //const char* source = unit_->firstSourceChar() + token.byteOffset;
        const char* comment = NULL;
        for (unsigned i = 0; i < unit_->commentCount(); i++)
        {
            const Token &tcomment = unit_->commentAt(i);
            unsigned line;
            unit_->getPosition(tcomment.utf16charOffset, &line);

            if (line ==  function->line() - 1)
            {
                comment = unit_->firstSourceChar() + tcomment.byteOffset;
                break;
            }
        }

        if (comment && strlen(comment) > 3)
        {
            if (comment[0] == '/' && comment[1] == '/' && comment[2] == '/')
            {
                int index = 3;
                while(comment[index] && comment[index] != '\n' && comment[index] != '\r')
                {
                    String docString = jfunction->GetDocString();
                    docString += comment[index++];
                    jfunction->SetDocString(docString);
                }

            }

        }


        return jfunction;

    }

    virtual bool visit(Namespace *nspace)
    {
        String name = getNameString(nspace->name());

        // LOGINFOF("Namespace: %s", name.CString());

        return true;

    }

    // reject template types
    virtual bool visit(Template *t)
    {
        return false;
    }

    // enums visited in preprocessor visitor
    virtual bool visit(Enum *penum)
    {
        return false;
    }

    // global var decl or function
    virtual bool visit(Declaration* decl)
    {
        if (decl->isTypedef())
            return true;

        FullySpecifiedType dtype = decl->type();

        Type* type = dtype.type();

        if (type->isPointerType() || type->isReferenceType())
            return true;

        if (type->asEnumType())
            return true;

        bool _unsigned = false;

        if (dtype.isUnsigned())
            _unsigned = true;


        if (!type->asFloatType() && !type->asIntegerType() && !_unsigned)
        {
            return true;
        }

        module_->RegisterConstant(getNameString(decl->name()).CString());

        return true;

    }

    virtual bool visit(Class *klass)
    {
        String name = getNameString(klass->name());

        JSBClass* jclass = module_->GetClass(name);

        if (!jclass)
        {
            return false;
        }

        jclass->SetHeader(header_);

        for (unsigned i = 0; i < klass->baseClassCount(); i++)
        {
            BaseClass* baseclass = klass->baseClassAt(i);
            String baseclassname = getNameString(baseclass->name());

            JSBClass* base = JSBPackage::GetClassAllPackages(baseclassname);

            if (!base)
            {
                LOGINFOF("Warning: %s baseclass %s not in bindings", name.CString(), baseclassname.CString());
            }
            else
            {
                jclass->SetBaseClass(base);
            }
        }

        for (unsigned i = 0; i < klass->memberCount(); i++)
        {
            Symbol* symbol = klass->memberAt(i);

            Declaration* decl = symbol->asDeclaration();

            // if the function describes the body in the header
            Function* function = symbol->asFunction();

            // otherwise it could be a decl
            if (!function && decl)
                function = decl->type()->asFunctionType();

            if (function)
            {
                if (function->isPureVirtual())
                    jclass->SetAbstract();

                // only want public functions
                if (!symbol->isPublic())
                    continue;

                JSBFunction* jfunction = processFunction(jclass, function);
                if (jfunction)
                    jclass->AddFunction(jfunction);
            }

        }
        // return false so we don't traverse the class members
        return false;
    }

};

}
