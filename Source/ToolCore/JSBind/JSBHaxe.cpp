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
#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/Resource/ResourceCache.h>

#include "JSBind.h"
#include "JSBPackage.h"
#include "JSBModule.h"
#include "JSBFunction.h"
#include "JSBHaxe.h"

namespace ToolCore
{

    String JSBHaxe::GetScriptType(JSBFunctionType* ftype)
    {
        String scriptType = "Dynamic";

        if (ftype->type_->asPrimitiveType())
        {
            JSBPrimitiveType* ptype = ftype->type_->asPrimitiveType();
            scriptType = GetPrimitiveType(ptype);
            return scriptType;
        }

        if (ftype->type_->asStringHashType() || ftype->type_->asStringType())
            scriptType = "String";

        if (ftype->type_->asEnumType())
            scriptType = ftype->type_->asEnumType()->enum_->GetName();

        if (ftype->type_->asClassType())
        {
            JSBClass* klass = ftype->type_->asClassType()->class_;

            scriptType = klass->GetName();

            if (klass->GetPackage()->GetName() != package_->GetName())
            {

                scriptType = klass->GetPackage()->GetName() + "." + klass->GetName();

            }

        }

        if (ftype->type_->asVectorType())
        {
            JSBVectorType* vectorType = ftype->type_->asVectorType();
            scriptType = "Array<String>";
        }

        return scriptType;

    }

    String JSBHaxe::GetPrimitiveType(JSBPrimitiveType* ptype)
    {
        if (ptype->kind_ == JSBPrimitiveType::Bool)
            return "Bool";
        if (ptype->kind_ == JSBPrimitiveType::Int && ptype->isUnsigned_)
            return "UInt";
        else if (ptype->kind_ == JSBPrimitiveType::Int)
            return "Int";
        if (ptype->kind_ == JSBPrimitiveType::Float)
            return "Float";
        return "Dynamic";
    }

    void JSBHaxe::Begin()
    {
        source_ += "//Atomic Haxe Definitions\n\n";

        source_ += "extern class " + package_->GetName() + " {\n\n";

        if (package_->GetName() == "Atomic") {
            //hand written data
            source_ += "    public static var engine : Engine;\n";
            source_ += "    public static var graphics: Graphics;\n";
            source_ += "    public static var renderer: Renderer;\n";
            source_ += "    public static var cache: ResourceCache;\n";
            source_ += "    public static var input: Input;\n";
            source_ += "    public static var fileSystem: FileSystem;\n";
            source_ += "    public static var network: Network;\n";
            source_ += "    public static var ui: UI;\n";
            source_ += "    public static var audio: Audio;\n";
        }
    }

    void JSBHaxe::End()
    {
        source_ += "\n}\n";
    }

    bool JSBHaxe::IsOverride(JSBFunction* function) {
        return findFunctionInBase(function);
    }

    JSBFunction* JSBHaxe::findFunctionInBase(JSBFunction* function) {
        PODVector<JSBClass*>& base = function->GetClass()->GetBaseClasses();
        for (unsigned j = 0; j < base.Size(); j++)
        {
            JSBClass* klass = base[j];

            PODVector<JSBFunction*>& functions = klass->GetFunctions();

            for (unsigned j = 0; j < functions.Size(); j++)
            {

                JSBFunction* func = functions[j];

                if (func->IsConstructor() || func->IsDestructor() )
                    continue;

                if (func->GetName() == function->GetName()) {
                    return func;
                }
            }
        }
        return NULL;
    }

    void JSBHaxe::ExportFunction(JSBFunction* function)
    {
        //probably no need to check
        if (function->Skip())
            return;

        String scriptName = "new";

        if (!function->IsConstructor())
        {
            scriptName = function->GetName();
            scriptName[0] = tolower(scriptName[0]);
        }

        if (function->GetDocString().Length())
            source_ += "      //" + function->GetDocString() + "\n";

        //Add @:overload annotation
        if (IsOverride(function))
        {
            //if (function->IsOverload())
            {
                source_ += "    @:overload(function(";
                Vector<JSBFunctionType*>& parameters = function->GetParameters();

                for (unsigned i = 0; i < parameters.Size(); i++)
                {
                    JSBFunctionType* ftype = parameters.At(i);

                    String scriptType = GetScriptType(ftype);

                    if (scriptType == "Context" || scriptType == "Atomic.Context")
                        continue;

                    String name = ftype->name_;

                    if (ftype->initializer_.Length())
                        source_ += "?";

                    source_ += name;
                    source_ += ": " + scriptType;

                    if (i < parameters.Size() - 1)
                        source_ += ", ";
                }

                if (!function->GetReturnType())
                    source_ += "): Void{})\n";
                else
                    source_ += "): " + GetScriptType(function->GetReturnType()) + "{})\n";
            }
            source_ += "    override function ";
        }
        else
        {
            source_ += "    function ";
        }

        source_ += scriptName + "(";

        Vector<JSBFunctionType*>& parameters = function->GetParameters();

        if (IsOverride(function))
            parameters = findFunctionInBase(function)->GetParameters();

        for (unsigned i = 0; i < parameters.Size(); i++)
        {
            JSBFunctionType* ftype = parameters.At(i);

            String scriptType = GetScriptType(ftype);

            if (scriptType == "Context" || scriptType == "Atomic.Context")
                continue;

            String name = ftype->name_;

            if (ftype->initializer_.Length())
                source_ += "?";

            source_ += name;
            source_ += ": " + scriptType;

            if (i < parameters.Size() - 1)
                source_ += ", ";
        }

        if (function->IsConstructor())
            source_ += ");\n";
        else
        {
            if (!function->GetReturnType())
                source_ += "): Void;\n";
            else
                source_ += "): " + GetScriptType(function->GetReturnType()) + ";\n";

        }
    }

    bool JSBHaxe::checkV(JSBClass* clazz, const String name, const String type) {
        PODVector<JSBClass*>& base = clazz->GetBaseClasses();
        for (unsigned j = 0; j < base.Size(); j++)
        {
            JSBClass* baseClass = base[j];

            Vector<String> propertyNames;

            baseClass->GetPropertyNames(propertyNames);

            for (unsigned i = 0; i < propertyNames.Size(); i++)
            {
                JSBProperty* prop = baseClass->GetProperty(propertyNames[i]);
                JSBFunctionType* ftype = NULL;

                if (prop->getter_ && !prop->getter_->Skip())
                {
                    ftype = prop->getter_->GetReturnType();
                }
                else if (prop->setter_ && !prop->setter_->Skip())
                    ftype = prop->setter_->GetParameters()[0];

                if (!ftype)
                    continue;

                String scriptName = prop->GetCasePropertyName();
                if (scriptName == name)
                {
                    return true;
                }
            }
        }
        return false;
    }

    void JSBHaxe::ExportModuleClasses(JSBModule* module)
    {
        Vector<SharedPtr<JSBClass>> classes = module->GetClasses();

        if (!classes.Size())
            return;

        source_ += "\n";

        for (unsigned i = 0; i < classes.Size(); i++)
        {
            JSBClass* klass = classes.At(i);

            if (klass->IsNumberArray()) {
                source_ += "typedef " + klass->GetName() + " = Array<Float>;\n";
                continue;
            }

            source_ += "@:native(\"Atomic." + klass->GetName() + "\")\n";

            source_ += "extern class " + klass->GetName();

            JSBClass* base = klass->GetBaseClass();

            if (base)
            {
                    source_ += " extends " + base->GetName();
            }

            source_ += " {\n\n";

            Vector<String> propertyNames;

            klass->GetPropertyNames(propertyNames);

            for (unsigned j = 0; j < propertyNames.Size(); j++)
            {

                JSBProperty* prop = klass->GetProperty(propertyNames[j]);

                JSBFunctionType* ftype = NULL;

                if (prop->getter_ && !prop->getter_->Skip())
                {
                    ftype = prop->getter_->GetReturnType();
                }
                else if (prop->setter_ && !prop->setter_->Skip())
                    ftype = prop->setter_->GetParameters()[0];

                if (!ftype)
                    continue;

                String scriptType = GetScriptType(ftype);
                String scriptName = prop->GetCasePropertyName();

                if (!checkV(klass, scriptName, scriptType)) {
                    //rename haxe reserved words
                    if (scriptName == "override") {
                        scriptName = "overide";
                    }
                    if (scriptName == "dynamic") {
                        scriptName = "dynamik";
                    }
                    source_ += "    var " + scriptName + ": " + scriptType + ";\n";
                }

            }

            if (propertyNames.Size())
                source_ += "\n";

            JSBFunction* constructor = klass->GetConstructor();
            if (constructor)
            {
                ExportFunction(constructor);
                source_ += "\n";
            }

            PODVector<JSBFunction*>& functions = klass->GetFunctions();

            for (unsigned j = 0; j < functions.Size(); j++)
            {

                JSBFunction* func = functions[j];

                if (func->IsConstructor() || func->IsDestructor() || func->Skip())
                    continue;

                ExportFunction(func);
            }

            for (unsigned j = 0; j < klass->GetNumHaxeDecl(); j++)
            {
                source_ += "      " + klass->GetHaxeDecl(j) + "\n";
            }

            source_ += "\n}\n\n";

        }

        source_ += "\n";

    }


    void JSBHaxe::ExportModuleConstants(JSBModule* module)
    {
        HashMap<String, JSBModule::Constant>& constants = module->GetConstants();

        const Vector<String>& constantsName = constants.Keys();

        if (!constants.Size())
            return;

        source_ += "\n";

        for (unsigned i = 0; i < constantsName.Size(); i++)
        {
            const String& cname = constantsName.At(i);

            JSBModule::Constant& constant = constants[cname];

            source_ += "    public static var " + cname + ": " + GetPrimitiveType(constant.type) + ";\n";
        }

        source_ += "\n";
    }

    void JSBHaxe::ExportEnums(JSBModule* module)
    {

        Vector<SharedPtr<JSBEnum>> enums = module->GetEnums();

        for (unsigned i = 0; i <enums.Size(); i++)
        {
            JSBEnum* _enum = enums[i];

            source_ += "@:native(\"Atomic\")\n";
            source_ += "extern enum " + _enum->GetName() + " {\n";

            HashMap<String, String>& values = _enum->GetValues();

            HashMap<String, String>::ConstIterator itr = values.Begin();

            while (itr != values.End())
            {
                String name = (*itr).first_;

                source_ += "    " + name + ";\n";

                itr++;
            }

            source_ += "}\n";
        }

    }

    void JSBHaxe::WriteToFile(const String &path)
    {
        File file(package_->GetContext());
        file.Open(path, FILE_WRITE);

        file.Write(source_.CString(), source_.Length());

        file.Close();

    }

    void JSBHaxe::Emit(JSBPackage* package, const String& path)
    {
        package_ = package;

        Vector<SharedPtr<JSBModule>>& modules = package->GetModules();

        source_ += "package atomic;\n\n";

        for (unsigned i = 0; i < modules.Size(); i++)
        {
            ExportEnums(modules[i]);
        }

        Begin();

        for (unsigned i = 0; i < modules.Size(); i++)
        {
            ExportModuleConstants(modules[i]);
        }

        End();

        for (unsigned i = 0; i < modules.Size(); i++)
        {
            source_ += "\n//----------------------------------------------------\n";
            source_ += "// MODULE: " + modules[i]->GetName() + "\n";
            source_ += "//----------------------------------------------------\n\n";
            ExportModuleClasses(modules[i]);
        }


        //End();
        WriteToFile(path);

    }

}
