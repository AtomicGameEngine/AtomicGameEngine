
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

class JSBModule : public Object
{

    OBJECT(JSBModule)

public:

    JSBModule(Context* context, JSBPackage* package);
    virtual ~JSBModule();

    JSBPackage* GetPackage() { return package_; }

    JSBClass* GetClass(const String& name);
    void RegisterClass(String name);

    JSBEnum* GetEnum(const String& name);
    void RegisterEnum(JSBEnum* jenum);

    bool ContainsConstant(const String& constantName);
    void RegisterConstant(const String& constantName);

    bool Load(const String& jsonFilename);
    void PreprocessHeaders();
    void VisitHeaders();

private:

    void ProcessOverloads();

    void ScanHeaders();

    String name_;

    SharedPtr<JSBPackage> package_;
    Vector<SharedPtr<JSBHeader>> headers_;

    Vector<String> sourceDirs_;
    Vector<String> classnames_;

    HashMap<String, String> classRenames_;

    // native name -> JSBClass
    HashMap<StringHash, SharedPtr<JSBClass> > classes_;
    HashMap<StringHash, SharedPtr<JSBEnum> > enums_;
    Vector<String> constants_;

    SharedPtr<JSONFile> moduleJSON_;

};


}
