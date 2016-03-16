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

#include <Atomic/Atomic.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/Resource/ResourceCache.h>

#include "JSBind.h"
#include "JSBPackage.h"
#include "JSBModule.h"
#include "JSBFunction.h"
#include "JSBDoc.h"

namespace ToolCore
{

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
        scriptType = "Atomic." + ftype->type_->asEnumType()->enum_->GetName();

    if (ftype->type_->asEnumType())
        scriptType = "Atomic." + ftype->type_->asEnumType()->enum_->GetName();

    if (ftype->type_->asClassType())
        scriptType = "Atomic." + ftype->type_->asClassType()->class_->GetName();

    return scriptType;

}

void JSBDoc::Begin()
{
    source_ += "//////////////////////////////////////////////////////////\n";
    source_ += "// IMPORTANT: THIS FILE IS GENERATED, CHANGES WILL BE LOST\n";
    source_ += "//////////////////////////////////////////////////////////\n\n";

    source_ += "//Atomic JSDoc Definitions\n\n";

    source_ += "/**\n * Atomic Game Engine\n * @namespace\n*/\n var " + package_->GetName() + " = {}\n\n";
}

void JSBDoc::End()
{
}

String JSBDoc::GenFunctionDoc(JSBFunction* function)
{
    if (function->Skip())
        return "";

    String params;

    Vector<JSBFunctionType*>& parameters = function->GetParameters();

    for (unsigned i = 0; i < parameters.Size(); i++)
    {
        JSBFunctionType* ftype = parameters.At(i);

        String scriptType = GetScriptType(ftype);

        if (scriptType == "Atomic.Context")
            continue;

        // mark as optional
        if (ftype->initializer_.Length())
            scriptType += "=";

        params += " * @param {" + scriptType + "} " +  ftype->name_ + "\n";
    }

    String returns;

    if (function->GetReturnType())
        returns = " * @returns { " +  GetScriptType(function->GetReturnType()) +  "}\n";

    String docString;

    if (function->IsConstructor())
    {
        docString.AppendWithFormat("%s", params.CString());

    }
    else
    {
        docString.AppendWithFormat(" * %s\n * @memberof Atomic.%s.prototype\n%s%s\n",
                                   function->GetDocString().CString(),
                                   function->GetClass()->GetName().CString(),
                                   params.CString(),
                                   returns.CString());

    }

    return docString;
}

void JSBDoc::ExportModuleClasses(JSBModule* module)
{
    Vector<SharedPtr<JSBClass>> classes = module->GetClasses();

    if (!classes.Size())
        return;

    source_ += "\n";

    for (unsigned i = 0; i < classes.Size(); i++)
    {
        JSBClass* klass = classes.At(i);

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

            String desc;

            if (prop->getter_ && !prop->getter_->Skip())
            {
                desc = prop->getter_->GetDocString();
                ftype = prop->getter_->GetReturnType();
            }
            else if (prop->setter_ && !prop->setter_->Skip())
            {
                ftype = prop->setter_->GetParameters()[0];
            }

            if (prop->setter_ && prop->setter_->GetDocString().Length())
            {
                // overwrite getter docstring if it exsists
                desc = prop->setter_->GetDocString();

            }


            if (!ftype)
                continue;

            String scriptType = GetScriptType(ftype);
            String scriptName = prop->GetCasePropertyName();

            if (desc.Length())
            {
                source_ += " * @property {" +  scriptType + "} " + scriptName + " - " + desc + "\n";
            }
            else
            {
                source_ += " * @property {" +  scriptType + "} " + scriptName + "\n";
            }

        }

        JSBFunction* constructor = klass->GetConstructor();
        if (constructor)
        {
            String docs = GenFunctionDoc(constructor);
            source_ += docs;
        }

        source_ += "*/ \nfunction " + klass->GetName() + "() {};\n\n";

        // FUNCTIONS

        PODVector<JSBFunction*>& functions = klass->GetFunctions();

        for (unsigned j = 0; j < functions.Size(); j++)
        {
            JSBFunction* func = functions[j];

            if (func->IsConstructor() || func->IsDestructor() || func->Skip())
                continue;

            String docs = GenFunctionDoc(func);

            String scriptName =  func->GetName();
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

void JSBDoc::ExportModuleConstants(JSBModule* module)
{

    const Vector<String>& constants = module->GetConstants().Keys();

    if (!constants.Size())
        return;

    source_ += "\n";
    for (unsigned i = 0; i < constants.Size(); i++)
    {
        const String& cname = constants.At(i);

        source_ += "/**\n * @memberof Atomic\n * @type {number}\n */\nvar " + cname + ";\n";
    }

    source_ += "\n";

}

void JSBDoc::ExportModuleEnums(JSBModule* module)
{
    Vector<SharedPtr<JSBEnum>> enums = module->GetEnums();

    for (unsigned i = 0; i < enums.Size(); i++)
    {
        JSBEnum* _enum = enums.At(i);

        source_ += "/**\n * @memberof Atomic\n * @readonly\n * @enum {number}\n */\n";

        source_ += " var " + _enum->GetName() + " =  {\n";

        HashMap<String, String>& values = _enum->GetValues();

        HashMap<String, String>::ConstIterator itr = values.Begin();

        while (itr != values.End())
        {
            String name = (*itr).first_;

            source_ += "    " + name + " : undefined";

            itr++;

            if (itr !=  values.End())
                source_ += ",\n";

        }

        source_ += "\n\n};\n\n";

    }

}
void JSBDoc::WriteToFile(const String &path)
{
    FileSystem* fs = package_->GetSubsystem<FileSystem>();

    String jsDocPath = GetPath(path);

    if (!fs->DirExists(jsDocPath))
        fs->CreateDir(jsDocPath);

    File file(package_->GetContext());
    file.Open(path, FILE_WRITE);

    file.Write(source_.CString(), source_.Length());

    file.Close();

}

void JSBDoc::Emit(JSBPackage* package, const String& path)
{
    package_ = package;

    Vector<SharedPtr<JSBModule>>& modules = package->GetModules();
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
        source_ += "// MODULE: " + modules[i]->GetName() + "\n";
        source_ += "//----------------------------------------------------\n\n";
        ExportModuleClasses(modules[i]);
    }


    End();
    WriteToFile(path);

}

}
