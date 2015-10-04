//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Core/Object.h>

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
class JSBPrimitiveType;

class JSBModule : public Object
{
    friend class JSModuleWriter;
    friend class CSModuleWriter;

    OBJECT(JSBModule)

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

    JSBClass* GetClass(const String& name);
    Vector<SharedPtr<JSBClass>> GetClasses();
    Vector<SharedPtr<JSBEnum>> GetEnums();
    HashMap<String, Constant>& GetConstants() { return constants_; }

    void RegisterClass(String name);

    JSBEnum* GetEnum(const String& name);
    void RegisterEnum(JSBEnum* jenum);

    bool ContainsConstant(const String& constantName);
    void RegisterConstant(const String& constantName, const String& value, unsigned type, bool isUnsigned = false);

    bool Requires(const String& requirement) { return requirements_.Contains(requirement); }

    bool Load(const String& jsonFilename);
    void PreprocessHeaders();
    void VisitHeaders();

    void PreprocessClasses();
    void ProcessClasses();
    void PostProcessClasses();

    void SetDotNetModule(bool value) { dotNetModule_ = value; }
    bool GetDotNetModule() { return dotNetModule_; }

private:

    void ProcessOverloads();
    void ProcessExcludes();
    void ProcessTypeScriptDecl();
    void ProcessHaxeDecl();

    void ScanHeaders();

    String name_;

    SharedPtr<JSBPackage> package_;
    Vector<SharedPtr<JSBHeader>> headers_;
    Vector<String> includes_;

    Vector<String> sourceDirs_;
    Vector<String> classnames_;

    HashMap<String, String> classRenames_;

    // native name -> JSBClass
    HashMap<StringHash, SharedPtr<JSBClass> > classes_;
    HashMap<StringHash, SharedPtr<JSBEnum> > enums_;

    HashMap<String, Constant> constants_;

    Vector<String> requirements_;

    SharedPtr<JSONFile> moduleJSON_;

    bool dotNetModule_;

};


}
