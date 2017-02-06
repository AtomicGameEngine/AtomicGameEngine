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
#include "JSBTypeScript.h"
#include "JSBEvent.h"

namespace ToolCore
{

String JSBTypeScript::GetScriptType(JSBFunctionType* ftype)
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
    {
        JSBEnum* jenum = ftype->type_->asEnumType()->enum_;

        scriptType = jenum->GetName();

        if (jenum->GetPackage()->GetName() != package_->GetName())
        {

            scriptType =jenum->GetPackage()->GetName() + "." + scriptType;

        }

    }

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
        if (ftype->type_->asVectorType()->isVariantVector_)
            scriptType = "ScriptVector";
        else
            scriptType = "string[]";
    }

    return scriptType;

}

void JSBTypeScript::Begin()
{
    source_ += "//////////////////////////////////////////////////////////\n";
    source_ += "// IMPORTANT: THIS FILE IS GENERATED, CHANGES WILL BE LOST\n";
    source_ += "//////////////////////////////////////////////////////////\n\n";

    source_ += "// Atomic TypeScript Definitions\n\n";

    if (package_->GetName() != "Atomic")
    {
        source_ += "/// <reference path=\"Atomic.d.ts\" />\n\n";
    }

    source_ += "declare module "+ package_->GetName() + " {\n\n";
}

void JSBTypeScript::End()
{
    source_ += "\n}\n";
}

void JSBTypeScript::ExportFunction(JSBFunction* function)
{
    if (function->Skip(BINDINGLANGUAGE_JAVASCRIPT))
        return;

    String scriptName = "constructor";

    //constructor(width?: number, height?: number, options?: PixiRendererOptions);

    if (!function->IsConstructor())
    {
        scriptName =  function->GetName();
        scriptName[0] = tolower(scriptName[0]);
    }

    if (function->GetDocString().Length())
        source_ += "      /** " + function->GetDocString() + " */\n";

    source_ += "      ";

    if (function->IsStatic())
        source_ += "static ";

    source_ += scriptName + "(";

    const Vector<JSBFunctionType*>& parameters = function->GetParameters();

    for (unsigned i = 0; i < parameters.Size(); i++)
    {
        JSBFunctionType* ftype = parameters.At(i);

        String scriptType = GetScriptType(ftype);

        if (scriptType == "Context" || scriptType == "Atomic.Context")
            continue;

        String name = ftype->name_;

        if (function->HasMutatedReturn() && i == parameters.Size() - 1)
        {
            // TODO: would be great to type this as Vector<ScriptVariant> somehow
            scriptType = "ScriptVector";
            name = "outVector";
        }

        // TS doesn't like arguments named arguments
        if (name == "arguments")
            name = "args";

        source_ += name;

        if (ftype->initializer_.Length())
            source_ += "?";

        source_ += ": " + scriptType;

        if (i < parameters.Size() - 1)
            source_ += ", ";
    }

    if (function->IsConstructor())
        source_ += ");\n";
    else
    {
        if (!function->GetReturnType())
            source_ += "): void;\n";
        else
            source_ += "): " + GetScriptType(function->GetReturnType()) + ";\n";

    }



}

inline bool CompareJSBClassesByName(const SharedPtr<JSBClass>& lhs, const SharedPtr<JSBClass>& rhs)
{
    return lhs->GetName() < rhs->GetName();
}

void JSBTypeScript::ExportModuleClasses(JSBModule* module)
{
    Vector<SharedPtr<JSBClass>> classes = module->GetClasses();

    if (!classes.Size())
        return;

    // Sort classes to ensure consistent output across machines
    Sort(classes.Begin(), classes.End(), CompareJSBClassesByName);

    source_ += "\n";

    for (unsigned i = 0; i < classes.Size(); i++)
    {
        JSBClass* klass = classes.At(i);

        source_ += "   export class " + klass->GetName();

        JSBClass* base = klass->GetBaseClass();

        if (base)
        {
            if (klass->GetPackage() != base->GetPackage())
            {
                source_ += " extends " + base->GetPackage()->GetName() + "." + base->GetName();
            }
            else
            {
                source_ += " extends " + base->GetName();
            }

        }

        source_ += " {\n\n";

        Vector<String> propertyNames;

        klass->GetPropertyNames(propertyNames);

        for (unsigned j = 0; j < propertyNames.Size(); j++)
        {

            JSBProperty* prop = klass->GetProperty(propertyNames[j]);

            JSBFunctionType* ftype = NULL;

            bool isStatic = false;

            if (prop->getter_ && !prop->getter_->Skip(BINDINGLANGUAGE_JAVASCRIPT))
            {
                ftype = prop->getter_->GetReturnType();
                isStatic = prop->getter_->IsStatic();
            }
            else if (prop->setter_ && !prop->setter_->Skip(BINDINGLANGUAGE_JAVASCRIPT))
            {
                ftype = prop->setter_->GetParameters()[0];
                isStatic = prop->setter_->IsStatic();
            }

            if (!ftype)
                continue;

            // TODO: static properies not currently working with generated TS
            if (isStatic)
                continue;

            String scriptType = GetScriptType(ftype);
            String scriptName = prop->GetCasePropertyName();

            source_ += "      " + scriptName + ": " + scriptType + ";\n";

        }

        if (propertyNames.Size())
            source_ += "\n";

        JSBFunction* constructor = klass->GetConstructor(BINDINGLANGUAGE_JAVASCRIPT);
        if (constructor)
        {
            ExportFunction(constructor);
            source_ += "\n";
        }

        PODVector<JSBFunction*>& functions = klass->GetFunctions();

        for (unsigned j = 0; j < functions.Size(); j++)
        {

            JSBFunction* func = functions[j];

            if (func->IsConstructor() || func->IsDestructor() || func->Skip(BINDINGLANGUAGE_JAVASCRIPT))
                continue;

            ExportFunction(func);


        }

        for (unsigned j = 0; j < klass->GetNumTypeScriptDecl(); j++)
        {
            source_ += "      " + klass->GetTypeScriptDecl(j) + "\n";
        }



        source_ += "\n   }\n\n";

    }

    source_ += "\n";

}


void JSBTypeScript::ExportModuleConstants(JSBModule* module)
{
    // we're going to modify the vector, so copy the keys locally instead of using a reference
    Vector<String> constants = module->GetConstants().Keys();

    if (!constants.Size())
        return;

    // Sort constants to ensure consistent output across machines
    Sort(constants.Begin(), constants.End());

    source_ += "\n";

    for (unsigned i = 0; i < constants.Size(); i++)
    {
        const String& cname = constants.At(i);

        source_ += "   export var " + cname + ": number;\n";
    }

    source_ += "\n";

}

inline bool CompareJSBEnumsByName(const SharedPtr<JSBEnum>& lhs, const SharedPtr<JSBEnum>& rhs)
{
    return lhs->GetName() < rhs->GetName();
}

void JSBTypeScript::ExportModuleEnums(JSBModule* module)
{

    Vector<SharedPtr<JSBEnum>> enums = module->GetEnums();

    // Sort enums alphabetically to ensure consistent output across machines
    Sort(enums.Begin(), enums.End(), CompareJSBEnumsByName);

    Vector<SharedPtr<JSBEnum>>::Iterator enumIter;
    for (enumIter = enums.Begin(); enumIter != enums.End(); enumIter++)
    {
        JSBEnum* _enum = *enumIter;

        source_ += "   /** enum " + _enum->GetName() + "*/\n";
        source_ += "   export enum " + _enum->GetName() + " {\n";

        HashMap<String, String>& values = _enum->GetValues();

        HashMap<String, String>::ConstIterator valsIter = values.Begin();

        while (valsIter != values.End())
        {
            String name = (*valsIter).first_;

            source_ += "       " + name;
            valsIter++;

            if (valsIter != values.End())
                source_ += ",";

            source_ += "\n";
        }
        source_ += "    }\n";
        source_ += "\n";
    }

}


void JSBTypeScript::ExportModuleEvents(JSBModule* module)
{

    String source;

    const Vector<SharedPtr<JSBEvent>>& events = module->GetEvents();

    for (unsigned i = 0; i < events.Size(); i++)
    {
        JSBEvent* event = events[i];
        String scriptEventName = event->GetScriptEventName(BINDINGLANGUAGE_JAVASCRIPT);

        // Write the event type
        source += ToString("    /** Event type to use in calls requiring the event such as 'sendEvent'.  Event Type is: \"%s\" **/\n", event->GetEventName().CString());
        source += ToString("    export var %sType : Atomic.EventType;\n\n", scriptEventName.CString());

        // Write the event interface
        source += ToString("    /** object returned in the callback for the %s event.**/\n", event->GetEventName().CString());
        source += ToString("    export interface %s extends Atomic.EventData {\n", scriptEventName.CString());

        // parameters

        const Vector<JSBEvent::EventParam>& params = event->GetParameters();

        for (unsigned j = 0; j < params.Size(); j++)
        {
            const JSBEvent::EventParam& p = params[j];

            JSBClass* cls = JSBPackage::GetClassAllPackages(p.typeInfo_);

            String typeName = p.typeInfo_;
            String enumTypeName = p.enumTypeName_;

            String paramName = p.paramName_;

            //TODO: Is there a standard naming module that could handle this?
            if (paramName == "GUID")
                paramName = "guid";
            else if (paramName == "RefID")
                paramName = "refid"; // do nothing
            else
                paramName[0] = tolower(paramName[0]);

            if (!cls)
                typeName = typeName.ToLower();
            else
                typeName = cls->GetName();

            bool mapped = false;

            if (typeName == "int" || typeName == "float" || typeName == "unsigned" || typeName == "uint" || typeName == "double")
            {
                typeName = "number";
                mapped = true;
            } else if (typeName == "bool" || typeName == "boolean") {
                typeName = "boolean";
                mapped = true;
            } else if (typeName == "string") {
                mapped = true;
            } else if (cls) {
                typeName = ToString("%s.%s", cls->GetPackage()->GetName().CString(), typeName.CString());
                mapped = true;
            } else if (typeName == "enum") {
                if (enumTypeName.Length())
                    typeName = package_->GetName() + "." + enumTypeName;
                else
                    typeName = "number";

                mapped = true;
            }

            if (p.comment_.Length())
            {
                source += ToString("        /** %s */\n", p.comment_.CString());
            }

            if (mapped == true) {
                source += ToString("        %s : %s;\n", paramName.CString(), typeName.CString());
            } else {
                source += ToString("        /** Unmapped Native Type: %s */\n", p.typeInfo_.CString());
                source += ToString("        %s : any;\n", paramName.CString());
            }
        }

        source += "    }\n\n";

        // Write the event function signature
        if (event->GetEventComment().Length())
        {
            source += "    /**\n";
            source += ToString("     Wrapper function to generate a properly formatted event handler to pass to 'subscribeToEvent' for the %s event. \n\n", event->GetEventName().CString());
            source += ToString("     %s\n", event->GetEventComment().CString());
            source += "    **/\n";
        } else {
            source += ToString("    /** Wrapper function to generate a properly formatted event handler to pass to 'subscribeToEvent' for the %s event. **/\n", event->GetEventName().CString());
        }
        source += ToString("    export function %s (callback : Atomic.EventCallback<%s>) : Atomic.EventMetaData;\n", scriptEventName.CString(), scriptEventName.CString());
        source += "\n";

        if (params.Size() > 0)
        {
            source += ToString("    /** Wrapper function to construct callback data to pass to 'sendEvent' for the %s event. **/ \n", event->GetEventName().CString());
            source += ToString("    export function %sData (callbackData : %s) : Atomic.EventCallbackMetaData; \n", scriptEventName.CString(), scriptEventName.CString());
        } else {
            source += ToString("    /** Wrapper function to construct object to pass to 'sendEvent' for the %s event. **/ \n", event->GetEventName().CString());
            source += ToString("    export function %sData () : Atomic.EventCallbackMetaData; \n", scriptEventName.CString());
        }

        source += "\n\n";

    }
    source_ += source;
}

void JSBTypeScript::WriteToFile(const String &path)
{
    File file(package_->GetContext());
    file.Open(path, FILE_WRITE);

    file.Write(source_.CString(), source_.Length());

    file.Close();

}

inline bool CompareJSBModulesByName(const SharedPtr<JSBModule>& lhs, const SharedPtr<JSBModule>& rhs)
{
    return lhs->GetName() < rhs->GetName();
}

void JSBTypeScript::Emit(JSBPackage* package, const String& path)
{
    package_ = package;

    Vector<SharedPtr<JSBModule>>& modules = package->GetModules();

    // Sort modules alphabetically to ensure consistent output across machines
    Sort(modules.Begin(), modules.End(), CompareJSBModulesByName);

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
        ExportModuleEvents(modules[i]);
    }


    End();
    WriteToFile(path);

}

}
