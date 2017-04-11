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
#include <Atomic/Resource/JSONValue.h>
#include "JSBindTypes.h"

using namespace Atomic;

namespace Atomic
{
class JSONFile;
}

namespace ToolCore
{

class JSBPackage;
class JSBHeader;
class JSBClass;
class JSBEnum;
class JSBEvent;
class JSBPrimitiveType;

class JSBModule : public Object
{
    friend class JSModuleWriter;
    friend class CSModuleWriter;

    ATOMIC_OBJECT(JSBModule, Object)

public:

    struct Constant
    {
        JSBPrimitiveType* type;
        String value;
    };


    JSBModule(Context* context, JSBPackage* package);
    virtual ~JSBModule();

    const String& GetName() { return name_; }
    JSBPackage* GetPackage() { return package_; }

    JSBClass* GetClass(const String& name, bool includeInterfaces = false);
    Vector<SharedPtr<JSBClass>> GetClasses(bool includeInterfaces = false);
    Vector<SharedPtr<JSBEnum>> GetEnums();
    HashMap<String, Constant>& GetConstants() { return constants_; }

    void RegisterClass(String name);

    JSBEnum* GetEnum(const String& name);
    void RegisterEnum(JSBEnum* jenum);

    bool ContainsConstant(const String& constantName);
    void RegisterConstant(const String& constantName, const String& value, unsigned type, bool isUnsigned = false);

    JSBEvent* GetEvent(const String& eventID, const String& eventName);
    void RegisterEvent(JSBEvent* event);

    const Vector<SharedPtr<JSBEvent>>& GetEvents();

    bool Requires(const String& requirement) { return requirements_.Contains(requirement); }

    bool Load(const String& jsonFilename);
    void PreprocessHeaders();
    void VisitHeaders();

    void PreprocessClasses();
    void ProcessClasses();
    void PostProcessClasses();

    void SetDotNetModule(bool value) { dotNetModule_ = value; }
    bool GetDotNetModule() { return dotNetModule_; }

    /// Define guard for specific module code
    String GetModuleDefineGuard() const;

    /// Define guard for specific module code
    String GetClassDefineGuard(const String& name, const String& language = String::EMPTY) const;

    /// Get the module's header files
    const Vector<SharedPtr<JSBHeader>>& GetHeaders() const { return headers_; }

private:

    void ProcessOverloads();
    void ProcessExcludes(const JSONValue& excludes, BindingLanguage language = BINDINGLANGUAGE_ANY);
    void ProcessExcludes();
    void ProcessClassExcludes();
    void ProcessTypeScriptDecl();
    void ProcessHaxeDecl();

    /// Process CSharp declarations, including interfaces
    void ProcessCSharpDecl();

    void ScanHeaders();

    String name_;

    SharedPtr<JSBPackage> package_;
    Vector<SharedPtr<JSBHeader>> headers_;
    Vector<String> includes_;
    Vector<String> jsmodulePreamble_;

    Vector<String> sourceDirs_;
    Vector<String> classnames_;
    Vector<String> interfaceNames_;

    Vector<String> genericClassnames_;

    HashMap<String, String> classRenames_;

    // native name -> JSBClass
    HashMap<StringHash, SharedPtr<JSBClass> > classes_;
    HashMap<StringHash, SharedPtr<JSBEnum> > enums_;

    Vector<SharedPtr<JSBEvent> > events_;

    HashMap<String, Constant> constants_;

    Vector<String> requirements_;

    SharedPtr<JSONFile> moduleJSON_;

    HashMap<String, Vector<String>> classExcludes_;

    bool dotNetModule_;

};


}
