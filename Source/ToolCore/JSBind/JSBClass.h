//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Core/Object.h>

#include "JSBHeader.h"
#include "JSBModule.h"

using namespace Atomic;

namespace ToolCore
{

class JSBPackage;
class JSBFunction;
class JSBFunctionType;
class JSBType;

// chosen function overrides
class JSBFunctionSignature
{

public:

    JSBFunctionSignature(const String& name, const Vector<String>& sig);

    String name_;
    Vector<String> sig_;
    Vector<JSBType*> types_;

    bool Match(JSBFunction* function);

    void Parse() ;

    bool parsed_;
};

class JSBProperty
{

public:
    String name_;
    JSBFunction* getter_;
    JSBFunction* setter_;

    JSBProperty() : getter_(0), setter_(0)
    {

    }

    // returns proper case for property name
    // based on whether the Getter/Setter is all caps
    // GetMyValue -> myValue;
    // GetGUID -> guid
    // URLEnabled -> urlEnabled
    String GetCasePropertyName()
    {
        if (!name_.Length())
            return name_;

        bool allUpper = true;

        // TODO: https://github.com/AtomicGameEngine/AtomicGameEngine/issues/587
        if (name_ == "URLEnabled")
        {
            return "urlEnabled";
        }

        for (unsigned k = 0; k < name_.Length(); k++)
        {
            if (!isupper(name_[k]))
            {
                allUpper = false;
                break;
            }
        }

        if (allUpper)
        {
            return name_.ToLower();
        }

        String name = name_;
        name[0] = tolower(name[0]);
        return name;
    }

};


class JSBClass : public Object
{
    friend class JSClassWriter;
    friend class CSClassWriter;

    OBJECT(JSBClass)

public:

    JSBClass(Context* context, JSBModule* module, const String& name, const String& nativeName);
    virtual ~JSBClass();

    const String& GetName() { return name_; }
    const String& GetNativeName() { return nativeName_; }
    JSBClass* GetBaseClass();
    PODVector<JSBClass*>& GetBaseClasses() {return baseClasses_; }
    PODVector<JSBFunction*>& GetFunctions() { return functions_; }

    bool IsAbstract() { return isAbstract_; }

    /// Note that if we at some point want to generate bindings for JSBClass
    /// this override will need to be addressed, as we'll need to know that JSBClass is
    /// itself an object
    bool IsObject() { return isObject_; }

    bool HasProperties() { return hasProperties_; }
    void GetPropertyNames(Vector<String>& names) { names = properties_.Keys(); }
    JSBProperty* GetProperty(const String& name)
    {
        if (!properties_.Contains(name))
            return 0;

        return properties_[name];
    }

    JSBFunction* MatchFunction(JSBFunction* function, bool includeBases = false);
    bool MatchProperty(JSBProperty* property, bool includeBases = false);

    JSBHeader* GetHeader() { return header_; }
    JSBModule* GetModule() { return module_; }
    JSBPackage* GetPackage() { return module_->GetPackage(); }

    bool IsNumberArray() { return numberArrayElements_ != 0; }
    int  GetNumberArrayElements() { return numberArrayElements_;}
    const String& GetArrayElementType() const { return arrayElementType_; }

    JSBFunction* GetConstructor();

    void SetAbstract(bool value = true) { isAbstract_ = value; }
    void SetObject(bool value = true) { isObject_ = value; }
    void SetHeader(JSBHeader* header) { header_ = header; }
    void SetBaseClass(JSBClass* baseClass);

    void SetSkipFunction(const String& name, bool skip = true);
    void AddFunction(JSBFunction* function);
    void AddFunctionOverride(JSBFunctionSignature* override) { overrides_.Push(override); }
    void AddFunctionExclude(JSBFunctionSignature* exclude) { excludes_.Push(exclude); }
    void AddPropertyFunction(JSBFunction* function);

    void AddTypeScriptDecl(const String& decl) { typeScriptDecls_.Push(decl); }
    unsigned GetNumTypeScriptDecl() { return typeScriptDecls_.Size(); }
    const String& GetTypeScriptDecl(unsigned idx) { return typeScriptDecls_[idx]; }

    void AddHaxeDecl(const String& decl) { haxeDecls_.Push(decl); }
    unsigned GetNumHaxeDecl() { return haxeDecls_.Size(); }
    const String& GetHaxeDecl(unsigned idx) { return haxeDecls_[idx]; }

    void Preprocess();
    void Process();
    void PostProcess();

    void Dump();

private:

    void RecursiveAddBaseClass(PODVector<JSBClass *> &baseClasses);

    String name_;
    String nativeName_;

    SharedPtr<JSBHeader> header_;
    SharedPtr<JSBModule> module_;

    PODVector<JSBFunction*> functions_;
    PODVector<JSBClass*> baseClasses_;

    PODVector<JSBFunctionSignature*> overrides_;
    PODVector<JSBFunctionSignature*> excludes_;

    Vector<String> typeScriptDecls_;
    Vector<String> haxeDecls_;

    bool isAbstract_;
    bool isObject_;

    // Vector3, Color, etc are marshalled via arrays
    int numberArrayElements_;
    String arrayElementType_;

    bool hasProperties_;
    HashMap<String, JSBProperty*> properties_;

};


}
