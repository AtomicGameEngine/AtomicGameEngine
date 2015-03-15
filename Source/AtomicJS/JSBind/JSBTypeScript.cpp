// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include <Atomic/Atomic.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/Resource/ResourceCache.h>
#include "JSBind.h"
#include "JSBindings.h"
#include "JSBModule.h"
#include "JSBFunction.h"
#include "JSBTypeScript.h"


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
        scriptType = ftype->type_->asEnumType()->enum_->name_;

    if (ftype->type_->asEnumType())
        scriptType = ftype->type_->asEnumType()->enum_->name_;

    if (ftype->type_->asClassType())
        scriptType = ftype->type_->asClassType()->class_->GetName();

    return scriptType;

}

void JSBTypeScript::Begin()
{
    source_ += "//Atomic TypeScript Definitions\n\n\n";
    source_ += "declare module Atomic {\n\n";
}

void JSBTypeScript::End()
{
    source_ += "\n}\n";
}

void JSBTypeScript::ExportFunction(JSBFunction* function)
{
    if (function->Skip())
        return;

    String scriptName = "constructor";

    //constructor(width?: number, height?: number, options?: PixiRendererOptions);

    if (!function->isConstructor_)
    {
        scriptName =  function->name_;
        scriptName[0] = tolower(scriptName[0]);
    }

    if (function->docString_.Length())
        source_ += "      " + function->docString_ + "\n";

    source_ += "      " + scriptName + "(";

    for (unsigned i = 0; i < function->parameters_.Size(); i++)
    {
        JSBFunctionType* ftype = function->parameters_.At(i);

        String scriptType = GetScriptType(ftype);

        if (scriptType == "Context")
            continue;

        source_ += ftype->name_;

        if (ftype->initializer_.Length())
            source_ += "?";

        source_ += ": " + scriptType;

        if (i < function->parameters_.Size() - 1)
            source_ += ", ";
    }

    if (function->isConstructor_)
        source_ += ");\n";
    else
    {
        if (!function->returnType_)
            source_ += "): void;\n";
        else
            source_ += "): " + GetScriptType(function->returnType_) + ";\n";

    }



}

void JSBTypeScript::ExportModuleClasses(const String& moduleName)
{
    JSBModule* module = JSBindings::Instance()->GetModuleByName(moduleName);

    if (!module->classes_.Size())
        return;

    source_ += "\n";

    for (unsigned i = 0; i < module->classes_.Size(); i++)
    {
        JSBClass* klass = module->classes_.At(i);

        source_ += "   export class " + klass->GetName();
        if (klass->GetBaseClass())
            source_ += " extends " + klass->GetBaseClass()->GetName();

        source_ += " {\n\n";

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

            source_ += "      " + scriptName + ": " + scriptType + ";\n";

        }

        if (propertyNames.Size())
            source_ += "\n";

        JSBFunction* constructor = klass->GetConstructor();
        if (constructor)
        {
            ExportFunction(constructor);
            source_ += "\n";
        }

        for (unsigned j = 0; j < klass->GetFunctionCount(); j++)
        {

            JSBFunction* func = klass->GetFunction(j);

            if (func->isConstructor_ || func->isDestructor_ || func->Skip())
                continue;

            ExportFunction(func);


        }



        source_ += "\n   }\n\n";

    }

    source_ += "\n";

}


void JSBTypeScript::ExportModuleConstants(const String& moduleName)
{
    JSBModule* module = JSBindings::Instance()->GetModuleByName(moduleName);

    if (!module->constants_.Size())
        return;

    source_ += "\n";
    for (unsigned i = 0; i < module->constants_.Size(); i++)
    {
        const String& cname = module->constants_.At(i);

        source_ += "   export var " + cname + ": number;\n";
    }

    source_ += "\n";

}

void JSBTypeScript::ExportModuleEnums(const String& moduleName)
{
    JSBModule* module = JSBindings::Instance()->GetModuleByName(moduleName);

    for (unsigned i = 0; i < module->enums_.Size(); i++)
    {
        JSBEnum* _enum = module->enums_.At(i);

        source_ += "   export enum " + _enum->name_;
        source_ += " {\n\n";

        for (unsigned j = 0; j < _enum->values_.Size(); j++)
        {
            source_ += "      " + _enum->values_[j];
            if (j !=  _enum->values_.Size() - 1)
                source_ += ",\n";
        }

        source_ += "\n\n   }\n\n";



    }

}

void JSBTypeScript::WriteToFile(const String &path)
{
    File file(JSBind::context_);
    file.Open(path, FILE_WRITE);

    file.Write(source_.CString(), source_.Length());

    file.Close();

}

void JSBTypeScript::Emit(const String& path)
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
