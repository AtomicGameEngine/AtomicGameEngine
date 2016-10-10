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
#include "JSBindTypes.h"
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

        if (type_->asStringType() || type_->asStringHashType())
        {
           if (other->type_->asStringType() || other->type_->asStringHashType())
               return true;
        }

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

    JSBFunction(JSBClass* klass);

    const String& GetName() const { return name_; }

    bool Match(JSBFunction* func);

    bool IsConstructor() const { return isConstructor_; }
    bool IsDestructor() const { return isDestructor_; }
    bool IsSetter() const { return isSetter_; }
    bool IsGetter() const { return isGetter_; }
    bool IsOverload() { return isOverload_; }
    bool IsVirtual() const { return isVirtual_; }
    bool IsStatic() const { return isStatic_; }

    bool Skip(BindingLanguage language = BINDINGLANGUAGE_ANY) const
    {
        if (skip_ || language == BINDINGLANGUAGE_ANY)
            return skip_;

        return GetSkipLanguage(language);
    }

    unsigned GetID() const { return id_; }

    JSBClass* GetClass() const { return class_; }
    const String& GetPropertyName() { return propertyName_; }
    JSBFunctionType* GetReturnType() const { return returnType_; }

    /// Get class return type or null
    JSBClass* GetReturnClass();

    const Vector<JSBFunctionType*>& GetParameters() const { return parameters_; }

    const String& GetDocString() const { return docString_; }

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

    void SetSkipLanguage(BindingLanguage language, bool skip = true)
    {
        if (skip)
        {
            if (!skipLanguages_.Contains(language))
                skipLanguages_.Push(language);
        }
        else
        {
            if (skipLanguages_.Contains(language))
                skipLanguages_.Remove(language);

        }
    }

    /// Returns true is _skip is set or skip is set for specific binding language
    bool GetSkipLanguage(BindingLanguage language) const
    {
        if (skip_)
            return true;

        return skipLanguages_.Contains(language);

    }

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
        String sig = ToString("Function ID: %u - ", id_);
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

        String skipText;

        if (GetSkipLanguage(BINDINGLANGUAGE_CSHARP))
            skipText += "C#";

        if (GetSkipLanguage(BINDINGLANGUAGE_JAVASCRIPT))
            skipText += " JavaScript";

        if (skipText.Length())
        {
            sig += ToString(" (Skipped: %s)", skipText.CString());
        }

        ATOMIC_LOGINFOF("      %s", sig.CString());

    }

private:

    unsigned id_;
    static unsigned idCounter_;

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
    PODVector<BindingLanguage> skipLanguages_;
};

}
