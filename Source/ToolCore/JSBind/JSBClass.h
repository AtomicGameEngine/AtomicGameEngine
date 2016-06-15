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

    void Parse();

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

    /* returns proper case for property name eg:
        URLEnabled->urlEnabled
        BaseType->baseType
        RGBA16Format->rgba16Format
        EncodeURLList->encodeUrlList
        EncodeURLListB->encodeUrlListB
        EncodeURLB->encodeUrlb// No way to split words*/
    String GetCasePropertyName()
    {
        if (!name_.Length())
            return name_;

        bool allUpper = true;
        String newName = name_;
        int nameLength = name_.Length();

        for (unsigned k = 0; k < nameLength; k++)
        {
            if (islower(name_[k]))
            {
                allUpper = false;
                break;
            }
        }

        if (allUpper)
            return name_.ToLower();

        if (isupper(name_[0]))
            newName[0] = tolower(name_[0]);

        if (nameLength == 1)
            return newName;

        char prevChar = name_[0];
        for (int k = 1; k < nameLength - 1; k++)
        {
            char currChar = name_[k];
            char nextChar = name_[k + 1];

            if ('0' <= nextChar && nextChar <= '9')
            {
                newName[k] = tolower(currChar);
            }
            else if (isupper(currChar) && isupper(prevChar) && isupper(nextChar))
            {
                newName[k] = tolower(currChar);
            }

            prevChar = currChar;
        }
        if (isupper(name_[nameLength - 1]) && isupper(prevChar))
            newName[nameLength - 1] = tolower(name_[nameLength - 1]);

        //Checks IOS at front
        if (name_.StartsWith("IOS", false))
        {
            newName[0] = tolower(name_[0]);
            newName[1] = tolower(name_[1]);
            newName[2] = tolower(name_[2]);
        }

        int lastLetter = nameLength - 1;
        int secondLast = nameLength - 2;
        int thirdLast = nameLength - 3;

        //Check IOS at back
        if (name_.EndsWith("IOS", false))
        {
            newName[lastLetter] = toupper(name_[lastLetter]);
            newName[secondLast] = toupper(name_[secondLast]);
            newName[thirdLast] = toupper(name_[thirdLast]);
        }

        return newName;
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

    const String& GetName() const { return name_; }
    const String& GetNativeName() { return nativeName_; }
    JSBClass* GetBaseClass();
    PODVector<JSBClass*>& GetBaseClasses() { return baseClasses_; }
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
    int  GetNumberArrayElements() { return numberArrayElements_; }
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
