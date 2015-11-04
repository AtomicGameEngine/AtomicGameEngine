//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/IO/Log.h>
#include "JSBClass.h"
#include "JSBType.h"
#include "JSBSymbol.h"

using namespace Atomic;

namespace ToolCore
{

class JSBFunctionType
{
public:

    JSBFunctionType(JSBType* type) : type_(type)
    {
        isSharedPtr_ = false;
        isPointer_ = false;
        isReference_ = false;
        isTemplate_ = false;
        isConst_ = false;
    }

    // returns true if the types match
    bool Match(const JSBFunctionType* other)
    {
        if (!other)
            return false;

        if (isSharedPtr_ != other->isSharedPtr_)
            return false;
        if (isPointer_ != other->isPointer_)
            return false;
        if (isReference_ != other->isReference_)
            return false;
        if (isTemplate_ != other->isTemplate_)
            return false;
        if (isConst_ != other->isConst_)
            return false;

        if (name_ != other->name_)
            return false;

        if (!type_ && !other->type_)
            return true;

        if (!type_ || !other->type_)
            return false;

        return type_->Match(other->type_);

    }

    bool isSharedPtr_;
    bool isPointer_;
    bool isReference_;
    bool isTemplate_;
    bool isConst_;

    String name_;
    String initializer_;

    JSBType* type_;

    String ToString()
    {
        String tstring = type_->ToString();
        if (isPointer_)
            tstring += "*";
        if (isReference_)
            tstring += "&";

        if (name_.Length())
        {
            tstring += " " + name_;
        }

        return tstring;
    }

    String ToArgString(int index)
    {
        String tstring = type_->ToString();

        if (isPointer_)
            tstring += "*";

        if (isReference_ && tstring != "String")
            tstring += "&";

        if (name_.Length())
        {
            tstring.AppendWithFormat(" __arg%i", index);
        }

        return tstring;
    }


};

class JSBFunction : public JSBSymbol
{
    friend class JSFunctionWriter;
    friend class CSFunctionWriter;

public:

    JSBFunction(JSBClass* klass) : class_(klass), returnType_(0),
                                   isConstructor_(false), isDestructor_(false),
                                   isGetter_(false), isSetter_(false),
                                   isOverload_(false), skip_(false), 
                                   isVirtual_(false), isStatic_(false)
    {

    }

    const String& GetName() { return name_; }

    bool Match(JSBFunction* func);

    bool IsConstructor() { return isConstructor_; }
    bool IsDestructor() { return isDestructor_; }
    bool IsSetter() { return isSetter_; }
    bool IsGetter() { return isGetter_; }
    bool IsOverload() { return isOverload_; }
    bool IsVirtual() { return isVirtual_; }
    bool IsStatic() { return isStatic_; }
    bool Skip() { return skip_; }

    JSBClass* GetClass() { return class_; }
    const String& GetPropertyName() { return propertyName_; }
    JSBFunctionType* GetReturnType() { return returnType_; }

    /// Get class return type or null
    JSBClass* GetReturnClass();

    Vector<JSBFunctionType*>& GetParameters() { return parameters_; }

    const String& GetDocString() { return docString_; }

    void SetName(const String& name) { name_ = name; }
    void SetConstructor(bool value = true) { isConstructor_ = value; }
    void SetDestructor(bool value = true) { isDestructor_ = value; }
    void SetSetter(bool value = true) { isSetter_ = value; }
    void SetGetter(bool value = true) { isGetter_ = value; }
    void SetOverload(bool value = true) { isOverload_ = value; }
    void SetVirtual(bool value = true) { isVirtual_ = value; }
    void SetStatic(bool value = true) { isStatic_ = value; }
    void SetSkip(bool value) { skip_ = value; }
    void SetReturnType(JSBFunctionType* retType) { returnType_ = retType; }
    void SetDocString(const String& docString) { docString_ = docString; }

    int FirstDefaultParameter()
    {
        for (unsigned i = 0; i < parameters_.Size(); i++)
        {
            if (parameters_[i]->initializer_.Length())
                return i;
        }

        return -1;
    }

    void AddParameter(JSBFunctionType* parm)
    {
        parameters_.Push(parm);
    }


    void Process();

    void WriteParameterMarshal(String& source);
    void WriteFunction(String& source);
    void WriteConstructor(String& source);

    void Write(String& source);

    void Dump()
    {
        String sig;
        if (!returnType_)
            sig += "void ";
        else
            sig += returnType_->ToString() + " ";

        sig += name_;
        sig += "(";
        for (unsigned i = 0; i < parameters_.Size(); i++)
        {

            sig += parameters_.At(i)->ToString();

            if (i != parameters_.Size() - 1)
                 sig += ", ";
        }
        sig += ");";

        LOGINFOF("      %s", sig.CString());

    }

private:

    SharedPtr<JSBClass> class_;

    String name_;
    String propertyName_;

    JSBFunctionType* returnType_;
    Vector<JSBFunctionType*> parameters_;

    String docString_;

    bool isConstructor_;
    bool isDestructor_;
    bool isGetter_;
    bool isSetter_;
    bool isOverload_;
    bool isVirtual_;
    bool isStatic_;
    bool skip_;

};

}
