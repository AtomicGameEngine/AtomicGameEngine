
#include <Atomic/Atomic.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/Resource/ResourceCache.h>
#include "JSBind.h"
#include "JSBindings.h"
#include "JSBModule.h"
#include "JSBFunction.h"
#include "JSBDoc.h"


static String GetScriptType(JSBFunctionType* ftype)
{
    String scriptType = "number";

    if (ftype->type_->asPrimitiveType())
    {
        JSBPrimitiveType* ptype = ftype->type_->asPrimitiveType();
        if (ptype->kind_ == JSBPrimitiveType::Bool)
            scriptType = "boolean";

    }

    if (ftype->type_->asStringHashType() || ftype->type_->asStringType())
        scriptType = "string";

    if (ftype->type_->asEnumType())
        scriptType = "Atomic." + ftype->type_->asEnumType()->enum_->name_;

    if (ftype->type_->asEnumType())
        scriptType = "Atomic." + ftype->type_->asEnumType()->enum_->name_;

    if (ftype->type_->asClassType())
        scriptType = "Atomic." + ftype->type_->asClassType()->class_->GetName();

    return scriptType;

}

void JSBDoc::Begin()
{
    source_ += "//Atomic JSDoc Definitions\n\n\n";

    source_ += "/**\n * Atomic Game Engine\n * @namespace\n*/\n var Atomic = {}\n\n";
}

void JSBDoc::End()
{
}

String JSBDoc::GenFunctionDoc(JSBFunction* function)
{
    if (function->Skip())
        return "";

    String params;

    for (unsigned i = 0; i < function->parameters_.Size(); i++)
    {
        JSBFunctionType* ftype = function->parameters_.At(i);

        String scriptType = GetScriptType(ftype);

        if (scriptType == "Atomic.Context")
            continue;

        // mark as optional
        if (ftype->initializer_.Length())
            scriptType += "=";

        params += " * @param {" + scriptType + "} " +  ftype->name_ + "\n";
    }

    String returns;

    if (function->returnType_)
        returns = " * @returns { " +  GetScriptType(function->returnType_) +  "}\n";

    String docString;

    if (function->isConstructor_)
    {
        docString.AppendWithFormat("%s", params.CString());

    }
    else
    {
        docString.AppendWithFormat(" * %s\n * @memberof %s.prototype\n%s%s\n",
                                   function->docString_.CString(),
                                   function->class_->GetName().CString(),
                                   params.CString(),
                                   returns.CString());

    }

    return docString;
}

void JSBDoc::ExportModuleClasses(const String& moduleName)
{
    JSBModule* module = JSBindings::Instance()->GetModuleByName(moduleName);

    if (!module->classes_.Size())
        return;

    source_ += "\n";

    for (unsigned i = 0; i < module->classes_.Size(); i++)
    {
        JSBClass* klass = module->classes_.At(i);

        source_ += "/**\n * @class\n* @memberof Atomic\n";

        if (klass->GetBaseClass())
            source_ += " * @augments Atomic." + klass->GetBaseClass()->GetName()+ "\n";

        // PROPERTIES
        Vector<String> propertyNames;

        klass->GetPropertyNames(propertyNames);

        for (unsigned j = 0; j < propertyNames.Size(); j++)
        {

            JSBProperty* prop = klass->GetProperty(propertyNames[j]);

            JSBFunctionType* ftype = NULL;

            if (prop->getter_ && !prop->getter_->Skip())
            {
                ftype = prop->getter_->returnType_;
            }
            else if (prop->setter_ && !prop->setter_->Skip())
                ftype = prop->setter_->parameters_[0];

            if (!ftype)
                continue;

            String scriptType = GetScriptType(ftype);

            String scriptName =  propertyNames[j];
            scriptName[0] = tolower(scriptName[0]);

            source_ += " * @property {" +  scriptType + "} " + scriptName + "\n";

        }

        JSBFunction* constructor = klass->GetConstructor();
        if (constructor)
        {
            String docs = GenFunctionDoc(constructor);
            source_ += docs;
        }

        source_ += "*/ \nfunction " + klass->GetName() + "() {};\n\n";

        // FUNCTIONS
        for (unsigned j = 0; j < klass->GetFunctionCount(); j++)
        {
            JSBFunction* func = klass->GetFunction(j);

            if (func->isConstructor_ || func->isDestructor_ || func->Skip())
                continue;

            String docs = GenFunctionDoc(func);

            String scriptName =  func->name_;
            scriptName[0] = tolower(scriptName[0]);

            if (scriptName == "delete")
                scriptName = "__delete";

            String docString;
            docString.AppendWithFormat("/**\n %s */\n function %s() {};\n\n",
                                       docs.CString(),
                                       scriptName.CString());

            source_ += docString;

        }

    }

}

void JSBDoc::ExportModuleConstants(const String& moduleName)
{
    JSBModule* module = JSBindings::Instance()->GetModuleByName(moduleName);

    if (!module->constants_.Size())
        return;

    source_ += "\n";
    for (unsigned i = 0; i < module->constants_.Size(); i++)
    {
        const String& cname = module->constants_.At(i);

        source_ += "/**\n * @memberof Atomic\n * @type {number}\n */\nvar " + cname + ";\n";
    }

    source_ += "\n";

}

void JSBDoc::ExportModuleEnums(const String& moduleName)
{
    JSBModule* module = JSBindings::Instance()->GetModuleByName(moduleName);

    for (unsigned i = 0; i < module->enums_.Size(); i++)
    {
        JSBEnum* _enum = module->enums_.At(i);

        source_ += "/**\n * @memberof Atomic\n * @readonly\n * @enum {number}\n */\n";

        source_ += " var " + _enum->name_ + " =  {\n";

        for (unsigned j = 0; j < _enum->values_.Size(); j++)
        {
            source_ += "    " + _enum->values_[j] + " : undefined";

            if (j !=  _enum->values_.Size() - 1)
                source_ += ",\n";
        }

        source_ += "\n\n};\n\n";

    }

}
void JSBDoc::WriteToFile(const String &path)
{
    File file(JSBind::context_);
    file.Open(path, FILE_WRITE);

    file.Write(source_.CString(), source_.Length());

    file.Close();

}

void JSBDoc::Emit(const String& path)
{
    Vector<String> modules;
    modules.Push("Container");
    modules.Push("Core");
    modules.Push("Engine");
    modules.Push("Resource");
    modules.Push("Scene");
    modules.Push("UI");
    modules.Push("Atomic2D");
    modules.Push("Audio");
    modules.Push("Graphics");
    modules.Push("Input");
    modules.Push("IO");
    //modules.Push("Math");
    modules.Push("Navigation");
    modules.Push("Network");
    modules.Push("Physics");
    modules.Push("Javascript");

    Begin();

    for (unsigned i = 0; i < modules.Size(); i++)
    {
        ExportModuleEnums(modules[i]);
    }

    for (unsigned i = 0; i < modules.Size(); i++)
    {
        ExportModuleConstants(modules[i]);
    }

    for (unsigned i = 0; i < modules.Size(); i++)
    {
        source_ += "\n//----------------------------------------------------\n";
        source_ += "// MODULE: " + modules[i] + "\n";
        source_ += "//----------------------------------------------------\n\n";
        ExportModuleClasses(modules[i]);
    }


    End();
    WriteToFile(path);

}
