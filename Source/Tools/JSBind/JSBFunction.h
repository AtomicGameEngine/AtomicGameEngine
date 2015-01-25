

#pragma once

#include <Atomic/IO/Log.h>
#include "JSBClass.h"
#include "JSBType.h"

class JSBFunctionType
{
public:

    JSBFunctionType(JSBType* type) : type_(type)
    {
        isSharedPtr_ = false;
        isPointer_ = false;
        isReference_ = false;
    }

    bool isSharedPtr_;
    bool isPointer_;
    bool isReference_;

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

class JSBFunction
{
public:

    JSBClass* class_;
    String name_;
    String propertyName_;
    JSBFunctionType* returnType_;
    Vector<JSBFunctionType*> parameters_;

    bool isConstructor_;
    bool isDestructor_;
    bool isGetter_;
    bool isSetter_;
    bool isOverride_;
    bool skip_;

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

    JSBFunction(JSBClass* klass) : class_(klass), returnType_(0),
                                   isConstructor_(false), isDestructor_(false),
                                   isGetter_(false), isSetter_(false),
                                   isOverride_(false), skip_(false)
    {

    }

    void SetSkip(bool value) { skip_ = value; }
    bool Skip() { return skip_; }

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

};
