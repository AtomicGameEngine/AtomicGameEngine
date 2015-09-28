

#include "CSTypeHelper.h"

namespace ToolCore
{

String CSTypeHelper::GetManagedPrimitiveType(JSBPrimitiveType* ptype)
{
    if (ptype->kind_ == JSBPrimitiveType::Bool)
        return "bool";
    if (ptype->kind_ == JSBPrimitiveType::Int && ptype->isUnsigned_)
        return "uint";
    else if (ptype->kind_ == JSBPrimitiveType::Int)
        return "int";
    if (ptype->kind_ == JSBPrimitiveType::Float)
        return "float";
    if (ptype->kind_ == JSBPrimitiveType::Char && ptype->isUnsigned_)
        return "byte";
    else if (ptype->kind_ == JSBPrimitiveType::Char)
        return "char";
    if (ptype->kind_ == JSBPrimitiveType::Short)
        return "short";

    assert(0);
    return "";
}


String CSTypeHelper::GetManagedTypeString(JSBType* type)
{
    String value;

    if (type->asClassType())
    {
        JSBClassType* classType = type->asClassType();
        value = classType->class_->GetName();
    }
    else if (type->asStringType() || type->asStringHashType())
    {
        value = "string";
    }
    else if (type->asEnumType())
    {
        value = type->asEnumType()->enum_->GetName();
    }
    else if (type->asPrimitiveType())
    {
        value = GetManagedPrimitiveType(type->asPrimitiveType());
    }
    else if (type->asVectorType())
    {
        JSBVectorType* vectorType = type->asVectorType();

        value = GetManagedTypeString(vectorType->vectorType_) + "[]";
    }

    return value;
}

String CSTypeHelper::GetManagedTypeString(JSBFunctionType* ftype, bool addName)
{
    if (!ftype)
        return "void";

    String parameter = GetManagedTypeString(ftype->type_);

    if (ftype->name_.Length())
    {
        if (addName)
        {
            if (ftype->name_ == "object")
            {
                parameter += " _object";
            }
            else
            {
                parameter += " " + ftype->name_;
            }

        }

        /*
        if (ftype->initializer_.Length())
            parameter += " = " + ftype->initializer_;
        */
    }

    return parameter;

}

String CSTypeHelper::GetNativeTypeString(JSBType* type)
{
    String value;

    if (type->asClassType())
    {
        JSBClassType* classType = type->asClassType();
        if (classType->class_->IsNumberArray())
            value = ToString("%s*", classType->class_->GetNativeName().CString());
    }
    else if (type->asStringType())
    {
        value = "const char*";
    }
    else if (type->asStringHashType())
    {
        value = "const char*";
    }
    else if (type->asEnumType())
    {
        value = type->asEnumType()->enum_->GetName();
    }
    else if (type->asPrimitiveType())
    {
        value = type->asPrimitiveType()->ToString();
    }
    else if (type->asVectorType())
    {
        assert(0);
    }

    return value;
}

String CSTypeHelper::GetNativeTypeString(JSBFunctionType* ftype)
{
    if (!ftype)
        return "void";

    String value = GetNativeTypeString(ftype->type_);

    return value;

}

String CSTypeHelper::GetPInvokeTypeString(JSBType* type)
{
    String value;

    if (type->asClassType())
    {
        JSBClassType* classType = type->asClassType();
        if (classType->class_->IsNumberArray())
            value = "void";
        else
            value = "IntPtr";
    }
    else if (type->asStringType() || type->asStringHashType())
    {
        value = "string";
    }
    else if (type->asEnumType())
    {
        value = type->asEnumType()->enum_->GetName();
    }
    else if (type->asPrimitiveType())
    {
        value = GetManagedPrimitiveType(type->asPrimitiveType());
    }
    else if (type->asVectorType())
    {
        JSBVectorType* vectorType = type->asVectorType();

        value = GetManagedTypeString(vectorType->vectorType_) + "[]";
    }

    return value;
}

String CSTypeHelper::GetPInvokeTypeString(JSBFunctionType* ftype)
{
    if (!ftype)
        return "void";

    String value = GetPInvokeTypeString(ftype->type_);

    return value;

}

bool CSTypeHelper::IsSimpleReturn(JSBType* type)
{
    if (type->asClassType())
    {
        return false;
    }
    else if (type->asStringType() || type->asStringHashType())
    {
        return true;
    }
    else if (type->asEnumType())
    {
        return true;
    }
    else if (type->asPrimitiveType())
    {
        return true;
    }
    else if (type->asVectorType())
    {
        return true;
    }

    return true;
}

bool CSTypeHelper::IsSimpleReturn(JSBFunctionType* ftype)
{
    if (!ftype)
        return true;

    return IsSimpleReturn(ftype->type_);

}

}
